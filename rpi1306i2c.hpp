#ifndef __RPI1306I2C_H__
#define __RPI1306I2C_H__

#include <cstdint>
#include <span>
#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>

// system headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

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

namespace i2c {

  class Device {
    private:
      uint8_t m_buffer[128];
      uint8_t m_dataSize = 0;
      int m_dev = -1;

    public:

      Device(uint8_t dev, uint8_t addr) {
        std::string devPath = "/dev/i2c-";
        devPath += std::to_string(dev);

        m_dev = open(devPath.c_str(), O_RDWR);
        if (m_dev < 0) {
          throw std::runtime_error(std::string("Could not open ") + devPath);
        }
        if (ioctl(m_dev, I2C_SLAVE, addr) < 0) {
          throw std::runtime_error(std::string("Could not open ") + std::to_string(addr));
        }
      }

      void bufferReset() {
        m_dataSize = 0;
      }

      void bufferWrite(uint8_t data) {
        m_buffer[m_dataSize] = data;
        m_dataSize++;
        if (m_dataSize == sizeof(m_buffer)) {
          bufferFlush();
          bufferWrite(0x40);
        }
      }

      void bufferFlush() {
        directWrite(m_buffer, m_dataSize);
        bufferReset();
      }

      void directWrite(const uint8_t* data, uint8_t size) {
        if (write(m_dev, data, size) != size) {
          throw std::runtime_error("Could not write on device");
        }
      }

      ~Device() {
        if (m_dev >= 0) {
          close(m_dev);
          m_dev = -1;
        }
      }
  };
}

namespace ssd1306 {

  using Bitmap = std::span<const uint8_t>;

  class Display: i2c::Device {
    private:
      uint8_t m_width = 128;

    protected:
      using i2c::Device::bufferWrite;

      uint8_t m_height = 0;

      Display(uint8_t dev, uint8_t addr): i2c::Device(dev, addr) {};

      void initDisplay(const uint8_t* sequence, uint8_t size) {
        bufferReset();
        uint8_t command[2] = { 0x00, 0x00 };
        for (uint8_t i = 0; i < size; i++) {
          command[1] = sequence[i];
          directWrite(&command[0], sizeof(command));
        }
      }

      void setBlock(uint8_t x, uint8_t y, uint8_t w) {
        bufferReset();
        bufferWrite(0x00);
        bufferWrite(COLUMNADDR);
        bufferWrite(x);
        bufferWrite(w ? (x + w - 1) : (m_width - 1));
        bufferWrite(PAGEADDR);
        bufferWrite(y);
        bufferWrite((m_height >> 3) - 1);
        bufferFlush();
        bufferWrite(0x40);      
      }

      void bufferWrite(const Bitmap& bitmap) {
        for (const auto& u: bitmap) {
          bufferWrite(u);
        }
      }

    public:

      void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const Bitmap& bitmap) {
        uint8_t j;
        setBlock(x, y >> 3, w);
        bufferWrite(bitmap);
        bufferFlush();  
      }

      void clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
        setBlock(x, y >> 3, w);
        // no need to iterate, write all the fucking way
        for (uint8_t m = (h >> 3); m > 0; m--) {
          for (uint8_t n = w; n > 0; n--) {
            bufferWrite(0x00);
          }
        }
        bufferFlush();
      }

      void clear() {
        clear(0, 0, m_width, m_height);
      }
  };

  class Display128x32: public Display {
    public:
      Display128x32(uint8_t dev, uint8_t addr): Display(dev, addr) {
        m_height = 32;
        initDisplay(ssd1306_128x32_init_seq, sizeof(ssd1306_128x32_init_seq));
      }
  };

  class Display128x64: public Display {
    public:
      Display128x64(uint8_t dev, uint8_t addr): Display(dev, addr) {
        m_height = 64;
        initDisplay(ssd1306_128x64_init_seq, sizeof(ssd1306_128x64_init_seq));
      }
  };
}

#endif // __RPI1306I2C_H__