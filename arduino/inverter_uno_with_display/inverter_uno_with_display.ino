#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
double count=0;

/*
 * PIN connection:
 * pin connection see: https://www.arduino.cc/en/Reference/Wire
 * for UNO: SDA to A4, SCL to A5
 * for Mega2560: SDA to 20, SCL to 21
 * for Leonardo: SDA to 2, SCL to 3
 * for Due: SDA to 20, SCL to 21
 * VCC to 5V
 * GND to GND :-)
 */


// this is the Width and Height of Display which is 128 xy 32 
#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH  128 


#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(2000);
   // Clear the buffer.
  display.clearDisplay(); 
}


void loop() {

  
  String solarPV =  "13.36";
  String batteryV =  "12.45";
  String tempValue = "20";
  
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

  BLevelOne();
  BLevelTwo();
  BLevelThree();
  BLevelFour();
  BLevelFive();
  BLevelSix();



//  displayText("Temprature: 32C", 4, 11, 1, false);
//  displayText("Capacity:   92.86L", 4, 21, 1, false);


  
// display.drawRect(1, 1, 127,31, WHITE);
//  display.drawCircle(63,31, 31, WHITE);
   //display.startscrollright(0x00, 0x0F);
  display.display();
  count +=0.173;



  
   delay(2000); 
   
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
    //Battery Level  belove 20%
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
      //Battery Level 100%
  displayText("_", 122,0,1, false);
  displayText(".", 121,-1,1, false);
  displayText(".", 121,-2,1, false);
}
