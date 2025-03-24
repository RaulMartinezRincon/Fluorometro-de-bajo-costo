/*
Low cost laser fluorometer

AS7341 (3.3V)
  SLC = 22;
  SDA = 21;

Laser (3.3V)

OLED (3.3V)
  SLC = 22;
  SDA = 21;

RTC (3.3V)
  SLC = 22;
  SDA = 21;

MicroSD (3.3V)
  CS    = 5;
  SCK   = 18;
  MOSI  = 23;
  MISO  = 19;  

*/
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Libraries for RTC
#include "RTClib.h"

// Libraries for AS7341
#include "DFRobot_AS7341.h"
#include <Wire.h>

// Libraries for OLED 
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO

// OLED
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// AS7341
DFRobot_AS7341 as7341;

// RTC
RTC_DS3231 rtc;

#define delayTime 5 // time in seconds between each measurement

const int SW_pin = 1; // digital pin connected to switch output
int lastState = HIGH;
int currentState; 

// Variables to hold RTC
String date;
String hour;

// Variables to hold AS7341
int f1;
int f2;
int f3;
int f4;
int f5;
int f6;
int f7;
int f8;
int nir;
int Clear;

// Variables to hold count
int id = 0;

int LaserRelay = 3;

String dataMessage;

// Initialize SD
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
}

// Initialize RTC card
void initRTC() {
  if (! rtc.begin()) {
  Serial.println("Couldn't find RTC");
  while (1);
  }

  // automatically sets the RTC to the date & time on PC this sketch was compiled
  // Run once for setting, then leave it as comment and rerun code, otherwise rct stamps the same date and hour!!!
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  Serial.begin(9600);

  pinMode(SW_pin, INPUT_PULLUP);

  initSDCard(); 
  
  initRTC();

  while (as7341.begin() != 0) {
    Serial.println("IIC init failed, please check if the wire connection is correct");
    delay(1000);
  }

  delay(250);
  display.begin(i2c_Address, true);

  display.display();
  delay(1000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(10, 0);
  display.print("MOAM LAB - CIBNOR");

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Estoy listo!!!!"); 

  display.setCursor(0, 40);
  display.print("Schiaccia il tasto rosso!"); 
  
  display.display();
  delay(1000);
  display.clearDisplay();
  
  File file = SD.open("/Box_Fluorometer_data.csv");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/Box_Fluorometer_data.csv", "Date (GMT-7), Hour (GMT-7), Time (s), F1 (405-425nm), F2 (435-455nm), F3 (470-490nm), F4 (505-525nm), F5 (545-565nm), F6 (580-600nm), F7 (620-640nm), F8 (670-690nm), NIR, Clear \r\n");

  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  pinMode(LaserRelay, OUTPUT);
  digitalWrite(LaserRelay, 1);

}

void loop() {

  // Trigger when you push the RED BUTTOM
  currentState = digitalRead(SW_pin);

  if(lastState == LOW && currentState == HIGH){

    for(int j = 1; j < 11; j++) {

      Serial.print("Lectura No.: ");
      Serial.println(j);

      digitalWrite(LaserRelay, 0); 

      // Get RTC readings
      DateTime now = rtc.now();
      date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day());
      hour = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

      // Get AS7341 readings
      DFRobot_AS7341::sModeOneData_t data1;
      DFRobot_AS7341::sModeTwoData_t data2;

      as7341.startMeasure(as7341.eF1F4ClearNIR);
      data1 = as7341.readSpectralDataOne();
      
      as7341.startMeasure(as7341.eF5F8ClearNIR);
      data2 = as7341.readSpectralDataTwo();

      id = id + delayTime;

      f1 = data1.ADF1;
      f2 = data1.ADF2;
      f3 = data1.ADF3;
      f4 = data1.ADF4;
      f5 = data2.ADF5;
      f6 = data2.ADF6;
      f7 = data2.ADF7;
      f8 = data2.ADF8;
      nir = data2.ADF5;
      Clear = data2.ADF5;

      //Concatenate all info separated by commas
      dataMessage = date + "," + hour + "," + String(id) + "," + String(f1) + "," + String(f2) + "," + String(f3) + "," + String(f4) + "," + String(f5) + "," + String(f6) + "," + String(f7) + "," + String(f8) + "," + String(nir) + "," + String(Clear) + "\r\n";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      appendFile(SD, "/Box_Fluorometer_data.csv", dataMessage.c_str());

      Serial.print("ID:");
      Serial.println(id);
      
      Serial.print("F1(405-425nm):");
      Serial.println(data1.ADF1);
      Serial.print("F2(435-455nm):");
      Serial.println(data1.ADF2);
      Serial.print("F3(470-490nm):");
      Serial.println(data1.ADF3);
      Serial.print("F4(505-525nm):");   
      Serial.println(data1.ADF4);

      Serial.print("F5(545-565nm):");
      Serial.println(data2.ADF5);
      Serial.print("F6(580-600nm):");
      Serial.println(data2.ADF6);
      Serial.print("F7(620-640nm):");
      Serial.println(data2.ADF7);
      Serial.print("F8(670-690nm):");
      Serial.println(data2.ADF8);
      Serial.print("Clear:");
      Serial.println(data2.ADCLEAR);
      Serial.print("NIR:");
      Serial.println(data2.ADNIR);

      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);

      display.setCursor(10, 0);
      display.print("MOAM LAB - CIBNOR");
    
      display.setCursor(0, 14);
      display.print(date);
      display.setCursor(64, 14);
      display.print(hour);

      display.setTextSize(2);
      display.setCursor(0, 32);
      display.print("F8:"); display.print(data2.ADF8);

      display.setTextSize(1);
      display.setCursor(64, 52);
      display.print("Lectura:"); display.print(id/5);

      display.display();
      display.clearDisplay();

      digitalWrite(LaserRelay, 1);

      delay((delayTime - 1) * 1000);

      }

  }
  
  lastState = currentState;
  
}
