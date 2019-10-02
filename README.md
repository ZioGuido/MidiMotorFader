# MidiMotorFader
Motor Fader MIDI controller with Arduino

This project is for a simple MIDI controller based on a single motorized fader in a small box, using USB for both MIDI communication and power.

The motorfader is an ALPS RS60N12M9000A, I bought a few of these spare motorfaders for my studio mixer  many years ago, 
and I decided to use one for this project. The problem is that it needs at least 8 Volts and at least 300 mA to move. It doesn't move with
just 5 Volts.

The solution is to use two components between the Arduino Pro Micro board (Leonardo clone) and the motorfader: 
1. the first is a power driver ST L293B, it is a 4-channel driver, but we need just 2 channels for the two motor poles;
2. to bring the USB 5V power to 8,5V I found a cheap step-up DC-DC converter based on an MT3608 chip.

The compoents used for this project are:
1. Arduino Pro Micro (Leonardo clone, ATmega 32u4)
2. L293B
3. Step-Up DC-DC converter MT3608
4. ALPS RS60N12M9000A
5. Hammond mfg. 1591 plastic container

See the images in the pic/ folder.

Watch the project in action:
https://www.youtube.com/watch?v=2A8huV0Kvhg

