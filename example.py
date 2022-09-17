#! /usr/bin/python3
#
# Exercise the SerSerial API


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

while True:
  for i in (0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 80, 60, 40, 20):
    slurp(ser)
    ser.write(b"p %d\n" % i)
    time.sleep(1.5)

ser.close()
