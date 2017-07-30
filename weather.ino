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
#include "barometer.h"
#include "gas_detection.h"


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
  setupBarometer();
  setupGasSensors();
}

float h;
float t;
float hic;
long pressureValue;
double baroTemp;

#define ERROR_VALUE -99;

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

  // get gas reading
  MQ135Reading(); // sets variable mq135Value;
  MQ5Reading(); // sets variable mq5Value;
  MQ6Reading(); // sets variable mq6Value;
  MQ9Reading(); // sets variable mq9Value;
  
  // read the value from the sensor:
  lightValue = getLightReading();

  // Using the BMP280 (barometer hw)
  Serial.println("[Barometer] starting pressure");
  pressureValue = getPressure();
  Serial.println("[Barometer] pressure done");
  Serial.println("[Barometer] getting baro temp");
  baroTemp = getBarometerTemperature();
  Serial.println("[Barometer] end baro temp");
  hic = dht.computeHeatIndex(t, h, false);
  // Using the DHT 11:
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  Serial.println("[TEMP] starting temp");
  h = dht.readHumidity();
  t =  dht.readTemperature();

  if (isnan(t)) {
    Serial.println("[TEMP] temp Failed to read from DHT sensor!");
    t = ERROR_VALUE;
    hic = ERROR_VALUE;
  }
  if (isnan(h)) {
    Serial.println("[TEMP] humidity Failed to read from DHT sensor!");
    h = ERROR_VALUE;
    hic = ERROR_VALUE;
  }

    Serial.print("[TEMP] temp: ");
    Serial.println(t);
    
    Serial.print("[TEMP] humidity: ");
    Serial.println(h);

    Serial.print("[TEMP] heat index: ");
    Serial.println(hic);
    
  Serial.println("[TEMP] done temp");

  // TIME CODE:
  unsigned long currentTime = getTimeReading();

  json["light"] = lightValue;
  json["humidity"] = h;
  json["temp_celcius"] = t;
  json["heat_index"] = hic;
  json["pressure_pa"] = pressureValue;
  json["baro_temp_celcius"] = baroTemp;
  json["mq135"] = mq135Value;
  json["mq5"] = mq5Value;
  json["mq6"] = mq6Value;
  json["mq9"] = mq9Value;
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
