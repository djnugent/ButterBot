#ifndef OPENMV_H__
#define OPENMV_H__

#include <Particle.h>
#include "define.h"

#define START_FRAME 0xFF
#define MV_PACKET_SIZE 5
#define TRACKING_RATE 50      //50 ms = 20 hz

class OpenMV {
    private:
      boolean tracking = false;
      int last_tracking_update = 0;
      int x = 0;
      int y = 0;
      int size = 0;
      int fps = 0;

    public:
        void init();
        void update(TCPServer *serv);
        void enable_tracking();
        void disable_tracking();
};
#endif
