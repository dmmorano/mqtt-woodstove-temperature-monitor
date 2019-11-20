#include <max6675.h>
#include <Wire.h>
#include <ESP8266Wifi.h>
#include <PubSubClient.h>

const int ktcCLK = 14;
const int ktcCS = 12;
const int ktcSO = 13;
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

const char* ssid = "Weavery";
const char* password = "HiddenValley";
const char* mqtt_server = "broker.hivemq.com";

const char* stoveTempTag = "Weavery_dmm/UpperStoveTemp";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("outTopic", "hello world");
      // ... and resubscribe
      mqttClient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
 void setup() {
  setup_wifi();
  Serial.begin(9600);
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqtt_callback);
  delay(2000);

}
void loop() {

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Read temperature as Celsius in to a char array (msg)_
  float DF = ktc.readFahrenheit();
  String tempString = String(DF,0);
  char msg[tempString.length() + 1];
  tempString.toCharArray(msg, tempString.length() + 1);

  Serial.print("Tempurature: ");
  Serial.println(msg);

  Serial.print("Publish message: ");
  Serial.print(msg);
  Serial.print(" to ");
  Serial.println(stoveTempTag);

  mqttClient.publish("Weavery_dmm/UpperStoveTemp", msg);
  delay(10000);
}
 
