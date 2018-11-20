
#include <Arduino.h>
#include <konker.h>


#define on_off_pin  D1
#define door_sensor_pin  D6
#define ledPin 2
#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0302"

char status_channel[] = "status";
unsigned long deltaTime=0;
unsigned long lastStateTime=0;
bool lastDoorState=0;

bool doorState=0;
ADC_MODE(ADC_VCC);

void sendDoorState(bool state){
    deltaTime=millis()-lastStateTime;
    lastStateTime=millis();
    checkConnections();

    StaticJsonBuffer<220> jsonBuffer;
    JsonObject& jsonMSG = jsonBuffer.createObject();


    delay(10);

    jsonMSG["deviceId"] = (String)getChipId();
    jsonMSG["state"] = state;
    if(!state){
        jsonMSG["timeOpen"]=deltaTime;
    }
    float vdd = ESP.getVcc() / 1000.0;
    jsonMSG["voltage"] =vdd;

    jsonMSG.printTo(bufferJ, sizeof(bufferJ));
    char mensagejson[1024];
    strcpy(mensagejson,bufferJ);
    Serial.println("Publishing at channel:" + (String)status_channel);
    Serial.println("The message:");
    Serial.println(mensagejson);

    if(!pubHttp(status_channel, mensagejson)){
    appendToFile(healthFile,(char*)"1", _httpFailureAdress);

    delay(3000);
    ESP.restart();
    }
}



//check door state
void checkDoorSensor(){
  doorState=digitalRead(door_sensor_pin);
  if(doorState!=lastDoorState){
    if(!doorState){
        sendDoorState(false);
        lastDoorState=false;
    }else{
        sendDoorState(true);
        lastDoorState=true;
    }
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
    lastStateTime=millis();

    pinMode(ledPin, OUTPUT);   
    pinMode(door_sensor_pin, INPUT);


	Serial.println("Setup finished");

    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();
    set_platform_credentials("data.demo.konkerlabs.net", "80", "<YOUR_DEVICE_USER>", "<YOUR_DEVICE_PASSWORD>", "data");
    setWifiCredentialsNotEncripted("<CLIENT_WIFI>","<CLIENT_WIFI_PASSWORD>");
    //OR IF YOU WHANT MORE THAN ONE WIFI (UP TO 3)
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>");
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>", "<CLIENT_WIFI3>","<CLIENT_WIFI_PASSWORD3>");
    
    //change flag to true to use encripted wifi password
    konkerConfig(konkerURL,devicePrefix,false);






	delay(1000);
}

void loop(){

    konkerLoop();
    delay(100);
    checkDoorSensor();

    subHttp("led",ledCallback);
    checkForUpdates();

    

}