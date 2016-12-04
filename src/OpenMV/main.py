# Face Detection Example
#
# This example shows off the built-in face detection feature of the OpenMV Cam.
#
# Face detection works by using the Haar Cascade feature detector on an image. A
# Haar Cascade is a series of simple area contrasts checks. For the built-in
# frontalface detector there are 25 stages of checks with each stage having
# hundreds of checks a piece. Haar Cascades run fast because later stages are
# only evaluated if previous stages pass. Additionally, your OpenMV Cam uses
# a data structure called the integral image to quickly execute each area
# contrast check in constant time (the reason for feature detection being
# grayscale only is because of the space requirment for the integral image).

import sensor, time, image, pyb
from pyb import UART
from array import array

START_FRAME = 0xFF


#indicate connection status
blu = pyb.LED(3)

for i in range(0,3):
    blu.on()
    time.sleep(125)
    blu.off()
    time.sleep(125)

#connect to arduino
uart = UART(3)
uart.init(38400, bits=8, parity=None, stop=1)


# Reset sensor
sensor.reset()

# Sensor settings
sensor.set_contrast(1)
sensor.set_gainceiling(16)
# HQVGA and GRAYSCALE are the best for face tracking.
sensor.set_framesize(sensor.HQVGA)
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_vflip(True)

# Load Haar Cascade
# By default this will use all stages, lower satges is faster but less accurate.
face_cascade = image.HaarCascade("frontalface", stages=25)

# FPS clock
clock = time.clock()

while (True):
    clock.tick()

    # Capture snapshot
    img = sensor.snapshot()

    # Find faces.
    # Note: Lower scale factor scales-down the image more and detects smaller objects.
    # Higher threshold results in a higher detection rate, with more false positives.
    faces = img.find_features(face_cascade, threshold=0.5, scale=1.5)

    # send larget face over UART
    largest_face = 0,0,0,0
    for face in faces:
        x,y,w,h = face
        area = x * y
        l_area = largest_face[2] * largest_face[3]

        if area > l_area:
            largest_face = face

    x = int(largest_face[0] + largest_face[2]/2)
    y = int(largest_face[1] + largest_face[3]/2)
    size = int((largest_face[2] + largest_face[3]) / 2)
    packet = array('b')
    packet.append(START_FRAME)
    packet.append(x)
    packet.append(y)
    packet.append(size)
    packet.append(int(clock.fps()))
    uart.write(packet)

    # Draw objects
    for r in faces:
        img.draw_rectangle(r)
