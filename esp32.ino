#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


const char* WLAN_SSID = "123";         
const char* WLAN_PASS = "1029384756";  
const char* mqtt_user="mde_test";
const char* mqtt_pass="mde_test";
const char* mqtt_server="137.184.70.171";
const char* mqtt_topic_voltage="/pebb/voltage";
const char* mqtt_topic_current="/pebb/current";
const char* mqtt_topic_power="/pebb/power";
const char* mqtt_topic_setVoltage="/pebb/setVoltage";
const char* mqtt_topic_setCurrent="/pebb/setCurrent";





WiFiClient espclient;
PubSubClient client(espclient);
char msg[50];

const char* device_name="esp32";
String command;
String vorc;
char outmessage[100];
char outmessagetemp[100];



void setup(){

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  
  Serial.begin(115200);
   setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  }

void setup_wifi() {
 Serial.println();
 Serial.print("connecting to ");
 Serial.println(WLAN_SSID);

 WiFi.begin(WLAN_SSID,WLAN_PASS);
int a=0;
  while (WiFi.status()!=WL_CONNECTED){
    delay(500);
    a++;
    if(a>3){
     ESP.restart();}
   Serial.print(".");
    }
  Serial.println();
  Serial.println("wifi connected");
  Serial.println("IP ");
  Serial.println(WiFi.localIP());
}


void callback(char *topic, byte* message, unsigned int length){
  Serial.print("message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");

  
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.println("PEBB voltage: ");
  tft.setCursor(0, 70);
  tft.println("PEBB current: ");
    tft.setCursor(0, 40);


  
  String messageTemp;
  

for (int i=0; i< length; i++){
Serial.print((char)message[i]);
  
messageTemp +=(char)message[i];
}

if (String(topic) == "/pebb/voltage") {
  tft.print(messageTemp);
} else if (String(topic) == "/pebb/current") {
    tft.setCursor(0, 100);
    tft.print(messageTemp);
  }


Serial.println();

delay(500);


}


void reconnect() {
while (!client.connected()){
Serial.print("attmepting mqtt connection ");
if (client.connect(mqtt_server,mqtt_user,mqtt_pass)){
Serial.println("connected");
client.subscribe(mqtt_topic_voltage);
client.subscribe(mqtt_topic_current);
}else{

Serial.print(client.state());
Serial.println(" try again in 2s");
delay(2000);
}

}

}

void loop(){
if(!client.connected()){
 reconnect();
}
if(Serial.available()){
        command = Serial.readStringUntil('\n');
         command.toCharArray(outmessage, 125);
        if(command.equals("voltage")){
            vorc="voltage";
           }

           if(command.equals("current")){
            vorc="current";
           }

        if (vorc.equals("voltage")){ 
         for(int i=0;i<1001;i++){
        if(command.toInt()==i) {
            client.publish(mqtt_topic_setVoltage,outmessage);

           }
        }
         }
        
        if (vorc.equals("current")){
        for(int i=0;i<501;i++){ 
        if(command.toInt()==i) { 
         client.publish(mqtt_topic_setCurrent,outmessage);
           }
        }
         }
        
        if (command.equals("on")){
          client.publish(mqtt_topic_power,outmessage);
        }
        if (command.equals("off")){
          client.publish(mqtt_topic_power,outmessage);
        }        
        
    }

client.loop();

}















