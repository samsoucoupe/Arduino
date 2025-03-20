#include "config.h"
#include "json.h"

void sendJSONReport() {
  StaticJsonDocument<512> doc;

  doc["status"]["temperature"] = temperature;
  doc["status"]["light"] = lightLevel;
  doc["status"]["fire"] = fireDetected;
  doc["status"]["fanspeed"] = fanSpeed;

  // Informations de localisation
  doc["location"]["room"] = "342";
  doc["location"]["gps"]["lat"] = 43.62453842;
  doc["location"]["gps"]["lon"] = 7.050628185;
  doc["location"]["address"] = "Les lucioles";

  // Informations système
  doc["info"]["ident"] = "ESP32 123";
  doc["info"]["user"] = "SBD";
  doc["info"]["loc"] = "A Biot";

  // Informations réseau
  doc["net"]["ssid"] = "OfflineMode";         
  doc["net"]["ip"] = "192.168.1.100";         
  doc["net"]["mac"] = "AC:67:B2:37:C9:48";    

  doc["reporthost"]["target_ip"] = "127.0.0.1";
  doc["reporthost"]["target_port"] = 1880;

  // Conversion du JSON en chaîne et affichage
  char json[512];
  serializeJson(doc, json);
  Serial.println(json);
}

//Fonction pour parse le json entrant 
void parseJsonCommand() {
  if (Serial.available()) {
    String jsonString = Serial.readStringUntil('\n');
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
      Serial.print("Erreur de parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    if (doc.containsKey("ht")) {
      seuilHaut = doc["ht"];
      Serial.print("Nouveau seuilHaut: ");
      Serial.println(seuilHaut);
    }

    if (doc.containsKey("lt")) {
      seuilBas = doc["lt"];
      Serial.print("Nouveau seuilBas: ");
      Serial.println(seuilBas);
    }
  }
}
