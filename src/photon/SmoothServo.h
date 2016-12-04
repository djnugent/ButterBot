#ifndef SMOOTHSERVO_H__
#define SMOOTHSERVO_H__


#define SERVO_UPDATE_RATE 100
#include <Particle.h>


class SmoothServo {
    private:
        Servo ser;      //servo object
        int max_us_per_sec = 400;
        int min_us = 1000;
        int max_us = 2000;
        int current_us = 1250;
        int target_us = 1250;
        bool attached = false;


    public:
        void attach(int pin, int start_us);
        void detach();
        void set_max_us_per_sec(int mups);
        void set_limits(int mi, int ma);
        void set_us(int us);
        void update();
};
#endif
