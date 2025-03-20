#include "config.h"

void setupActuators() {
  pinMode(CLIMATISATION_PIN, OUTPUT);
  pinMode(RADIATEUR_PIN, OUTPUT);
  pinMode(FIRE_LED_PIN, OUTPUT);
  strip.begin();
  strip.show();
  ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION);
}

void setLEDColor(uint32_t color) {
  for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void controlSystem() {


  if (fireDetected) {
    digitalWrite(FIRE_LED_PIN, HIGH);
    setLEDColor(strip.Color(255, 0, 0));  // Rouge
  } else {
    digitalWrite(FIRE_LED_PIN, LOW);
    if (temperature > seuilHaut) {
      digitalWrite(CLIMATISATION_PIN, HIGH);
      digitalWrite(RADIATEUR_PIN, LOW);
      fanSpeed = map(temperature, seuilHaut, seuilHaut + 10, 50, 255);
      fanSpeed = constrain(fanSpeed, 50, 255);
      ledcWrite(FAN_PIN, fanSpeed);
      regulState = "COOL";
      coldState = "ON";
      setLEDColor(strip.Color(255, 0, 0)); // Rouge
    } else if (temperature < seuilBas) {
      digitalWrite(RADIATEUR_PIN, HIGH);
      digitalWrite(CLIMATISATION_PIN, LOW);
      ledcWrite(FAN_PIN, 0);
      regulState = "HEAT";
      heatState = "ON";
      setLEDColor(strip.Color(0, 255, 255)); // Cyan
    } else {
      digitalWrite(CLIMATISATION_PIN, LOW);
      digitalWrite(RADIATEUR_PIN, LOW);
      ledcWrite(FAN_PIN, 0);
      fanSpeed = 0;
      regulState = "HALT";
      setLEDColor(strip.Color(0, 255, 0)); // Vert
    }
  }
}
