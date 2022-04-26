#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RF24_CE_PIN 9 
#define RF24_CSN_PIN 8

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

struct PanelStat{
  float PanelVoltage;
  float PanelAmpere;
//  int CoveredLDR;
//  int Raindrop;
//  int OpenedLDR;
//  int DHT;
//  float EnTemperature;
};


float Num1;
float Num2;
double Sum;

void setup() {
   Serial.begin(9600);
   Serial.println("Code is here start ");
   radio.begin();
   radio.openReadingPipe(1, addresses[0]); // 00001
   radio.openWritingPipe(addresses[1]); // 00002
   radio.setPALevel(RF24_PA_MIN);
  }
  
void loop() {
  struct PanelStat PSone;
  
   radio.startListening();
//   while (!radio.available());

  if(radio.available()){
       radio.read(&PSone, sizeof(PSone));

   
   Num1 = PSone.PanelVoltage;
   Num2 = PSone.PanelAmpere;
   
   Serial.print("Recieved 2 numbers : ");
   Serial.print(Num1);
   Serial.print(" , ");
   Serial.println(Num2);
   
   radio.stopListening();
   
   Sum = Num1 + Num2;
   Serial.println("Result Sent");
   Serial.print("\n");
   radio.write(&Sum, sizeof(Sum));
  }
  
  else{
    Serial.println("Time out\n");
  }
   
  }
