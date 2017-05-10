#include "MicroServer.h"
#include "Mechanical.h"
#include <WiFiUdp.h>
#include <Ticker.h>
using namespace std;

#define LOCALUDPPORT 4210

WiFiUDP serverWifi;
unsigned int localUdpPort = LOCALUDPPORT;  // local port to listen on

#define COMMANDTOKEN "/"

char incomingPacket[255];  // buffer for incoming packets
String stringCommand; //String for parsing the commands

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
  mechanical->run();
  int packetSize = serverWifi.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    int len = serverWifi.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    
    stringCommand = (String) incomingPacket;
    
    //crop command name
    String cropCommand = stringCommand.substring(0,stringCommand.indexOf(COMMANDTOKEN));
    
    //After isolating the command, pass control to the handler.
    if(cropCommand.equals("stopJog")){
      handleStopJog();
    }else if(cropCommand.equals("ayylmao")){
      handleAyyLmao();
    }else if(cropCommand.equals("client")){
      handleWhomst();
    }else if(cropCommand.equals("jogAxis")){
      handleJogAxis();
    }else if(cropCommand.equals("moveAxis")){
      handleMoveAxis();
    }else if(cropCommand.equals("unlockAxis")){
      handleUnlockAxis();
    }else if(cropCommand.equals("toggle")){
      handleToggle();
    }else if(cropCommand.equals("homeAxis")){
      handleHomeAxis();
    }else if(cropCommand.equals("getPos")){
      handleGetPos();
    }else{
      update(stringCommand + " could not be parsed");
    }
  }
}


//////////////////////
// Server responses //
//                  //
//////////////////////


void MicroServer::update(String msg) { //serverWifi.send(200, "application/json", "Success: " + msg); 
  serverWifi.beginPacket(serverWifi.remoteIP(), serverWifi.remotePort());
  int len = successful.length() + msg.length() + 1;
  char buffer[len];
  msg.toCharArray(buffer,len);
  serverWifi.write(buffer);
  serverWifi.endPacket();
}



//////////////////////
// Command Handlers //
//                  //
//////////////////////

void MicroServer::handleWhomst() { update(serverWifi.remoteIP().toString()); }
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
  int x,y,f;
  x = stringCommand.indexOf("x=");
  y = stringCommand.indexOf("y=");
  f = stringCommand.indexOf("f=");
  if (x > 0 && y > 0 && f > 0) { 
    mechanical->moveAxis(stringCommand.substring(x+2, y), 
      stringCommand.substring(y+2, f),
      stringCommand.substring(f+2));
  }else{ update("Error: One or more position arguments are missing!"); }
}

void MicroServer::handleJogAxis() { 
  int x,y,f,s, r;
  x = stringCommand.indexOf("x=");
  y = stringCommand.indexOf("y=");
  f = stringCommand.indexOf("f=");
  r = stringCommand.indexOf("r=");
  s = stringCommand.indexOf("s=");
  if (x > 0 && y > 0 && f > 0) { 
    mechanical->jogAxis(stringCommand.substring(x+2, y), 
      stringCommand.substring(y+2, f),
      stringCommand.substring(f+2,s),
      stringCommand.substring(r+2,s),
      stringCommand.substring(s+2));
  }else{ update("Error: One or more position arguments are missing!"); }
}

void MicroServer::handleToggle() {
    if(stringCommand.indexOf("true") > 0) mechanical->toggle(true);
    else if(stringCommand.indexOf("false") > 0) mechanical->toggle(false);
    else update("Error: Invalid 'option' value!");
}

void MicroServer::handleToggleLight(){
  int l;
  l = stringCommand.indexOf("l=");
  if (l >= 0){
    mechanical->toggleLight(stringCommand.substring(l+2).toInt());
  }else{
    update("Error: No intensity value provided!");
  }
}
