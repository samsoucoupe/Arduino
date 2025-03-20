#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "json.h"

void setup() {
  Serial.begin(9600);
  setupSensors();
  setupActuators();
  Serial.println("✅ Système démarré...");
}

void loop() {
  readSensors();
  controlSystem();
  sendJSONReport();
  delay(10000);
}
