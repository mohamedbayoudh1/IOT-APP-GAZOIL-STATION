#include "PubSubClient.h" 
#include <WiFi.h>
#include <OneWire.h>
#include <ArduinoJson.h>


// Data wire is plugged into digital pin 2 on the Arduino


#define ONE_WIRE_BUS 5
#define POWER_PIN  18
#define SIGNAL_PIN 33
#define SENSOR_MIN 0
#define SENSOR_MAX 521
// Setup a oneWire instance to communicate with any OneWire device
	

// Pass oneWire reference to DallasTemperature library


// WiFi
const char* ssid = "";                 // Your personal network SSID
const char* wifi_password = ""; // Your personal network password

// MQTT
const char* mqtt_server = "";  // IP of the MQTT broker
const char* mqtt_username = ""; // MQTT username
const char* mqtt_password = ""; // MQTT password
const char* clientID = "client_livingroom"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

    if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

int Buzzer = 32;        // used for ESP32
int Fire_analog = 35;    // used for ESP32
int Fire_digital = 2; 
int value = 0; // variable to store the sensor value
int level = 0; // variable to store the water level
void setup(void)
{
 
  Serial.begin(9600);
   pinMode(Buzzer, OUTPUT);      
  pinMode(Fire_digital, INPUT);
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
}

void loop(void)
{ connect_MQTT();
  Serial.setTimeout(2000);
  // Send the command to get temperatures

  float fire_value=0;
   int firesensorAnalog = analogRead(Fire_analog);
  int firesensorDigital = digitalRead(Fire_digital);
  //print the temperature in Celsius
  Serial.print("Fire Sensor: ");
  Serial.print(firesensorAnalog);
  Serial.print("\t");
  Serial.print("Fire Class: ");
  Serial.print(firesensorDigital);
  Serial.print("\t");
  Serial.print("\t");
  
  if (firesensorAnalog < 1000) {
    Serial.println("Fire");
     fire_value=1;
    digitalWrite (Buzzer, HIGH) ; //send tone
    delay(1000);
    digitalWrite (Buzzer, LOW) ;  //no tone
  }
  else {
    Serial.println("No Fire");
    fire_value=0;
  }
  Serial.println("***");
   digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 4); // 12 levels
  Serial.print("Water level: ");
  Serial.println(level);
 
StaticJsonDocument<200> doc;
  doc["device"] = "ESP32";
  doc["value1"] = level;
  doc["value2"] = fire_value;
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("home/pi/pi/data", buffer);

 
  client.disconnect();  // disconnect from the MQTT broker
  delay(1000*10);       // print new values every 1 Minute
}