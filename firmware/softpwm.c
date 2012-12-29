
/// very simple interrupt-driven 8-bit PWM @ ~244 Hz
#define F_CPU	16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define CHMAX 3 // maximum number of PWM channels

#define RED_CLEAR (pinlevelB &= ~(1 << RED)) // map RED to PB0
#define GREEN_CLEAR (pinlevelB &= ~(1 << GREEN)) // map GREEN to PB1
#define BLUE_CLEAR (pinlevelB &= ~(1 << BLUE)) // map BLUE to PB2

//! Set bits corresponding to pin usage above
#define PORTB_MASK  (1 << PB0)|(1 << PB1)|(1 << PB2)

#define set(x) |= (1<<x)
#define clr(x) &=~(1<<x)
#define inv(x) ^=(1<<x)

#define RED PB0
#define GREEN PB1
#define BLUE PB2
#define LED_PORT PORTB
#define LED_DDR DDRB

void init(void);

unsigned char compare[CHMAX];
volatile unsigned char compbuff[CHMAX];

int r_val = 0x00;
int g_val = 0x55;
int b_val = 0xAA;

int main(void) {
  init();

  int r_dir = 1;
  int g_dir = 2;
  int b_dir = 4;

  for(;;) {
    if (r_val > 254 - 1) {
      r_dir = -1;
    }
    if (r_val < 1 + 1) {
      r_dir = 1;
    }

    if (g_val > 254 - 3) {
      g_dir = -3;
    }
    if (g_val < 1 + 3) {
      g_dir = 3;
    }

    if (b_val > 254 - 5) {
      b_dir = -5;
    }
    if (b_val < 1 + 5) {
      b_dir = 5;
    }

    r_val += r_dir;
    g_val += g_dir;
    b_val += b_dir;

    compbuff[0] = r_val;
    compbuff[1] = g_val;
    compbuff[2] = b_val;

    _delay_ms(100);
  }
}


void init(void) {
  // set the direction of the ports
  LED_DDR set(RED);
  LED_DDR set(GREEN);
  LED_DDR set(BLUE);

  unsigned char i;

  CLKPR = (1 << CLKPCE);        // enable clock prescaler update
  CLKPR = 0;                    // set clock to maximum (= crystal)

  // initialise all channels
  for(i=0 ; i<CHMAX ; i++) {
    compare[i] = 0;             // set default PWM values
    compbuff[i] = 0;            // set default PWM values
  }

  TIFR = (1 << TOV0);           // clear interrupt flag
  TIMSK = (1 << TOIE0);         // enable overflow interrupt
  TCCR0B = (1 << CS00);         // start timer, no prescale

  sei();
}


ISR (TIM0_OVF_vect) {
  static unsigned char pinlevelB=PORTB_MASK;
  static unsigned char softcount=0xFF;

  PORTB = pinlevelB;            // update outputs

  if(++softcount == 0){         // increment modulo 256 counter and update
                                // the compare values only when counter = 0.
    compare[0] = compbuff[0];   // verbose code for speed
    compare[1] = compbuff[1];
    compare[2] = compbuff[2];

    pinlevelB = PORTB_MASK;     // set all port pins high
  }
  // clear port pin on compare match (executed on next interrupt)
  if(compare[0] == softcount) RED_CLEAR;
  if(compare[1] == softcount) GREEN_CLEAR;
  if(compare[2] == softcount) BLUE_CLEAR;
}
