#ifndef __RPI1306I2C_H__
#define __RPI1306I2C_H__

#include <cstdint>
#include <span>
#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>
#include <mutex>

// system headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// debug prints
#include <stdio.h>
#if 1
#include <iostream>

namespace debug
{
    template<typename... Args>
    void print(const char* file, int line, Args... args) {
      fprintf(stderr, std::forward<Args>(args)...);
    }
}

#define DEBUG(...)  debug::print(__FILE__, __LINE__, __VA_ARGS__)
#else
#define DEBUG(...)  ((void)0)
#endif

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

  template <uint8_t devid>
  class Device {
    private:
      std::string m_i2c_dev_path;
      int m_dev = -1;
      std::mutex& m_mutex;

      uint8_t m_buffer[128];
      uint8_t m_dataSize = 0;

      Device(std::mutex& mutex): m_mutex(mutex) {
        m_i2c_dev_path = "/dev/i2c-" + std::to_string(devid);
        m_dev = open(m_i2c_dev_path.c_str(), O_RDWR);
        if (m_dev < 0) {
          throw std::runtime_error(std::string("Could not open ") + m_i2c_dev_path);
        }
        DEBUG("Opened i2c device %s.\n", m_i2c_dev_path.c_str());
      }

    public:

      virtual ~Device() {
        if (m_dev >= 0) {
          close(m_dev);
          m_dev = -1;
          DEBUG("Closed i2c device %s.\n", m_i2c_dev_path.c_str());
        }
      }

      static const Device<devid>& get() {
        static std::mutex s_mutex;
        static const Device<devid> device(s_mutex);
        return device;
      }

      void i2cOpen(uint8_t address) {
        // HERE: check dev is not already in use (i2cOpen)
        m_mutex.lock();
        if (ioctl(m_dev, I2C_SLAVE, address) < 0) {
          DEBUG("Could not open i2c address 0x%x.\n", address);
          throw std::runtime_error(std::string("Could not open ") + std::to_string(address));
        }
        DEBUG("Opened i2c address 0x%x.\n", address);
      }

      void i2cClose() {
        // HERE: make dev not in use
        m_mutex.unlock();
      }

      void bufferReset() {
        // NOTE: might also need to protect the buffer
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
  };

}

namespace ssd1306 {

  using Bitmap = std::span<const uint8_t>;
  enum Height {
    HEIGHT_32,
    HEIGHT_64,
  };

  template <uint8_t devid>
  class Display {
    private:
  
      uint8_t m_i2c_address;

      uint8_t m_width = 128;
      uint8_t m_height = 0;

      void initDisplay(const uint8_t* sequence, uint8_t size) {
        auto i2c_device = i2c::Device<devid>::get();
        i2c_device.i2cOpen(m_i2c_address);

        i2c_device.bufferReset();
        uint8_t command[2] = { 0x00, 0x00 };
        for (uint8_t i = 0; i < size; i++) {
          command[1] = sequence[i];
          i2c_device.directWrite(&command[0], sizeof(command));
        }

        i2c_device.i2cClose();
      }

      void setBlock(uint8_t x, uint8_t y, uint8_t w) {
        auto i2c_device = i2c::Device<devid>::get();
        i2c_device.i2cOpen(m_i2c_address);

        i2c_device.bufferReset();
        i2c_device.bufferWrite(0x00);
        i2c_device.bufferWrite(COLUMNADDR);
        i2c_device.bufferWrite(x);
        i2c_device.bufferWrite(w ? (x + w - 1) : (m_width - 1));
        i2c_device.bufferWrite(PAGEADDR);
        i2c_device.bufferWrite(y);
        i2c_device.bufferWrite((m_height >> 3) - 1);
        i2c_device.bufferFlush();
        i2c_device.bufferWrite(0x40);

        i2c_device.i2cClose();
      }

      void writeBitmap(const Bitmap& bitmap) {
        auto i2c_device = i2c::Device<devid>::get();
        i2c_device.i2cOpen(m_i2c_address);

        for (const auto& u: bitmap) {
          i2c_device.bufferWrite(u);
        }

        i2c_device.i2cClose();
      }

    public:

      Display(uint8_t addr, Height height): m_i2c_address(addr) {
        init(height);
      };

      void init(Height height) {
        switch (height) {
          case HEIGHT_32:
            m_height = 32;
            initDisplay(ssd1306_128x32_init_seq, sizeof(ssd1306_128x32_init_seq));
            break;
          case HEIGHT_64:
            m_height = 64;
            initDisplay(ssd1306_128x64_init_seq, sizeof(ssd1306_128x64_init_seq));
            break;
          default:
            throw std::runtime_error("Nope.");
        }
      }

      void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const Bitmap& bitmap) {
        uint8_t j;
        auto i2c_device = i2c::Device<devid>::get();
        i2c_device.i2cOpen(m_i2c_address);

        setBlock(x, y >> 3, w);
        writeBitmap(bitmap);
        i2c_device.bufferFlush();

        i2c_device.i2cClose();
      }

      void clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
        auto i2c_device = i2c::Device<devid>::get();
        i2c_device.i2cOpen(m_i2c_address);

        setBlock(x, y >> 3, w);
        // no need to iterate, write all the fucking way
        for (uint8_t m = (h >> 3); m > 0; m--) {
          for (uint8_t n = w; n > 0; n--) {
            i2c_device.bufferWrite(0x00);
          }
        }
        i2c_device.bufferFlush();

        i2c_device.i2cClose();
      }

      void clear() {
        clear(0, 0, m_width, m_height);
      }
  };
}

#endif // __RPI1306I2C_H__
