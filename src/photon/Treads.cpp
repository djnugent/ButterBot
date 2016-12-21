#include "Treads.h"



void Treads::attach(int baud, int min_spd, int max_spd){
  //Uses Serial1 TX. RX is used by OpenMV
    Serial1.begin(baud);
    Serial1.write(0xAA);
    Serial1.write(0x82); //clear errors

    min_speed = min_spd;
    max_speed = max_spd;

}

void Treads::set_motors(int spd0, int spd1){
    target_speed0 = spd0 * -1;
    target_speed1 = spd1 * -1;
}


void Treads::update(){
    bool cst0 = true;
    bool cst1 = true;

    int thres = int(MAX_ACCEL * 1.0/TREADS_UPDATE_RATE);

    //motor 0
    if(target_speed0 <= max_speed && target_speed0 >= min_speed){
        int diff = target_speed0 - current_speed0;
        current_speed0 += min(max(diff,-thres),thres);
    }
    if(current_speed0 == 0 && target_speed0 ==0){
        cst0 = false;
    }

    //motor 1
    if(target_speed1 <= max_speed && target_speed1 >= min_speed){
        int diff = target_speed1 - current_speed1;
        current_speed1 += min(max(diff,-thres),thres);
    }
    if(current_speed1 == 0 && target_speed1 ==0){
        cst1 = false;
    }

    write_motors(current_speed0, current_speed1, cst0, cst1);
    Serial1.flush();
}

void Treads::write_motors(int spd0, int spd1, bool cst0, bool cst1) {

  byte cmd0[2];
  byte cmd1[2];
  bool m0_rev = false;
  bool m1_rev = false;

  //Select track direction
  if(spd0<0){
    m0_rev =  true;
  }
  if(spd1<0){
    m1_rev = true;
  }

  //limit speed
  byte speed0 = (byte)min(abs(spd0), 255);
  byte speed1 = (byte)min(abs(spd1), 255);

  //motor0
  //coast instead of brake when speed is zero
  if (speed0 == 0 && cst0) {
    Serial1.write(QIK_2S9V1_MOTOR_M0_COAST);
  }
  else {
    if (speed0 > 127) {
      //if driver is configured for 8 bit mode this will accomedate for that
      //if it is not configured for 8 bit it will still work in 7-bit mode
      //8-bit mode: actual speed is (speed + 128)
      cmd0[0] = m0_rev ? QIK_MOTOR_M0_REVERSE_8_BIT : QIK_MOTOR_M0_FORWARD_8_BIT;
      cmd0[1] = speed0 - 128;
    }
    else {
      cmd0[0] = m0_rev ? QIK_MOTOR_M0_REVERSE : QIK_MOTOR_M0_FORWARD;
      cmd0[1] = speed0;
    }
    Serial1.write(cmd0, 2);
  }

  //motor1
  //coast instead of brake when speed is zero
  if (speed1 == 0 && cst1) {
    Serial1.write(QIK_2S9V1_MOTOR_M1_COAST);
  }
  else {
    if (speed1 > 127) {
      //if driver is configured for 8 bit mode this will accomedate for that
      //if it is not configured for 8 bit it will still work in 7-bit mode
      //8-bit mode: actual speed is (speed + 128)
      cmd1[0] = m1_rev ? QIK_MOTOR_M1_REVERSE_8_BIT : QIK_MOTOR_M1_FORWARD_8_BIT;
      cmd1[1] = speed1 - 128;
    }
    else {
      cmd1[0] = m1_rev ? QIK_MOTOR_M1_REVERSE : QIK_MOTOR_M1_FORWARD;
      cmd1[1] = speed1;
    }
    Serial1.write(cmd1, 2);
  }

}

void Treads::set_params(byte pwm_mode, int timeout) {
  /*PWM Modes
     0 = high-frequency, 7-bit mode (PWM frequency of 31.5 kHz, which is ultrasonic)
     1 = high-frequency, 8-bit mode (PWM frequency of 15.7 kHz)
     2 = low-frequency, 7-bit mode (PWM frequency of 7.8 kHz)
     3 = low-frequency, 8-bit mode (PWM frequency of 3.9 kHz)
  */
  if (pwm_mode >= 0 && pwm_mode <= 3) {
    byte cmd[5] = {0x84, 0x01, pwm_mode, 0x55, 0x2A};
    Serial1.write(cmd,5);
  }

  /*Timeout
     timeout is specified in 0.262 sec intervals
     timeout = 0.262 * {lower 4 bits} * 2 ^ {upper 3 bits}
     Hard coded upper 3 bits to 0b000 which provides a range of 0 - 8.122 sec with 0.262 intervals
     timeout of 0 disables the timeout
     timeout is only enabled if the "shutdown motors on error" is enabled
  */
  if (timeout > 0) {
    //set timeout
    byte raw = (byte)ceil(timeout / 0.262) & 0b00001111;
    byte time_cmd[5] = {0x84, 0x03, raw, 0x55, 0x2A};
    Serial1.write(time_cmd, 5);
    delay(10);
    //enable shutdown errors
    byte shut_cmd[5] = {0x84, 0x02, 0x01, 0x55, 0x2A};
    Serial1.write(shut_cmd, 5);
    delay(10);

  }
  else {
    //disable timout
    byte time_cmd[5] = {0x84, 0x03, 0x00, 0x55, 0x2A};
    Serial1.write(time_cmd, 5);
    delay(5);
    //disable shutdown errors
    byte shut_cmd[5] = {0x84, 0x02, 0x00, 0x55, 0x2A};
    Serial1.write(shut_cmd, 5);
    delay(5);

  }

}
