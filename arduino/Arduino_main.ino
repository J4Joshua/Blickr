#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier

// For LCD
#include <SPI.h>
#include <Adafruit_GFX.h>//請安裝Adafruit_GFX
#include <Adafruit_SSD1306.h>//請安裝Adafruit SSD1306
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);





#define GPIO_DEEP_SLEEP_DURATION     10  // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;   // BLE Advertisement type
struct timeval now;
//blicker
#define BEACON_UUID "00000000-0000-32B1-9d46-d64f5fe79284" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

//make a stand
//#define BEACON_UUID "A2AAAAAA-AAAA-F0F0-F0F0-F0F0F0F0F0F0"

//origonal 87b99b2c-90fd-11e9-bc42-526af7764f64/
//blicker 8492E75F-4FD6-469D-B132-000000000000
//00000000-0000-232B1-9d46-d64f5fe79284 somehow numbers are flipped when actually broadcast
//make a stand -> F0F0F0F0-F0F0-F0F0-F0F0-AAAAAAAAAAA1


int xdelay = 20;
int choice = 1;
bool flag = true;
void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  //oBeacon.setMajor((bootcount & 0xFFFF0001) >> 16);
  oBeacon.setMajor(0x0002);
  if (bootcount == 5 ) bootcount = 1;
  oBeacon.setMinor(choice & 0xFFFF);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  std::string strServiceData = "";

  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  
  
}
void testdrawchar(int  choice) {
  display.clearDisplay();

  display.setTextSize(5);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  display.write(48+choice);

  display.display();
  
}

void setup() {
  //start serial connection
  Serial.begin(115200);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(16, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  gettimeofday(&now, NULL);
  Serial.printf("start ESP32 %d\n", bootcount++);
  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);
  last = now.tv_sec;

  // Create the BLE Device
  BLEDevice::init("ESP32 as iBeacon");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  setBeacon();
  // Start advertising
  pAdvertising->start();
  Serial.println("Advertizing started...");

  delay(1000);
  //pAdvertising->stop();
  Serial.printf("enter deep sleep\n");
  // esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  Serial.printf("in deep sleep\n");

  //LCD
  Wire.begin(19,23);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }else{Serial.println("SSD1306 test OK");}

  

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  


  
  

  

}

void loop() {

  
  //read the pushbutton value into a variable
  
  int sensor4Val = digitalRead(16);
  int sensor3Val = digitalRead(4);
  int sensor2Val = digitalRead(0);
  int sensor1Val = digitalRead(2);
  //print out the value of the pushbutton
  digitalWrite(LED_BUILTIN, LOW);
  

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (sensor4Val == LOW) {
    Serial.printf("4");
    pAdvertising->stop();
    
    choice = 4;
    digitalWrite(LED_BUILTIN, HIGH);

    setBeacon();
    pAdvertising->start();
    testdrawchar(choice+48);
    delay(xdelay);
    
    
    
  } 
  else if (sensor3Val == LOW) {
    Serial.printf("3");
    pAdvertising->stop();
    
    choice = 3;
    digitalWrite(LED_BUILTIN, HIGH);

    setBeacon();
    pAdvertising->start();
    testdrawchar(choice+48);
    delay(xdelay);
    
    
  }
  else if (sensor2Val == LOW) {
    Serial.printf("2");
    pAdvertising->stop();
    choice = 2;
    digitalWrite(LED_BUILTIN, HIGH);

    setBeacon();
    pAdvertising->start();
    testdrawchar(choice+48);
    delay(xdelay);
    
    
  }
  else if (sensor1Val == LOW) {
    Serial.printf("1");
    pAdvertising->stop();
    choice = 1;
    digitalWrite(LED_BUILTIN, HIGH);

    setBeacon();
    pAdvertising->start();
    testdrawchar(choice+48);
    delay(xdelay);
    
    
  }
}
