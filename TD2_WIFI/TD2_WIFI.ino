#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFi.h>

// Définition des broches
#define TEMPERATURE_SENSOR_PIN 23
#define CLIMATISATION_PIN 19
#define FAN_PIN 27
#define RADIATEUR_PIN 21
#define LED_STRIP_PIN 13
#define FIRE_LED_PIN 2
#define LIGHT_SENSOR_PIN 33

// PWM pour ventilateur
#define PWM_FREQ 25000
#define PWM_RESOLUTION 8

// Nombre de LEDs
#define NUMLEDS 5

// Seuils
float seuilHaut = 25.9;
float seuilBas = 25.0;
int seuilLumiere = 400;

// Capteur de température
OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

// Bande LED
Adafruit_NeoPixel strip(NUMLEDS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Variables système
float temperature = 0;  // Valeur par défaut
int lightLevel = 0;         // Valeur par défaut
bool fireDetected = false;
int fanSpeed = 0;          // Valeur par défaut

void setup() {
  Serial.begin(9600);
  tempSensor.begin();
  strip.begin();
  strip.show();

  pinMode(CLIMATISATION_PIN, OUTPUT);
  pinMode(RADIATEUR_PIN, OUTPUT);
  pinMode(FIRE_LED_PIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION);
  
  WiFi.begin("Livebox-B870", "password"); // Remplace par ton mot de passe
  
  Serial.println("✅ Système démarré...");
}

void loop() {
  // Lire la température et la lumière
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);
  lightLevel = analogRead(LIGHT_SENSOR_PIN);

  // Détection incendie
  fireDetected = (temperature > seuilHaut + 5.0 && lightLevel > seuilLumiere);

  // Contrôle des équipements
  String regulState = "HALT";
  String heatState = "OFF";
  String coldState = "OFF";

  if (fireDetected) {
    digitalWrite(FIRE_LED_PIN, HIGH);
    setLEDColor(strip.Color(255, 0, 0)); // Rouge
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
      setLEDColor(strip.Color(255, 0,0)); // Rouge
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
      setLEDColor(strip.Color(0, 255,0)); // Vert
    }
  }

  // Génération du JSON
  StaticJsonDocument<512> doc;
  doc["status"]["temperature"] = temperature;
  doc["status"]["light"] = lightLevel;
  doc["status"]["regul"] = regulState;
  doc["status"]["fire"] = fireDetected;
  doc["status"]["heat"] = heatState;
  doc["status"]["cold"] = coldState;
  doc["status"]["fanspeed"] = fanSpeed;

  doc["location"]["room"] = "342";
  doc["location"]["gps"]["lat"] = 43.62453842;
  doc["location"]["gps"]["lon"] = 7.050628185;
  doc["location"]["address"] = "Les lucioles";

  doc["regul"]["lt"] = seuilBas;
  doc["regul"]["ht"] = seuilHaut;

  doc["info"]["ident"] = "ESP32 123";
  doc["info"]["user"] = "SBD";
  doc["info"]["loc"] = "A Biot";

  doc["net"]["uptime"] = "55";
  doc["net"]["ssid"] = "FreeBox";
  doc["net"]["mac"] = "AC:67:B2:37:C9:48";
  doc["net"]["ip"] = "192.0.0.1";

  doc["reporthost"]["target_ip"] = "127.0.0.1";
  doc["reporthost"]["target_port"] = 1880;
  doc["reporthost"]["sp"] = 2;

  char json[512];
  serializeJson(doc, json);
  Serial.println(json);

  delay(10000);
}

// Fonction pour allumer la bande LED
void setLEDColor(uint32_t color) {
  for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}