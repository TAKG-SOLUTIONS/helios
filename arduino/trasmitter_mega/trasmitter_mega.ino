#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



//Volgate and Current

#define VOLTAGE_PIN A0
#define CURRENT_PIN A3

// Floats for ADC voltage & Input voltage
float cal_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 

// Float for Reference Voltage
float ref_voltage = 5.0;

// Integer for ADC value
int sensor_value = 0;


//nrf module
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

double Sum;

void setup() {
    radio.begin();
    radio.openWritingPipe(addresses[0]); // 00001
    radio.openReadingPipe(1,addresses[1]); // 00002
    radio.setPALevel(RF24_PA_MIN);
    Serial.begin(9600);
}
                                                    
void loop() {

  struct PanelStat PSone;

  PSone.PanelVoltage = VoltageSensor();
  PSone.PanelAmpere = CurrentSensor();

  radio.stopListening();
  
  Serial.print("Two values sent are :");
  Serial.print(PSone.PanelVoltage);
  Serial.print(" , ");
  Serial.println(PSone.PanelAmpere);
  
  // Sending data
  radio.write(&PSone, sizeof(PSone));

  unsigned long start_time = micros();      
  if (!radio.write( &PSone, sizeof(PSone) )) {
    Serial.println(F("Failed"));
  }

  //Start listening to get data
  radio.startListening();

  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

  while ( ! radio.available() ) {                            // While nothing is received
    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  }

  if(timeout){
    Serial.println(F("Failed, response timed out."));
  }
  else{
    radio.read(&Sum, sizeof(Sum));
    Serial.print("Sum of the given numbers is :");
    Serial.println(Sum);
    Serial.print("\n");
    radio.stopListening();
  }
  
//  delay(1000);
  }

float VoltageSensor(){
     // Read the Analog Input
   sensor_value = analogRead(VOLTAGE_PIN);
   
   // Determine voltage at ADC input
   cal_voltage  = (sensor_value * ref_voltage) / 1024.0; 
   
   // Calculate voltage at divider input
   in_voltage = cal_voltage / (R2/(R1+R2)) ; 
   
   // Print results to Serial Monitor to 2 decimal places
//  Serial.print("Input Voltage = ");
//  Serial.println(in_voltage, 2);

  return in_voltage;
  
  // Short delay
//  delay(500);
}

float CurrentSensor() {
  unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(CURRENT_PIN);     //Read current sensor values   
  Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/150.0;//Taking Average of Samples

//((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
//2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
//out to be 2.5 which is out offset. If your arduino is working on different voltage than 
//you must change the offset according to the input voltage)
//0.100v(100mV) is rise in output voltage when 1A current flows at input
AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.100;

//Serial.print("Input Current = ");
//Serial.println(AcsValueF);//Print the read current on Serial monitor

return AcsValueF;
//delay(500);

}
  
