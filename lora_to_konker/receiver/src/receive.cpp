
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>
#include "./ttgo.h"

#define presence_pin  5



char status_channel[] = "status";

unsigned long intPeriodoEnvio=10000;


int CURRENT_FIRMWARE_VERSION=1;


char *type;
char typeC[32];
int presenceCount=0;
bool presenceDetected=0;
unsigned long lasttimeCheck=0;
int statusLed=15;


void enviaPresencas(){
      checkConnections();
      //Serial.println("lasttimeCheck=" +(String)lasttimeCheck);
      //Serial.println("intPeriodoEnvio=" +(String)intPeriodoEnvio);
      lasttimeCheck = millis();


      StaticJsonBuffer<220> jsonBuffer;
      JsonObject& jsonMSG = jsonBuffer.createObject();


      delay(10);

      jsonMSG["deviceId"] = (String)getChipId();
      jsonMSG["p"] = presenceCount;

      jsonMSG.printTo(bufferJ, sizeof(bufferJ));
      char mensagemjson[1024];
      strcpy(mensagemjson,bufferJ);
      Serial.println("Publicando no canal:" + (String)status_channel);
      Serial.println("A mensagem:");
      Serial.println(mensagemjson);

      pubMQTT(status_channel, mensagemjson);

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
    pubMQTT(status_channel, mensagemjson);
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
            Serial.println("Received lora message: " + String(receivedMsg));
            Serial.println("Sending package to Konker..");
            pubLora(receivedMsg,String(rf95.lastSNR()));


            digitalWrite(LED, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(100);           // wait for a second
            digitalWrite(LED, LOW);  // turn the LED off by making the voltage LOW			
            delay(100);           // wait for a second
        }
    }
}


//atualiza contador de presença
void presencafunc(){
  if (digitalRead(presence_pin) == LOW){

		if(presenceDetected==0){
			presenceCount=presenceCount+1;
			Serial.println("presenceCount=" + (String)presenceCount);
		}
		presenceDetected=1;
  }else {
		presenceDetected=0;
  }
}


// byte* payload, unsigned int length or you could use CHANNEL_CALLBACK_SIGNATURE
void ledCallback(byte* payload, unsigned int length){
    
    int i;
    int state=0;
    char *receivedMsg= new char[length];
    Serial.print("Received message [");
    Serial.print("led");
    Serial.print("] :");
    for (i = 0; i < length; i++) {
        receivedMsg[i] = (char)payload[i];
    }
    receivedMsg[i] = '\0';

    Serial.println(String(receivedMsg));
    
    char ligado[8];
    if(parseJSON_data(receivedMsg,"ligado",ligado)){
        bool state =atoi(ligado);
        digitalWrite(statusLed, state);
        Serial.println("Led : " + String(state));
    }else{ 
        Serial.println("Failed to parse");
    }

   //dont remove these lines below. Risk of Out Of Memory. Since it is used-> char *receivedMsg= new char[length];
   //delete receivedMsg;
   //receivedMsg = NULL;
}




void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();

    //change flag to true to use encripted wifi password
    konkerConfig((char*)"data.staging.konkerlabs.net:80",(char*)"S0101",false);

    pinMode(statusLed, OUTPUT);    
    pinMode(presence_pin, INPUT);

    lasttimeCheck = millis();
    initLora();
    Serial.println("Setup finished");
}

void loop(){
    konkerLoop();

    checkLora();
    
}
