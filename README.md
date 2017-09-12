# Butterbot
---
![Video](https://img.youtube.com/vi/BhRFnhcuLr8/0.jpg)](https://youtu.be/BhRFnhcuLr8)

## TL;DR

While [working with Jamie at M5 Industries](http://danielnugent.com/m5-industries-proj.html) I set out to diversify my skill set and embarked on a mechanically oriented project.  If you have read my other write up about my summer at M5 industries you will know that I was simultaneously working on a software robotics project, and I wanted a project which focused on mechanical design and fabrication. Butterbot strictly exists as a fictional character from the show Rick and Morty, so I set out to make him a real robot.

## Shell

![parts](http://danielnugent.net/img/projects/bb-parts.jpg)

Butterbot’s body is mostly made of aluminum. 5 sides of his head are made of bent sheet metal with his face being made of machined aluminum plate. His base and neck are made of machined aluminum stock. All other parts are made from acrylic, plastics, wire, and various do-dads. All parts are secured using pressure fitting, welding, bolts, or epoxy.

## Motion

![motion](http://danielnugent.net/img/projects/bb-gearbox.jpg)

Butterbot contains 5 actuators:
- 2 variable speed track elements at his base
- 2 micro servo motors in his shoulders
- 1 mini servo motor at the base of his head for his main neck joint

This doesn’t seems like that much, but it was a lot to fit into a small form factor. The motors and gearbox for the track elements take up 7/8 of the space in the base. Only leaving just enough room for a dual h-bridge motor controller and small battery. The shoulder servos took up 2/3 of the width of his head, making it extremely hard to place all the electronics and neck servo.

## Neck

![neck](http://danielnugent.net/img/projects/bb-neck-1.jpg)

Butterbot’s neck was the hardest part of this build so I felt it required its own section. As you can see Butterbot’s neck is way too small to fit an actuator inside of it so the servo motor had to be placed in his head. In addition to his head being tight on space, it was heavy as a result of electronics, motors, arms, and its size. The force it exerted on the neck joint was multiplied by that fact that the upper joint of his neck acted as a 4in lever.  So what I had to design was a small yet high torque head mounted system which transferred energy to his neck. This was achieved using a cable/pulley system and a high torque servo (highest torque you can buy in the mini servo category). The servo was mounted at the base of the head with its output shaft running parallel with his base (perpendicular to the neck joint shaft). This 90 degree offset was due to the limited space in the head. This meant the pulley cables had to twist 90 degrees as they traveled down his neck. In order to prevent the cables from slipping on the pulleys, the cables were crimped onto the pulleys. Keep in mind these pulleys were only 0.5in in diameter. Adding cable crimping wasn’t easy. The biggest issue was that after the 4in neck, heavy head, and small pulley the resulting force on the cable was roughly 22lbs! I had to go through many iterations of pulley designs, cable types, and crimping methods so that the cable wouldn’t slip or snap. What should have taken 3 days took 2 weeks.

## Electronics

![electronics](http://danielnugent.net/img/projects/bb-electronics.jpg)

Butterbot is packed with cool features. He contains:
- Wifi enabled microcontroller(Particle Photon)
- A programmable sound card(Adafruit)
- ~~D class mono audio amplifier(Adafruit)~~ Lm386 Mono Amp(Amazon)
- ~~3 watt speaker~~ HTC M8 One speaker
- Dual h-bridge serial motor controller(Polulo)
- ~~2 amp voltage regulator~~ 3 amp LT1764A voltage regulator
- ~~Face tracking Camera (OpenMV)~~
- LED

Everything but the motor controller fits in his head. He is wired with 3 different voltage levels (3.3v, 5v, 8v). He runs off a 2 cell 300mah battery. He has the ability to talk, track your face*, and move. ~~The camera used to fit in the head, but the audio amplifier wasn’t loud enough so I had to add a larger speaker in place of the camera. If I get a better amplifier(that’s the same size) then I should be able to put the camera back in(which I really want to do considering I already programmed it!)~~  I upgraded the speaker system and the camera fits again! Update: I didn't own the camera and the owner took it back....right as I got it working.

## [App](https://github.com/djnugent/butterbot-app)

![app](http://danielnugent.net/img/projects/bb-control.gif)

Butterbot is controlled over wifi using an android app. The app allows me to drive him, play audio clips, track faces, and monitor battery level. It communicates with the microcontroller using TCP sockets. Below you can find some gifs of UI functionality.

## Microcontroller

![electronics](http://danielnugent.net/img/projects/bb-harness.jpg)

The microcontroller manages all his electrical systems and makes sure he operates smoothly. He is mechanically unstable(top heavy) so filters and constraints are in place so he doesn’t wreck himself. The microcontroller runs an realtime operating system(RTOS) so it uses threading and system timers to handle continuous filtering and task scheduling. It also has built in wifi so it acts as TCP server and also communicates with a cloud server to send connection credentials to the app. If butterbot can’t connect to the local network he will setup a wifi spot so his network setting can be reconfigured(like a chromecast).

## Problems
I had some unexpected electrical problems along the way. First his speaker wasn’t loud enough. I tried to fix this with a larger speaker, but that barely helped and caused me to remove the camera due to space constraints. I eventually settled for a new amp and an HTC M8 speaker(best I could find). This allowed for the camera to be put back in.

Next his neck would glitch when not enabled(no PWM). It’s not enabled on power up because he needs to be upside down to plug in his battery. This was fixed by grounding the servo signal line with a 10k resistor.

After that, the arm servos would cause a voltage drop on the 5v line and it would reboot the audio card and microcontroller. This was frustrating because the servos were really nice but drew too much current during acceleration. This was fixed by designing a 5v regulator with a fast transient response based on the LT1764A.

Finally, the most troubling issue I encountered was the motor controller for the base would lock up randomly when the neck was enabled. It required a full reset in order to get it operational again. It was completely undocumented behavior for the controller(which was essentially a microcontroller with dual HBridge). It was really strange. At first I thought the issue was caused by PWM noise from the neck servo leaking into the serial line of the motor controller. But after poking and prodding and causing butterbot to drive off the table and BREAK HIS NECK! I figured out that the cheap hobbyking neck servo, that was powered in parallel with the motor controller, was creating noise on the power lines(which are supposedly isolated from the TTL power). Apparently the isolation wasn’t good enough and the power noise caused the microcontroller to brick. I fixed this by putting a 100uF capacitor across the main power lines. Long story short hobby king servos suck. They had noise issues on their signal and power lines.

The only major software problems I encountered was a TCP delay issue which I solved by forcing TCP to send packets when they were ready, not when they hit 1500 bytes. Additionally, I had trouble programming the photon to go into configuration mode when it couldn’t connect to the specified wifi network. This was caused by lack of documentation and solved by trial and error.


## Additional Photos

Visit my official [write up](http://danielnugent.net/butterbot-proj.html)
