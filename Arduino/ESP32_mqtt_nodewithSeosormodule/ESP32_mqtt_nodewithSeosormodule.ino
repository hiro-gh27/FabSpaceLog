#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPassword";
const char* mqttServer = "m11.cloudmqtt.com";
const int mqttPort = 12948;
const char* mqttUser = "yourInstanceUsername";
const char* mqttPassword = "yourInstancePassword";

int x_pin = 32;
int y_pin = 33;
int z_pin = 34;

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
}

void loop(){
  StaticJsonBuffer<200> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Accerarete";
  JsonArray& values = JSONencoder.createNestedArray("values");

  values.add(readAccValue(x_pin));
  values.add(readAccValue(y_pin));
  values.add(readAccValue(z_pin));

  char JSONamessageBuffer[100];
  JSONencoder.printTo(JSONamessageBuffer, sizeof(JSONamessageBuffer));
  Serial.println("sending message to MQTT topic...");
  Serial.println(JSONamessageBuffer);

  if(client.publish("esp/test",JSONamessageBuffer) == true) {
    Serial.println("Success sending message");
  }else{
    Serial.println("Error sending message");
  }
  client.loop();
  Serial.println("---------");
  delay(10000);
}

double readAccValue(int pinNum){
  double vdd = 3.3;
  double v_g = vdd / 10;
  double offset = vdd / 2;
  double val = analogRead(pinNum); //read
  double offsetVal = ((val * vdd - offset) / v_g) - 0;
  return offsetVal;
}

