/*
 * Using a BMP280 device over the i2c bus
 * NOTE: on the ESP8266, SDA is D2 and SCL is D1
 * (that took me way too long to figure out
 */
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bme; // I2C
void setupBarometer() {
  Serial.println(F("BMP280 test"));
  if (!bme.begin((0x76)) ) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }
}

int getBarometerReading() {
  Serial.println("[Barometer] reading sensors");
    
    
    Serial.print("[Barometer] Pressure = ");
    Serial.print(bme.readPressure());
    Serial.println(" Pa");

    Serial.print("[Barometer] Approx altitude = ");
    Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcast
    Serial.println(" m");
    
    Serial.println();
}

long getPressure() {
  long pressure = bme.readPressure();
  Serial.print("[Barometer] Pressure = ");
  Serial.print(pressure);
  Serial.println(" Pa");
  return pressure;
}

double getBarometerTemperature() {
  double temp = bme.readTemperature();
  Serial.print("[Barometer] Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  return temp;
}

