
#include <Arduino.h>
#include <konkerMQTT.h>
#include <konker.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ds18b20pin  2         // ds18b20 sensor pin.

#define konkerURL (char*)"data.demo.konkerlabs.net:80"
#define devicePrefix (char*)"S0201"

long lasttimeCheck=0;
long dataSendFrenquency=60000; //miliseconds



// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ds18b20pin);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;


long getTemperature(){


  sensors.requestTemperatures();
  float tempC = sensors.getTempC(sensor1);

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" *C");
  return tempC;

}


void sensorAdress(DeviceAddress deviceAddress){
 for (uint8_t i = 0; i < 8; i++) {
   if (deviceAddress[i] < 16) Serial.print("0");
   Serial.print(deviceAddress[i], HEX);
 }
}

void ds18b20Sensor(){
  if ((millis()-lasttimeCheck) > dataSendFrenquency){
        float tempC = getTemperature();

        if (tempC ==-273){
            return;
        }

        StaticJsonBuffer<220> jsonBuffer;
        JsonObject& jsonMSG = jsonBuffer.createObject();

        delay(10);

        jsonMSG["Temp"] = tempC;

        jsonMSG.printTo(bufferJ, sizeof(bufferJ));
        char mensagemjson[1024];
        strcpy(mensagemjson,bufferJ);
        pubMQTT("ds18b20", mensagemjson);


    lasttimeCheck = millis();
  }
}


void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();

    //set_platform_credentials("mqtt.demo.konkerlabs.net", "1883", "48rn8bv0db53", "pUW8gprLaSlJ", "data");
    //setWifiCredentialsNotEncripted("yellow","!yellow2016");
    //OR IF YOU WHANT MORE THAN ONE WIFI (UP TO 3)
    
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>");
    //setWifiCredentialsNotEncripted("<CLIENT_WIFI1>","<CLIENT_WIFI_PASSWORD1>","<CLIENT_WIFI2>","<CLIENT_WIFI_PASSWORD2>", "<CLIENT_WIFI3>","<CLIENT_WIFI_PASSWORD3>");

    //use this for connections without DHCP
    setIp(10,11,182,122);
    setGateway(10,11,182,253);
    setSubnet(255,255,255,0);
    //change flag to true to use encripted wifi password
    konkerConfig(konkerURL,devicePrefix,false);

    //statusUpdate();

  Serial.println("Sensor ds18b20.");
  if (!sensors.getAddress(sensor1, 0))
    Serial.println("Sensor ds18b20 not found !");
  // Show adress
  Serial.print("Sensor ds18b20 adress: ");
  sensorAdress(sensor1);


	Serial.println("Setup finished");

	delay(100);
}

void loop(){

    konkerLoop();

    ds18b20Sensor();


}