#! /usr/bin/python3
#
# Exercise the SerServo API
# (C) 2022, jw@owncloud.com, distribute under GPLv2 or ask
#
# The pointer moves in steps from left to right, and back.
# While it moves left, three LEDS are green on the left side, while it moves to the right, they are on the right side.

import serial, sys, time

if len(sys.argv) < 2:
  print("\nUsage: %s PORT\n\nAvailable ports are:" % sys.argv[0])

  import serial.tools.list_ports as L

  for p in map(lambda x: x.device, L.comports()):
     print("  " + p)
  print("\n")
  sys.exit(1)

ser = serial.Serial(sys.argv[1], baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_TWO)

def slurp(ser):
  # slurp whatever is incoming.
  while ser.in_waiting > 0:
    print(ser.read(1))

def moveto(x):
  time.sleep(0.1)
  slurp(ser)
  ser.write(b"P %d\n" % x)
  time.sleep(0.1)

def led(r, g, b, first=None, last=None):
  if last is None: last = first
  time.sleep(0.1)
  if first is None:
    ser.write(b"C %d %d %d\n" % (r,g,b))
  else:
    ser.write(b"C %d %d %d %d %d\n" % (r,g,b,first,last))
  time.sleep(0.1)


while True:

  led(0,0,0,   6,  8)
  led(0,20,0, 26, 28)
  for i in (10, 20, 30, 40, 50, 60, 70, 80, 90, 100):
    moveto(i)
    time.sleep(1.5)

  led(0,20,0,  6,  8)
  led(0,0,0,  26, 28)
  for i in (80, 60, 40, 20, 0):
    moveto(i)
    time.sleep(1.5)

ser.close()
