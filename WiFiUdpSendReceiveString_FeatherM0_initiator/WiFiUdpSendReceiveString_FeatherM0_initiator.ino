/*
  WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using a WiFi shield.
 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 Circuit:
 * WiFi shield attached

 created 30 December 2012
 by dlf (Metodo2 srl)

 */


#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "Adafruit_NeoTrellis.h"
int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char ReplyBuffer[] = "a";       // a string to send back

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds

WiFiUDP Udp;

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys
//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {

  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F)}
  ,{ Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x31)}

};
int flag = 0;

//pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM/4, X_DIM/4);

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

//define a callback for key presses
TrellisCallback blink(keyEvent evt){

  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    trellis.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, X_DIM*Y_DIM, 0, 255))); //on rising
  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
    trellis.setPixelColor(evt.bit.NUM, 0); //off falling
  trellis.show();
  return 0;
}
//define a callback for key presses
TrellisCallback sequencer(keyEvent evt){

  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    flag +=1;
    if (flag == 1)
      trellis.setPixelColor(evt.bit.NUM, 0xFF0000); //on rising
    else if (flag == 2)
      trellis.setPixelColor(evt.bit.NUM, 0xff3200); //on rising
    else if (flag == 3)
      trellis.setPixelColor(evt.bit.NUM, 0xff6400); //fa6200  ff5d00on rising
    else if (flag == 4)
      trellis.setPixelColor(evt.bit.NUM, 0xfeaa06); //on rising
    else if (flag> 4){
      trellis.setPixelColor(evt.bit.NUM, 0x000000); //on rising
      flag = 0;}

//  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
//    trellis.setPixelColor(evt.bit.NUM, 0); //off falling
  trellis.show();
  return 0;
}
TrellisCallback send_UDP(keyEvent evt){

  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    flag +=1;
    if (flag == 1)
      {trellis.setPixelColor(evt.bit.NUM, 0xFF0000); //on rising
      // int len = Udp.read(packetBuffer, 255);
      // if (len > 0) packetBuffer[len] = 0;
      Udp.beginPacket(IPAddress (192,168,1,47), 2390);
      Udp.write(ReplyBuffer);
      Udp.endPacket();
      }
    else if (flag == 2)
      trellis.setPixelColor(evt.bit.NUM, 0xff3200); //on rising
    else if (flag == 3)
      trellis.setPixelColor(evt.bit.NUM, 0xff6400); //fa6200  ff5d00on rising
    else if (flag == 4)
      trellis.setPixelColor(evt.bit.NUM, 0xfeaa06); //on rising
    else if (flag> 4){
      trellis.setPixelColor(evt.bit.NUM, 0x000000); //on rising
      flag = 0;}

//  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
//    trellis.setPixelColor(evt.bit.NUM, 0); //off falling
  trellis.show();
  return 0;
}

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if(!trellis.begin()){
    Serial.println("failed to begin trellis");
    while(1);
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
//  // send a UDP, to the IP address 192.168.1.47 (for other arduino) and port (2390, can be changed) to start a communication. other arduino does not have this line of code and wil wait till it gets the UDP and responds start back and fort since then
//  Udp.beginPacket(IPAddress (192,168,1,47), 2390);
//  Udp.write(ReplyBuffer);
//  Udp.endPacket();
  for(int i=0; i<Y_DIM*X_DIM; i++){
      trellis.setPixelColor(i, Wheel(map(i, 0, X_DIM*Y_DIM, 0, 255))); //addressed with keynum
      trellis.show();
      delay(50);
  }
  for(int y=0; y<Y_DIM; y++){
    for(int x=0; x<X_DIM; x++){
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      //trellis.registerCallback(x, y, blink);
      trellis.registerCallback(x, y, send_UDP);
      trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
      trellis.show(); //show all LEDs
      delay(10);
    }
  }
}
//IPAddress ip (192,168,1,47);
int i=0;
void loop() {
  trellis.read();
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    i++;
    //lastConnectionTime = millis();
//    Serial.print("Received packet of size ");
//    Serial.println(packetSize);
//    Serial.print("From ");
//    IPAddress remoteIp = Udp.remoteIP();
//    Serial.print(remoteIp);
//    Serial.print(", port ");
//    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
//    Finally if the length of the packet is greater than zero, we will terminate it
//    with an ASCII zero character so that it conforms to the Arduino recommendation
//    for strings. This prevents errors when we print the string to the serial port.
    if (len > 0) packetBuffer[len] = 0;

//    Serial.println("Contents:");
//    Serial.println(packetBuffer);
    Serial.println(i);

    //delay(100);

    //delay(10);

  }
//  if (millis() - lastConnectionTime > postingInterval) {
//    // send a reply, to the IP address and port that sent us the packet we received
//    Udp.beginPacket(IPAddress (192,168,1,47), 2390);
//    Udp.write(ReplyBuffer);
//    Udp.endPacket();
//    lastConnectionTime = millis();
//  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
