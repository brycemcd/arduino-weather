// NOTE: set MQTT_MAX_PACKET_SIZE to 512
// in PubSubClient.h
#include <PubSubClient.h>

#define MQTT_SERVER "spark4.thedevranch.net"
#define WEATHER_TOPIC "weather"
#define QUEUE_CLIENT_IDENTIFIER "weather001"

WiFiClient espClient;
PubSubClient client(espClient);

void setupMessageQueue() {
  client.setServer(MQTT_SERVER, 1883);
}

void reconnectToMessageQueue() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(QUEUE_CLIENT_IDENTIFIER)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishToMessageQueue(char* postable) {
   if (!client.connected()) {
    reconnectToMessageQueue();
  }
  
  Serial.print("publishing topic: ");
  Serial.print(client.state());
  Serial.print(" ");
  Serial.print(sizeof(postable));
  Serial.print(" ");
  Serial.print(MQTT_MAX_PACKET_SIZE);
  Serial.print(" ");
  Serial.println( client.publish(WEATHER_TOPIC, postable, sizeof(postable)) );
}

