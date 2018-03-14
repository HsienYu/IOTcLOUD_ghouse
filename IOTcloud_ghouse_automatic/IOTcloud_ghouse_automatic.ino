/*
 Created by Igor Jarc
 See http://iot-playground.com for details
 Please use community fourum on website do not contact author directly
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/*
 modify by HYC for skyrock grennHouse project
 * 
 * */

/*pinout between D1 R1 and arduino 
 * 
 * static const uint8_t D0   = 3;
static const uint8_t D1   = 1;
static const uint8_t D2   = 16;
static const uint8_t D3   = 5;
static const uint8_t D4   = 4;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 0;
static const uint8_t D9   = 2;
static const uint8_t D10  = 15;
static const uint8_t D11  = 13;
static const uint8_t D12  = 12;
static const uint8_t D13  = 14;
static const uint8_t D14  = 4;
static const uint8_t D15 = 5;
 * 

*/

#include <ESP8266WiFi.h>
#include "EIoTCloudRestApiV1.0.h"
#include <EEPROM.h>
#include "DHT.h"

//#define DEBUG_PROG 

#ifdef DEBUG_PROG
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINT(x)    Serial.print(x)
#else
  #define DEBUG_PRINTLN(x) 
  #define DEBUG_PRINT(x)
#endif


EIoTCloudRestApi eiotcloud;

// change those lines
#define AP_USERNAME "SkyrockProjects2"
#define AP_PASSWORD "rememberable"
#define INSTANCE_ID "5a605cbb47976c3229e27599"


#define CONFIG_START 0
#define CONFIG_VERSION "v01"

#define REPORT_INTERVAL 60 // in sec

struct StoreStruct {
  // This is for mere detection if they are your settings
  char version[4];
  // The variables of your settings
  char token[41];
  uint moduleId;
  //bool tokenOk; // valid token  
} storage = {
  CONFIG_VERSION,
  // token
  "1234567890123456789012345678901234567890",
  // The default module 0 - invalid module
  0,
  //0 // not valid
};


float oldTemp;
float oldHum;

int treshold_value = 580; // adjust value for soil moisture sensor
int old_soilMH_value;

DHT dht;

String moduleId = "";
String parameterId1 = "";
String parameterId2 = "";
String parameterId3 = ""; 


void setup() {
    Serial.begin(115200);
    DEBUG_PRINTLN("Start...");

    EEPROM.begin(512);
    loadConfig();// for second init commemt out this line to avoid pre-stored data

    eiotcloud.begin(AP_USERNAME, AP_PASSWORD);

    // if first time get new token and register new module
    // here hapend Plug and play logic to add module to Cloud
    if (storage.moduleId == 0)
    {
      // get new token - alternarive is to manually create token and store it in EEPROM
      String token = eiotcloud.TokenNew(INSTANCE_ID);
      DEBUG_PRINT("Token: ");
      DEBUG_PRINTLN(token);
      eiotcloud.SetToken(token);

      // remember token
      token.toCharArray(storage.token, 41);

      // add new module and configure it
      moduleId = eiotcloud.ModuleNew();
      DEBUG_PRINT("ModuleId: ");
      DEBUG_PRINTLN(moduleId);
      storage.moduleId = moduleId.toInt();
      

      // set module type
      bool modtyperet = eiotcloud.SetModulType(moduleId, "MT_GENERIC");
      DEBUG_PRINT("SetModulType: ");
      DEBUG_PRINTLN(modtyperet);
      
      // set module name
      bool modname = eiotcloud.SetModulName(moduleId, "Humidity&Temp sensor");
      DEBUG_PRINT("SetModulName: ");
      DEBUG_PRINTLN(modname);

      // add image settings parameter
      String parameterImgId = eiotcloud.NewModuleParameter(moduleId, "Settings.Icon1");
      DEBUG_PRINT("parameterImgId: ");
      DEBUG_PRINTLN(parameterImgId);

      // set module image
      bool valueRet1 = eiotcloud.SetParameterValue(parameterImgId, "humidity.png");
      DEBUG_PRINT("SetParameterValue: ");
      DEBUG_PRINTLN(valueRet1);
      
      // now add parameter to display temperature
      parameterId1 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter1");
      DEBUG_PRINT("parameterId1: ");
      DEBUG_PRINTLN(parameterId1);

      //set parameter description
      bool valueRet2 = eiotcloud.SetParameterDescription(parameterId1, "Temperature");
      DEBUG_PRINT("SetParameterDescription: ");
      DEBUG_PRINTLN(valueRet2);
      
      //set unit
      // see http://meyerweb.com/eric/tools/dencoder/ how to encode °C 
      bool valueRet3 = eiotcloud.SetParameterUnit(parameterId1, "%C2%B0C");
      DEBUG_PRINT("SetParameterUnit: ");
      DEBUG_PRINTLN(valueRet3);

      //Set parameter LogToDatabase
      bool valueRet4 = eiotcloud.SetParameterLogToDatabase(parameterId1, true);
      DEBUG_PRINT("SetLogToDatabase: ");
      DEBUG_PRINTLN(valueRet4);

      //SetAvreageInterval
      bool valueRet5 = eiotcloud.SetParameterAverageInterval(parameterId1, "10");
      DEBUG_PRINT("SetAvreageInterval: ");
      DEBUG_PRINTLN(valueRet5);


      // now add parameter to display humidity
      parameterId2 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter2");
      DEBUG_PRINT("parameterId2: ");
      DEBUG_PRINTLN(parameterId2);

      //set parameter description
      bool valueRet6 = eiotcloud.SetParameterDescription(parameterId2, "Humidity");
      DEBUG_PRINT("SetParameterDescription: ");
      DEBUG_PRINTLN(valueRet2);
      
      //set unit
      bool valueRet7 = eiotcloud.SetParameterUnit(parameterId2, "%");
      DEBUG_PRINT("SetParameterUnit: ");
      DEBUG_PRINTLN(valueRet7);

      //Set parameter LogToDatabase
      bool valueRet8 = eiotcloud.SetParameterLogToDatabase(parameterId2, true);
      DEBUG_PRINT("SetLogToDatabase: ");
      DEBUG_PRINTLN(valueRet8);

      //SetAvreageInterval
      bool valueRet9 = eiotcloud.SetParameterAverageInterval(parameterId2, "10");
      DEBUG_PRINT("SetAvreageInterval: ");
      DEBUG_PRINTLN(valueRet9);

      // now add parameter to display soil Moisture
      parameterId3 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter3");
      DEBUG_PRINT("parameterId3: ");
      DEBUG_PRINTLN(parameterId3);

      //set parameter description
      bool valueRet10 = eiotcloud.SetParameterDescription(parameterId3, "Soil Moisture");
      DEBUG_PRINT("SetParameterDescription: ");
      DEBUG_PRINTLN(valueRet10);
      
      //set unit
      bool valueRet11 = eiotcloud.SetParameterUnit(parameterId3, "%");
      DEBUG_PRINT("SetParameterUnit: ");
      DEBUG_PRINTLN(valueRet11);

      //Set parameter LogToDatabase
      bool valueRet12 = eiotcloud.SetParameterLogToDatabase(parameterId3, true);
      DEBUG_PRINT("SetLogToDatabase: ");
      DEBUG_PRINTLN(valueRet12);

      //SetAvreageInterval
      bool valueRet13 = eiotcloud.SetParameterAverageInterval(parameterId3, "10");
      DEBUG_PRINT("SetAvreageInterval: ");
      DEBUG_PRINTLN(valueRet13);

      // save configuration
      saveConfig();
    }

    // if something went wrong, wiat here
    if (storage.moduleId == 0)
      delay(1);

    // read module ID from storage
    moduleId = String(storage.moduleId);
    // read token ID from storage
    eiotcloud.SetToken(storage.token);    
    // read Sensor.Parameter1 ID from cloud
    parameterId1 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter1");
    DEBUG_PRINT("parameterId1: ");
    DEBUG_PRINTLN(parameterId1);

    parameterId2 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter2");
    DEBUG_PRINT("parameterId2: ");
    DEBUG_PRINTLN(parameterId2);

    parameterId3 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter3");
    DEBUG_PRINT("parameterId3: ");
    DEBUG_PRINTLN(parameterId3);



    Serial.println();
    Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");

    dht.setup(4); // data pin 4
  
    oldTemp = -1;
    oldHum = -1;

    old_soilMH_value = -1;

    pinMode(13, OUTPUT);// d7 as digital pin 13
    pinMode(16, OUTPUT);// d2 as digital pin 16
}


void loop() {
  delay(dht.getMinimumSamplingPeriod());
  
  float hum = dht.getHumidity();
  float temp = dht.getTemperature();

  int soilMH_value = analogRead(A0);
  soilMH_value = map(soilMH_value, 0, treshold_value, 0, 100);
  
  //digitalWrite(13,HIGH);
  //digitalWrite(16,HIGH);

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(hum, 1);
  Serial.print("\t\t");
  Serial.print(temp, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temp), 1);
  
  Serial.print("SoilMositure:");
  Serial.print(soilMH_value);
  Serial.println("%");

  if(soilMH_value < 60 || hum < 20){
    digitalWrite(13,HIGH);
    delay(5000);
  }else if(temp > 27){
    digitalWrite(16,HIGH);
  }else{
    digitalWrite(16,LOW);
    digitalWrite(13,LOW);
  }
  

  if (temp != oldTemp || hum != oldHum || soilMH_value != old_soilMH_value)
  {
    //sendTeperature(temp);
    eiotcloud.SetParameterValues("[{\"Id\": \""+parameterId1+"\", \"Value\": \""+String(temp)+"\" },{\"Id\": \""+parameterId2+"\", \"Value\": \""+String(hum)+"\" },{\"Id\": \""+parameterId3+"\", \"Value\": \""+String(soilMH_value)+"\" }]");
    oldTemp = temp;
    oldHum = hum;
    old_soilMH_value = soilMH_value;
  }

  int cnt = REPORT_INTERVAL;
  
  while(cnt--)
    delay(1000);
  
}


void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
}


void saveConfig() {
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));

  EEPROM.commit();
}
