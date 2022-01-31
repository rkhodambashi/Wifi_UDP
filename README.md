# Wifi_UDP_Neotrellis
This code sends a UDP message over wifi using Adafruit FeatherM0+ATWINC1500 wifi module. It uses Adafruit Neotrellis 8x8 keyboard as an interface. 
the UDP message is captured by another FeatherM0+ATWINC1500 connected to the USB port of a computer. It generates a MIDI message and sends it over 
USB where a MIDI enabled software such as GarageBAnd can pick it up.  
The folder WiFiUdpSendReceiveString_FeatherM0 is the arduino sketch for receiving UDP. This device is connected to computer through USB.  
The folder WiFiUdpSendReceiveString_FeatherM0 is the arduino sketch for sending UDP. It can use a battery powered arduino board.  
**important note**
Currently the boards should be conncted to the usb port with serial monitor open to initialize the boards and connect to the wifi. Once this step is done, the transmitter board can be disconnected from the USB and run on battery.
