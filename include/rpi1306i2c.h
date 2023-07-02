#ifndef __RPI1306I2C_H__
#define __RPI1306I2C_H__

#include <cstdint>
#include <span>

namespace ssd1306 {

using Bitmap = std::span<const uint8_t>;

class Display {
  private:
    uint8_t m_buffer[128];
    uint8_t m_dataSize = 0;
    int m_dev = -1;
    uint8_t m_width = 128;

  protected:
    uint8_t m_height = 0;

    Display(uint8_t dev, uint8_t addr);

    void initDisplay(const uint8_t* sequence, uint8_t size);
    void setBlock(uint8_t x, uint8_t y, uint8_t w);

    void bufferReset();
    void bufferWrite(uint8_t data);
    void bufferWrite(const uint8_t* data, uint8_t size);
    void bufferFlush();

    void directWrite(uint8_t data);
    void directWrite(const uint8_t* data, uint8_t size);

  public:
    ~Display();

    void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const Bitmap& bitmap);
    void clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void clear();
};

class Display128x32: public Display {
  public:
    Display128x32(uint8_t dev, uint8_t addr);
};

class Display128x64: public Display {
  public:
    Display128x64(uint8_t dev, uint8_t addr);
};
}

#endif // __RPI1306I2C_H__