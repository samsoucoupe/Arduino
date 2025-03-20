#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFi.h>

// 🛠️ Broches des capteurs et actionneurs
#define TEMPERATURE_SENSOR_PIN 23
#define CLIMATISATION_PIN 19
#define FAN_PIN 27
#define RADIATEUR_PIN 21
#define LED_STRIP_PIN 13
#define FIRE_LED_PIN 2
#define LIGHT_SENSOR_PIN 33

// ⚙️ PWM pour le ventilateur
#define PWM_FREQ 25000
#define PWM_RESOLUTION 8

// 🌈 Nombre de LEDs sur la bande
#define NUMLEDS 5

// 🔥 Seuils de température et de lumière
float seuilHaut = 24;
float seuilBas = 23;
int seuilLumiere = 400;

// 🌡️ Capteur de température (OneWire + DallasTemperature)
OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

// 💡 Bande LED
Adafruit_NeoPixel strip(NUMLEDS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// 📊 Variables système
float temperature = 0;  
int lightLevel = 0;     
bool fireDetected = false;
int fanSpeed = 0;      

#endif
