#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Rapcassrae";
const char* password = "4389224193";
const char* mqtt_server = "10.0.0.171";  // Your MQTT Broker IP address (e.g., Raspberry Pi)


WiFiClient espClient;
PubSubClient client(espClient);


#define LIGHT_SENSOR_PIN A0     // Correct analog pin for ESP8266


long lastMsg = 0;
char msg[50];


#define ANALOG_THRESHOLD  400   // Adjust this threshold as needed


void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 is starting...");
  
  // Disable the watchdog temporarily during Wi-Fi setup
  ESP.wdtDisable();
  setup_wifi();
  ESP.wdtEnable(0);  // Re-enable watchdog after Wi-Fi setup
  
  client.setServer(mqtt_server, 1883);
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(const_cast<char*>(ssid), const_cast<char*>(password));


  int attemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && attemptCount < 20) { // Limit attempts to avoid infinite loop
    delay(500);  // Add delay between attempts
    Serial.print(".");
    attemptCount++;
  }


  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Wi-Fi.");
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // Retry after 5 seconds
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }


  int analogValue = analogRead(LIGHT_SENSOR_PIN);  // Read from correct analog pin
  Serial.print("Light sensor value: ");
  Serial.println(analogValue);


  snprintf(msg, 50, "Light sensor value: %d", analogValue);


  if (millis() - lastMsg > 5000) {
    client.publish("sensor/light_value", msg);  // Publish to the "sensor/light_value" topic
    lastMsg = millis();
  }


  // Publish alert if light level is below threshold
  if (lightValue < ANALOG_THRESHOLD) {
    client.publish("sensor/light_alert", "The Light is ON due ti low light intensity");
    serial.println("Alert: Low light detected. Notification sent.");
  }


  client.loop();
}
 
