#include <cstdint>
#include <unistd.h>

#include <span>
#include <map>
#include <mutex>

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

// class Tca9548a: public ssd1306::Display {
//   private:
//     std::map<uint8_t, ssd1306::Display&> m_displays;
//     std::mutex m_display_lock;
//   public:
//     Tca9548a(uint8_t dev, uint8_t addr): Display(dev, addr) {};

//     void addDisplay(uint8_t slot, ssd1306::Display& display) {
//       m_displays.insert({ slot, display });
//     }

//     void releaseDisplay(uint8_t slot) {
//      }
// };

template <uint8_t device, uint8_t addr>
class Mux {
  public:
    void select(uint8_t channel) {
      auto i2c_device = i2c::Device<device>::get();
      i2c_device.i2cOpen(addr);
      uint8_t buf = 1 << channel;
      i2c_device.directWrite(&buf, sizeof(buf));
      i2c_device.i2cClose();
    };
};

int main(void) {
  Mux mux = Mux<1, 0x70>();
  mux.select(0);
  ssd1306::Display<1> screen(0x3C, ssd1306::HEIGHT_32);
  screen.clear();
  screen.draw(0, 0, 19, 32, bitmap);
  screen.draw(22, 0, 19, 32, bitmap);
  sleep(3);
  screen.clear(0, 0, 19, 32);
  sleep(2);
  screen.clear();
  return 0;
}
