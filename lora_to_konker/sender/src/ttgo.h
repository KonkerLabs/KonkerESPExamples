#ifndef ttgo
#define ttgo

#include <Wire.h>
//#include <SSD1306.h>
//#include <OLEDDisplayUi.h>

#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 868.0

//esp32
//#define RFM95_CS 18
//#define RFM95_RST 14
//#define RFM95_INT 26

//esp8266
#define RFM95_CS 15
#define RFM95_RST 16
#define RFM95_INT 5


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


//  RF95                            => ESP8266 Pin:
//1. ANT                            => Solder to the Antenna provided
//2. GND                            => GND
//3. DIO1                           => D2 / GPIO 4
//4. VCC                            => 3.3V
//5. DIO0 (interrupt request out)   => D1 / GPIO 5
//6. RESET                          => D0 / GPIO 16
//7. SCK  (SPI clock in)            => D5 / GPIO 14
//8. MISO  (SPI Data out)           => D6 / GPIO 12
//9. MOSI  (SPI Data in)            => D7 / GPIO 13
//10. NSS (CS chip select in)       => D8 / GPIO 15


///                 Arduino      RFM95/96/97/98
///                 GND----------GND   (ground in)
///                 3V3----------3.3V  (3.3V in)
/// interrupt 0 pin D2-----------DIO0  (interrupt request out)
///          SS pin D10----------NSS   (CS chip select in)
///         SCK pin D13----------SCK   (SPI clock in)
///        MOSI pin D11----------MOSI  (SPI Data in)
///        MISO pin D12----------MISO  (SPI Data out)

#define LED 2

//SSD1306 display(OLED_ADDRESS, OLED_SDA, OLED_SCL);
//OLEDDisplayUi ui(&display);

unsigned int counter = 0;
#endif