#include <stdio.h>
#include <string.h>
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "SD.h"
#include "SPIFFS.h"
//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//struct of sensordatas
struct SensorData{
  String temp;
  String hum;
  String latitude;
  String longitude;  
  bool uploaded; 
};

//datastructure
std::vector<SensorData> datas;
int actualSize = 0;
String ringNumber;
SensorData finalDatas[365];
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
String LoRaData;
//const char* ssid     = "UPC38C38A7-LENT";
//const char* password = "mvzer7wP4raw";
const char* ssid     = "Vodafone-F59D";
const char* password = "uX8jpdwxhbCz";
//const char* serverName = "http://192.168.0.109/post-esp-data.php";
const char* serverName = "http://192.168.0.213/post-esp-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";
int counter = -2;

void processTheData(String LoRaData);
void dataToFinalData();
void postDataToWeb();
void displayPacketRecieve();
void setup() { 
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();
  
  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Initializing OK!");
  display.display();  
  
}

void loop() {
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");
    //LoRa.readBytes((uint8_t *)&datas, packetring lattitude;
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      Serial.println();
    }
    processTheData(LoRaData);
    Serial.println(datas.size());
    Serial.println(actualSize);
    Serial.println(counter);
    dataToFinalData();
    displayPacketRecieve();
    if(actualSize/7 <= counter){
      postDataToWeb();
      counter =-2; 
    }
    counter++;
  }  
}
void displayPacketRecieve(){
  //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("LORA RECEIVER");
    display.setCursor(0,20);
    display.print("Received packet:");
    display.print("RSSI:");
    display.setCursor(30,50);
    display.print(rssi);
    display.display();
}
   
void postDataToWeb(){
  if(WiFi.status()== WL_CONNECTED){
          for(int i=0; i<actualSize;i++){
            
            if(finalDatas[i].uploaded == false){
                HTTPClient http;
                http.begin(serverName);
                http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                String httpRequestData = "api_key=" + apiKeyValue + "&id=" + i +
                                      "&ringnumber=" + ringNumber + "&latitude=" +
                                      finalDatas[i].latitude + "&longitude=" +
                                      finalDatas[i].longitude  +"&temperature=" +
                                      finalDatas[i].temp + "&humidity=" +
                                      finalDatas[i].hum + "";
                Serial.print("httpRequestData: ");
                Serial.println(httpRequestData);
                finalDatas[i].uploaded = true;
                int httpResponseCode = http.POST(httpRequestData);
                if (httpResponseCode>0) {
                  Serial.print("HTTP Response code: ");
                  Serial.println(httpResponseCode);
                }
                else {
                  Serial.print("Error code: ");
                  Serial.println(httpResponseCode);
                }
                // Free resources
                http.end();
            }
            
        }
      }
      else {
        Serial.println("WiFi Disconnected");
      }
      
       for (int i=0;i<actualSize;++i){
          Serial.print(finalDatas[i].latitude);
          Serial.print(',');
          Serial.print(finalDatas[i].longitude);
          Serial.print(',');
          Serial.print(finalDatas[i].temp);
          Serial.print(',');
          Serial.print(finalDatas[i].hum);
          Serial.print(',');
          Serial.print(finalDatas[i].uploaded);
          Serial.print(';');
          Serial.println();
      } 
      Serial.println("Vector deleted");
      datas.clear();
}
void dataToFinalData(){
  for(int i = 0; i<datas.size(); ++i){
        if(datas[i].latitude == finalDatas[i].latitude &&
           datas[i].longitude == finalDatas[i].longitude &&
           datas[i].temp == finalDatas[i].temp &&
           datas[i].hum == finalDatas[i].hum){
             continue;
           } else {
            
             finalDatas[i].latitude = datas[i].latitude;
             finalDatas[i].longitude = datas[i].longitude;
             finalDatas[i].temp = datas[i].temp;
             finalDatas[i].hum = datas[i].hum;
             finalDatas[i].uploaded = false;
             
           } 
       }
}
void processTheData(String LoRaData){
  if(LoRaData.length() == 16){
    ringNumber = LoRaData;
  } else if(LoRaData.length() == 1 || 
            LoRaData.length() == 2 ||
            LoRaData.length() == 3){
      actualSize = LoRaData.toInt();      
  } else {
    std::string str = LoRaData.c_str();
    int i=0;
    struct SensorData sD;
    String tmp = "";
    String tmpLat = "";
    String tmpLong = "";
    String tmpTemp = "";
    String tmpHum = "";
    for(char& c : str) {
      if(c == ','){
        tmpLat = tmp;
        tmp="";
      } else if(c == '*'){
        tmpLong = tmp;
        tmp="";
      } else if(c == '#'){
        tmpTemp = tmp;
        tmp = "";
      } else if(c == ';') {
        tmpHum = tmp;
        tmp="";
        ++i;
        datas.push_back({tmpTemp, tmpHum, tmpLat, tmpLong, false});
        
      } else {
        tmp += c;
      }
    }
  }
}
