//  Include the required libraries:
//#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
//#include <BlynkSimpleEsp32.h>

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
#define SERIAL1_RXPIN 12 //to GPS TX
#define SERIAL1_TXPIN 13 //to GPS RX
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

//struct of sensordatas
struct SensorData{
  String temp;
  String hum;
  String latitude;
  String longitude;  
};
//datastructure
std::vector<SensorData> datas;
const String ID = "EXAMPLE_RING_123";
const int MAX_PACKETS_TO_SEND = 7;
int dataSize;
int GPScounter = 0;
//WidgetMap myMap(V0); 
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);
Adafruit_AM2320 AM2320 = Adafruit_AM2320();

void measuring();
void SendData(int beginning, int ending, std::vector<SensorData> const& aData);
void sendLenghtAndID();
void sendAllData();
void viewTheData();
void goToLightSleep();

void setup() {
  // Begin serial communication at a baud rate of 115200:
  Serial.begin(115200);
  Serial.println("LoRa Sender Test");
  //SPI LoRa pins
  //Temp & Hum sensor
  AM2320.begin();
  Serial.println("LoRa Initializing OK!");
  //GPS
  SerialGPS.begin(9600, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
}

void loop() {
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        if (datas.size() <= 365){
          measuring();
        }
        //SPI LoRa pins
        SPI.begin(SCK, MISO, MOSI, SS);
        //setup LoRa transceiver module
        LoRa.setPins(SS, RST, DIO0);
        if (!LoRa.begin(BAND)) {
          Serial.println("Starting LoRa failed!");
          while (1);
        }
        Serial.println("LoRa Initializing OK!");
        sendLenghtAndID();
        sendAllData();
        Serial.println("Packet sending Done!");
        viewTheData();
        GPScounter = 0;
        goToLightSleep();
     }
     delay(1000);
     GPScounter++;
     Serial.println(".");
     if(GPScounter == 900){
        GPScounter = 0;
        goToLightSleep();
      }
    }
  } 
}
void goToLightSleep(){
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_light_sleep_start();
}
void viewTheData(){
  for(int i=0;i<datas.size();i++){
      Serial.print(datas[i].latitude);
      Serial.print(",");
      Serial.print(datas[i].longitude);
      Serial.print(",");
      Serial.print(datas[i].temp);
      Serial.print(",");
      Serial.print(datas[i].hum);
      Serial.println();
    }
}
void sendLenghtAndID(){
      dataSize = datas.size();
      LoRa.beginPacket();
      LoRa.print(ID);
      LoRa.endPacket();
      delay(1000);
      LoRa.beginPacket();
      LoRa.print(dataSize);
      LoRa.endPacket();
      delay(1000);
}
void measuring(){
  float latitude = gps.location.lat();
  String lat_str = String(latitude , 6);
  float longitude = gps.location.lng();
  String lng_str = String(longitude , 6);
  float temp = AM2320.readTemperature();
  String temp_str = String(temp);
  float hum = AM2320.readHumidity();
  String hum_str = String(hum);
  datas.push_back({temp_str,hum_str,lat_str,lng_str});
}

void sendData(int beginning, int ending, std::vector<SensorData> const& aData){
  LoRa.beginPacket();
  for(int i=beginning;i<ending;++i){
      LoRa.print(aData[i].latitude);
      LoRa.print(",");
      LoRa.print(aData[i].longitude);
      LoRa.print("*");
      LoRa.print(aData[i].temp);
      LoRa.print("#");
      LoRa.print(aData[i].hum);
      LoRa.print(";");
      LoRa.println();
  }
  LoRa.endPacket();
  delay(1000);
}

void sendAllData(){
  int numberOfPackets = datas.size();
  for(int i=0;i<numberOfPackets;i+=7){

    sendData(i,i+MAX_PACKETS_TO_SEND,datas);
  }
  
}
