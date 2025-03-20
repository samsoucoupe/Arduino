#include "wifi_utils.h"
#include "ota.h"
#include <uptime.h>
#include <RemoteDebug.h>

#define USE_SERIAL Serial

RemoteDebug Debug;

void setup() {
  /* Serial connection -----------------------*/
  Serial.begin(9600);
  while(!Serial); //wait for a serial connection  

  /* WiFi connection  -----------------------*/
  String hostname = "Mon petit objet ESP32";
  wifi_connect_multi(hostname);               

  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED){
    USE_SERIAL.print("\nWiFi connected : yes ! \n"); 
    wifi_printstatus(0);  
  } 
  else {
    USE_SERIAL.print("\nWiFi connected : no ! \n"); 
    //  ESP.restart();
  }

  /* init remote debug  -------------------*/
  Debug.begin("GMESP32");  
  
  /* Install OTA --------------------------*/
  setupOTA("GM_ESP");

  /* your init code  ? --------------------*/
}

String get_uptime() {
  /* Return a String of the uptime of the esp */
  String UPTIME;
  uptime::calculateUptime();
  UPTIME = String(uptime::getMinutes()) +"min and " + String(uptime::getSeconds()) + "sec";
  return UPTIME;
}

/* Delay can be non compatible with OTA handle !!!! 
 *  because ESP32 pauses your program during the delay(). 
 *  If next OTA request is generated while Arduino is paused waiting for 
 *  the delay() to pass, your program will miss that request.
 *   => Use a non-blocking method, using mills() and a delta to decide to read or not. eg */
void DoSmtg (int delay){ /* Do stuff here every delay */
  static uint32_t tick = 0;
  if ( millis() - tick < delay)
    return; /* Do nothing */
  else{     
    USE_SERIAL.println("delay elapsed => we can process !");
    
    /* your loop code here ? --------------*/
    Debug.printf("Loop : %s\n", get_uptime().c_str());
  }
  tick = millis();
}

void loop() {
   ArduinoOTA.handle(); // gestion OTA => demande ?
   Debug.handle();
   
   DoSmtg(10000);
}
