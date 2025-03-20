#include "config.h"

void setupSensors() {
  tempSensor.begin();
  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void readSensors() {
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);
  lightLevel = analogRead(LIGHT_SENSOR_PIN);

  Serial.println("lightLevel"+lightLevel);

  // Détection incendie
  fireDetected = (temperature > seuilHaut + 5.0 && lightLevel > seuilLumiere);
}
