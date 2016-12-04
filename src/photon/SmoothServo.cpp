#include <Particle.h>
#include "SmoothServo.h"


void SmoothServo::attach(int pin, int start_us){
  if(!attached){
      ser.attach(pin);
      ser.writeMicroseconds(start_us);
      current_us = start_us;
      target_us = start_us;
      attached = true;
  }
}

void SmoothServo::detach(){
    ser.detach();
    attached = false;
}

void SmoothServo::set_max_us_per_sec(int mups){
  max_us_per_sec = mups;
}

void SmoothServo::set_limits(int mi, int ma){
  min_us = mi;
  max_us = ma;
}

void SmoothServo::set_us(int us){
    target_us = us;
}

void SmoothServo::update(){
    int thres = max_us_per_sec * 1.0/SERVO_UPDATE_RATE;
    if(target_us <= max_us && target_us >= min_us){
        int diff = target_us - current_us;
        current_us += min(max(diff,-thres),thres);
    }
    ser.writeMicroseconds(current_us);
}
