#include "ThingSpeak.h"
#include "WiFiEsp.h"
#define RELAY_OFF 1
#define RELAY_ON 0
#define RELAY_1 10
#define RELAY_2 11
#include <SoftwareSerial.h> 
SoftwareSerial BlueT(2, 3);

const int sensorPIR = 7; //Pin Sensor PIR
int statusPIR = 0;
const int sensorApi = A0;
int nilaiApi = 0;

const int pinAlrm = 9;

#define ssid "Mi Phone"
#define pass "1 sampe 8"
#define SECRET_CH_ID 1429491
#define SECRET_WRITE_APIKEY "85FZNA7IJTDKS5MB"
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiEspClient client;

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 5);  // RX, TX
#define ESP_BAUDRATE 9600
#else
#define ESP_BAUDRATE 19200
#endif

//
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//Initialize our Values
//Status nilai sensor PIR
int dataPIR = 0;

String myStatus = "";

void setup() {
  //wifi
  Serial.begin(115200);
  Serial.begin(9600);
  BlueT.begin(9600);
  while (!Serial) {;}
 // initialize serial for ESP Module
  setEspBaudRate(ESP_BAUDRATE);
  while (!Serial) {;}

 Serial.print("Mencari board ESP8266 : ");
 WiFi.init(&Serial1);
 if (WiFi.status() == WL_NO_SHIELD)
 {
   Serial.println("Tidak ditemukan!");
   while (true);
 }
 Serial.println("Berhasil ditemukan :)");
 
 ThingSpeak.begin(client);
  
  pinMode (sensorPIR, INPUT);
  pinMode(pinAlrm, OUTPUT);
  pinMode (RELAY_1, OUTPUT);
  pinMode (RELAY_2, OUTPUT);
  digitalWrite(RELAY_1, RELAY_OFF);
  digitalWrite(RELAY_2, RELAY_OFF);
  delay (10);
} 

void loop() {
  //module wifi
 if(WiFi.status() != WL_CONNECTED)
  {
   Serial.print("Menyambung ke : ");
   Serial.println(ssid);
   while(WiFi.status() !=WL_CONNECTED)
   {
     WiFi.begin(ssid, pass);
     Serial.print("Tunggu sebentar ...");
     delay(5000);
    }
    Serial.println("\nTerhubung.");
  }

//sensor pir
  dataPIR = digitalRead(sensorPIR);
 if((dataPIR == HIGH)&&(statusPIR == LOW))
 {
   Serial.println("Gerakan Terdeteksi!");
   statusPIR = HIGH;
   dataPIR =1;
   digitalWrite(RELAY_1, RELAY_ON);
   delay (5000);
 }
 else{
  if((dataPIR == LOW)&&(statusPIR == HIGH)){
   Serial.println("Tidak Ada Gerakan!");
   statusPIR = LOW;
   dataPIR = 0;
   digitalWrite(RELAY_1, RELAY_OFF);
  }
 }
//Sensor Pendeteksi Api

  nilaiApi = analogRead(sensorApi);
  Serial.println(nilaiApi); 
if (nilaiApi <100)
{
  Serial.println ("Api Terdeteksi");
  digitalWrite(pinAlrm, HIGH);
  delay(1000);
}
  Serial.println("Tidak ada Api");
  digitalWrite(pinAlrm, LOW);
  delay(nilaiApi);
  
  
 //set the fields with the values
  ThingSpeak.setField(1, dataPIR);
  ThingSpeak.setField(2, nilaiApi);
  
 //write to the ThingSpeak channel
 int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
 if(x == 200)
 {
   Serial.println("Status PIR" + String ( dataPIR));
   Serial.println("Status Api" + String ( nilaiApi));
   Serial.println("Channel update successful.");
 }
 else
 {
   Serial.println("Problem updating channel. HTTP error code "+ String(x));
 }
 delay(20000); //wait 20 seconds to update the channel again
 }

void setEspBaudRate(unsigned long baudrate)
{
  long rates[2] = {9600,19200};

 Serial.print("Setting baudrate ESP8266 ke : ");
 Serial.print(baudrate);
 Serial.println("...");

 for(int i = 0; i < 6; i++){
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial.print(",8,1,0,0\r\n");
    delay(100);
  }
 Serial1.begin(baudrate);

 //Modul Bluetooth
  if (BlueT.available())//jika ada data serial dari module bluetooth
  {
    char data1 = BlueT.read();//simpan data itu di variable 'data'
      Serial.println(data1);//print varibel data ke serial monitor
    if (data1 == 'H')//jika mendapat karakter '1'
      {//maka
       digitalWrite(RELAY_2, RELAY_ON);
       delay (10000);
     }
     else if (data1 == 'L')//namun jika mendapat karakter '0'
      {//maka
        digitalWrite(RELAY_2, RELAY_OFF);
     }
    }
 }
