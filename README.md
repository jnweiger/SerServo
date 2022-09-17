 SerServo
A serial line text protocol for a servo, using an arduino nano

This is used to drive the pointer on a 20cm diameter prehistoric ampere-meter.
It also features a ws2812 led-strip - also controlled trhough the serial protocol, and some 3d printed parts to implement this particular usecase of SerServo

## USB-serial

The baud rate is 115200, no parity, two stop bits.

### Power supply
To simplify the circuit, we use the USB line also as a 5V power source.
According to https://www.arduino.cc/en/uploads/Main/ArduinoNano30Schematic.pdf, USB is coupled to the onboard +5V pins with a 0.5A Schottky diode MBR0520, so the total current draw should not exceed 0.5A.

The micro-servo can draw up to 1A, when under load or when moving at high speed.
The LED-strip can use max 60mA per LED, when all 3 channels R,G,B are at full brightness. This is a total of 2.1A

#### Precautions
- the arduino nano schematics has a 100 nF and 4.7 uF capacitor on the USB side.
- we provide an external 5600 uF capacitor to buffer motor spikes.
- There is a default B brightness setting, that dims all values to below 50%.
- The default illumniation powers only the bottom half of the ring, at a dim orange.
- We use a custom servo driver that only sends a few pulses when moving, then stops the PWM, so that the motor gets powered down.
- We remember the previous PWM position and move the motor slowly towards the next set position to save power.

## Power up and power down

The standard arduino setup() function is used to initialize the serial line, servo and led strip.
The servo is moved to position 0, the bottom half of the led strip starts glowing.

At power down, we "plan to" return the servo to position 0 before power really runs out.
To do so, we use the energy stored in the big 5600 uF capacitor together with a hardware modification on the arduino nano board:
- connect pin D12 with the positive end of the 4.7uF onboard capacitor. This can be done with a short piece of wire soldered onto the pads, which are only 2mm apart.
At powerloss, the shottky diode immediately disconnects USB power and the +5V rails. At pin D12 we detect the USB power loss, while the +5V rails still provide power. This allows us, to send one last motor moveto(0) command and hope for the best.

This mimics what an original analog hardware meter would do. It also helps us saving power at the next startup, as the servo is already at the desired position.

## Commands
Commmands start with a letter, can contain any amount of whitespace (ignored).
Then (optionally) decimal numbers follow, separated by whitespace, the command ends in a newline and or carriage retun
An empty command is ignored.

P [x]
	x defaults to 0. Moves the servo to a position 0...1000
O
	Report servo calibration.
O min max
	Set servo calibration. For identity use 0 100.

B nn
	Set LED brightness in percent for subsequent C commands, default 50.
C
	Set all LEDs to 255, 255, 255
C x
	Sets all LEDs to color r=x, g=x, b=x	(0..255)
C r g b
	Sets the RGB value of all LEDs
C r g b n
	Set only LED n to the r g b value. n in 0..30)
	All other LEDs remain as they are.
C r g b n m
	Set LEDs n to m (inclusive)

H
?
	Print the above command summary.

