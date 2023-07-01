#include <iostream>
#include <cstdint>
#include <unistd.h>

#include "rpi1306i2c.h"

const uint8_t bitmap[] = {
  0x00, 0x00, 0xF8, 0xF0, 0x04, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0xF0,
  0xF8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x0F, 0x20, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x20, 0x0F, 0x1F, 0x00, 0x00, 
};

int main(void) {
  Ssd1306Display32 screen(1, 0x3C);
  screen.draw(0, 0, 19, 32, bitmap);
  sleep(5);
  screen.clear();
  return 0;
}
