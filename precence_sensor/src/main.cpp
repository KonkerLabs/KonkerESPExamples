
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>


#define presence_pin  D3
#define ledPin 2
#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0101"

char status_channel[] = "status";

unsigned long intSendPeriod=10000;


int presenceCount=0;
bool presenceDetected=0;
unsigned long lasttimeCheck=0;


void sendPresences(){
      checkConnections();
      Serial.println("lasttimeCheck=" +(String)lasttimeCheck);
      Serial.println("intSendPeriod=" +(String)intSendPeriod);
      lasttimeCheck = millis();


      StaticJsonBuffer<220> jsonBuffer;
      JsonObject& jsonMSG = jsonBuffer.createObject();


      delay(10);

      jsonMSG["deviceId"] = (String)getChipId();
      jsonMSG["p"] = presenceCount;

      jsonMSG.printTo(bufferJ, sizeof(bufferJ));
      char mensagejson[1024];
      strcpy(mensagejson,bufferJ);
      Serial.println("Publishing at channel:" + (String)status_channel);
      Serial.println("The message:");
      Serial.println(mensagejson);

      if(!pubMQTT(status_channel, mensagejson)){
        appendToFile(healthFile,(char*)"1", _mqttFailureAdress);

        delay(3000);
        ESP.restart();
      }
}



//update presence counter
void presencefunc(){
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
    Serial.print("Message received [");
    Serial.print("led");
    Serial.print("] :");
    for (i = 0; i < length; i++) {
        receivedMsg[i] = (char)payload[i];
    }
    receivedMsg[i] = '\0';

    Serial.println(String(receivedMsg));

    char onoff[8];
    if(parseJSON_data(receivedMsg,"onoff",onoff)){
        bool state =atoi(onoff);
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
    set_platform_credentials("mqtt.demo.konkerlabs.net", "1883", "cab2s3dpg50n", "qhAq0XB1v4vM", "data");
    setWifiCredentialsNotEncripted("Erico","batatinha123");
    //OR IF YOU WHANT MORE THAN ONE WIFI (UP TO 3)
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>");
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>", "<CLIENT_WIFI3>","<CLIENT_WIFI_PASSWORD3>");
    
    //change flag to true to use encripted wifi password
    konkerConfig(konkerURL,devicePrefix,false);




    pinMode(ledPin, OUTPUT);   
    pinMode(presence_pin, INPUT);

	Serial.println("Setup finished");

	delay(1000);
    lasttimeCheck = millis();
}

void loop(){

    konkerLoop();
    delay(100);
    presencefunc();

    subHttp("led",ledCallback);
    
    if ((millis()-lasttimeCheck) > intSendPeriod){

        sendPresences();

        checkForUpdates();

        lasttimeCheck = millis();
        presenceCount=0;
	}
}