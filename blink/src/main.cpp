
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>


#define ledPin 2



unsigned long intPeriodoEnvio=10000;


char *type;
char typeC[32];
int presenceCount=0;
bool presenceDetected=0;
unsigned long lasttimeCheck=0;
int ledPin=12;





// byte* payload, unsigned int length or you could use CHANNEL_CALLBACK_SIGNATURE
void ledCallback(byte* payload, unsigned int length){
    int i;
    int state=0;
    char *receivedMsg= new char[length];
    Serial.print("Mensagem recebida [");
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
        digitalWrite(ledPin, state);
        Serial.println("Led : " + String(state));
    }else{ 
        Serial.println("Failed to parse");
    }

}

void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();

    //change flag to true to use encripted wifi password
    konkerConfig((char*)"data.demo.konkerlabs.net:80",(char*)"S0101",false);

    //statusUpdate();

    pinMode(ledPin, OUTPUT);
    
    
    pinMode(presence_pin, INPUT);

	Serial.println("Setup finished");
	//Serial.println("Turning off Wifi");
	//client.disconnect();
	//WiFi.mode(WIFI_OFF);
	delay(1000);


    lasttimeCheck = millis();

}

void loop(){

    konkerLoop();
    delay(100);

    subHttp("led",ledCallback);
    

}