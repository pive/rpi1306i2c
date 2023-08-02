#include <cstdint>
#include <unistd.h>

#include <span>

#include "rpi1306i2c.hpp"

constexpr uint8_t bitmap_data[] = {
  0x00, 0x00, 0xF8, 0xF0, 0x04, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0xF0,
  0xF8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x0F, 0x20, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x20, 0x0F, 0x1F, 0x00, 0x00,
};

constexpr std::span<const uint8_t> bitmap {
  &bitmap_data[0],
  sizeof(bitmap_data)
};

int main(void) {
  ssd1306::Display<1> screen(0x3C, ssd1306::HEIGHT_32);
  screen.draw(0, 0, 19, 32, bitmap);
  screen.draw(22, 0, 19, 32, bitmap);
  sleep(3);
  screen.clear(0, 0, 19, 32);
  sleep(2);
  screen.clear();
  return 0;
}
