
// SerServo 0.1
// (C) 2022 jw@owncloud.com, distribute under GPLv2 or ask
//
// Calibration of unit 1: min 33, max 102

// include the Servo library
#include <Servo.h>
// and make some light...
#include <Adafruit_NeoPixel.h>

Servo servo;

#define SERVO_PIN 9
#define SERVO_MIN_DEF 33p
// p 0 moves to the 0 positon on the display
#define SERVO_MAX_DEF 102   // p 100 moves to the max value on the display

#define WS2812_PIN 6
#define NUM_PIXELS 35  // how many LEDs we have

#define VERSION "0.3"

#define DUMMY 0

#if DUMMY
#include <string.h>

struct Serial
{
    int (*available)();
    int (*read)();
    void (*print)(char *);
} Serial;

struct servo
{
    void (*write)(int);
} servo;

#endif

Adafruit_NeoPixel pixels(NUM_PIXELS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

void usage(char *s);
void set_led(int n, int r, int g, int b);
void moveto(int n);

struct cmd {
  unsigned char letter; // the letter seen, converted to upper case.
  int val[5];   // any decimal parameter seen
  int nval;   // number of valid numbers in values 0..5
  int more;   // 1: last seen some digit
} cmd = { '\0', { 0, 0, 0, 0, 0}, 0, 0 };

unsigned char led_brightness[255];

struct cfg {
  int servo_min;
  int servo_max;
} cfg = { SERVO_MIN_DEF, SERVO_MAX_DEF };

void do_cmd()
{
  if (cmd.letter == '\0')
    return;

  if (cmd.letter == 'P')
    {
      if (cmd.nval == 0) cmd.val[0] = 0;  // position defaults to 0
      moveto(cmd.val[0]);
    }

  else if (cmd.letter == 'O')
    {
      if (cmd.nval == 2)
        {
	   cfg.servo_min = cmd.val[0];
	   cfg.servo_max = cmd.val[1];
        }
      else
        {
           Serial.print("servo_min=");
	   Serial.print(cfg.servo_min);
           Serial.print(" servo_max=");
	   Serial.print(cfg.servo_max);
           Serial.print("\n");
        }
    }
    
  else if (cmd.letter == 'B')
    {
       double b;
       if (cmd.nval == 0) cmd.val[0] = 50;  // default brightness 50%
       b = 0.01 * cmd.val[0];
       if (b > 1.0) b = 1.0;

       for (int n = 0; n < 256; n++)
         {
           Serial.print(int(n * b + .5));
           Serial.print("\n");
           led_brightness[n] = int(n * b + .5);
         }
    }

  else if (cmd.letter == 'C')
    {
      if (cmd.nval == 0) cmd.val[cmd.nval++] = 255;   // default full bright white
      if (cmd.nval == 1) cmd.val[cmd.nval++] = cmd.val[0];  // copy r to g
      if (cmd.nval == 2) cmd.val[cmd.nval++] = cmd.val[1];  // copy g to b
      int n = 0; int m = NUM_PIXELS;
      if (cmd.nval >= 4) n = m = cmd.val[3];
      if (cmd.nval == 5)     m = cmd.val[4];
      if (n < 0 || n > NUM_PIXELS) { usage("bad LED idx"); return; }
      if (m < 0 || m > NUM_PIXELS) { usage("bad second LED idx"); return; }
      while (n <= m)
        {
          set_led(n, cmd.val[0], cmd.val[1], cmd.val[2]);
          n++;
        }
    }

  else if (cmd.letter == 'H' || cmd.letter == '?')
    {
       Serial.print("\n\
\n\
P [x]\n\
  x defaults to 0. Moves the servo to a position 0...100\n\
\n\
O\n\
  Report servo calibration.\n\
\n\
O min max\n\
  Set servo calibration. For identity use 0 100.\n\
B nn\n\
  Set LED brightness in percent for subsequent C commands, default 50.\n\
C\n\
  Set all LEDs to 255, 255, 255\n\
C x\n\
  Sets all LEDs to color r=x, g=x, b=x  (0..255)\n\
C r g b\n\
  Sets the RGB value of all LEDs\n\
C r g b n\n\
  Set only LED n to the r g b value. n in 0..30)\n\
  All other LEDs remain as they are.\n\
C r g b n m\n\
  Set LEDs n to m (inclusive) \n\
\n");
    }

  else
    {
      usage("unknown command, try ?");
    }
  cmd.letter = '\0';
}

void usage(char *msg)
{
  Serial.print(msg);
}

void set_led(int i, int r, int g, int b)
{
  pixels.setPixelColor(i, pixels.Color(led_brightness[r], led_brightness[g], led_brightness[b]));
  pixels.show();
}

void moveto(int p)
{
  double x = p * 0.01 * (cfg.servo_max - cfg.servo_min) + cfg.servo_min;
  servo.write(x);
}

void setup()
{
    Serial.begin(115200);
    servo.attach(SERVO_PIN);
    pixels.begin();
    pixels.clear();
    Serial.print("SerServo v");
    Serial.print(VERSION);
    Serial.print("\n");

    int n;
    for (n = 0; n < 256; n++)
       led_brightness[n] = int(n * 0.5 + .5);
    for (n = 0; n < NUM_PIXELS; n++)
       set_led(n, 40, 20, 0);  // dim orange
    Serial.print("default brightness 50%, default color 30 20 10\n");
    Serial.print("servo calibration: min=");
    Serial.print(cfg.servo_min);
    Serial.print(" max=");
    Serial.print(cfg.servo_max);
    Serial.print("\n");
}

void loop()
{
  while (Serial.available() > 0)
    {
      int c = Serial.read();

      if (cmd.letter == '\0')
        {
          if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ) continue; // ignore whitespace
          if (c >= 'a' && c <= 'z') c -= 'a'-'A';       // convert to upper case
          if ((c != '?') && (c < 'A' || c > 'Z')) { usage("Letter expected"); continue; }
          memset((void *)&cmd, 0, sizeof(cmd));
          cmd.letter = c;
        }
      else
        {
    if (c == ' ' || c == '\t')
      {
        if (cmd.more)
          { 
            if (cmd.nval >= 5) { cmd.letter = '\0'; usage("Too many numbers"); continue; }
      cmd.nval++;
    }
        cmd.more = 0;
        continue;
      }
    if (c == '\n' || c == '\r')
      {
        if (cmd.more) cmd.nval++;
        do_cmd();
        continue;
      }
    if (c < '0' || c > '9') { cmd.letter = '\0'; usage("Digit expected"); continue; }
    cmd.val[cmd.nval] = 10 * cmd.val[cmd.nval] + c - '0';
    cmd.more = 1;
  }
    }
}


#if DUMMY
// dummy framework below
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>

int serial_available()
{
  fd_set rfd;
  struct timeval tv;

  FD_ZERO(&rfd);
  FD_SET(0, &rfd);
  tv.tv_sec = 0;
  tv.tv_usec= 10000;  // wait max 10 msec

  return select(1, &rfd, NULL, NULL, &tv);
}

void serial_print(char *msg)
{
  printf("serial_print(%s)\n", msg);
}

int serial_read()
{
  char buf[4];
  int c = read(0, (void *)&buf, 1);
  if (c != 1) return 0;
  return buf[0]; 
}

void servo_moveto(int p)
{
  printf("servo_moveto(%d)\n", p);
}

void usage(char *s)
{
  printf("usage(%s)\n", s);
}

void set_led(int n, int r, int g, int b)
{
  printf("set_led(n=%d, rgb = %d %d %d)\n", n, r, g, b);
}

int main()
{
  Serial.available = &serial_available;
  Serial.read = &serial_read;
  Serial.print = &serial_print;
  servo.moveto = &servo_moveto;

  for (;;) loop();
  for (;;)
  {
    if (Serial.available())
    {
      char c = Serial.read();
      printf("Seen %d = %c\n", c, c);
    }
  }
}
#endif
