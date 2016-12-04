#ifndef LED_H__
#define LED_H__

#include <math.h>

#define LED_MODE_TALK   0x00        //Flash LED while talking
#define LED_MODE_BLINK  0x01        //Flash at a continous rate
#define LED_MODE_IO     0x02        //LED either on or off
#define LED_MODE_CNCT   0x03        //LED breathes while waiting for the app to connnect
#define LED_MODE_BATT   0x04        //LED flashes fast when battery is low
#define LED_TALK_RATE   170         //Rate to flash the LED when talking. Half the period in ms
#define LED_BATT_RATE   100          //Rate to flash the LED when low on battery. Half the period in ms
#define LED_CNCT_RATE   1000          //Rate to breathe the LED when connecting. Half the period in ms


class LED{
    private:
        int led_pin;
        int led_mode;
        int led_param = 0;
        int led_state = -1;
        int length = 0;
        int last_update = 0;
        int breathe_dir = 1;
        int breathe_pwm = 0;


    public:
        void init(int pin);
        void set_LED(int mode, int param);
        void talk(int len);
        void update();

};

#endif
