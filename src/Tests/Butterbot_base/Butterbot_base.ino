#define QIK_MOTOR_M0_FORWARD             0x88
#define QIK_MOTOR_M0_FORWARD_8_BIT       0x89
#define QIK_MOTOR_M0_REVERSE             0x8A
#define QIK_MOTOR_M0_REVERSE_8_BIT       0x8B
#define QIK_MOTOR_M1_FORWARD             0x8C
#define QIK_MOTOR_M1_FORWARD_8_BIT       0x8D
#define QIK_MOTOR_M1_REVERSE             0x8E
#define QIK_MOTOR_M1_REVERSE_8_BIT       0x8F
#define QIK_2S9V1_MOTOR_M0_COAST         0x86
#define QIK_2S9V1_MOTOR_M1_COAST         0x87

int speed = 0;

void setup() {
  Serial.begin(38400);
  Serial.write(0x82); //clear errors
  delay(5);
  Serial.setTimeout(100);

  set_params(1, 8);
}

void loop() {
  int raw = Serial.parseInt();
  if (raw != 0) {
    speed = raw;
  }

  if (speed == 1111) {
    set_motors(0, 0, true);
  }
  else {
    set_motors(speed, speed, false);
  }

}





void set_motors(int speed0, int speed1, boolean coast) {
  byte cmd0[2];
  byte cmd1[2];

  boolean m0_rev = speed0 / abs(speed0);
  boolean m1_rev = speed1 / abs(speed1);
  speed0 = (byte)min(abs(speed0), 255);
  speed1 = (byte)min(abs(speed1), 255);

  //motor0
  //coast instead of brake when speed is zero
  if (speed0 == 0 && coast) {
    Serial.write(QIK_2S9V1_MOTOR_M0_COAST);
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
    Serial.write(cmd0, 2);
  }


  //motor1
  //coast instead of brake when speed is zero
  if (speed1 == 0 && coast) {
    Serial.write(QIK_2S9V1_MOTOR_M1_COAST);
  }
  else {
    if (speed0 > 127) {
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
    Serial.write(cmd1, 2);
  }

}

void set_params(byte pwm_mode, int timeout) {
  /*PWM Modes
     0 = high-frequency, 7-bit mode (PWM frequency of 31.5 kHz, which is ultrasonic)
     1 = high-frequency, 8-bit mode (PWM frequency of 15.7 kHz)
     2 = low-frequency, 7-bit mode (PWM frequency of 7.8 kHz)
     3 = low-frequency, 8-bit mode (PWM frequency of 3.9 kHz)
  */
  if (pwm_mode >= 0 && pwm_mode <= 3) {
    byte cmd[5] = {0x84, 0x01, pwm_mode, 0x55, 0x2A};
    //Serial.write(cmd,5);
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
    //Serial.println(raw, BIN);
    byte time_cmd[5] = {0x84, 0x03, raw, 0x55, 0x2A};
    Serial.write(time_cmd, 5);
    delay(10);
    //enable shutdown errors
    byte shut_cmd[5] = {0x84, 0x02, 0x01, 0x55, 0x2A};
    Serial.write(shut_cmd, 5);
    delay(10);

  }
  else {
    //disable timout
    byte time_cmd[5] = {0x84, 0x03, 0x00, 0x55, 0x2A};
    Serial.write(time_cmd, 5);
    delay(5);
    //disable shutdown errors
    byte shut_cmd[5] = {0x84, 0x02, 0x00, 0x55, 0x2A};
    Serial.write(shut_cmd, 5);
    delay(5);

  }

}

