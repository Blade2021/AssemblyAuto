/*  VERSION 1.3.1
    Last succcessful run: 1.2.8
    Last Upload SHA Token: 8de98c
*/
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SENARRAYSIZE 8
#define SOLARRAYSIZE 4
#define MEMVECTORMULTIPLE 11
#define MPSMEMLOC 110
#define DEBUGMEMLOC 112
#define VERSIONMEM 770
#define VECTORMEMLOC 100
#define POSDEFAULT 15
#define DATASPEED 19200

//Panel Buttons
//const byte manualButton = 62; //Manual feed button
const byte nextButton = 42;   // Next Button
const byte saveButton = 46;   // Savel/Select Button
const byte upButton = 48;     // Up Button
const byte downButton = 44;   // Down Button
const byte toggleButton = 50; // toggle Button
const byte startButton = 0;
const byte stopButton = 0;

// Panel LEDs
const byte panelLed1 = 51;
const byte panelLed2 = 49;
const byte panelLed3 = 47;
const byte panelLed4 = 45;
const byte panelLed5 = 43;
const byte errorLed = 13;
//Sensors
const byte sensorArray[SENARRAYSIZE] = {A0, A1, A2, A3, A4, A5, A6, A7};
/* SENSOR LIST
   A0 - LWR-HookRail
   A1 - UPR-HookRail
   A2 - LWR-HeadSensor
   A3 - UPR-HeadSensor
   A4 - LWR-MaterialSensor
   A5 - UPR-MaterialSensor
   A6 - FeedTable
   A7 - Reserved
*/
//Solenoids
const byte solenoidArray[SOLARRAYSIZE] = {12, 11, 10, 9};
/*
  12 - [AL-0] Hanger Feed
  11 - [AL-1] Main Head
  10 - [AL-2] Vibrator
   9 - [AL-3] Main Air
*/
//LCD Variables
byte sysPosition = 0; // Position of sysArray
const int lcdClearTime = 7000;
byte pos = POSDEFAULT;           //LCD position for key input
byte jindx = 0;                  //Key Input Position (Array)
char arraya[] = {0, 1, 2, 3, 0}; //Key input array
const byte sysLength = 9;        // System timer array length

//Time Controls
const int buttonWait = 400;           // Button Debounce Time
unsigned long preLCDClear = 0;        // LCD Clear Timer
unsigned long buttonPreviousTime = 0; // Button Debounce Timer
unsigned long precountTime = 0;       // Part Count Timer
//System Time Variables
int sysArray[sysLength] = {1000, 1000, 1000, 1000, 2300, 2000, 300, 2000, 1200};
/*AL-0 - Feed Cycle Delay
  AL-1 - Feed Open Delay
*/
//LiquidCrystal
LiquidCrystal lcd(18, 19, 5, 4, 3, 2);

//Keypad
const byte ROWS = 4; // # of rows for keypad
const byte COLS = 4; // # of columns for keypad
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {25, 27, 29, 31}; //row pins
byte colPins[COLS] = {33, 35, 37, 39}; //column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
    //Set pinModes
    for (byte k; k < SOLARRAYSIZE; k++)
    {
        pinMode(solenoidArray[k], OUTPUT);
    }
    for (byte k; k < SENSORARRAYSIZE; k++)
    {
        pinMode(sensorArray[k], INPUT_PULLUP);
    }
    pinMode(startButton, INPUT_PULLUP);
    pinMode(stopButton, INPUT_PULLUP);

    Serial.begin(DATASPEED);
    lcd.begin(20, 4);
    debug = EEPROM.read(DEBUGMEMLOC);
    mpsEnable = EEPROM.read(MPSMEMLOC);
    memoryLoad();
}

void loop()
{
    startButtonLogic = digitalRead(startButton);
    stopButtonLogic = digitalRead(stopButton);
    if ((startButtonLogic == LOW) && (stopButtonLogic == LOW) && (feedOverSeq != 1))
    {
        previousTimer1 = millis();
        //Begin feed Override sequence
        feedOverSeq = 1;
    }
    if((feedOverSeq == 1) && (previousTimer1 - millis() >= TRIGGERTIME){
        if ((startButtonLogic == LOW) && (stopButtonLogic == LOW))
        {
            feedOverride = !feedOverride;
            feedOverSeq = 0;
        }
        else
        {
            // Released
        }
    }
    if ((stopButtonLogic == LOW) && (startButtonLogic == HIGH) && (buttonPreTime - millis() >= buttonWait)){
        runCycle = 0;
        buttonPreTime = millis();
    }
    if ((startButtonLogic == LOW) && (startButtonLogic == HIGH) && (buttonPreTime - millis() >= buttonWait)){
        if (feedOverride == 1)
        {
            runCycle = 2; //  Ignore feed sensor
        }
        else
        {
            runCycle = 1; //  Run based on feed sensor
        }
        buttonPreTime = millis();
    }

    if (sMode == 0){
        //Solenoid Reset
        sMode = 1;
    }
    if (sMode == 1){
        //Send Hanger Up
        byte feedSensorLogic = digitalRead(sensorArray[6]);
        if (((runCycle == 1) && (feedSensorLogic == LOW)) || (runCycle == 2) && (cycleStep == 0) && (partError == 0))
        {
            byte hookCheck = digitalRead(sensorArray[0]);
            if (hookCheck == HIGH)
            {
                partError = 1;
                //Serial print
                return;
            }
            digitalWrite(solenoidArray[0], HIGH); //Hanger solenoid HIGH
            preTimer2 = millis();
            cycleStep = 2;
        }
        //Send Head Down
        byte hangerCheck = digitalRead(sensorArray[5]); //Read uppr hanger sensor
        if ((hangerCheck == LOW) && (cycleStep == 2))
        {
            hookCheck = digitalRead(sensorArray[0]); //LWR Hook Rail
            if (hookCheck == HIGH)
            {
                partError = 1;
                return;
            }
            digitalWrite(solenoidArray[1], HIGH); // Send Head down
            cycleStep = 3;
        }
        //Send Hanger Down
        byte headCheck = digitalRead(sensorArray[2]); // Read LWR Head
        if ((headCheck == LOW) && (cycleStep == 3))
        {
            digitalWrite(solenoidArray[0], LOW); // Hanger Solenoid LOW
            if(cycleMode == 0){
                cycleStep = 4;
            }
            if(cycleMode == 1){
                cycleStep = 5;
            }
            preTimer2 = millis();
        }
        //Send Head Up
        if (cycleStep == 4)
        {
            byte upperHeadCheck = digitalRead(sensorArray[4]); // Check upper head sensor
            if (upperHeadCheck == LOW)
            {
                digitalWrite(solenoidArray[1], LOW); // Send Head UP
                cycleStep = 0;
            }
        }
        if ((cycleStep == 5) && (preTimer2 - millis() >= headWaitTime))
        {
            digitalWrite(solenoidArray[1], LOW); // Send Head UP
            cycleStep = 0;
        }
    }
}