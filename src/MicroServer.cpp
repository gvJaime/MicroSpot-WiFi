#include "MicroServer.h"
#include "Mechanical.h"
#include <WiFiUdp.h>
#include <Ticker.h>

WiFiUDP serverWifi;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
String  successful = "Received! ";  // a reply string to send back
String  erroneous = "Error! ";

///////////////////////////////////////////////
// LED ticker and functions to make a Blink
//
///////////////////////////////////////////////

#define LEDPIN 14 //GPIO for the LED

Ticker ledBlink;

void ledFlick(){
  digitalWrite(LEDPIN,!digitalRead(LEDPIN));
}

/**
 * Server implementation
 */

MicroServer::MicroServer(Mechanical *m) {
  mechanical = m;
  mechanical->addObserver(this);
}

void MicroServer::setUp(String hostname) {

  //Set the hostname of the server
  WiFi.hostname(hostname);

  //Check of there has been a change in WiFi configuration.
  String station_ssid, station_psk;

    // Load wifi connection information.
  if (! fileManager.loadWifiConfig(&station_ssid, &station_psk)) {
    station_ssid = "";
    station_psk = "";
  }

  // Check WiFi connection
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  // ... Compare file config with sdk config.
  if (WiFi.SSID() != station_ssid || WiFi.psk() != station_psk) {

    WiFi.begin(station_ssid.c_str(), station_psk.c_str());

  }else{
    // ... Begin with sdk config.
    WiFi.begin();
  }
  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) { delay(500); }

  // Check connection
  if(WiFi.status() != WL_CONNECTED) {
    // Go into software AP mode.
    pinMode(LEDPIN,OUTPUT);
    digitalWrite(LEDPIN,LOW);
    delay(100);
    ledBlink.attach(1,ledFlick);

    WiFi.mode(WIFI_AP);

    delay(10);

    WiFi.softAP((const char *)hostname.c_str(), this->ap_default_psk);
  }

  /////////////////////
  // Server commands //
  /////////////////////
<<<<<<< HEAD

  /*
  serverWifi.on("/client", [this](){ handleWhomst();});
  serverWifi.on("/homeAxis", [this](){ handleHomeAxis();});
  serverWifi.on("/moveAxis", [this](){ handleMoveAxis();});
  serverWifi.on("/jogAxis", [this](){ handleJogAxis();});
  serverWifi.on("/stopJog", [this](){ handleStopJog();});
  serverWifi.on("/ayy/lmao", [this](){ handleAyyLmao();});
  serverWifi.on("/unlockAxis", [this](){handleUnlockAxis();});
  serverWifi.on("/toggle", [this](){handleToggle();});
  serverWifi.on("/getPos", [this](){handleGetPos();});
  serverWifi.on("/toggleLight",[this](){handleToggleLight();});
  */

  serverWifi.begin(localUdpPort);

  mechanical->toggle(true);
}

void MicroServer::run() {
  int packetSize = serverWifi.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    int len = serverWifi.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    
    // send back a reply, to the IP address and port we got the packet from
    serverWifi.beginPacket(serverWifi.remoteIP(), serverWifi.remotePort());
    if(strcmp(incomingPacket,"ayylmao") == 0){
      success("Ayy LMAO");
    }else{
      error((String)incomingPacket + " could not be parsed");
    }
    serverWifi.endPacket();
  }
  mechanical->run();
}


//////////////////////
// Server responses //
//                  //
//////////////////////

void MicroServer::update(String msg) { //serverWifi.send(200, "application/json", "Success: " + msg); 
  int len = successful.length() + msg.length() + 1;
  char buffer[len];
  (successful + msg).toCharArray(buffer,len);
  serverWifi.write(buffer);
}


/*
//////////////////////
// Command Handlers //
//                  //
//////////////////////

void MicroServer::handleWhomst() { update(serverWifi.client().remoteIP().toString()); }
void MicroServer::handleAyyLmao() { update("Ayy LMAO"); }
void MicroServer::handleUnlockAxis() {mechanical->unlockAxis();}
void MicroServer::handleHomeAxis() { 
  if(mechanical->getStatus() != MOVING) {
    mechanical->homeAxis(); 
  }else{
    update("BUSY");
  }
}
void MicroServer::handleStopJog() { mechanical->stopJog(); }
void MicroServer::handleGetPos() { mechanical->getPos(); }

void MicroServer::handleMoveAxis() {
  if (serverWifi.arg("x") != "" && serverWifi.arg("y") != "" && serverWifi.arg("f") != "") {
    mechanical->moveAxis((String)serverWifi.arg("x"), (String)serverWifi.arg("y"), (String)serverWifi.arg("f"));
  }else{ update("Error: One or more position arguments are missing!"); }
}

void MicroServer::handleJogAxis() {
  if (serverWifi.arg("x") != "" && serverWifi.arg("y") != "" && serverWifi.arg("f") != "") {
    mechanical->jogAxis((String)serverWifi.arg("x"), (String)serverWifi.arg("y"), (String)serverWifi.arg("f"), 
      (String)serverWifi.arg("r"), (String)serverWifi.arg("s"));
  }else{ update("Error: One or more position arguments are missing!"); }
}

void MicroServer::handleToggle() {
  if (serverWifi.arg("option") !=  "") {
    if(serverWifi.arg("option") == "true") mechanical->toggle(true);
    else if(serverWifi.arg("option") == "false") mechanical->toggle(false);
    else update("Error: Invalid 'option' value!");
  }else{ update("Error: No 'option' value provided!"); }
}

void MicroServer::handleToggleLight(){
  if (serverWifi.arg("l") != "") {
    mechanical->toggleLight(serverWifi.arg("l").toInt());
  }else{
    update("Error: No intensity value provided!");
  }
}
*/
