import serial
import struct
import socket
import time
import threading
import traceback
import sys
import argparse



######Command Line interface for testing butterbot over USB/TCP#####################



class butterbot():

    CMD_ERROR      = 0x00        #Invalid command
    CTRL_MOTORS    = 0x01        #control treads        - {Left(-255,255),right(-255,255)}
    CTRL_ARMS      = 0x02        #control arms          - {Left(1000,2000).right(1000,2000)}
    CTRL_NECK      = 0x03        #control neck          - {(1000,2000)}
    CTRL_LED       = 0x04        #control LED           - {LED_MODE, On/Off or blink_ms}
    CTRL_AUDIO     = 0x05        #play sound track      - {AUDIO}
    CTRL_LIST_MODE = 0x06        #activate listen mode  - {}
    BATT_LVL       = 0x07        #Battery Level
    ATTACH_NECK    = 0x08        #Activate the neck
    OPENMV_ENABLE  = 0x09        #Enable OpenMV tracking- {0/1}
    OPENMV_POS     = 0x0A        #OpenMV position packet


    LED_MODE_TALK   = 0x00        #Flash LED while talking
    LED_MODE_BLINK  = 0x01        #Flash at a continous rate
    LED_MODE_IO     = 0x02        #LED either on or off
    LED_MODE_CNCT   = 0x03        #LED breathes while waiting for the app to connnect
    LED_MODE_BATT   = 0x04        #LED flashes fast when battery is low


    def __init__(self):
        self.ser = None
        self.tcp = None
        self.batt_level = None
        self.stream_camera = False
        #request battery status
        t = threading.Thread(target=self.check_battery)
        t.start()
        #parse incoming data
        t1 = threading.Thread(target=self.recv_command)
        t1.start()


    def start_serial(self,port,baud):
        self.ser = serial.Serial(
            port=port,
            baudrate=baud,
            timeout=0.1
        )
        self.ser.reset_input_buffer()

    def start_tcp(self,server,port):
        # Create a TCP/IP socket
        self.tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp.connect((server, port))


    def send_command(self,cmd_id, arg1, arg2):
        cmd = struct.pack(">bhhc",cmd_id,arg1,arg2,b'\n')
        if(self.ser is not None):
            self.ser.write(cmd)
        if(self.tcp is not None):
            self.tcp.send(cmd)

    def recv_command(self):
        time.sleep(1)
        while(True):
            raw = []
            if(self.ser is not None):
                raw = self.ser.read(6)
            if(self.tcp is not None):
                raw = self.tcp.recv(6)
            if(len(raw) == 6):
                cmd_id = raw[0]
                arg1 = (raw[1] << 8) | raw[2]
                arg2 = (raw[3] << 8) | raw[4]
                if(cmd_id  == self.BATT_LVL):
                    self.batt_level = arg1
                elif(self.stream_camera and cmd_id == self.OPENMV_POS):
                    print(arg1,arg2)
                else:
                    print((hex(cmd_id),arg1,arg2,raw))

    def check_battery(self):
        while(True):
            self.send_command(self.BATT_LVL,0,0)
            time.sleep(0.15)

    def close(self):
        if(self.ser is not None):
            self.ser.close()
        if(self.tcp is not None):
            self.send_command(self.OPENMV_ENABLE, 0,0)
            self.tcp.close()




if __name__ == "__main__":


    bb = butterbot()

    parser = argparse.ArgumentParser(description='Command line interface for testing butterbot')
    parser.add_argument('connection_string', action="store", help="Serial port or IP address")
    parser.add_argument('--baud', default=115200, help="Serial port baud")
    parser.add_argument('--port', default=88, help="TCP port")
    args = parser.parse_args()

    #TCP connnection
    if(args.connection_string.count('.') == 3):
        bb.start_tcp(args.connection_string,args.port)
        print("Connected on {}:{}".format(args.connection_string,args.port))
    #Serial Connection
    else:
        bb.start_serial(args.connection_string,args.baud)
        print("Connected on {}:{}".format(args.connection_string,args.baud))

    #run a CLI for managing users
    exit = False
    while not exit:
        try:
            time.sleep(0.1)
            cli = input('>>>').split()
            if len(cli) > 0:
                cmd = cli[0]
                if cmd == "help":
                    print("CLI for butterbot")
                    print("    base   [throttle] or [left thr] [right thr]      #controls base")
                    print("    arm   [us] or [left us] [right us]               #controls arms")
                    print("    neck  [us]                                       #controls neck")
                    print("    led   on/off/talk/batt/connect/blink{blink rate} #controls led")
                    print("    play  [track number]                             #plays audio")
                    print("    batt                                             #displays battery status")
                    print("    listen                                           #put photon into listen mode")
                    print("    exit                                             #exit CLI")

                elif cmd == "base": #base
                    if len(cli) == 2:
                        bb.send_command(bb.CTRL_MOTORS,int(cli[1]) + 255,int(cli[1])+255)
                    elif len(cli) == 3:
                        bb.send_command(bb.CTRL_MOTORS,int(cli[1]) + 255,int(cli[2])+255)
                    else:
                        print("invalid usage")

                elif cmd == "arm": #arms
                    if len(cli) == 2:
                        bb.send_command(bb.CTRL_ARMS,int(cli[1]),int(cli[1]))
                    elif len(cli) == 3:
                        bb.send_command(bb.CTRL_ARMS,int(cli[1]),int(cli[2]))
                    else:
                        print("invalid usage")

                elif cmd == "neck": #neck
                    if len(cli) == 2:
                        if cli[1] == 'enable':
                            bb.send_command(bb.ATTACH_NECK,1,0)
                            bb.send_command(bb.CTRL_NECK,1500,0)
                        elif cli[1] == 'disable':
                            bb.send_command(bb.ATTACH_NECK,0,0)
                        else:
                            bb.send_command(bb.CTRL_NECK,int(cli[1]),0)
                    else:
                        print("invalid usage")

                elif cmd == "led": #led
                    if len(cli) == 2:
                        if cli[1] == "on":
                            bb.send_command(bb.CTRL_LED,bb.LED_MODE_IO,1)
                        elif cli[1] == "off":
                            bb.send_command(bb.CTRL_LED,bb.LED_MODE_IO,0)
                        elif cli[1] == "batt":
                            bb.send_command(bb.CTRL_LED,bb.LED_MODE_BATT,0)
                        elif cli[1] == "connect":
                            bb.send_command(bb.CTRL_LED,bb.LED_MODE_CNCT,0)
                        elif cli[1] == "talk":
                            bb.send_command(bb.CTRL_LED,bb.LED_MODE_TALK,0)
                        else:
                            print("invalid usage")
                    elif len(cli) == 3 and cli[1] == 'blink':
                        bb.send_command(bb.CTRL_LED,bb.LED_MODE_BLINK,int(cli[2]))
                    else:
                        print("invalid usage")

                elif cmd == "play": #sound
                    if len(cli) == 2:
                        bb.send_command(bb.CTRL_AUDIO,int(cli[1]),0)
                    else:
                        print("invalid usage")

                elif cmd == "batt":
                    print(bb.batt_level)

                elif cmd == "listen":
                    bb.send_command(bb.CTRL_LIST_MODE,0,0)

                elif cmd == "camera":
                    if len(cli) == 2:
                        if cli[1] == "stream":
                            bb.stream_camera = True;
                            bb.send_command(bb.OPENMV_ENABLE,1,0)
                        elif cli[1] == "stop":
                            bb.stream_camera = False;
                            bb.send_command(bb.OPENMV_ENABLE,0,0)
                        else:
                            print("invalid usage")
                    else:
                        print("invalid usage")


                elif cmd == "demo":
                    #look up
                    bb.send_command(bb.CTRL_NECK,1900,0)
                    time.sleep(1.5)
                    #What is my purpose
                    bb.send_command(bb.CTRL_AUDIO,4,0)
                    time.sleep(1)
                    #Center head
                    bb.send_command(bb.CTRL_NECK,1500,0)
                    time.sleep(1.5)
                    #look at hands
                    bb.send_command(bb.CTRL_NECK,1300,0)
                    time.sleep(0.3)
                    #look left hand
                    bb.send_command(bb.CTRL_ARMS,1850,1500)
                    bb.send_command(bb.CTRL_MOTORS,-20,20)
                    time.sleep(.3)
                    #look right
                    bb.send_command(bb.CTRL_ARMS,1700,1850)
                    bb.send_command(bb.CTRL_MOTORS,30,-30)
                    time.sleep(.5)
                    bb.send_command(bb.CTRL_MOTORS,0,0)
                    #head and arms down
                    bb.send_command(bb.CTRL_NECK,1100,0)
                    #bb.send_command(bb.CTRL_ARMS,1500,1500)
                    bb.send_command(bb.CTRL_AUDIO,3,0)


                elif cmd == "exit":
                    exit = True

                else:
                    print("invalid command: enter 'help' to see commands")

        except KeyboardInterrupt:
            exit = True
