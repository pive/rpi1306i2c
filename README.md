# rpi1306i2c

A minimal C++ library for driving I2C SSD 1306 OLED displays on Rapsberry Pi, without dependencies.
Handles 128x32 and 128x64 screens.

## Disclaimers

- Experimental, unstable, for learning purpose. This repo is made public as it might be useful for a couple of people.
- Yet another SSD1306 lib, why? Mostly because of exisiting libs out there:
    - Much Arduino/ESP32 oriented
    - Not easy to use / install
    - Handling all kinds of other devices, buses, and providing GFX primitives, making them complex
    - Using BCM drivers instead of plain Linux I2C
- Only provides bitmap blit, no GFX primitives, no fonts.
- Based on the excellent work of Alexey Dynda: https://github.com/lexus2k/ssd1306
- Yep the name is awful.

## Work in progress

- Thinking of making it a header-only library
- Optimization of writes/buffer operations
- Use of `std::span` instead of plain byte buffer
- Document usage, bitmap structure
- Proper build flags

## Build and use

```
mkdir build && cd build
cmake ..
make
```

Public include is `include/rpi1306i2c.h`, link with `-lrpi1306i2c`.
See `test.cpp` for usage.

You'll need to have a SSD1306 wired in I2C mode. Use `raspi-config` to enable I2C on Raspberry. Default device is `/dev/i2c-1` and I2C address `0x3C`.
