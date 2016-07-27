#include <Servo.h>

int current_pwm = 1250;
int target_pwm = 1250;
int max_us_per_sec = 100;
int last_servo_update = 0;
Servo neck;


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
  neck.attach(13);
  neck.writeMicroseconds(current_pwm);
  // put your setup code here, to run once:

}

void loop() {
  target_pwm = Serial.parseInt();
  update_servos();
}


void update_servos(){
  if(millis() - last_servo > 20){ 
   if(target_pwm <= 2000 && target_pwm >= 1000){
    int thres = max_us_per_sec * (millis()-last_servo_update) / 1000;
    while(current_pwm != target_pwm){
      int pwm;
      Serial.println(thres);
      if(abs(target_pwm-current_pwm) > thres){
        int diff = target_pwm - current_pwm;
        pwm = current_pwm + min(max(diff,-thres),thres);
      }
      else{
        pwm = target_pwm;
      }
      Serial.println(pwm);
      neck.writeMicroseconds(pwm);
      current_pwm = pwm;
      last_servo_update = millis();
    }
  }
 }
}


