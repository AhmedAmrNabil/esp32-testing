#ifndef PINOUT_H_
#define PINOUT_H_

/// @brief UART0 TX (debug console)
#define TX0 1

/// @brief GPIO2 - Strapping pin. Must be HIGH at boot to avoid boot issues.
#define D2 2

/// @brief UART0 RX
#define RX0 3

/// @brief GPIO4 - Safe GPIO. Often used for LED.
#define D4 4

/// @brief GPIO5 - Strapping pin. Must be HIGH at boot. Often used as VSPI SS (CS).
#define D5 5

// GPIOs 6 to 11 are used for flash memory and should not be used.

/// @brief GPIO12 - Safe GPIO as i burned the efuse on my ESP32.
#define D12 12

/// @brief GPIO13 - Safe GPIO. Often used as HSPI CLK.
#define D13 13

/// @brief GPIO14 - Safe GPIO. Often used as HSPI MISO.
#define D14 14

/// @brief GPIO15 - Strapping pin. Must be HIGH at boot to enable boot messages. Often used as HSPI MOSI.
#define D15 15

// #define RX2 16 // Defined by Arduino core: (gpio_num_t)16
// #define TX2 17 // Defined by Arduino core: (gpio_num_t)17

/// @brief GPIO18 - Safe GPIO. Often used as VSPI CLK (SCK).
#define D18 18

/// @brief GPIO19 - Safe GPIO. Often used as VSPI MISO.
#define D19 19

// GPIO20 does not exist on ESP32

/// @brief GPIO21 - Safe GPIO. Commonly used for I2C SDA.
#define D21 21

/// @brief GPIO22 - Safe GPIO. Commonly used for I2C SCL.
#define D22 22

/// @brief GPIO23 - Safe GPIO. Often used as VSPI MOSI.
#define D23 23

/// @brief GPIO25 - Safe GPIO.
#define D25 25

/// @brief GPIO26 - Safe GPIO.
#define D26 26

/// @brief GPIO27 - Safe GPIO.
#define D27 27

/// @brief GPIO32 - Safe GPIO. Can also be used as analog input.
#define D32 32

/// @brief GPIO33 - Safe GPIO. Can also be used as analog input.
#define D33 33

/// @brief GPIO34 - Input only.
#define D34 34

/// @brief GPIO35 - Input only.
#define D35 35

/// @brief GPIO36 (VP) - Input only. ADC input.
#define VP 36

/// @brief GPIO39 (VN) - Input only. ADC input.
#define VN 39

#endif  // PINOUT_H_
