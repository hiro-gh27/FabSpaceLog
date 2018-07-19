#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "mac-book-air";
const char* password =  "fabspacefab";
const char* mqttServer = "192.168.2.2";
const int mqttPort = 1883;
const char* mqttUser = "yourInstanceUsername";
const char* mqttPassword = "yourInstancePassword";

/*
const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPassword";
const char* mqttServer = "m11.cloudmqtt.com";
const int mqttPort = 12948;
const char* mqttUser = "yourInstanceUsername";
const char* mqttPassword = "yourInstancePassword";
*/

const int x_pin = 32;
const int y_pin = 33;
const int z_pin = 34;
const int analog_bit = 4096;
const int hz = 1000;
double x_offset;
double y_offset;
double z_offset;

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  Serial.begin(115200);
  Serial.println();
  
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer,mqttPort);

  while(!client.connected()){
    Serial.println("Connecting to mqtt...");

    if(client.connect("ESP32Client",mqttUser,mqttPassword)){
      Serial.println("cconnected");
    }else{
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //calibration
  x_offset = 0+readAccValue(x_pin,0);
  y_offset = 0+readAccValue(y_pin,0);
  z_offset = -9.8+readAccValue(z_pin,0);
  
}

void loop(){
  StaticJsonBuffer<200> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Accerarete";
  JsonArray& values = JSONencoder.createNestedArray("values");

  values.add(readAccValue(x_pin,x_offset));
  values.add(readAccValue(y_pin,y_offset));
  values.add(readAccValue(z_pin,z_offset));
  
  char JSONmessageBuffer[300];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("sending message to MQTT topic...");
  Serial.println(JSONmessageBuffer);

  if(client.publish("esp/test",JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  }else{
    Serial.println("Error sending message");
  }
  client.loop();
  Serial.println("---------");
  delay(hz);
}

double readAccValue(int pinNum,double calib){
  double vdd = 3.3;
  double v_g = vdd / 10;
  double offset = vdd / 2;
  double val = analogRead(pinNum); //read
  double offsetVal = (((val/analog_bit) * vdd - offset) / v_g) - 0;
  return offsetVal - calib;
}

