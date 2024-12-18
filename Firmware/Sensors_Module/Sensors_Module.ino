/*COTIJA Firmware - AIM Sensors Module
 * Authors: Kennan Bays, Joachim Blohm, Brent Naumann, Ethan Toste
*/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <tone.h>
#include <flashTable.h>
#include <SerialFlash.h>
#include <SoftwareSerial.h>
#include "pinouts.h"
#include <CANpackets.h>
#include "STM32_CAN.h"

//buzzer
const uint32_t BEEP_DELAY = 6000;
const uint32_t BEEP_LENGTH = 1000;
const uint32_t BUZZER_TONE = 1000;
const uint32_t BUZZER_TONE_Q = 500;

//canbus
const uint32_t STATUS_REPORT_DELAY = 1000; //How frequently the status report should be sent
uint32_t lastStatusReport = 0; //When the last status report sent


//--- FLASH SETTINGS
uint16_t flashDelay = 250; // How frequently the debug LED should be toggled
uint32_t lastFlash = 0; // Last millis() the debug LED was toggle at

const uint32_t LOG_INTERVAL = 50;
uint32_t lastLog = 0;

const uint8_t TABLE_NAME = 0;
const uint8_t TABLE_COLS = 5;
const uint32_t TABLE_SIZE = 16646144;
// IMPORTANT!!!; AT LEAST 2 BLOCK OF SPACE MUST BE RESERVED FOR FILE SYSTEM
// 16MiB = 16777216B, 2x 64KiB blocks = 131072B
// 16MiB - 128KiB = 16646144B

const uint32_t CAM_RECORD_DELAY = 360000; //5min after power on
const uint32_t CAM_POWER_TIME = 10800000; //3hrs after record start

//--- DATALOGGING SETTINGS

FlashTable flash = FlashTable(TABLE_COLS, 16384, TABLE_SIZE, TABLE_NAME, 256); 
Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

//GLOBAL VARIABLES
STM32_CAN canb( CAN1, ALT );    //CAN1 ALT is PB8+PB9
static CAN_message_t CAN_TX_msg ;



void setup() {
  // Configure pinmodes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(CAMERA_POWER_PIN, OUTPUT);

  // Start USB debugging
  // Configure I2C bus
  Serial.begin(115200);

  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  // Configure SPI
  SPI.setSCLK(FLASH_SCK_PIN);
  SPI.setMISO(FLASH_MISO_PIN);
  SPI.setMOSI(FLASH_MOSI_PIN);

  //LED FLASHES TO ALLOW FOR SERIAL OPEN
  for (int i=0 ; i <5 ; i++){
    digitalWrite(STATUS_LED_PIN,HIGH);
    delay(250);
    digitalWrite(STATUS_LED_PIN,LOW);
    delay(250);
  }//for


  //Set up flash device
  while (!SerialFlash.begin(FLASH_CS_PIN)) {
      delay(250);
      //toggleStatusLED();
  }//while


  // Initialize FlashTable object
  delay(BUZZER_TONE);
  Serial.println("STARTING FLASH");
  for (int i=0; i<3; i++) {
    tone(BUZZER_PIN, BUZZER_TONE_Q);
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
  }//for
  flash.init(&SerialFlash, &Serial);

  // STARTUP BEEP
  delay(BEEP_DELAY);
  tone(BUZZER_PIN, BUZZER_TONE);
  delay(BEEP_LENGTH);
  noTone(BUZZER_PIN);
  Serial.println("STARTED");

  //Connect to BME680
  Serial.print("Preparing BME680...");
  while (!bme.begin(BME680_ADDR)) {
    digitalWrite(STATUS_LED_PIN,HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN,LOW);
    delay(100);
  }//while
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  Serial.println("BME680 Ready");

  Serial.println("[MDE] ENTER D FOR DEBUG");
  digitalWrite(STATUS_LED_PIN,HIGH);
  
  uint32_t startTime = millis();
  while (!Serial.available() and millis()-startTime < 5000) {}

  // Prompt for entering debug mode
  if (Serial.available()) {
    byte d = Serial.read();
    emptySerialBuffer();
    Serial.println("[MDE] Entered Debug Mode");
    debugMode();
    while (true) {}
  }//if

  digitalWrite(STATUS_LED_PIN,LOW);

  //Canbus Setup
  //Start CANBUS
  canb.begin(); //automatic retransmission can be done using arg "true"
  canb.setBaudRate(500000); //500kbps
}//setup()


/*---\/---\/---\/---\/---*\
      |  DEBUG MENU |    
\*---/\---/\---/\---/\---*/

// Empties all bytes from incoming serial buffer.
// Used by Debug mode
void emptySerialBuffer() {
  while (Serial.available()) {Serial.read();}
}//emptySerialBuffer()

// Called when Debug mode is activated;
// all normal board functions will cease.
// Only purpose is to respond to serial
// commands.
void debugMode() {

  // Status LED static "ON"
  digitalWrite(STATUS_LED_PIN, HIGH);

  while (true) {

    // Empty buffer
    emptySerialBuffer();

    // Wait for command
    while (!Serial.available()) {}
    uint8_t cmd = Serial.read();

    if (cmd == 'I') {
      // "Identify" command; return board name
      Serial.println(F("[MDE] SENSORS"));
    }//if
    if (cmd == 'F') {
      // "FlashInfo" command; return flash usage stats
      Serial.print("[MDE] ");
      Serial.print(flash.getMaxSize());
      Serial.print(F(","));
      Serial.println(flash.getCurSize());   
    }//if
    if (cmd == 'D') {
      // "DumpFlash" command; dump all flash contents via serial
      flash.beginDataDump(&Serial);
    }//if
    if (cmd == 'E') {
      // "EraseFlash" command; completely erase contents of flash.
      // Should be restarted afterwards
      Serial.println(F("[MDE] Erasing Flash"));
      SerialFlash.eraseAll();
      while (SerialFlash.ready() == false) {}
      //flash.init(&SerialFlash);
      Serial.println(F("[MDE] Complete"));
    }//if
    if (cmd == 'Q') {
        // QUERY SENSORS
        
          //Read bme 680
        bme.performReading();
        
        Serial.println("[MDE] --BME 680--");
        Serial.print("[MDE] PRESSURE [Pa]: ");
        Serial.println(bme.pressure);
        Serial.print("[MDE] TEMPERATURE [K]: ");
        Serial.println(bme.temperature+273);
        Serial.print("[MDE] HUMIDITY [%]: ");
        Serial.println(bme.humidity);
        Serial.print("[MDE] GAS RESISTANCE [Ohm]: ");
        Serial.println(bme.gas_resistance);
    }//if

  }//while
}//debugMode()

void loop() {  
  // Check if should log BME & other data
  if (millis() - lastLog > LOG_INTERVAL) {

    lastLog = millis();

    // TODO: Are all of these proper UINTs? Do we need to convert signed ints or floats?
    bme.performReading();
    uint32_t dataArr[5] = {0,0,0,0,0};
    dataArr[0] = millis();
    dataArr[1] = bme.pressure;
    dataArr[2] = bme.temperature+273;
    dataArr[3] = bme.humidity*100;
    dataArr[4] = bme.gas_resistance*100;

    //write to FLASH
    flash.writeRow(dataArr);
  }//if

  // Check if camera power should be given
  // TODO: Make this a bit more efficient? Only trigger at a certain time?
  if (millis() > CAM_RECORD_DELAY+CAM_POWER_TIME) {
    //turn off camera
    digitalWrite(CAMERA_POWER_PIN, LOW);
    Serial.println("CAM OFF");
  } else if (millis() > CAM_RECORD_DELAY) {
    //turn on camera
    digitalWrite(CAMERA_POWER_PIN, HIGH);
    Serial.println("CAM ON");
  }//if

  //Checking if it is time to report status to CANBUS
  if(millis() - lastStatusReport > STATUS_REPORT_DELAY){
    lastStatusReport = millis();
    sendCANStatus(); //Reporting status
  }//if


}//loop()


// ---CANBUS 
void sendCANStatus(){

    //Build the CANBUS message
    CAN_TX_msg.id = (SENSOR_MOD_CANID+STATUS_CANID);
    CAN_TX_msg.len = 1;

    CAN_TX_msg.buf[0] = 1;

    canb.write(CAN_TX_msg); //send
} //sendCANStatus()
