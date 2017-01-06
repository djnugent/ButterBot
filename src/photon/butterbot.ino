#include "LED.h"
#include "Audio.h"
#include "Treads.h"
#include "SmoothServo.h"
#include "OpenMV.h"
#include "define.h"
#include "Safe_Connect.h"
#include "Serial2/Serial2.h"

//////////DO NOT CHANGE////////////
SYSTEM_THREAD(ENABLED);  //Allows for special wifi config DURING setup
SYSTEM_MODE(SEMI_AUTOMATIC); //Allows for custom wifi
//////////DO NOT CHANGE////////////

//LED
LED led;

//Servo
SmoothServo armR;
SmoothServo armL;
SmoothServo neck;

//Treads
Treads base;

//sound
Audio speaker;

//OpenMV camera
OpenMV camera;

//battery
int batt_level = 1000;
int batt_low = 0;
int last_batt_check = 0;

//TCP server
TCPServer server = TCPServer(TCP_PORT);
TCPClient client;
int just_connected = true;
int last_publish = 0;
int last_packet = 0;

//timer to update servos
Timer servo_timer(int(1000/SERVO_UPDATE_RATE), update_servos);
//Timer to update treads
Timer base_timer(int(1000/TREADS_UPDATE_RATE), update_base);



void setup() {

    //USB debug
    Serial.begin(115200);

    //Sound card
    speaker.init(9600);

    //Init base
    base.attach(38400,-100,100);
    base.set_params(1, 0);

    //LED
    led.init(LED_PIN);

    //battery
    pinMode(BATT_PIN,INPUT);

    //attach the servos
    armR.attach(RIGHT_ARM_PIN,1000);
    armL.attach(LEFT_ARM_PIN,2000);

    //limit their speeds
    armR.set_max_us_per_sec(4000);
    armL.set_max_us_per_sec(4000);
    neck.set_max_us_per_sec(500);

     //Set range limits
    armR.set_limits(1000,2000);
    armL.set_limits(1000,2000);
    neck.set_limits(1100,1900);


    //Start timers
    base_timer.start();
    servo_timer.start();

    //connect to the network or start our own
    safe_connect();

    //start TCPServer for app
    server.begin();
}

void loop() {

  //check for commands
  check_tcp();
  check_usb();

  //update LED
  led.update();

  //update camera
  camera.update(&server);

  //check battery
  check_battery();

}


void update_servos(){
    armL.update();
    armR.update();
    neck.update();
}

void update_base(){
    base.update();
}


//Get commands from the app
void check_tcp(){
  if (client.connected()) {
    //modify state of led upon connection
    if(just_connected){ //stop led breathing when we connect
        led.set_LED(LED_MODE_TALK,0);
        just_connected = false;
        last_packet = millis();
    }

    //Process incoming data
    if(client.available() >= PACKET_SIZE) {
        last_packet = millis();
        byte raw[PACKET_SIZE];
        client.read(raw,PACKET_SIZE);
        process_command(raw,SRC_TCP);
        //server.write(raw,PACKET_SIZE);
    }
    //check for client disconnect
    else if(millis()-last_packet > TCP_TIMEOUT){
        client.stop();
    }
  }//Wait for connection from app
  else {
        //Broadcast our connection status every 1.5 seconds
        if(millis()-last_publish >1500){
            String connection_info = "";
            connection_info.concat(WiFi.SSID()); //SSID
            connection_info.concat(":");
            connection_info.concat(String(WiFi.localIP())); //IP
            connection_info.concat(":");
            connection_info.concat(String(TCP_PORT)); //TCP Port
            Particle.publish("ConnectionInfo", connection_info,0,PRIVATE);
            last_publish = millis();
        }
        // if no client is yet connected, check for a new connection
        client = server.available();

        //put butterbot in resting state if we went from connected to disconnected
        check_battery();
        if(batt_low){
            led.set_LED(LED_MODE_BATT,0);
        }
        else{
            led.set_LED(LED_MODE_CNCT,0);
        }
        armL.set_us(2000);
        armR.set_us(1000);
        neck.detach();
        base.set_motors(0,0);
        just_connected = true;
  }
}

//Get commands from USB
void check_usb(){
    if(Serial.available() >= PACKET_SIZE) {
        byte raw[PACKET_SIZE];
        for(int i = 0; i < PACKET_SIZE; i++){
            raw[i] = Serial.read();
        }
        //Serial.write(raw,PACKET_SIZE);
        process_command(raw,SRC_USB);
    }
}

void check_battery(){
    if(millis()-last_batt_check > BATT_CHECK_RATE){
        batt_level = int(analogRead(BATT_PIN)*BATT_ALPHA + batt_level*(1 - BATT_ALPHA)); //Exp average
        batt_low = batt_level < BATT_MIN;

        //flash led on low battery
        if(batt_low){
            led.set_LED(LED_MODE_BATT,0);
        }
        last_batt_check = millis();
    }
}


//Process commands from TCP/USB client
void process_command(byte *buffer, int cmd_src){

    char cmd = buffer[0];
    int arg1 = buffer[1] << 8 | buffer[2];
    int arg2 = buffer[3] << 8 | buffer[4];

    switch(cmd){
        case CTRL_MOTORS:{
            base.set_motors(arg1-255,arg2-255);
        break;}
        case CTRL_ARMS:{
            armL.set_us(arg1);
            armR.set_us(arg2);
        break;}
        case CTRL_NECK:{
            neck.set_us(arg1);
        break;}
        case CTRL_LED:{
            led.set_LED(arg1,arg2);
        break;}
        case CTRL_AUDIO:{
            speaker.play(arg1,&led);
        break;}
        case CTRL_LIST_MODE:{
            listen();
        break;}
        case BATT_LVL:{ //Send battery level
            byte cmd[PACKET_SIZE] = {BATT_LVL,0,0,0,0,'\n'};
            cmd[1] = (batt_level & 0xff00) >> 8; //msb
            cmd[2] = batt_level & 0x00ff; //lsb
            cmd[3] = (batt_low & 0xff00) >> 8; //msb
            cmd[4] = batt_low & 0x00ff; //lsb
            if(cmd_src == SRC_TCP){
                server.write(cmd,PACKET_SIZE);
            }
            else if(cmd_src == SRC_USB){
                Serial.write(cmd,PACKET_SIZE);
            }
            break;}
        case ATTACH_NECK:{
            if(arg1 == 0){
                //neck.set_us(1100);
                //delay(1500);
                neck.detach();
            }
            else if (arg1 ==1){
                neck.attach(NECK_PIN,1050);
            }
            break;}
        case OPENMV_ENABLE:{
            if(arg1 == 0){
              camera.disable_tracking();
            }
            else if (arg1 ==1){
              camera.enable_tracking();
            }
            break;}
        default:{ //Invalid Command
            byte cmd[PACKET_SIZE] = {CMD_ERROR,0,0,0,0,'\n'};
            if(cmd_src == SRC_TCP){
                server.write(cmd,PACKET_SIZE);
            }
            else if(cmd_src == SRC_USB){
                Serial.write(cmd,PACKET_SIZE);
            }
        break;}
    }
}
