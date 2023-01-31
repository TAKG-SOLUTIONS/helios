#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include <MsgPack.h>
#include <RTClib.h>


//Volgate and Current
#define VOLTAGE_PIN A0
#define CURRENT_PIN A3

// Init 
RTC_DS3231 rtc;

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


//Raindrop Sensor
#define rd_apin A2


// LDR Sensor
#define ldr_apin A4


//DHT11 Temperature and Humidity Sensor
#define dht_dpin 10
dht DHTSensor;


//DS18B20 Environment Temperature Sensor
// Data wire is conntec to the Arduino digital pin 4
#define en_temp_dpin 11

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(en_temp_dpin);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature EnTempSensor(&oneWire);


// Panel dust checking LDR sensor
#define dust_ldr_apin A1
Servo servo;


//nrf module
#define RF24_CE_PIN 9 
#define RF24_CSN_PIN 8

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN); // CE, CSN
const byte addresses[][6] = {"02019", "02020"};

#define MAX_PANELS 128

typedef struct PanelNumber {
  int x; // Panel position x cordinate
  int y; // y cordinate
  MSGPACK_DEFINE(x, y);
} PanelNumber;


typedef struct Panel {
  PanelNumber id;
  int volt;
  int amp;
  int ldr;
  int temp;
  MSGPACK_DEFINE(id, volt, amp, ldr, temp);
} Panel;


typedef struct EnvironmentStat {
  int temp;
  int humid;
  int raindop;
  MSGPACK_DEFINE(temp, humid, raindop);
} EnvironmentStat;


typedef struct PanelArrayStat {
  int session;
  EnvironmentStat envStat;
  MsgPack::arr_t<Panel> panels;
  uint32_t timestamp;
  MSGPACK_DEFINE(session, envStat, panels, timestamp);
} PanelArrayStat;


void setup() {
    Serial.begin(9600);
    radio.begin();
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
  
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      rtc.adjust(DateTime(2022, 5, 5, 15,43,0));
    }

    radio.openWritingPipe(0xE8E8F0F0E1LL);
    radio.setChannel(0x77);    
    radio.setPALevel(RF24_PA_MIN);
    radio.enableDynamicPayloads();
    
    

    pinMode(rd_apin, INPUT);
    pinMode(ldr_apin, INPUT);
    pinMode(dht_dpin, INPUT);
    
    EnTempSensor.begin();

  

    
}
                                                    
void loop() {

  PanelArrayStat stat;

  PanelNumber id;
  id.x = 0;
  id.y = 1;

  Panel p1;
  p1.id = id;
  p1.volt = int(VoltageSensor()*100);
  p1.amp = int(CurrentSensor()*100);
  p1.ldr = PanelLDRSensor();
  p1.temp = DHTSensorTemperature();

  Serial.println("Panel Stat One Values are :");
  Serial.print(p1.volt);
  Serial.print(" , ");
  Serial.print(p1.amp);
  Serial.print(" , ");
  Serial.print(p1.ldr);
  Serial.print(" , ");
  Serial.print(p1.temp);

  MsgPack::arr_t<Panel> panels;
  panels.push_back(p1);
  PanelNumber id2;
  
  id2.x = 0;
  id2.y = 0;
  p1.id = id2;
  
//  panels.push_back(p1);

  EnvironmentStat env;
  env.temp = int(DSTemperatureSensor()*100);;
  env.humid = DHTSensorHumidity();
  env.raindop = RaindropSensor();

  stat.session = 1;
  stat.panels = panels;
  stat.envStat  =  env;
  DateTime now = rtc.now();
  stat.timestamp = now.unixtime();
  
  radio.stopListening(); // Stop listening so that we can start writing
  

  
  MsgPack::Packer packer;
  packer.serialize(stat);
  
  // Sending data
  radio.write(packer.data(), packer.size());

  Serial.print("\n  Payload Size: ");
  Serial.println(packer.size());

//  unsigned long start_time = micros();      
//  if (! radio.write(packer.data(), packer.size())) {
//    Serial.println(F("Failed to write data"));
//  }

  //Start listening to get data
  radio.startListening();

//  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
//  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
//
//  while ( ! radio.available() ) {                            // While nothing is received
//    if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
//      timeout = true;
//      break;
//    }
//  }
//
//  if(timeout){
//    Serial.println(F("Failed, response timed out."));
//  }
//  else{
//    radio.read(&responseM, sizeof(responseM));
//    Serial.print("Response Message :");
//    Serial.println(responseM);
//    Serial.print("\n");
//    radio.stopListening();
//  }


  
  delay(10000);
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


int RaindropSensor(){
  int RaindropIn = analogRead(rd_apin);
  return RaindropIn;
}


int PanelLDRSensor(){
  int LDRIn = analogRead(ldr_apin);
  return LDRIn;
}


int DHTSensorTemperature(){
  int DHTIn = DHTSensor.read11(dht_dpin);
  return DHTSensor.temperature;
}


int DHTSensorHumidity(){
  int DHTIn = DHTSensor.read11(dht_dpin);
  return DHTSensor.humidity;
}

float DSTemperatureSensor(){
    EnTempSensor.requestTemperatures(); 
    float TempValue = EnTempSensor.getTempCByIndex(0);
    return TempValue;
}

void ServoOpen(){
  
  servo.write(180);
//  Serial.println("Servo Opened");
  delay(2000);
}

void ServoClose(){
  
  servo.write(5);
//  Serial.println("Servo Closed");
  delay(2000);
}

int LDROpenRead(){
    int OpenLDRIn = analogRead(dust_ldr_apin);
//    Serial.print("LDR Open: ");
//    Serial.println(OpenLDRIn);
  return OpenLDRIn;
    delay(100);
}


  
