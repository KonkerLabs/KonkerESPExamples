#ifndef ttgo
#define ttgo

#include <Wire.h>
#include <SSD1306.h>
#include <OLEDDisplayUi.h>

#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 868.0

#define RFM95_CS 18
#define RFM95_RST 14
#define RFM95_INT 26

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// OLED pins to ESP32 GPIOs via this connecthin:
#define OLED_ADDRESS 0x3c
#define OLED_SDA 21  // GPIO4
#define OLED_SCL 22 // GPIO15
#define OLED_RST 16 // GPIO16

#define SCK 5      // GPIO5 - SX1278's SCK
#define MISO 19    // GPIO19 - SX1278's MISO
#define MOSI 27    // GPIO27 - SX1278's MOSI
#define SS 18      // GPIO18 - SX1278's CS
#define RST 14     // GPIO14 - SX1278's RESET
#define DI0 26     // GPIO26 - SX1278's IRQ (interrupt request)
#define BAND 868E6 // 915E6

#define LED 2

SSD1306 display(OLED_ADDRESS, OLED_SDA, OLED_SCL);
OLEDDisplayUi ui(&display);

unsigned int counter = 0;
#endif