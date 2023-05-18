#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ADS1115_lite.h>
#include <Adafruit_MAX31865.h>
#include <U8x8lib.h>
#include <Wire.h>

// Class for running the UDP connection /////////////////////
class UDPClient{
  public:
    UDPClient() = delete;
    UDPClient( const unsigned int l_port ):m_port(l_port){}
    virtual ~UDPClient() = default;
    UDPClient( const UDPClient& l_client ) = delete;
    UDPClient& operator = ( const UDPClient& l_client ) = delete;

    bool setup(){
      if( m_udp.begin( m_port )){
        m_setup = true;
        return true;
      }else{
        m_setup = false;
        return false;
      }
    }

    bool recievePacket(){
      if( m_setup ){
        m_currentPacketSize = m_udp.parsePacket();
        if( m_currentPacketSize ){
          // read the packet into packetBuffer
          m_udp.read( m_packetBuffer, UDP_TX_PACKET_MAX_SIZE );
          return true;
        }
      }
      return false;
    }

    unsigned int getRemotePort() const{
      return m_remotePort;
    }

    IPAddress getRemoteIP() const{
      return m_remoteIP;
    }

    void setRemotePort( unsigned int port ){
      m_remotePort = port;
    }

    void setRemoteIP( IPAddress IP ){
      m_remoteIP = IP;
    }

    int getMessageLength() const{
      return m_currentPacketSize;
    }

    const char* getMessage() const{
      return m_packetBuffer;
    }

    void flushPacketBuffer(){
      for( int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++ ){
        m_packetBuffer[i] = '\0';
      }
    }

    bool writeMessage( const char* message ){
      if( m_setup ){
        m_udp.beginPacket(getRemoteIP(), getRemotePort());
        m_udp.write(message);
        m_udp.endPacket();
        return true;
      }
      return false;
    }

  private:
    EthernetUDP m_udp;
    const unsigned int m_port;

    int m_currentPacketSize = 0;
    char m_packetBuffer[UDP_TX_PACKET_MAX_SIZE];

    bool m_setup = false;

    unsigned int m_remotePort;
    IPAddress m_remoteIP;
};
/////////////////////////////////////////////////////////////////////

// TIMING REGULAR INTERVALS VARIABLES ///
const unsigned long samplingInterval = 3000;    // How often to collect a batch of readings in ms (e.g. every 20 sec = 20000 ms)
unsigned long previousSampledMillis = 0;        // Time at which previous sample was acquired
unsigned long currentMillis = 0;                // Current time elapsed since program started in ms

////// ADC VARIABLES /////
ADS1115_lite adc(ADS1115_DEFAULT_ADDRESS); //Initializes wire library, sets private configuration variables to ADS1115 default(2.048V, 128SPS, Differential mode between  AIN0 and AIN1.  The Address parameter is not required if you want default
int16_t Raw;
int N = 100;    // Number of samples to use for averaging
long Raw_runningsum;

////// OLED SCREEN VARIABLES ///
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
char result1[7]; // Preallocate character arrays for text to appear on screen
char result2[7];
char result3[7];
char result4[7];

////// PT100 VARIABLES ///
Adafruit_MAX31865 thermo = Adafruit_MAX31865(4, 5, 6, 7);
#define RREF      430.0 // The value of the Rref resistor. Use 430.0 for PT100
#define RNOMINAL  100.0 // The 'nominal' 0-degrees-C resistance of the sensor, 100.0 for PT100

////// ETHERNET VARIABLES /////
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB };
unsigned int localPort = 8040;

UDPClient udpClient( localPort );

IPAddress SERVER_NAME(192, 168, 10, 2);
int    SERVER_PORT = 8089;
int    INFLUX_SENDER_PORT = 8234;
String INFLUXDB_DATABASE = "quantbatt";
String INFLUXDB_USERNAME = "";
String INFLUXDB_PASSWORD = "";
String HTTP_METHOD = "POST";

String SERVER_NAME_STR = String(SERVER_NAME[0]) + "." + SERVER_NAME[1] + "." + SERVER_NAME[2] + "." + SERVER_NAME[3] ;
String endPoint = "/write?db=" + INFLUXDB_DATABASE + "&u=" + INFLUXDB_USERNAME + "&p=" + INFLUXDB_PASSWORD;

UDPClient udpInfluxClient( INFLUX_SENDER_PORT );

String payload = "";  //data for HTTP Post;
char response_buffer[500];
int index = 0;
int response_code;
bool NetworkConnected;
///////////////////////////////////////////////////////

float average_counts;
float Bfield_X_Sensorvoltage;
float Bfield_Y_Sensorvoltage;
float Bfield_Z_Sensorvoltage;
float Bfield_X;
float Bfield_Y;
float Bfield_Z;
float T;

float microTeslaPerVolt = 7.0; // Should be 7 uT/V for real device

int interuptRate;
bool postDataPackets = true;
const String axis[] = {"X", "Y", "Z"};

String experimentID = "none";

void setup() {
  Serial.begin(57600);

  // Start OLED display screen, and display a welcome message /////
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  u8x8.drawString(4, 0, "Welcome!");
  u8x8.drawString(2, 2, "Environment");
  u8x8.drawString(4, 3, "Monitor");
  u8x8.drawString(2, 6, "Starting...");
  delay(3000);
  /////////////////////////////////////////////////////////////////


  // SET UP ETHERNET //
  u8x8.clearDisplay();
  u8x8.drawString(0, 2, "Connecting to");
  u8x8.drawString(0, 3, "network...");
  pinMode(10, OUTPUT);   // SPI SS pin for most Arduino models (need to connect)
  Serial.println("Waiting for IP...");
  if (Ethernet.begin(mac) == 0) { // Putting only mac here means dhcp will be used (and assigned ip address will appear in router table)
    NetworkConnected = false;
    u8x8.drawString(2, 6, "Failed!");
    Serial.println("Network Connection Failed");
  } else {
    NetworkConnected = true;
    u8x8.drawString(11, 3, "Done!");
    u8x8.drawString(2, 6, IpAddress2String(Ethernet.localIP()).c_str() );
    Serial.println(IpAddress2String(Ethernet.localIP()));
    Serial.println("Network Connected");
    Serial.println(Ethernet.localIP());

    udpClient.setup();
    udpInfluxClient.setup();
    udpInfluxClient.setRemotePort( SERVER_PORT );
    udpInfluxClient.setRemoteIP( SERVER_NAME );
  }

  delay(3000);
  /////////////////////

  // SET UP ADC ///////
  adc.setGain(ADS1115_REG_CONFIG_PGA_4_096V);
  adc.setSampleRate(ADS1115_REG_CONFIG_DR_860SPS); // Sample rate affects noise (see Fig. 14 and 15 of datasheet)
  Wire.setClock(400000);
  /////////////////////

  // SET UP PT100 /////
  thermo.begin(MAX31865_4WIRE);  // initialize the sensor
  /////////////////////

  // Initialise parameters on the OLED display screen /////////////
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, IpAddress2String(Ethernet.localIP()).c_str());
  u8x8.drawString(0, 2, "BX = ");
  u8x8.drawString(5, 2, "00.000");
  u8x8.drawString(13, 2, "uT");
  u8x8.drawString(0, 3, "BY = ");
  u8x8.drawString(5, 3, "00.000");
  u8x8.drawString(13, 3, "uT");
  u8x8.drawString(0, 4, "BZ = ");
  u8x8.drawString(5, 4, "00.000");
  u8x8.drawString(13, 4, "uT");
  u8x8.drawString(0, 5, "T  = ");
  u8x8.drawString(5, 5, "00.00");
  u8x8.drawString(12, 5, "degC");
  /////////////////////////////////////////////////////////////////
  //u8x8.setFont(u8x8_font_chroma48medium8_r);



  // Set up interupt for sending influx data //////////////////////
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  interuptRate = 34286;   // preload timer 65536-16MHz/256/2Hz
  
  TCNT1 = interuptRate;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
  /////////////////////////////////////////////////////////////////
}

ISR( TIMER1_OVF_vect ){        // interrupt service routine
  TCNT1 = interuptRate;   // preload timer

  // Post data packets to influx if requested
  if( postDataPackets ){
    // Construct InfluxDB payload
    float flux[] = {Bfield_X, Bfield_Y, Bfield_Z};
    String payload = "";

    for( int i = 0; i < 3; i++ ){
      payload = "bartington,axis=" + axis[i] + ",experiment_ID=" + experimentID + " flux=" + String(flux[i], 5);

      // Send values to remote InfluxDB database
      if (NetworkConnected) {
        udpInfluxClient.writeMessage( payload.c_str());
      }
    }

    payload = "thermocouple,experiment_ID=" + experimentID + " temperature=" + String(T, 5);
    // Send values to remote InfluxDB database
    if (NetworkConnected) {
      udpInfluxClient.writeMessage( payload.c_str());
    }
  }
}

void loop() {

  udpClient.recievePacket();
  
  String input = udpClient.getMessage();

  String command = input.substring( 0, 8 );
  String action = input.substring( 9 );

  if( command.equals( "setExpID" )){
    experimentID = action;
  }
  if( command.equals( "startDAq" )){
    postDataPackets = true;
  }
  if( command.equals( "stop-DAq" )){
    postDataPackets = false;
  }
  if( command.equals( "setDataR" )){
    if( action.equals("slow") ){
      interuptRate = 34286; // preload timer 65536-16MHz/256/2Hz
    }
    if( action.equals( "standard" )){
      interuptRate = 64286; // preload timer 65536-16MHz/256/50Hz
    }
    if( action.equals( "fast" )){
      interuptRate = 64911; // preload timer 65536-16MHz/256/100Hz
    }
  }
  if( command.equals("setSampA" )){
    int rate = action.toInt();
    if( !(rate <= 0 )){
      N = rate;
    }
  }

  udpClient.flushPacketBuffer();

  //Measure the temperature using PT100 probe
  T = thermo.temperature(RNOMINAL, RREF);

  // Calculate temperature-dependent calibration factors for converting from ADC counts to sensor voltage
  float SLOPE = -0.01474697320 * T - 1599.32272039864;
  float OFFSET = 0.14383886517 * T + 16376.1858163842;

  // Read Channel #1 ///////////////////
  average_counts = collect_ADC_averagecounts(0);
  Bfield_X_Sensorvoltage = (average_counts - OFFSET) / SLOPE;
  Bfield_X_Sensorvoltage = Bfield_X_Sensorvoltage + 0.00;   // Minor correction to each channels reading (on the order of 100uV only)
  Bfield_X = Bfield_X_Sensorvoltage * microTeslaPerVolt;
  //////////////////////////////////////

  // Read Channel #2 ///////////////////
  average_counts = collect_ADC_averagecounts(1);
  Bfield_Y_Sensorvoltage = (average_counts - OFFSET) / SLOPE;
  Bfield_Y_Sensorvoltage = Bfield_Y_Sensorvoltage + 0.00;
  Bfield_Y = Bfield_Y_Sensorvoltage * microTeslaPerVolt;
  //////////////////////////////////////

  // Read Channel #3 ///////////////////
  average_counts = collect_ADC_averagecounts(2);
  Bfield_Z_Sensorvoltage = (average_counts - OFFSET) / SLOPE;
  Bfield_Z_Sensorvoltage = Bfield_Z_Sensorvoltage + 0.00;
  Bfield_Z = Bfield_Z_Sensorvoltage * microTeslaPerVolt;
  //////////////////////////////////////

  updateOLED(Bfield_X,Bfield_Y,Bfield_Z,T);

  delay(1000);
}


float collect_ADC_averagecounts(int channel) {

  switch (channel) {
    case 0:
      adc.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0); //Set single ended mode between AIN0 and GND
      break;
    case 1:
      adc.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_1); //Set single ended mode between AIN1 and GND
      break;
    case 2:
      adc.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_2); //Set single ended mode between AIN2 and GND
      break;
  }

  Raw_runningsum = 0;
  for (int i = 0; i < N; i++) {
    adc.triggerConversion(); //Start a conversion.  This immediatly returns
    Raw = adc.getConversion(); //This polls the ADS1115 and wait for conversion
    Raw_runningsum = Raw_runningsum + Raw;
  }
  average_counts = (float)Raw_runningsum / (float)N ;
  return average_counts;
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +
         String(ipAddress[1]) + String(".") +
         String(ipAddress[2]) + String(".") +
         String(ipAddress[3]);
}

void updateOLED(float Bfield_X, float Bfield_Y, float Bfield_Z, float T){
  // Convert floats to character arrays, so that they can be printed to screen
  dtostrf(Bfield_X, 5, 3, result1);
  dtostrf(Bfield_Y, 5, 3, result2);
  dtostrf(Bfield_Z, 5, 3, result3);
  dtostrf(T,  5, 2, result4);

  if (Bfield_X >= 0) {
    u8x8.drawString(5, 2, "+");
    u8x8.drawString(6, 2, result1);
  }
  else {
    u8x8.drawString(5, 2, result1);
  }

  if (Bfield_Y >= 0) {
    u8x8.drawString(5, 3, "+");
    u8x8.drawString(6, 3, result2);
  }
  else {
    u8x8.drawString(5, 3, result2);
  }

  if (Bfield_Z >= 0) {
    u8x8.drawString(5, 4, "+");
    u8x8.drawString(6, 4, result3);
  }
  else {
    u8x8.drawString(5, 4, result3);
  }

  u8x8.drawString(5, 5, result4);

  if( postDataPackets ){
    u8x8.drawString(0, 1, "Posting");
  }else{
    u8x8.drawString(0, 1, "Not Posting");
  }
}
