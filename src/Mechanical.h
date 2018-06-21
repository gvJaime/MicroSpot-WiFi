#ifndef MECHANICAL_H
#define MECHANICAL_H


#include "Arduino.h"
#include "MechTypes.h"
#include <WiFiClient.h>

#define MAX_X "50"
#define MAX_Y "15"

/////////////////////////////////////////
// TMC2130 Configuration
/////////////////////////////////////////

#define TMC_CSX 2 //TO BE CONFIGURED
#define TMC_CSY 4 //TO BE CONFIGURED

#define TMC_MAX_CURRENT 300 //mA
#define TMC_MICROSTEP 64 



class MicroServer;

class Mechanical {

  private:
    MicroServer * microServer; //ADDED - for the observation pattern
    int baudios;
    Status st = OFF;
    Position pos;
    Position maxpos;
    
    struct AfterStatus{
      Status success, failure;
    }after;
    

    //Asks GRBL its position with "?".
    bool askPos();
    //Safely send the command in the buffer, and expect a response or not.
    bool sendCommand(Status atLeast, Status success, Status failure);
    //flush input serial stream
    void flush();
    //Change the status
    void setStatus(Status stat);
    //loop for checking the serial
    void serialListen();
    //Restart serial communication.
    void restartAll();
    //Analyze the error and act in consequence.
    void errorHandler(int errNum);
    //Analyze the alarm and act in consequence.
    void alarmHandler(int alarmNum);

    String statusToString(Status status) {
        switch (status) {
            case OFF: return "{\"msg\":\"OFF\"}";
            case OFFLINE: return "{\"msg\":\"OFFLINE\"}";
            case ERROR: return "{\"msg\":\"ERROR\"}";
            case LOCK: return "{\"msg\":\"LOCK\"}";
            case HOMING: return "{\"msg\":\"HOMING\"}";
            case MOVING: return "{\"msg\":\"MOVING\"}";
            //TODO update. This was set for compatibility purposes
            case JOGGING: return "{\"msg\":\"JOGGING\",\"pos\":{\"x\":" + pos.x + ",\"y\":" + pos.y + "}}";
            case IDLE: return "{\"msg\":\"IDLE\",\"pos\":{\"x\":" + pos.x + ",\"y\":" + pos.y + "}}";
            default: return "{\"msg\":\"BAD STATUS\"}";
        }
    }

  public:
    //Instantiation
    Mechanical(int baud); //Instantiate the object and choose baudrate.

    //when true, the mechanical stage is making a movement that may
    //take a while to confirm. (homing and moves)
    bool longWait;

    //Serial activation and release
    bool toggle(bool state); //Turn on or off the serial interface
    
    //Reset GRBL
    bool reset();

    //Movement
    bool homeAxis(); //Take axis to home position.
    bool moveAxis(char * request, int x, int y, int f); //Ininterruptible move to (X,Y) at speed F.
    bool jogAxis(char * request, int x, int y, int f, int r, int s); //Interruptible move to (X,Y) at speed F.
    bool panAxis(char * request, int x, int y, int f); //fast jogging for panning.
    bool uniJog(char * request , int c, int f); //jog in a cartesian direction.
    bool stopJog();  //Stop an interruptible movement.
    bool unlockAxis(); //Send and unlock token to GRBL. Breaks stability. Devs only.
    bool toggleLight(char * request, int l); //turn on or off the lights.

    //Status reporting
    bool getPos(WiFiClient client); //Reports current position.
    String getStatus(); //Returns a number corresponding the status.

    //TMC2130 SPI Comm
    bool initDrivers();

    void addObserver(MicroServer * ms); //ADDED - for the observation pattern
    void run(); //loop function to access scheduling features.
};

#endif //MECHANICAL_H
