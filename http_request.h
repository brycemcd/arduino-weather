#include <ESP8266HTTPClient.h>

#define PERSIST_HOST "http://web02.thedevranch.net:9200/weather/device001"

void postToHTTP(char* postable) {
  Serial.println("[NETWORK] posting json request");
  HTTPClient http;
  http.begin(PERSIST_HOST);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(postable);
  http.writeToStream(&Serial);
  http.end();
}

