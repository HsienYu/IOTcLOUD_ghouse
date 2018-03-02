#include <ESP8266WiFi.h>
#include <MQTT.h>

#define AP_SSID     "SkyrockProjects2"
#define AP_PASSWORD "rememberable"  

#define EIOTCLOUD_USERNAME "***"
#define EIOTCLOUD_PASSWORD "***"

// create MQTT object
#define EIOT_CLOUD_ADDRESS "cloud.iot-playground.com"

#define DO_TOPIC        "/Sensor.Parameter1"

#define PIN_DO_1         D0  // DO pin1 
#define MODULE_ID_1     1

MQTT myMqtt("", EIOT_CLOUD_ADDRESS, 1883);


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);  
  WiFi.begin(AP_SSID, AP_PASSWORD);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(AP_SSID);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };

  Serial.println("WiFi connected");
  Serial.println("Connecting to MQTT server");  

  //set client id
  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);
  myMqtt.setClientId((char*) clientName.c_str());

  Serial.print("MQTT client id:");
  Serial.println(clientName);

  // setup callbacks
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  //////Serial.println("connect mqtt...");
  myMqtt.setUserPwd(EIOTCLOUD_USERNAME, EIOTCLOUD_PASSWORD);  
  myMqtt.connect();

  delay(500);

  pinMode(PIN_DO_1, OUTPUT);  

  subscribe();
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


void subscribe(){
    myMqtt.subscribe("/" + String(MODULE_ID_1) + DO_TOPIC); //DO 1
}


void myConnectedCb() {
  Serial.println("connected to MQTT server");
  subscribe();
}

void myDisconnectedCb() {
  Serial.println("disconnected. try to reconnect...");
  delay(500);
  myMqtt.connect();
}

void myPublishedCb() {
  Serial.println("published.");
}

void myDataCb(String& topic, String& data) {  
  if (topic == String("/"+String(MODULE_ID_1)+ DO_TOPIC))
  {
    if (data == String("1"))
      digitalWrite(PIN_DO_1, HIGH);     
    else
      digitalWrite(PIN_DO_1, LOW);

    Serial.print("Do 1:");
    Serial.println(data);
  }
}



