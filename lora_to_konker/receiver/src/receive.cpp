
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>
#include "./ttgo.h"

#define presence_pin  5
#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0101"





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

void pubLora(char *loraMsg, String rssi){

    StaticJsonBuffer<220> jsonBuffer;
    JsonObject& jsonMSG = jsonBuffer.createObject();

    delay(10);

    jsonMSG["deviceId"] = (String)getChipId();
    jsonMSG["lora"] = (String)loraMsg;
    jsonMSG["rssi"] = rssi;

    jsonMSG.printTo(bufferJ, sizeof(bufferJ));
    char mensagemjson[1024];
    strcpy(mensagemjson,bufferJ);
    pubMQTT("lora", mensagemjson);
}

void checkLora(){

    if (rf95.available())
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {
            display.clear();
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);

            display.drawString(0, 0, "Received packet:");
            display.drawString(90, 0, String(counter));
            display.drawString(0, 20, "RSSI:");
            display.drawString(90, 20, String(rf95.lastRssi()));
            display.drawString(0, 40, "SNR:");
            display.drawString(90, 40, String(rf95.lastSNR()));
            display.display();
            counter++;

            int i;
            char *receivedMsg= new char[len];
            Serial.print("Received lora :");
            for (i = 0; i < len; i++) {
                receivedMsg[i] = (char)buf[i];
            }
            receivedMsg[i] = '\0';
            Serial.println("");
            Serial.println("Received lora message: " + String(receivedMsg));
            Serial.println("Sending package to Konker..");
            pubLora(receivedMsg,String(rf95.lastRssi()));


        }
    }
}


void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();
    set_platform_credentials("mqtt.demo.konkerlabs.net", "1883", "<YOUR_DEVICE_USER>", "<YOUR_DEVICE_PASSWORD>", "data");
    setWifiCredentialsNotEncripted("<CLIENT_WIFI>","<CLIENT_WIFI_PASSWORD>");
    //OR IF YOU WHANT MORE THAN ONE WIFI (UP TO 3)
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>");
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>", "<CLIENT_WIFI3>","<CLIENT_WIFI_PASSWORD3>");
    //change flag to true to use encripted wifi password
    //konkerConfig(konkerURL,devicePrefix,false);


    initLora();
    Serial.println("Setup finished");
}

void loop(){
    //22konkerLoop();

    checkLora();
    
}
