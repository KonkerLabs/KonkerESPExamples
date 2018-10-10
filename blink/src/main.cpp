
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>


#define ledPin 2
#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0101"



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

    set_platform_credentials("mqtt.demo.konkerlabs.net", "1883", "<YOUR_DEVICE_USER>", "<YOUR_DEVICE_PASSWORD>", "data");
    setWifiCredentialsNotEncripted("<CLIENT_WIFI>","<CLIENT_WIFI_PASSWORD>");
    //OR IF YOU WHANT MORE THAN ONE WIFI (UP TO 3)
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>");
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>", "<CLIENT_WIFI3>","<CLIENT_WIFI_PASSWORD3>");

    //change flag to true to use encripted wifi password
    konkerConfig(konkerURL,devicePrefix,false);

    //statusUpdate();

    pinMode(ledPin, OUTPUT);
    

	Serial.println("Setup finished");

	delay(100);
}

void loop(){

    konkerLoop();
    delay(100);

    subHttp("led",ledCallback);

}