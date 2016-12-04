#ifndef DEFINE_H__
#define DEFINE_H__

#define WIFI_TIMEOUT    5000        //5 seconds
#define TCP_TIMEOUT     1500        //1.5 second of app disconnecting
#define PACKET_SIZE     6           //byte(1) + int(2) + int(2) + NL(1)
#define TCP_PORT        88          //TCP port for app

//Commands
#define CMD_ERROR       0x00        //Invalid command
#define CTRL_MOTORS     0x01        //control treads        - {Left(-255,255),right(-255,255)}
#define CTRL_ARMS       0x02        //control arms          - {Left(1000,2000).right(1000,2000)}
#define CTRL_NECK       0x03        //control neck          - {(1000,2000)}
#define CTRL_LED        0x04        //control LED           - {LED_MODE, On/Off or blink_ms}
#define CTRL_AUDIO      0x05        //play sound track      - {AUDIO}
#define CTRL_LIST_MODE  0x06        //activate listen mode  - {}
#define BATT_LVL        0x07        //Battery Level
#define ATTACH_NECK     0x08        //Activate the neck

//Sources
#define SRC_USB         0           //Commands came from USB
#define SRC_TCP         1           //Commands came from TCP

//PINS
#define LEFT_ARM_PIN    A5
#define RIGHT_ARM_PIN   WKP
#define NECK_PIN        A4
#define LED_PIN         A3
#define BATT_PIN        A2

//Battery
#define BATT_CHECK_RATE 100
#define BATT_ALPHA      0.1
#define BATT_MIN        845         //6.4v on a 0 - 4096 scale. digital = 137.7*(voltage) - 53

#endif
