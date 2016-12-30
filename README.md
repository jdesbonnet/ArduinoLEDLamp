# ArduinoLEDLamp

Simple Arduino sketch to drive RGB LED lamp. Each channel is controlled
by a PWM on Arduino pins 9,10,11.

Using a PCB-Components.de 4 channel LED buck driver board:
https://pcb-components.de/led-abwaertswandler-buck-step-down/cam-v2-common-anode-multichannel-4x-1000-700-350ma-5v-55v-detail.html


Commands:

 * Prrggbb  Set PWM (example P00FF20)
 * Srrggbb  Slew to new PWM values
 * Frrggbbtttt  Flash LED with color rrggbb for duration tttt milliseconds.  
 * more...

