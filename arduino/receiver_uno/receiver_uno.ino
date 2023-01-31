#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Time.h>
#include <TimeLib.h>


#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  255   // Header tag for serial time sync message


#define RF24_CE_PIN 9 
#define RF24_CSN_PIN 8

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN); // CE, CSN
const byte addresses[][6] = {"02019", "02020"};



struct PanelStat{
  float PanelVoltage;
  float PanelAmpere;
  int Raindrop;
  int PanelLDR;
  int PanelTemp;
  int PanelHumi;
  float Temperature;
//  int DustCheckLDR;
};

struct PanelArrayOneStat{
  float AOnePanelVoltage;
  float AOnePanelAmpere;
  int EnRaindrop; //En - Environment
  int AOnePanelLDR;
  int AOnePanelTemp;
  int AOnePanelHumi;
  float EnTemperature;
//  int AoneDustCheckLDR;
};


char responseMsg[] = "OK" ;
bool responseM = 1; 

void setup() {
   Serial.begin(9600);
   Serial.println("Code is here start ");
   radio.begin();
   radio.openReadingPipe(1, addresses[0]); // 00001
   radio.openWritingPipe(addresses[1]); // 00002
   radio.setPALevel(RF24_PA_MIN);
   radio.setChannel(0x66);


  }
  
void loop() {
  struct PanelStat PSone;
  struct PanelArrayOneStat PAOS; 
  
   radio.startListening();
//   while (!radio.available());

  if(radio.available()){
       radio.read(&PSone, sizeof(PSone));

   
   PAOS.AOnePanelVoltage = PSone.PanelVoltage;
   PAOS.AOnePanelAmpere = PSone.PanelAmpere;
   PAOS.EnRaindrop = PSone.Raindrop;
   PAOS.AOnePanelLDR = PSone.PanelLDR;
   PAOS.AOnePanelTemp = PSone.PanelTemp;
   PAOS.AOnePanelHumi = PSone.PanelHumi;
   PAOS.EnTemperature = PSone.Temperature;
   
   
   Serial.println("Recieved numbers : ");;
   Serial.print(PAOS.AOnePanelVoltage);
   Serial.print(" , ");
   Serial.print(PAOS.AOnePanelAmpere);
   Serial.print(" , ");
   Serial.print(PAOS.EnRaindrop);
   Serial.print(" , ");
   Serial.print(PAOS.AOnePanelLDR);
   Serial.print(" , ");
   Serial.print(PAOS.AOnePanelTemp);
   Serial.print(" , ");
   Serial.print(PAOS.AOnePanelHumi);
   Serial.print(" , ");
   Serial.println(PAOS.EnTemperature);
   
   radio.stopListening();
   
   Serial.println("Response Sent");
   Serial.print("\n");
   radio.write(&responseM, sizeof(responseM));
  }
  
//  else{
//    Serial.println("Time out\n");
//  }
   
  }


void digitalClockDisplay()
{
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();  
}
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);  
}

  
