#include <SPI.h>
#include <Ethernet.h>
#include<EthernetUdp.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB };
IPAddress IPADDRESS(192, 168, 0, 50);
byte host[] = {192, 168, 0, 19};
const unsigned int port = 8888;
EthernetUDP Udp;

float Bfield_X = 1001.63;
float Bfield_Y = 909.5;
float Bfield_Z = 1006.812;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000); // Allow ethernet chip time to initialise after being powered up
  Serial.begin(57600);

  //pinMode(53, OUTPUT); // Needed for slave select SPI for Mega (not connected)
  pinMode(10, OUTPUT); // SPI SS pin for most Arduino models (need to connect)

  Ethernet.begin(mac); // Putting only mac here means dhcp will be used (and assigned ip address will appear in router table)
  Udp.begin(port);
  delay(1000); // Allow ethernet chip time to initialise

  Udp.beginPacket(host, port);
  Udp.print("fluxgate BX=");
  Udp.print(String(Bfield_X,2));
  Udp.print(",BY=");
  Udp.print(String(Bfield_Y,2));
  Udp.print(",BZ=");
  Udp.print(String(Bfield_Z,2));
  Udp.print("\n");
  Udp.endPacket();
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
