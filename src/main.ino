///////////////////////////////////////////////////////////////////
//
// MicroSpot WiFi firmware for esp8266.
//
// @Authors: Jaime Garcia <garciavillena.jaime@gmail.com>
//           Alexander Bakardyev <alexander.vladimirov3@gmail.com>
//
//
//
//
// Based on Pascal Gollor OTA-mDNS-SPIFFS example
//
///////////////////////////////////////////////////////////////////

#include <ArduinoOTA.h>

#include "MicroServer.h"
#include "Mechanical.h"

#define HOSTNAME "MicroSpot-" //Hostname and AP name root.
#define LEDPIN 5

///////////////////////////////////////////////
// Server declaration.
//
///////////////////////////////////////////////

#ifdef DEBUG_ESP_PORT
  Mechanical mechanical(9600);
#else
  Mechanical mechanical(115200);
#endif
MicroServer microServer(&mechanical);



///////////////////////////////////////////////
// Arduino Setup
//
///////////////////////////////////////////////
void setup() { 

  //begin execution with a blink
  for(int i = 0; i < 7; i++){
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
    delay(500);
  }




  delay(2000);
  mechanical.initDrivers();
  delay(1000);
  mechanical.toggle(true);
  
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);

  microServer.setup(hostname);

  // Start OTA server.
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();
}

///////////////////////////////////////////////
// Arduino Loop
//
///////////////////////////////////////////////
void loop() {
  mechanical.run();
  microServer.run();
  yield();
  // Handle OTA server.
  ArduinoOTA.handle();
  yield();
}
