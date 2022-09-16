#! /usr/bin/python3
#
# Exercise the SerSerial API


import serial, sys

if len(sys.argv) < 2:
  print("\nUsage: %s PORT\n\nAvailable ports are:" % sys.argv[0])

  import serial.tools.list_ports as L

  for p in map(lambda x: x.device, L.comports()):
     print("  " + p)
  print("\n")
  sys.exit(1)

ser = serial.Serial(sys.argv[1], baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_TWO)

def slurp(ser):
  # slurp whatever is incoming.
  while ser.in_waiting > 0:
    print(ser.read(1))

slurp(ser)

ser.close()
