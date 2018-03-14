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

#include <ESP8266WiFi.h> //library for the WeMos Board
#include "DHT.h" //Library for the Temperature and humidity sensor

float oldTemp; //not relevant
float oldHum;  //not relevant

int treshold_value = 580; // adjust value for soil moisture sensor
int old_soilMH_value; // not relevant

DHT dht; //dht variable from the library



void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");

    dht.setup(4); // data pin 4
  
    oldTemp = -1; // not relevant
    oldHum = -1; // not relevant

    old_soilMH_value = -1;

    pinMode(13, OUTPUT);// d7 as digital pin 13
    pinMode(16, OUTPUT);// d2 as digital pin 16
}

//the block of clode below is used to output the data.

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

//Task - Students need to explain the system control below.

  if(soilMH_value < 60 || hum < 20){
    digitalWrite(13,HIGH);
    delay(5000);
  }else if(temp > 27){
    digitalWrite(16,HIGH);
  }else{
    digitalWrite(16,LOW);
    digitalWrite(13,LOW);
  }
  

  
}
