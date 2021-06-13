#include<SoftwareSerial.h>
#include <NTPClient.h>          
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

String str_terimaData, data1, data2,strKirim, linkTerima, linkKirim;
int index1, index2, index3, mode_1, relay_1, relay_2,relay_3,relay_4,relay_5, suhu, kelembaban;
char c;
unsigned int jam, menit, detik;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"time.nist.gov",25200,6000);
const char *ssid = "Maerasari IV";
const char *pass = "t1suwajah";


//==========================================================================================
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  //set up connection
  WiFi.begin(ssid, pass);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  waktu();
  terimaDataWeb(); //terima data dari web
  kirimDataArduino(); //kirim data ke arduino
  terimaData(); //terima data dari arduinno
  parsingData(); //parsingData
  kirimDataWeb();//kirim ke web
  Serial.print(data1);Serial.println(" ");Serial.println(data2);
  delay(1000); 
}


//=======================================FUNCTION LIST======================================
void terimaData(){
  str_terimaData="";
  while(Serial.available()>0){
    c = (char)Serial.read();
    str_terimaData += c;
  }
}

void parsingData(){
  if(str_terimaData.length()>0){
    index1 = str_terimaData.indexOf('|');
    index2 = str_terimaData.indexOf('|',index1+1);
    index3 = str_terimaData.indexOf('|',index2+1);

    data1 = str_terimaData.substring(index1+1,index2);//kelembaban
    data2 = str_terimaData.substring(index2+1,index3);//suhu

    suhu = data2.toInt();
    kelembaban = data1.toInt();
  }
}

void terimaDataWeb(){
  HTTPClient http;
  linkTerima = "http://kementan-undip-riset.org/jagung/index.php/API";
  http.begin(linkTerima);
  int httpCode = http.GET();
  if (httpCode>0){
    const size_t bufferSize = JSON_OBJECT_SIZE(2)+370;
    DynamicJsonDocument jsonBuffer(bufferSize);
    //JsonObject& root = jsonBuffer.parseObject(http.getString());
    deserializeJson(jsonBuffer,http.getString());
     mode_1 = jsonBuffer["mode"];
     relay_1 = jsonBuffer["relay1"];
     relay_2 = jsonBuffer["relay2"]; 
     relay_3 = jsonBuffer["relay3"];
     relay_4 = jsonBuffer["relay4"];
     relay_5 = jsonBuffer["relay5"];
}}

void kirimDataArduino(){
  strKirim = String("|")+String(mode_1)+String("|")+String(relay_1)+String("|")+String(relay_2)+String("|")+String(relay_3)+String("|")+String(relay_4)+String("|")+String(relay_5)+String("|")+String(jam)+String("|")+String(menit);
  Serial1.println(strKirim);
}

void kirimDataWeb(){
    HTTPClient http;
    linkKirim = "http://kementan-undip-riset.org/jagung/index.php/API/save/"+String(suhu)+"/"+String(kelembaban)+"/0/0/0/0/0/0/4";           //"http://cossmit.my.id/tambak2/index.php/API2/save/11/22/33/44/55/66/77/88/99/1010";
    http.begin(linkKirim);
    //delay(5000);
    int httpCode = http.GET();
    //delay(5000);
    String payLoad = http.getString();
    //delay(3000);
    //Serial.println(httpCode);
    //Serial.println(payLoad);
    //delay(1000);
    http.end();
  }

void waktu(){
  timeClient.update();
  jam=timeClient.getHours();
  menit=timeClient.getMinutes();
  detik=timeClient.getSeconds();
}
