/*
 Basic MQTT example 
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// your network name also called SSID
char ssid[] = "2226";
// your network password
char password[] = "apt2226wifi";
// MQTTServer to use
char server[] = "iot.eclipse.org";

// globals
int sensorVal;        // ADC val from light sensor
int autoOutputVal;     // value received from Node-Red Server


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print("with length ");
  Serial.println(length);
  
  // convert ascii to int
  payload[length] = '\0';
  autoOutputVal = atoi((char *) payload);    // ascii-to-int conversion
  
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup()
{
  Serial.begin(115200);
  
  // Start Ethernet with the build in MAC Address
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  // Print the WiFi status.
  printWifiStatus();
}

void loop()
{
  sensorVal = analogRead(24);
  // Reconnect if the connection was lost
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting....");

    if(!client.connect("smartWindowClient")) {
      Serial.println("Connection failed");
    } else {
      Serial.println("Connection success");
        if(client.subscribe("mspSensorVal")) {
          Serial.println("Subscription successfull");
      }
    } 
  }
  
  
  // convert into to char array
  String str = (String)sensorVal; 
  int str_len = str.length() + 1;  // Length (with one extra character for the null terminator)
  char char_array[str_len];  // Prepare the character array (the buffer) 
  str.toCharArray(char_array, str_len);  // Copy it over 

  if(client.publish("mspSensorVal", char_array)) {
    Serial.println("Publish success");
    Serial.println(char_array);
  } else {
    Serial.println("Publish failed");
  }
 
  // Check if any message were received
  // on the topic we subsrcived to
  client.loop();
  delay(100);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
