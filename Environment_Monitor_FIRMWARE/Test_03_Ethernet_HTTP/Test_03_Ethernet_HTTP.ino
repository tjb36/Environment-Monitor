#include <SPI.h>
#include <Ethernet.h>

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

float Bfield_X;
float Bfield_Y;
float Bfield_Z;

void setup() {  
  delay(500);
  Serial.begin(57600);
  //pinMode(53, OUTPUT); // Needed for slave select SPI for Mega (not connected)
  pinMode(10, OUTPUT); // SPI SS pin for most Arduino models (need to connect)
  Ethernet.begin(mac); // Putting only mac here means dhcp will be used (and assigned ip address will appear in router table)
  if (Ethernet.begin(mac) == 0) 
{ 
  Serial.println("Failed to connect ethernet");
}
  else{
    Serial.println("Ethernet connected");
  }

  delay(1000); // Allow ethernet chip time to initialise
  Serial.println("Starting");
}

// the loop function runs over and over again forever
void loop() {

  // Obtain magnetic field values
  Bfield_X = -5.63;
  Bfield_Y = 3.5;
  Bfield_Z = 2.000;

  // Construct InfluxDB payload
  String payload = "fluxgate BX=" + String(Bfield_X, 2) + ",BY=" + String(Bfield_Y, 2) + ",BZ=" + String(Bfield_Z, 2);

  // Send values to remote InfluxDB database
  response_code  = post_data_to_influxdb(payload); // Takes 25 ms to connect, post data, and read response
  
  // Check if successful
  if (response_code == 204) {
    Serial.println("SUCCESSFUL");
  } else if (response_code == 401) {
    Serial.println("UNAUTHORIZED");
  } else if (response_code == 400) {
    Serial.println("BAD REQUEST");
  }

  delay(5000);

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
