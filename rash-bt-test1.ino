#include <OctoWS2811.h>

// Config 
#define HW_SERIAL Serial1
#define VIDEO_HEIGHT 3
#define STRIP_ID 2

// Constants
#define HW_SERIAL_SPEED 115200
#define MAX_STRIPS 6
#define STRIP_OFFSET VIDEO_HEIGHT * STRIP_ID
#define VIDEO_BUFFER_SIZE VIDEO_HEIGHT * MAX_STRIPS

/*
 * To Connect on the mac, use:
 * ```
 *   screen /dev/tty.Makeblock-ELETSPP-1 115200
 * ```
 *
 */

DMAMEM int displayMemory[VIDEO_BUFFER_SIZE];
int drawingMemory[VIDEO_BUFFER_SIZE];
int incoming[VIDEO_HEIGHT];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(VIDEO_HEIGHT, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(9600);
  leds.begin();
  leds.show();
  HW_SERIAL.begin(HW_SERIAL_SPEED);
  HW_SERIAL.println("TV Booting");
  HW_SERIAL.print("Total led memory: "); HW_SERIAL.println(VIDEO_BUFFER_SIZE);
  HW_SERIAL.print("Strip length: "); HW_SERIAL.println(VIDEO_HEIGHT);
  HW_SERIAL.print("Strip id: "); HW_SERIAL.println(STRIP_ID);
  HW_SERIAL.println("RDY");
}

void loop() {
  /* echo_test(); */

  while (true) {
    Serial.println("Ready!");
    read_and_show();
  }
}

uint8_t nibble_from_char(int c) {
  Serial.println(c,HEX);
  if ((c >= 0x30) && (c <= 0x39)) {
    Serial.println("Num");
    return c - 0x30;
  } else if ((c >= 0x61) && (c <= 0x66)) {
    Serial.println("lc");
    return 10 + (c - 0x61);
  } else if ((c >= 0x41) && (c <= 0x46)) {
    Serial.println("uc");
    return 10 + (c - 0x41);
  }
  return 0;
}

uint8_t byte_from_chars(int ms, int ls) {
  return (nibble_from_char(ms) << 8) + nibble_from_char(ls);
}

uint8_t byte_from_serial() {
  return byte_from_chars(HW_SERIAL.read(), HW_SERIAL.read());
}

void read_and_show() {
  uint8_t incomingIndex = 0;

  while (incomingIndex < VIDEO_HEIGHT) {
    if (HW_SERIAL.available() >= 6) {
      uint8_t red = byte_from_serial();
      uint8_t green = byte_from_serial();
      uint8_t blue = byte_from_serial();
      Serial.print("Got a Color for index: "); Serial.println(incomingIndex);
      Serial.print("Setting color for index, led, r, g, b: "); 
      Serial.print(incomingIndex);
      Serial.print(",\t");
      Serial.print(STRIP_OFFSET+incomingIndex);
      Serial.print(",\t");
      Serial.print(red);
      Serial.print(",\t");
      Serial.print(green);
      Serial.print(",\t");
      Serial.print(blue);
      Serial.print(",\t");
      Serial.println();
      leds.setPixel(STRIP_OFFSET + incomingIndex, red, green, blue); 
      incomingIndex++;
    }
  }
  Serial.println("Got all colors"); 

  leds.show();
  HW_SERIAL.println("RDY");
}

void echo_test() {
  char c;

  if (Serial.available() > 0) {
    c = char(Serial.read());
    Serial.print(c);
    HW_SERIAL.print(c);
  }
  if (HW_SERIAL.available() > 0) {
    c = char(HW_SERIAL.read());
    Serial.print(c);
    HW_SERIAL.print(c);
  }
} 
