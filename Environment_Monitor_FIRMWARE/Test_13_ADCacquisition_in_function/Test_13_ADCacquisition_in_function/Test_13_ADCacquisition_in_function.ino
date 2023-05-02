#include <ADS1115_lite.h>
#include <Ethernet.h>

// Variables for ensuring data is collected at regular time intervals
const unsigned long samplingInterval = 5000;    // How often to take a data sample in ms (must be >25ms, since this is time needed to update OLED screen in loop)
unsigned long previousSampledMillis = 0;        // Time at which previous sample was acquired
unsigned long currentMillis = 0;                // Current time elapsed since program started in ms

ADS1115_lite adc(ADS1115_DEFAULT_ADDRESS); //Initializes wire library, sets private configuration variables to ADS1115 default(2.048V, 128SPS, Differential mode between  AIN0 and AIN1.  The Address parameter is not required if you want default
int16_t Raw;
const int N = 20;
long Raw_runningsum;

////// ETHERNET VARIABLES /////
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB };

IPAddress SERVER_NAME(192, 168, 0, 19);
int    SERVER_PORT = 8086;
String INFLUXDB_DATABASE = "arduino";
String INFLUXDB_USERNAME = "arduino";
String INFLUXDB_PASSWORD = "physics1234";
String HTTP_METHOD = "POST";

String SERVER_NAME_STR = String(SERVER_NAME[0]) + "." + SERVER_NAME[1] + "." + SERVER_NAME[2] + "." + SERVER_NAME[3] ;
String endPoint = "/write?db=" + INFLUXDB_DATABASE + "&u=" + INFLUXDB_USERNAME + "&p=" + INFLUXDB_PASSWORD;

String payload = "";  //data for HTTP Post;
char response_buffer[500];
int index = 0;
int response_code;
///////////////////////////////////////////////////////

float average_counts;
float Bfield_X_Sensorvoltage;
float Bfield_Y_Sensorvoltage;
float Bfield_Z_Sensorvoltage;
float Bfield_X;
float Bfield_Y;
float Bfield_Z;
float T;

float microTeslaPerVolt = 1.0; // Should be 7 uT/V for real device


void setup() {
  Serial.begin(57600);

  pinMode(10, OUTPUT); // SPI SS pin for most Arduino models (need to connect)
  Ethernet.begin(mac); // Putting only mac here means dhcp will be used (and assigned ip address will appear in router table)

  delay(1000); // Allow ethernet chip time to initialise

  adc.setGain(ADS1115_REG_CONFIG_PGA_4_096V);
  adc.setSampleRate(ADS1115_REG_CONFIG_DR_860SPS); //Set the slowest and most accurate sample rate
  Wire.setClock(400000);

}

void loop() {

  currentMillis = millis();

  // When sampling interval has elapsed, go into this loop and take data points ////////////////////////////////////
  if ((currentMillis - previousSampledMillis) >= samplingInterval) {

    // Read Channel #1 ///////////////////
    average_counts = collect_ADC_averagecounts(0);
    Bfield_X_Sensorvoltage = (average_counts - 16234.7057) / 1597.761647;
    Bfield_X = Bfield_X_Sensorvoltage * microTeslaPerVolt;
    //////////////////////////////////////

    // Read Channel #2 ///////////////////
    average_counts = collect_ADC_averagecounts(1);
    Bfield_Y_Sensorvoltage = (average_counts - 16234.7057) / 1597.761647;
    Bfield_Y = average_counts * 0.000125;
    //////////////////////////////////////

    // Read Channel #3 ///////////////////
    average_counts = collect_ADC_averagecounts(2);
    Bfield_Z_Sensorvoltage = (average_counts - 16234.7057) / 1597.761647;
    Bfield_Z = average_counts * 0.000125;
    //////////////////////////////////////

    T = 22.16;

    // Construct InfluxDB payload
    String payload = "fluxgate BX=" + String(Bfield_X, 5) + ",BY=" + String(Bfield_Y, 5) + ",BZ=" + String(Bfield_Z, 5) + ",T=" + String(T, 2);

    // Send values to remote InfluxDB database
    response_code  = post_data_to_influxdb(payload); // Takes 25 ms to connect, post data, and read response

    previousSampledMillis += samplingInterval; // Keep track of last time that a sample was taken
  }

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

int post_data_to_influxdb(String payload) {

  // Make a HTTP request to the InfluxDB server
  client.connect(SERVER_NAME, SERVER_PORT);
  while (!client.connected()) {
    // IMPLEMENT TIMEOUT HERE
  }

  client.println("POST " + endPoint + " HTTP/1.1");
  client.println("Host: " + SERVER_NAME_STR + ":" + String(SERVER_PORT));
  client.println("User-Agent: Arduino/1.6");
  client.println("Connection: close"); // It's possible to use "keep-alive" here instead
  client.println("Content-Type: application/x-www-form-urlencoded;");
  client.print("Content-Length: ");
  client.println(payload.length());
  client.println();
  client.println(payload);

  // Wait for a response from the server
  while (!client.available()) {
    // IMPLEMENT TIMEOUT HERE
  }

  // Read response from server (takes around 10 ms)
  while (client.available()) {
    char c = client.read();
    if (index < 499) {
      response_buffer[index] = c;
      index++;
      response_buffer[index] = '\0';
    }
  }

  //Serial.println(response_buffer);
  client.stop();

  // Return response code (204=success, 401=unauthorized, ... etc)
  String response_code_string = String(response_buffer[9]) + response_buffer[10] + response_buffer[11];
  return response_code_string.toInt();
}
