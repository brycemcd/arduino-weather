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
// Temp and Humidity
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Broadcast readings
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Time Requirements
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

#define DHTPIN 0 // GPIO0 = D3 on the esp8266
#define DHTTYPE DHT11

// used in time calc
#define countof(a) (sizeof(a) / sizeof(a[0]))

int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int lightValue = 0;  // variable to store the value coming from the sensor
int delayBetweenReadings = 15000; // in ms

const char* ssid     = "lolpackets-2.4G";
const char* password = "BryceRules";

//const char* persistHost = "http://10.1.2.128:9999/foo";
const char* persistHost = "http://web01.thedevranch.net:9200/weather/device001";

// DHT_Unified dht(DHTPIN, DHTTYPE);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  dht.begin();
  Serial.begin(115200);
/* NETWORK SET UP */
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  //--------RTC SETUP ------------
  Serial.print("[RTC] compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();

  // if you are using ESP-01 then uncomment the line below to reset the pins to
  // the available pins for SDA, SCL
  // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) 
  {
      // Common Cuases:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

}

float h;
float t;
float hic;
void loop() {
  
  // TODO figure out how to do this more rationally
  // RESET JSON
  StaticJsonBuffer<200> jsonBuffer; // increase the value of 200 if the json string gets larger
  JsonObject& json = jsonBuffer.createObject();
  
  Serial.println("[JSON] FIRST");
  json.prettyPrintTo(Serial);
  Serial.println();
  
  // Blink the LED to indicate it's taking a reading
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);

  // read the value from the sensor:
  Serial.println("[LIGHT] reading light");
  lightValue = analogRead(sensorPin);
  Serial.print("[LIGHT] light read ");
  Serial.println(lightValue);

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
  RtcTemperature temp_rtc = Rtc.GetTemperature();
  Serial.print("[TEMP] temp from RTC: ");
  Serial.println(temp_rtc.AsFloat());
  Serial.println("[TEMP] done temp");

  // TIME CODE:
  Serial.println("[TIME] starting time");
  if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and
        //    the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u-%02u-%02uT%02u:%02u:%02u"),
            now.Year(),
            now.Month(),
            now.Day(),
            now.Hour(),
            now.Minute(),
            now.Second() );
  Serial.print("[TIME] time is ");
  Serial.println(datestring);
`
  json["light"] = lightValue;
  json["humidity"] = h;
  json["temp_celcius"] = t;
  json["temp_rtc_celcius"] = temp_rtc.AsFloat();
  json["heat_index"] = hic;
  json["capture_dttm"] = datestring;

  Serial.println("[JSON] populated:");
  json.prettyPrintTo(Serial);
  Serial.println();

  char postable[1024];
  json.printTo(postable, sizeof(postable));

  // POST IT
  Serial.println("[NETWORK] posting json request");
  HTTPClient http;
  http.begin(persistHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(postable);
  http.writeToStream(&Serial);
  http.end();
  
  Serial.println();
  Serial.println("[DONE]---");
  delay(delayBetweenReadings);
}

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u-%02u-%02uT%02u:%02u:%02u"),
            dt.Year(),
            dt.Month(),
            dt.Day(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
