#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>
#include "./ttgo.h"

char bufferMsg[251];


void scanWifiandPost(){

    rf95.send((uint8_t *) "newscann", 251);
    rf95.waitPacketSent();

    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {

            String ssid = (String)WiFi.SSID(i);
            String rssi = (String)WiFi.RSSI(i);

            Serial.println("ssid: " + ssid);
            Serial.println("rssi: " + rssi);


            char mensagem[251];
            strcpy (mensagem,ssid.c_str());
            strcat (mensagem,":");
            strcat (mensagem,rssi.c_str());



            rf95.send((uint8_t *) mensagem, 251);
            rf95.waitPacketSent();
        }
    }
}



void initDisplay(){
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW); // low to reset OLED
    delay(50);
    digitalWrite(OLED_RST, HIGH); // must be high to turn on OLED

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}

void initLora(){

    initDisplay();

    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init())
    {
        Serial.println("LoRa radio init failed");
        while (1)
            ;
    }
    Serial.println("LoRa radio init OK!");
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
}


void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    initLora();

    Serial.println("Setup finished");
}

void loop(){
    // put your main code here, to run repeatedly:
    Serial.println("loop");
    delay(1000);

    scanWifiandPost();
    
}
