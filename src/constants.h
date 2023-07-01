#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cstdint>

#define HZ_ADDR_MODE        0x00
#define VT_ADDR_MODE        0x01
#define PG_ADDR_MODE        0x02

#define SETLOWCOLUMN        0x00
#define SETHIGHCOLUMN       0x10
#define MEMORYMODE          0x20
#define COLUMNADDR          0x21
#define PAGEADDR            0x22
#define SETSTARTLINE        0x40
#define DEFAULT_ADDRESS     0x78
#define SETCONTRAST         0x81
#define CHARGEPUMP          0x8D
#define SEGREMAP            0xA0
#define DISPLAYALLON_RESUME 0xA4
#define DISPLAYALLON        0xA5
#define NORMALDISPLAY       0xA6
#define INVERTDISPLAY       0xA7
#define SETMULTIPLEX        0xA8
#define DISPLAYOFF          0xAE
#define DISPLAYON           0xAF
#define SETPAGE             0xB0
#define COMSCANINC          0xC0
#define COMSCANDEC          0xC8
#define SETDISPLAYOFFSET    0xD3
#define SETDISPLAYCLOCKDIV  0xD5
#define SETPRECHARGE        0xD9
#define SETCOMPINS          0xDA
#define SETVCOMDETECT       0xDB
#define SWITCHCAPVCC        0x02
#define NOP                 0xE3

constexpr uint8_t ssd1306_128x32_init_seq[] =
{
  DISPLAYOFF,
  SETDISPLAYCLOCKDIV,
  0x80,
  SETMULTIPLEX,
  0x1F,
  SETDISPLAYOFFSET,
  0x00,
  SETSTARTLINE | 0x00,
  CHARGEPUMP,
  0x14,
  SEGREMAP | 0x01,
  COMSCANDEC,
  SETCOMPINS,
  0x02,
  SETCONTRAST,
  0x7F,
  SETPRECHARGE,
  0x22,
  SETVCOMDETECT,
  0x40,
  MEMORYMODE,
  HZ_ADDR_MODE,
  DISPLAYALLON_RESUME,
  NORMALDISPLAY,
  DISPLAYON,
};

constexpr uint8_t ssd1306_128x64_init_seq[] = {
  DISPLAYOFF,
  MEMORYMODE,
  HZ_ADDR_MODE,
  COMSCANDEC,
  SETSTARTLINE | 0x00,
  SETCONTRAST,
  0x7F,
  SEGREMAP | 0x01,
  NORMALDISPLAY,
  SETMULTIPLEX,
  0x3F,
  SETDISPLAYOFFSET,
  0x00,
  SETDISPLAYCLOCKDIV,
  0x80,
  SETPRECHARGE,
  0x22,
  SETCOMPINS,
  0x12,
  SETVCOMDETECT,
  0x20,
  CHARGEPUMP,
  0x14,
  DISPLAYALLON_RESUME,
  DISPLAYON,
};

#endif // __CONSTANTS_H__
