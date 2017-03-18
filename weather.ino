/*
 * Read ambient light with a photoresistor
 * Read temp and humidity from a DHT 11
 * Blinks the LED when taking a reading
 * 
 * RTC:
 * SDA => D2
 * SCL => D1
 * 
 * Photo Resistor:
 * Negative side goes to A0
 * 
 * DHT11 (L to R while looking at the device):
 * 1 -> VCC
 * 2 -> D3 && resistor -> VCC
 * 3 -> empty
 * 4 -> GND
*/
 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Broadcast readings
#include <ArduinoJson.h>

#include "network.h"
#include "message_queue.h"
#include "light.h"
#include "time.h"
#include "http_request.h"


#define DHTPIN 0 // GPIO0 = D3 on the esp8266
#define DHTTYPE DHT11

int ledPin = 13;      // select the pin for the LED
int lightValue = 0;  // variable to store the value coming from the sensor
int delayBetweenReadings = 15000; // in ms

// DHT_Unified dht(DHTPIN, DHTTYPE);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  dht.begin();

  setupNetwork();
  setupMessageQueue();
}

float h;
float t;
float hic;
void loop() {
  // RESET JSON
  StaticJsonBuffer<300> jsonBuffer; // increase the value of 200 if the json string gets larger
  JsonObject& json = jsonBuffer.createObject();
  
  Serial.println("[JSON] FIRST");
  json.prettyPrintTo(Serial);
  Serial.println();
  
  // Blink the LED to indicate it's taking a reading
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);

  // read the value from the sensor:
  lightValue = getLightReading();
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  Serial.println("[TEMP] starting temp");
  h = dht.readHumidity();//event.relative_humidity;
  delay(275);
  // Read temperature as Celsius (the default)
  t =  dht.readTemperature(); //event.temperature;
  delay(275);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
  delay(275);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("[TEMP] Failed to read from DHT sensor!");
    delay(delayBetweenReadings);
    return;
  } else {
    Serial.print("[TEMP] temp: ");
    Serial.println(t);
    
    Serial.print("[TEMP] humidity: ");
    Serial.println(h);

    Serial.print("[TEMP] heat index: ");
    Serial.println(hic);
    
  }
  
  Serial.println("[TEMP] done temp");

  // TIME CODE:
  unsigned long currentTime = getTimeReading();

  json["light"] = lightValue;
  json["humidity"] = h;
  json["temp_celcius"] = t;
  json["heat_index"] = hic;
  json["capture_dttm"] = currentTime;

  Serial.println("[JSON] populated:");
  json.prettyPrintTo(Serial);
  Serial.println();

  char postable[300];
  json.printTo(postable);

  publishToMessageQueue(postable);
  postToHTTP(postable);
  
  Serial.println("[DONE]---");
  delay(delayBetweenReadings);
}
