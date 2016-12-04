#include "LED.h"
#include <Particle.h>

void LED::init(int pin){
    led_mode = LED_MODE_CNCT;

    led_pin = pin;
    pinMode(led_pin,OUTPUT);
    digitalWrite(led_pin, LOW);
}

void LED::set_LED(int mode, int param){
    led_mode = mode;
    led_param = param;
}

void LED::talk(int len){
    length = len;
}

void LED::update(){
    switch(led_mode){
        case LED_MODE_TALK:{
            //blink led while audio is playing
            if(length > 0 && millis() - last_update > LED_TALK_RATE){
                led_state = !led_state;
                digitalWrite(led_pin,led_state);
                last_update = millis();
                length--;
            }
            if(length == 0){
                led_state = 0;
                digitalWrite(led_pin,led_state);
            }
        break;}
        case LED_MODE_BLINK:{
            //blink led continuously
            if(millis() - last_update > led_param){
                    led_state = !led_state;
                    digitalWrite(led_pin,led_state);
                    last_update = millis();
            }
        break;}
        case LED_MODE_IO:{
            //On or off
            if(led_state != led_param){
                led_state = led_param;
                digitalWrite(led_pin,led_state);
            }
        break;}
        case LED_MODE_CNCT:{
            float top = 2700.0;
            float bottom = 1750.0;
            //breathe LED
            int step = int((millis() -last_update) * (top-bottom) /LED_CNCT_RATE);
            if(step > 0){
                //increase brightness
                if(breathe_dir == 1){
                    breathe_pwm = min(top, breathe_pwm + step);
                    analogWrite(led_pin,breathe_pwm);
                    if(breathe_pwm == top){
                        breathe_dir = -1;
                    }
                }
                //decrease brightness
                else{
                    breathe_pwm = max(bottom, breathe_pwm - step);
                    analogWrite(led_pin,breathe_pwm);
                    if(breathe_pwm == bottom){
                        breathe_dir = 1;
                    }
                }
                last_update = millis();
            }
            led_state = -1; //led state is no longer known
        break;}
        case LED_MODE_BATT:{
            //blink led fast
            if(millis() - last_update > LED_BATT_RATE){
                    led_state = !led_state;
                    digitalWrite(led_pin,led_state);
                    last_update = millis();
            }
        break;}

    }
}
