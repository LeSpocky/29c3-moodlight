#avrdude -v -p t85 -c dragon_isp -P usb -B 10 -U lfuse:w:0xe1:m -U hfuse:w:0xd7:m
#avrdude -v -p t85 -c avrisp2 -P usb -B 10 -U lfuse:w:0xe1:m -U hfuse:w:0xd7:m
avrdude -v -p t85 -c usbtiny -P usb -B 10 -U lfuse:w:0xe1:m -U hfuse:w:0xd7:m
