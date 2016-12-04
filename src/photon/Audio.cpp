#include "Audio.h"
#include <Particle.h>


void Audio::init(int baud){
    Serial2.begin(baud);
}

void Audio::play(int track_num,LED *led){
    //pause any track that might be playing
    Serial2.println("q");
    delay(10);

    //play track
    Serial2.print("#0");
    Serial2.println(track_num);

    //Set led
    switch(track_num){
        case AUDIO_OMG:
            led->talk(AUDIO_OMG_LEN);
        break;
        case AUDIO_PURPOSE:
            led->talk(AUDIO_PURPOSE_LEN);
        break;
        case AUDIO_BEEP:
            led->talk(AUDIO_BEEP_LEN);
        break;
        case AUDIO_BOOP:
            led->talk(AUDIO_BOOP_LEN);
        break;
        case AUDIO_FRIENDSHIP:
            led->talk(AUDIO_FRIENDSHIP_LEN);
        break;
    }
}
