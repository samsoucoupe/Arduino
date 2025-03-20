#include "config.h"

void setupSensors() {
  tempSensor.begin();
  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void readSensors() {
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);
  lightLevel = 4095 - analogRead(LIGHT_SENSOR_PIN);

  // Détection incendie
  fireDetected = (temperature > seuilHaut + 5.0 && lightLevel > seuilLumiere);
}
