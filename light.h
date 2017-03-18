int getLightReading() {
  int lightReading;
  int sensorPin = A0;
  
  Serial.println("[LIGHT] reading light");
  lightReading = analogRead(sensorPin);
  Serial.print("[LIGHT] light read ");
  Serial.println(lightReading);
  return lightReading;
}

