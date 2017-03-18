// https://github.com/arduino-libraries/NTPClient
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

unsigned long getTimeReading() {
  Serial.println("[TIME] starting time");
  timeClient.update();
  unsigned long currentTime = timeClient.getEpochTime();
  Serial.print("[TIME] time is ");
  Serial.println(currentTime);
  
  return currentTime;
}

void setupTime() {
  timeClient.begin();
  timeClient.setUpdateInterval(600000); // in ms
}

