#ifndef TREADS_H__
#define TREADS_H__

#include <Particle.h>
#include <math.h>

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

#define TREADS_UPDATE_RATE 100
#define MAX_ACCEL 100

class Treads{
    private:
        int current_speed0 = 0;
        int target_speed0 = 0;
        int current_speed1 = 0;
        int target_speed1 = 0;
        int min_speed = 255;
        int max_speed = -255;
        void write_motors(int spd0, int spd1, bool cst0, bool cst1);

    public:
        void attach(int baud, int min_spd, int max_spd);
        void update();
        void set_motors(int speed0, int speed1);
        void set_params(byte pwm_mode, int timeout);

};


#endif
