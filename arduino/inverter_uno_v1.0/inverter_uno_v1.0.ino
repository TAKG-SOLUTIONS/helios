#include <SPI.h>
#include <dht.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// this is the Width and Height of Display which is 128 xy 32 
#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH  128 

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


//Volgate and Current
#define SOLAR_VOLTAGE_PIN A1
#define BATTERY_VOLTAGE_PIN A0
#define SOLAR_CURRENT_PIN A2

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


#define buzzer 13


#define array_size 4

//DHT11 Temperature and Humidity Sensor
#define dht_dpin 7
dht DHTSensor;


void setup() {
  Serial.begin(9600);

  pinMode(buzzer,OUTPUT);
  SetupSound();

  pinMode(dht_dpin, INPUT);
  
   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();

}

void loop() {

  float solarVoltage = SolarVoltageSensor();
  float batteryVoltage = BatteryVoltageSensor();
  float solarAmp = SolarCurrentSensor();
  float inverterTemp = float(DHTSensorTemperature());

  float arr[array_size] = {solarVoltage, batteryVoltage, solarAmp, inverterTemp};

  for(int i = 0; i < array_size; i++){
    Serial.print(arr[i]);
    Serial.print(",");
  }
  Serial.println("");

  String solarPV =  String(solarVoltage);
  String batteryV =  String(batteryVoltage);
  String tempValue = String(int(inverterTemp));
  
  
  display.clearDisplay();
  
  displayText("> SOLAR PV: ", 1, 4, 1, false);
  displayText(solarPV, 78, 4, 1, false);
  displayText("V", 112, 4, 1, false);
  
  displayText("> BATTERY V:", 1,15,1, false);
  displayText(batteryV, 78, 15, 1, false);
  displayText("V",112, 15, 1, false);

  displayText("> TEMPERATURE:", 1,25,1, false);
  displayText(tempValue, 90, 25, 1, false);
  displayText(".",107, 20, 1, false);
  displayText("C",112, 25, 1, false);

  if(batteryVoltage >= 13.00){
      BLevelOne();
      BLevelTwo();
      BLevelThree();
      BLevelFour();
      BLevelFive();
      BLevelSix();
  }
  else if(batteryVoltage >= 12.00){
      BLevelOne();
      BLevelTwo();
      BLevelThree();
      BLevelFour();
      BLevelFive();
  }
  else if(batteryVoltage >= 11.50){
      BLevelOne();
      BLevelTwo();
      BLevelThree();
      BLevelFour();
  }
  else if(batteryVoltage >= 11.00){
      BLevelOne();
      BLevelTwo();
      BLevelThree();
  }
  else if(batteryVoltage >= 10.00){
      BLevelOne();
      BLevelTwo();
      BatteryLevelWarningOne();
  }
  else{
      BLevelOne();
      BatteryLevelWarningTwo();
  }

  

  display.display();
  

  delay(10000);
  
}


float SolarVoltageSensor(){
     // Read the Analog Input
   sensor_value = analogRead(SOLAR_VOLTAGE_PIN);
   
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


float BatteryVoltageSensor(){
     // Read the Analog Input
   sensor_value = analogRead(BATTERY_VOLTAGE_PIN);
   
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


float SolarCurrentSensor() {
  unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(SOLAR_CURRENT_PIN);     //Read current sensor values   
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


int DHTSensorTemperature(){
  int DHTIn = DHTSensor.read11(dht_dpin);
  return DHTSensor.temperature;
}


/*
 * displayText(String text, int x, int y,int size, boolean d)
 * text is the text string to be printed
 * x is the integer x position of text
 * y is the integer y position of text
 * z is the text size, 1, 2, 3 etc
 * d is either "true" or "false". Not sure, use true
 */
void displayText(String text, int x, int y,int size, boolean d) {

  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
  display.println(text);
  if(d){
    display.display();
  }
  
  //delay(100);
}


void BLevelOne(){
    //Battery Level belove 20%
  displayText("_", 122,25,1, false);
  displayText("_", 122,24,1, false);
  displayText("_", 122,23,1, false);
  displayText("_", 122,22,1, false);
}

void BLevelTwo(){
    //Battery Level 20% - 40%
  displayText("_", 122,20,1, false);
  displayText("_", 122,19,1, false);
  displayText("_", 122,18,1, false);
  displayText("_", 122,17,1, false);
}

void BLevelThree(){
    //Battery Level 40% - 60%
  displayText("_", 122,15,1, false);
  displayText("_", 122,14,1, false);
  displayText("_", 122,13,1, false);
  displayText("_", 122,12,1, false);
}

void BLevelFour(){
    //Battery Level 60% - 80%
  displayText("_", 122,10,1, false);
  displayText("_", 122,9,1, false);
  displayText("_", 122,8,1, false);
  displayText("_", 122,7,1, false);
}

void BLevelFive(){
    //Battery Level 80% - 100%
  displayText("_", 122,5,1, false);
  displayText("_", 122,4,1, false);
  displayText("_", 122,3,1, false);
  displayText("_", 122,2,1, false);
}

void BLevelSix(){
      //Battery Level 100% and Charging
  displayText("_", 122,0,1, false);
  displayText(".", 121,-1,1, false);
  displayText(".", 121,-2,1, false);
}


void SetupSound(){
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
 delay(40);
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
 delay(40);
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
}

void BatteryLevelWarningOne(){
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
}

void BatteryLevelWarningTwo(){
 digitalWrite(buzzer,HIGH);
 delay(300);
 digitalWrite(buzzer,LOW);
}

void HighLoadWarning(){
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
 delay(120);
 digitalWrite(buzzer,HIGH);
 delay(80);
 digitalWrite(buzzer,LOW);
}
