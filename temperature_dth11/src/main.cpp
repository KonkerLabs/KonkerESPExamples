
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>

#include <NTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN  D4         // DHT11 sensor pin.
#define DHTTYPE  DHT11   

#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0201"

long lasttimeCheck=0;
long dataSendFrenquency=120000; //miliseconds

WiFiUDP ntpUDP;
int16_t utc = -3; //UTC -3:00 Brazil

NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);

DHT_Unified dht(DHTPIN, DHTTYPE);



long getTemperature(){

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    long temp = event.temperature;
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C");
    return temp;
  }
  return -273;
}


long getHumidity(){

  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    long humi = event.relative_humidity;
    Serial.print("humidity: ");
    Serial.print(humi);
    Serial.println("%");
    return humi;
  }
  return -1;
}


void dth11Sensor(){
  if ((millis()-lasttimeCheck) > dataSendFrenquency){
        float tempC = getTemperature();
        float humiC = getHumidity();
        if (tempC ==-273 || humiC==-1){
            return;
        }

        StaticJsonBuffer<220> jsonBuffer;
        JsonObject& jsonMSG = jsonBuffer.createObject();

        delay(10);


        jsonMSG["Temp"] = tempC;
        jsonMSG["Humi"] = humiC;

        jsonMSG.printTo(bufferJ, sizeof(bufferJ));
        char mensagemjson[1024];
        strcpy(mensagemjson,bufferJ);
        pubMQTT("dth11", mensagemjson);


    lasttimeCheck = millis();
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

    

	Serial.println("Setup finished");

	delay(100);
}

void loop(){

    konkerLoop();

    dth11Sensor();


}