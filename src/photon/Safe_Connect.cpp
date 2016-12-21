#include <Particle.h>
#include "Safe_Connect.h"
#include "define.h"

//Fall back to setup mode if we can't connect to regular wifi network
void safe_connect(){
    //Turn on Wifi
    WiFi.on();
    WiFi.connect();

    //Wait for connection
    if (waitFor(WiFi.ready, WIFI_TIMEOUT)){
       //wifi connection success: connect to cloud
        Particle.connect();
        waitUntil(Particle.connected);
    }else{
        //wifi connection failed go into setup mode
       listen();
    }
}

void listen(){
    digitalWrite(LED_PIN,HIGH);
    WiFi.on();
    SINGLE_THREADED_BLOCK() {
      WiFi.listen(); //Takes up to 10 seconds for network to appear
    }

    //Wait for the device to connect to new network
    waitUntil(WiFi.ready);
    System.reset();
}
