// c++ headers
#include <cmath>
#include <string>
#include <stdexcept>

// system headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// public headers
#include "rpi1306i2c.h"

// private headers
#include "constants.h"

// Ssd1306Display base class

Ssd1306Display::Ssd1306Display(uint8_t dev, uint8_t addr) {
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

void Ssd1306Display::initDisplay(const uint8_t* sequence, uint8_t size) {
  bufferReset();
  uint8_t command[2] = { 0x00, 0x00 };
  for (uint8_t i = 0; i < size; i++) {
    command[1] = sequence[i];
    directWrite(&command[0], sizeof(command));
  }
}

void Ssd1306Display::setBlock(uint8_t x, uint8_t y, uint8_t w) {
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

void Ssd1306Display::bufferReset() {
  m_dataSize = 0;
}

void Ssd1306Display::bufferWrite(uint8_t data) {
  m_buffer[m_dataSize] = data;
  m_dataSize++;
  if (m_dataSize == sizeof(m_buffer)) {
    bufferFlush();
    bufferWrite(0x40);
  }
}

void Ssd1306Display::bufferWrite(const uint8_t* data, uint8_t size) {
  // todo: faster, copy data in buffer
  while (size--) {
    bufferWrite(*data);
    data++;
  }
}

void Ssd1306Display::directWrite(uint8_t data) {
  directWrite(&data, 1);
}

void Ssd1306Display::directWrite(const uint8_t* data, uint8_t size) {
  if (write(m_dev, data, size) != size) {
    throw std::runtime_error("Could not write on device");
  }
}

void Ssd1306Display::bufferFlush() {
  directWrite(m_buffer, m_dataSize);
  bufferReset();
}

Ssd1306Display::~Ssd1306Display() {
  if (m_dev >= 0) {
    close(m_dev);
    m_dev = -1;
  }
}

void Ssd1306Display::draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) {
  uint8_t j;
  setBlock(x, y >> 3, w);
  for (j = (h >> 3); j > 0; j--) {
    bufferWrite(bitmap, w);
    bitmap += w;
  }
  bufferFlush();  
}

void Ssd1306Display::clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  // TODO
}

void Ssd1306Display::clear() {
  setBlock(0, 0, 0);
  for (uint8_t m = (m_height >> 3); m > 0; m--) {
    for (uint8_t n = m_width; n > 0; n--) {
      bufferWrite(0x00);
    }
  }
  bufferFlush();
}

// Ssd1306Display32 - 128x32

Ssd1306Display32::Ssd1306Display32(uint8_t dev, uint8_t addr) : Ssd1306Display(dev, addr) {
  m_height = 32;
  initDisplay(ssd1306_128x32_init_seq, sizeof(ssd1306_128x32_init_seq));
}

// Ssd1306Display64 - 128x64

Ssd1306Display64::Ssd1306Display64(uint8_t dev, uint8_t addr) : Ssd1306Display(dev, addr) {
  m_height = 64;
  initDisplay(ssd1306_128x64_init_seq, sizeof(ssd1306_128x64_init_seq));
}
