/*
 * Read values from MQ-X sensors
 * Assign MQ analog sensor to appropriate analog in connector and then overwrite
 * sensor's value on each sensor read.
 * 
 * https://playground.arduino.cc/Main/MQGasSensors
 * http://www.winsen-sensor.com/d/files/semiconductor/mq135.pdf
 * MQ-135 - NH3, Sulfide and benzene steam, hydrogen gas, smoke
 * http://elecfreaks.com/store/download/datasheet/Brick/MQ5.pdf
 * MQ-5 - LPG, LNG, town gas, iso-butane, propane, H2
 * https://www.sparkfun.com/datasheets/Sensors/Biometric/MQ-6.pdf
 * MQ-6 - LPG, iso-butane, propane, LNG
 * http://particle-sensor.com/immagini/MQ-9.pdf
 * MQ-9 - CO, CH4, LPG
 * 
 * Uses an ADS1015 (12 bit ADC) to convert analog inputs to digital
 * over the I2C bus
 */

#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1015 ads;

int16_t mq135Value, mq5Value, mq6Value, mq9Value;

void setupGasSensors(void) {
  ads.begin();
}

int16_t MQ135Reading() {
  Serial.println("[MQ135] reading sensor");
  mq135Value = ads.readADC_SingleEnded(0);
  Serial.print("[MQ135] value: ");
  Serial.println(mq135Value);
  return mq135Value;
}

int16_t MQ5Reading() {
  Serial.println("[MQ5] reading sensor");
  mq5Value = ads.readADC_SingleEnded(1);
  Serial.print("[MQ5] value: ");
  Serial.println(mq5Value);
  return mq5Value;
}

int16_t MQ6Reading() {
  Serial.println("[MQ6] reading sensor");
  mq6Value = ads.readADC_SingleEnded(2);
  Serial.print("[MQ6] value: ");
  Serial.println(mq6Value);
  return mq6Value;
}

int16_t MQ9Reading() {
  Serial.println("[MQ9] reading sensor");
  mq9Value = ads.readADC_SingleEnded(3);
  Serial.print("[MQ9] value: ");
  Serial.println(mq9Value);
  return mq9Value;
}
