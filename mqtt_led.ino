#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
// put function declarations here:


#define SSID "Wifi" // Wifi name
#define password "12345678" // Wifi passwrd
#define port_mqtt 1883 // MQTT port
#define mqtt_server "mqtt.innoway.vn"
#define mqtt_user "Smarthome" // MQTT username
#define mqtt_pwd "s6z4qI7J5mHhHrQZYTNeI0IzracynXVw" // device token
#define device_id "7f83f0f9-8323-496d-b4ee-0e2621d75315" 


#define MSG_BUFFER_SIZE	(100)
#define led 2
char msg[MSG_BUFFER_SIZE];
WiFiClient client;
PubSubClient Esp32_client(client);
char status[100];
unsigned long pre_time = 0;
long Temperature = 0;
char status_mes[100] = {0};

void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("Message arrived: ");
    Serial.println(topic);
    Serial.print("content: ");
    Serial.write(payload, length);
    Serial.println();  
    for(unsigned int i = 0; i < length; i++){
      status[i] = payload[i];
    }
    status[length] = '\0';
    if(strstr(status, "on"))
    {
      digitalWrite(led, HIGH);
      Serial.println("TURN ON LED");
    }
    else if(strstr(status, "off"))
    {
      digitalWrite(led, LOW);
      Serial.println("TURN OFF LED");
    }
    Serial.println(" ");
}

void init_mqtt(){
    Esp32_client.setServer(mqtt_server, port_mqtt);
    Esp32_client.setCallback(callback);
}

void connect_to_broker(){
  while(!Esp32_client.connected()){
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    Serial.println("Attemping to connect with broker");
    if(Esp32_client.connect(clientId.c_str(),mqtt_user,mqtt_pwd)){
      Serial.println("Connected Success");
      // Esp32_client.publish("device/temp1","MQTT server is connected");
      // Esp32_client.subscribe("Temp_node1");
      // Esp32_client.subscribe("Hum_node1");
      // Esp32_client.subscribe("Temp_node2");
      char subTopic[100];
      sprintf(subTopic, "messages/%s/control", device_id);
      Serial.println(subTopic);
      Esp32_client.subscribe(subTopic);
    }
    else{
      Serial.print("Connect fail");
      Serial.println(Esp32_client.state());
      Serial.print("Try again in 1 seconds");
      delay(1000);
    }
  }
}

void init_wifi(){
  Serial.print("Connecting status: ");
  Serial.println(WiFi.status());
  Serial.print("Connecting to:");
  Serial.println(SSID);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(SSID,password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
  }
  Serial.println("");
  Serial.printf("Connected successful to SSID: %s\n",SSID);
  Serial.print("IP Address of this device: ");
  Serial.println(WiFi.localIP());
  pinMode(led, OUTPUT);
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    init_wifi();
    init_mqtt();
    connect_to_broker();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!Esp32_client.connected()){
      connect_to_broker();
  }
  // if(millis() - pre_time > 2000){
  //     Temperature = random(20,30);
  //     sprintf(status_mes,"{\"Temperature\":\"%d\"}",Temperature);
  //     char pubTopic[100];
  //     sprintf(pubTopic, "messages/%s/temp", device_id);
  //     Esp32_client.publish(pubTopic,status_mes);
  //     Serial.println("Publish Temperature");
  //     pre_time = millis();
  // }
  Esp32_client.loop();
}