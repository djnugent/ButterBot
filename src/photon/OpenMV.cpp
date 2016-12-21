#include <Particle.h>
#include "OpenMV.h"


void OpenMV::update(TCPServer *serv){
  //Uses Serial1 RX. TX is used by Treads. Make sure treads are attached
  //read serial buffer
  if(Serial1.available()>= MV_PACKET_SIZE){
    while(Serial1.read() != START_FRAME);
    x = Serial1.read();
    y = Serial1.read();
    size = Serial1.read();
    fps = Serial1.read();
  }

  //send tracking data to app
  if(tracking && millis()-last_tracking_update > TRACKING_RATE){
    byte cmd[PACKET_SIZE] = {OPENMV_POS,0,0,0,0,'\n'};
    cmd[1] = (x & 0xff00) >> 8; //msb
    cmd[2] = x & 0x00ff; //lsb
    cmd[3] = (y & 0xff00) >> 8; //msb
    cmd[4] = y & 0x00ff; //lsb
    serv->write(cmd,PACKET_SIZE);
    last_tracking_update = millis();
  }
}

void OpenMV::enable_tracking(){
  tracking = true;
  last_tracking_update = 0;
  x = 0;
  y = 0;
  size = 0;
  fps = 0;
}

void OpenMV::disable_tracking(){
  tracking = false;
}
