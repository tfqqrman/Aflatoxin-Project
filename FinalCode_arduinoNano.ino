#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
int n = 1;
LiquidCrystal_I2C    lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial espSerial(3,4);//rx, tx
  

//int tblRelayPin = 6; // menentukan tombol untuk relay
int relayPin1 = 7;   // untuk AC
int relayPin2 = 6;// untuk blower
int relayPin3 = 3;
int relayPin4 = 5;
int relayPin5 = 8;
int val =0;     // variable for reading the pin status
int tPlus = 10;
int tMin = 11;
int tEnter = 12;
int setSuhu, setKelembaban, index1, index2, index3, index4, index5, index6, index7, index8, index9;
String str_terimaData, data1, data2, data3, data4, data5, data6, data7, data8, strKirim;
float kelembaban, suhu;
char c;
//===========================================================================================
void setup(){
  Serial.begin(115200);
  espSerial.begin(115200);
  setup_pin();
  Serial.println(F("DHTxx test!"));
  setupLcd();
  dht.begin();
  lcd.setCursor(0,0);
  lcd.print("set suhu:");
  aturSuhu();
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("set lembab:");
  aturKelembaban();
  delay(1000);
  lcd.clear();
  delay(1000);
}

void loop(){
  
  terimaData();//terima data dari nodemcu
  parsingData(); //parsing data
  
  logika();  //baca data 
  kirimDataNodeMCU();//kirim data ke nodemcu  
  Serial.print(data1);Serial.print(" ");Serial.print(data2);Serial.println(data3);
  Serial.println(strKirim);
  
  delay(1000);
  
}


//============================================================================================
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
    index4 = str_terimaData.indexOf('|',index3+1);
    index5 = str_terimaData.indexOf('|',index4+1);
    index6 = str_terimaData.indexOf('|',index5+1);
    index7 = str_terimaData.indexOf('|',index6+1);
    index8 = str_terimaData.indexOf('|',index7+1);
    index9 = str_terimaData.indexOf('|',index8+1);

    data1 = str_terimaData.substring(index1+1, index2);//mode
    data2 = str_terimaData.substring(index2+1, index3);//relay1
    data3 = str_terimaData.substring(index3+1, index4);//relay2
    data4 = str_terimaData.substring(index4+1, index5);//relay3
    data5 = str_terimaData.substring(index5+1, index6);//relay4
    data6 = str_terimaData.substring(index6+1, index7);//relay5
    data7 = str_terimaData.substring(index7+1, index8);//jam
    data8 = str_terimaData.substring(index8+1, index9);//menit
  }
}

void setup_pin(){
  pinMode(relayPin1, OUTPUT);  // deklarasi relay sebagai output
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);   
  pinMode(tPlus, INPUT_PULLUP); // tombol sbg input
  pinMode(tMin, INPUT_PULLUP);
  pinMode(tEnter, INPUT_PULLUP);
  digitalWrite(relayPin1, HIGH);  // turn RELAY OFF
  digitalWrite(relayPin2, HIGH);
}

void aturSuhu(){
  setSuhu = 25;
  int inputTPlus;
  int inputTMin;
  int inputTEnter=12;
  while(inputTEnter>0){
    int inputTPlus = digitalRead(tPlus);
    int inputTMin = digitalRead(tMin);
  
    if (inputTPlus==LOW && inputTMin==HIGH){
      setSuhu=setSuhu+1;
      delay(200);
    }
    else if(inputTPlus==HIGH && inputTMin==LOW){
      setSuhu=setSuhu-1;
      delay(200);
    }
    //else{Serial.print("hey!!!");}
    lcd.setCursor(10,0);
    lcd.print(setSuhu);
    inputTEnter=digitalRead(tEnter);
    
  }
}

void aturKelembaban(){
  int inputTPlus;
  int inputTMin;
  int inputTEnter=12;
  setKelembaban=80;
  while(inputTEnter>0){
    int inputTPlus = digitalRead(tPlus);
    int inputTMin = digitalRead(tMin);
    if (inputTPlus==LOW && inputTMin==HIGH){
      setKelembaban=setKelembaban+1;
      delay(200);
    }
    else if(inputTPlus==HIGH && inputTMin==LOW){
      setKelembaban=setKelembaban-1;
      delay(200);
    }
    //else{Serial.print("hey!!!");}
    lcd.setCursor(12,1);
    lcd.print(setKelembaban);
    inputTEnter=digitalRead(tEnter);
  }
}

void setupLcd(){
  lcd.begin(16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();                  
}

void logika(){
  kelembaban = dht.readHumidity();
  suhu = dht.readTemperature();
  lcd.clear();
  lcd.setCursor(0,1); lcd.print("RT:");lcd.setCursor(3,1);lcd.print(kelembaban);lcd.setCursor(8,1);lcd.print("%");
  lcd.setCursor(10,1);lcd.print("SP:");lcd.setCursor(13,1);lcd.print(setKelembaban);lcd.setCursor(15,1);lcd.print("%");
  lcd.setCursor(0,0);lcd.print("RT:");lcd.setCursor(3,0);lcd.print(suhu);lcd.setCursor(8,0);lcd.print("C");
  lcd.setCursor(10,0);lcd.print("SP:");lcd.setCursor(13,0);lcd.print(setSuhu);lcd.setCursor(15,0);lcd.print("C");
  
  if (data1.toInt()==0){
    Serial.println("auto mode");
    //blower
    if(data7.toInt() >= 6 && data7.toInt() <= 18){
      digitalWrite(relayPin1,LOW);
      digitalWrite(relayPin2,LOW);
    }else{
      digitalWrite(relayPin1,HIGH);
      digitalWrite(relayPin2,HIGH);
    }
    //AC
    if(data7.toInt() >= 18){
      if(suhu>=setSuhu){
        digitalWrite(relayPin3,LOW); //on
      }else if(suhu>=setSuhu-4){
        digitalWrite(relayPin3,LOW);
      }else{
        digitalWrite(relayPin3,HIGH);//off
      }
    }else{
      digitalWrite(relayPin3,HIGH);//off
    }
    //Dehumidifier
    if (kelembaban >= setKelembaban) {         // check if the input is HIGH (button released)
      digitalWrite(relayPin4, LOW);// turn RELAY ON
      digitalWrite(relayPin5, LOW);
    }else if(kelembaban >= setKelembaban-15){
      digitalWrite(relayPin4, LOW);// turn RELAY ON
      digitalWrite(relayPin5, LOW);
    }else{
    digitalWrite(relayPin4, HIGH);// turn RELAY OFF
    digitalWrite(relayPin5, HIGH); 
    }
  }else{
    Serial.println("manual");
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Manual Mode");
    lcd.setCursor(0,1); lcd.print("Periksa monitor");
    if(data2.toInt()==0){
      digitalWrite(relayPin1, HIGH);//off
    }else{
      digitalWrite(relayPin1, LOW);
    }
    if(data3.toInt()==0){
      digitalWrite(relayPin2, HIGH);//off
    }
    else{
      digitalWrite(relayPin2, LOW);
    }
    if(data4.toInt()==0){
      digitalWrite(relayPin3,HIGH);
    }else{
      digitalWrite(relayPin3,LOW);
    }
    if(data5.toInt()==0){
      digitalWrite(relayPin4,HIGH);
    }else{
      digitalWrite(relayPin4,LOW);
    }
    if(data6.toInt()==0){
      digitalWrite(relayPin5,HIGH);
    }else{
      digitalWrite(relayPin5,LOW);
    }
  }
  
}
void kirimDataNodeMCU(){
  strKirim = String("|")+String(kelembaban)+String("|")+String(suhu);
  espSerial.println(strKirim);
}
