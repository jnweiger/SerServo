# SerServo
A serial line text protocol for a servo, using an arduino nano

This is used to drive the pointer on a 20cm diameter prehistoric ampere-meter.
It also features a ws2812 led-strip - also controlled trhough the serial protocol, and some 3d printed parts to implement this particular usecase of SerServo

## USB-serial

Power supply at USB must provide 5V, 1.5A min.
The baud rate is 115200

## Commands
Commmands start with a letter, can contain any amount of whitespace (ignored).
Then (optionally) decimal numbers follow, separated by whitespace, the command ends in a newline and or carriage retun
An empty command is ignored.

P [x]
	x defaults to 0. Moves the servo to a position 0...1000

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

