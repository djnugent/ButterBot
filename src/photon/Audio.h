#ifndef AUDIO_H__
#define AUDIO_H__

#include "LED.h"

#define AUDIO_OMG           3
#define AUDIO_PURPOSE       4
#define AUDIO_FRIENDSHIP    2
#define AUDIO_BEEP          0
#define AUDIO_BOOP          1

#define AUDIO_OMG_LEN           6
#define AUDIO_PURPOSE_LEN       8
#define AUDIO_FRIENDSHIP_LEN    12
#define AUDIO_BEEP_LEN          2
#define AUDIO_BOOP_LEN          4


class Audio{

    public:
        void init(int baud);
        void play(int track_num, LED* led);

};

#endif
