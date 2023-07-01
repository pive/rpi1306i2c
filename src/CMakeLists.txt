add_library(rpi1306i2c rpi1306i2c.cpp)

target_include_directories(rpi1306i2c PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/include"
                          )
