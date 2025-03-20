/*** Basic/Static Wifi connection
     Fichier wificonnect_simple/wificonnect_simple.ino ***/
#include <WiFi.h> // https://www.arduino.cc/en/Reference/WiFi
#include "wifi_utils.h"
#define USE_SERIAL Serial
/*------ Arduino IDE paradigm : setup+loop -----------*/
void setup(){
  Serial.begin(9600); /* Serial connection -----------*/
  while(!Serial); //wait for a serial connection  

  /* WiFi   -------------------------------*/
  String hostname = "Mon petit objet de samsoucoupe";

  /* Connection from a list of SSID */
  wificonnect_multi(hostname);               
  
  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED){
    USE_SERIAL.print("\nWiFi connected : yes ! \n"); 
    wifi_printstatus(0);  
  } 
  else {
    USE_SERIAL.print("\nWiFi connected : no ! \n"); 
    //  ESP.restart();
  }
}

void loop(){
  // no code
  
  // WiFi.disconnect(); // at the end !
}
