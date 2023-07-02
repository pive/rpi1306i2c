#ifndef __RPI1306I2C_H__
#define __RPI1306I2C_H__

#include <cstdint>

// todo: namespace?

class Ssd1306Display {
  private:
    uint8_t m_buffer[128];
    uint8_t m_dataSize = 0;
    int m_dev = -1;
    uint8_t m_width = 128;

  protected:
    uint8_t m_height = 0;

    Ssd1306Display(uint8_t dev, uint8_t addr);

    void initDisplay(const uint8_t* sequence, uint8_t size);
    void setBlock(uint8_t x, uint8_t y, uint8_t w);

    void bufferReset();
    void bufferWrite(uint8_t data);
    void bufferWrite(const uint8_t* data, uint8_t size);
    void bufferFlush();

    void directWrite(uint8_t data);
    void directWrite(const uint8_t* data, uint8_t size);

  public:
    virtual ~Ssd1306Display();

    void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);
    void clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void clear();
};

class Ssd1306Display32: public Ssd1306Display {
  public:
    Ssd1306Display32(uint8_t dev, uint8_t addr);
};

class Ssd1306Display64: public Ssd1306Display {
  public:
    Ssd1306Display64(uint8_t dev, uint8_t addr);
};

#endif // __RPI1306I2C_H__