/*  VERSION 1.3.1
    Last succcessful run: 1.2.8
    Last Upload SHA Token: 8de98c
*/
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <EEPROM.h>

#define SENARRAYSIZE 8
#define SOLARRAYSIZE 9
#define MEMVECTORMULTIPLE 11
#define MPSMEMLOC 110
#define DEBUGMEMLOC 112
#define VECTORMEMLOC 100
#define POSDEFAULT 15
#define DATASPEED 19200
#define MPSLENGTH 4
#define LEDARRAYLENGTH 5
#define LEDSPEED 100

//Panel Buttons
const byte manualButton = 6;  //Manual feed button
const byte nextButton = 42;   // Next Button
const byte saveButton = 46;   // Savel/Select Button
const byte upButton = 48;     // Up Button
const byte downButton = 44;   // Down Button
const byte toggleButton = 50; // toggle Button

const byte ledArray[LEDARRAYLENGTH] = {13, 49, 47, 45, 43};
/*
// Panel LEDs
const byte ledArray[1] = 51;
const byte ledArray[2] = 49;
const byte ledArray[3] = 47;
const byte ledArray[4] = 45;
const byte ledArray[5] = 43;
const byte ledArray[0] = 13;
*/
//Sensors
const byte sensorArray[SENARRAYSIZE] = {A0, A1, A2, A3, A4, A5, A6, A7};
/* SENSOR LIST
   A0 - HookRailEmpty
   A1 - Unused
   A2 - HookCycleStart
   A3 - CrimpCycleStart
   A4 - HookRailFull
   A5 - StripOffOut
   A6 - HeadDown
   A7 - HeadUp
*/
//Solenoids
const byte solenoidArray[SOLARRAYSIZE] = {7, 8, 16, 17, 18, 19, 15, 14, 9};
/*
    7 - [AL-0] Unused
    8 - [AL-1] Hook Stopper
   16 - [AL-2] Head/Tooling
   17 - [AL-3] Strip Off
   18 - [AL-4] Crimp Stopper
   19 - [AL-5] Crimp
   15 - [AL-6] Vibrator
   14 - [AL-7] MainAir  
   9  - [AL-8] Motor Relay
*/
//LCD Variables
byte sysPosition = 0; // Position of sysArray
const int lcdClearTime = 7000;
byte pos = POSDEFAULT;           //LCD position for key input
byte jindx = 0;                  //Key Input Position (Array)
char arraya[] = {0, 1, 2, 3, 0}; //Key input array
const byte sysLength = 7;        // System timer array length

//Time Controls
const int buttonWait = 400;           // Button Debounce Time
unsigned long preLCDClear = 0;        // LCD Clear Timer
unsigned long buttonPreviousTime = 0; // Button Debounce Timer
//unsigned long previousTimer1 = 0;     // Feed Timer  *Disabled on Semi Boards
unsigned long previousTimer2 = 0; // Hook Cycle Timer
unsigned long previousTimer3 = 0; // Crimp Cycle Timer
unsigned long previousTimer4 = 0; // Vibrator Timer
unsigned long precountTime = 0;   // Part Count Timer
//System Time Variables
int sysArray[sysLength] = {1000, 1000, 1000, 1000, 2300, 2000, 300};
/*AL-0 - Hook Cycle Delay
  AL-1 - Crimp Cycle Delay
  AL-2 - Crimp Time
  AL-3 - RailCheck Delay
  AL-4 - Sensor Ignore Delay
  AL-5 - Feed/Hook [MPS]
  AL-6 - Head LOC [MPS]
*/

byte mpsArray[MPSLENGTH] = {0, 0, 0, 0};
/*AL-0 - Feed Protection [ DISABLED ON SEMI BOARDS ]
        0 - disabled
        1 - enabled
  AL-1 - Head location mode
        0 - disabled
        1 - Feed Check
        3 - Keep track of jams for crimping
        3 - Shut down if jammed after getting to lower sensor
        4 - Timed shutdown (going Down)
        5 - Timed shutdown (coming Up)
  AL-2 - Crimp Protection
        0 - disabled
        1 - enabled
*/

//LiquidCrystal
Adafruit_LiquidCrystal lcd(0);

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

//System Variables
boolean active = LOW;                    // System active variable
byte partError = 0;                      // Hook status
byte toggleLogic = 0;                    // Value of toggle button
byte hookNext = 0;                       // Hook loop position
byte hookLoop = 0;                       // Main cycle sensor
byte hookCheck = 0;                      // Lower Rail Sensor
byte crimpLoop = 0;                      // Crimp cycle sensor
byte crimpNext = 0;                      // Crimp cycle position
byte railCheck = 0;                      // Upper Rail sensor
byte railCheckNext = 0;                  // Vibrator cycle position
byte rswitch = 0;                        // System override, solenoid position variable
byte sOverride = 1;                      // System Override toggle 0 - Resets solenoids, 1 - Skip reset, active machine, 2 - System Override enabled
byte stateArray[SOLARRAYSIZE + 1] = {0}; //State array for status of all solenoids [Include extra 0 for the NULL END]
const int passcode = 7777;               //System override passcode
byte runCheck = 1;                       //Machine protection variable, Initalize as 1 until machine error.
int mfcount;                             // Malfunction counter
int lastMFcount;                         // Previous malfunction count, Used for MPS 3+
byte vector;                             // Memory vector postion
byte dispOverride = 0;

//LOGIC CONTROLS
byte logicCount = 0;      //Counter of material flow
byte bNextLogic = 0;      //Button Next Logic
byte bUpLogic = 0;        //Button Up Logic
byte bDownLogic = 0;      //Button Down Logic
byte saveButtonLogic = 0; //Save Button Logic
byte manualFeed = 0;      //Manual Feed Logic
byte secStart = 0;        //Second Start

//PC Control
const byte numChars = 32;             // Array character limit
char receivedChars[numChars];         // Recieved bytes from serial input
unsigned long preSerialCheck;         // Previous sensor check variable
int senWait = 100;                    // Sensor data wait time
boolean senBool = false;              // Sensor data output toggle
boolean newData = false;              // New serial data toggle
String apple = "";                    // Incoming serial data string
byte initial = 1;                     //Initial contact toggle
byte orchard[SENARRAYSIZE + 1] = {0}; // Sensor output toggle
byte debug = 0;                       // Debug value

void setup()
{
    //LEDs
    pinMode(ledArray[1], OUTPUT);
    pinMode(ledArray[2], OUTPUT);
    pinMode(ledArray[3], OUTPUT);
    pinMode(ledArray[4], OUTPUT);
    //pinMode(ledArray[5], OUTPUT);
    pinMode(ledArray[0], OUTPUT);
    //Buttons
    pinMode(manualButton, INPUT_PULLUP);
    pinMode(nextButton, INPUT_PULLUP);
    pinMode(saveButton, INPUT_PULLUP);
    pinMode(upButton, INPUT_PULLUP);
    pinMode(downButton, INPUT_PULLUP);
    pinMode(toggleButton, INPUT_PULLUP);
    //Solenoids
    pinMode(solenoidArray[0], OUTPUT);
    pinMode(solenoidArray[2], OUTPUT);
    pinMode(solenoidArray[3], OUTPUT);
    pinMode(solenoidArray[4], OUTPUT);
    pinMode(solenoidArray[5], OUTPUT);
    pinMode(solenoidArray[6], OUTPUT);
    pinMode(solenoidArray[7], OUTPUT);
    pinMode(solenoidArray[1], OUTPUT);
    pinMode(solenoidArray[8], OUTPUT);
    //Photo
    pinMode(sensorArray[2], INPUT_PULLUP);
    pinMode(sensorArray[1], INPUT_PULLUP);
    pinMode(sensorArray[3], INPUT_PULLUP);
    //Prox
    pinMode(sensorArray[0], INPUT_PULLUP);
    pinMode(sensorArray[4], INPUT_PULLUP);
    pinMode(sensorArray[7], INPUT_PULLUP);
    pinMode(sensorArray[6], INPUT_PULLUP);
    pinMode(sensorArray[5], INPUT_PULLUP);
    // END OF PINMODE

    Serial.begin(DATASPEED);
    Serial.println("Starting...");
    Serial.println("Program Version 1.3.6");
    lcd.begin(20, 4);
    lcd.setCursor(0, 0);
    lcd.print("Run Time: ");
    lcd.setCursor(2, 1);
    lcd.print("*** BOOTING ***");

    debug = EEPROM.read(DEBUGMEMLOC);

    //Reset all solenoids to LOW
    for (byte k; k < SOLARRAYSIZE; k++)
    {
        digitalWrite(solenoidArray[k], LOW);
        delay(1);
    }
    // Read memory for MPS DATA
    for (byte k = 0; k < MPSLENGTH; k++) // debug for issues with array length
    {
        byte mpsMemoryAddress = MPSMEMLOC + k;
        mpsArray[k] = EEPROM.read(mpsMemoryAddress);
        delay(1);
    }
    vector = EEPROM.read(VECTORMEMLOC);
    Serial.print("Vector: ");
    Serial.println(vector);
    delay(10);
    memoryLoad();

    // Display time setting:
    switch (vector)
    {
    case 0:
        lcd.setCursor(16, 0);
        lcd.print("VCT0");
        break;
    case 1:
        lcd.setCursor(16, 0);
        lcd.print("VCT1");
        break;
    case 2:
        lcd.setCursor(16, 0);
        lcd.print("VCT2");
        break;
    default:
        lcd.setCursor(16, 0);
        lcd.print("    ");
        break;
    }

    Serial.println(F("*** System Variables ***"));
    Serial.print(F("Button Wait Time: "));
    Serial.println(buttonWait);
    Serial.print("LCD Clear Time: ");
    Serial.println(lcdClearTime);
    Serial.print("LCD Default POS: ");
    Serial.println(POSDEFAULT);
    Serial.print("Override Passcode: ");
    Serial.println(passcode);
    Serial.println();
    Serial.println("<Controller Ready>");
    lcd.setCursor(0, 1);
    lcd.print("                    ");
}

void loop()
{
    // PC Controls
    if (Serial.available() > 0)
    {
        recvWithEndMarker();
    }
    if (newData == true)
    {
        if (debug >= 1)
        {
            Serial.println(F("DEBUG: newData function ran [REF:0234]"));
        }
        checkData();
    }
    if ((senBool == true) && (millis() - preSerialCheck > senWait))
    {
        sensorCheckActivator();
        preSerialCheck = millis();
    }
    // End of PC Controls
    lcdControl(); //Call LCD Clear function
    if ((sOverride == 0) || (sOverride == 1))
    {
        if ((sOverride == 0) && (active == 1))
        {
            dispOverride = 0;
            systemReset(1);
        }
        //if ((mpsEnable > 0) && (runCheck == 0))
        if (((mpsArray[1] >= 1) || (mpsArray[2] >= 1)) && (runCheck == 0))
        {
            manualFeed = digitalRead(manualButton);
            if ((manualFeed == LOW) && (millis() - buttonPreviousTime >= buttonWait))
            {
                buttonPreviousTime = millis() + 2000;
                runCheck = 1;
                digitalWrite(solenoidArray[8], LOW);
                errorReport(4, 0);
            }
        }
        toggleLogic = digitalRead(toggleButton);
        if (toggleLogic == LOW)
        {
            active = 0;
            inactive();
        }
        else
        {
            active = 1;
        }

        bUpLogic = digitalRead(upButton);
        if ((bUpLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            if (dispOverride == 1)
            {
                sysArray[sysPosition] = sysArray[sysPosition] + 20;
                buttonPreviousTime = millis();
                // Serial print function
            }
            // Reserved
        }
        bDownLogic = digitalRead(downButton);
        if ((bDownLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            if (dispOverride == 1)
            {
                sysArray[sysPosition] = sysArray[sysPosition] - 20;
                buttonPreviousTime = millis();
                // Serial print function
            }
            // Reserved
        }
        saveButtonLogic = digitalRead(saveButton);
        if ((saveButtonLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            if (dispOverride == 1)
            {
                buttonPreviousTime = millis();
                saveTrigger(sysPosition);
                // Serial print function
            }
            // Reserved
        }
        bNextLogic = digitalRead(nextButton);
        if ((bNextLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            if (dispOverride == 1)
            {
                buttonPreviousTime = millis();
                sysPosition++;
                if (sysPosition >= sysLength)
                {
                    sysPosition = 0;
                }
                // Serial print function
            }
            // Reserved
        }
        if ((active == 1) && (sOverride == 1))
        {
            // Activate display Override
            char key = keypad.getKey();
            if (key == 'D')
            {
                dispOverride = 1;
            }
            // Call functions for display override
            if (dispOverride >= 1)
            {
                displaySwitch(sysPosition);
            }

            setLEDS(0);

            if (runCheck == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Run Time: ");
                digitalWrite(solenoidArray[7], HIGH); // Turn air on
                digitalWrite(solenoidArray[9], LOW);  // Turn motor on
                // Vibrator Cycle
                railCheck = digitalRead(sensorArray[4]);
                if ((railCheck == HIGH) && (railCheckNext == 0))
                {
                    if (debug >= 2)
                    {
                        Serial.print(F("Rail Check Activated ["));
                        Serial.print(millis() / 1000);
                        Serial.println("]");
                    }
                    previousTimer2 = millis();
                    digitalWrite(ledArray[1], HIGH);
                    digitalWrite(solenoidArray[6], HIGH);
                    railCheckNext = 1;
                }
                if (railCheckNext == 1)
                {
                    railCheck = digitalRead(sensorArray[4]);
                    if (railCheck == LOW)
                    {
                        previousTimer2 = millis();
                        railCheckNext = 2;
                    }
                }
                if (railCheckNext == 2)
                {
                    railCheck = digitalRead(sensorArray[4]);
                    if (railCheck == HIGH)
                    {
                        railCheckNext = 1;
                    }
                    if ((railCheck == LOW) && (millis() - previousTimer2 >= sysArray[3]))
                    {
                        digitalWrite(solenoidArray[6], LOW);
                        digitalWrite(ledArray[1], LOW);
                        previousTimer2 = millis();
                        if (debug >= 3)
                        {
                            Serial.println(F("Rail Check Finished"));
                        }
                        railCheckNext = 0;
                    }
                }
                // END OF VIBRATOR CYCLE
                // Crimp Cycle
                crimpLoop = digitalRead(sensorArray[3]);
                if (
                    //Trigger All
                    // ((crimpLoop == LOW) && (crimpNext == 0) && (mpsEnable < 3) && (millis() - previousTimer4 >= sysArray[2])) ||
                    ((crimpLoop == LOW) && (crimpNext == 0) && (mpsArray[1] < 2) && (millis() - previousTimer4 >= sysArray[2])) ||
                    //Protection - Only crimp if malfunction was not detected
                    //((crimpLoop == LOW) && (crimpNext == 0) && (mpsEnable >= 3) && (mfcount <= lastMFcount) && (millis() - previousTimer4 >= sysArray[2])))
                    ((crimpLoop == LOW) && (crimpNext == 0) && (mpsArray[1] >= 2) && (mfcount <= lastMFcount) && (millis() - previousTimer4 >= sysArray[2])))
                {
                    if (debug >= 2)
                    {
                        Serial.print(F("Crimp Cycle Activated ["));
                        Serial.print(millis() / 1000);
                        Serial.println("]");
                    }
                    digitalWrite(ledArray[4], HIGH);
                    digitalWrite(solenoidArray[4], HIGH); //Stopper Out
                    previousTimer4 = millis();
                    crimpNext = 1;
                }
                //Crimp Protection Reset
                if ((crimpLoop == LOW) && (crimpNext == 0) && (mfcount > lastMFcount))
                {
                    //Reset lastMFcount to continue cycles after one pass.
                    errorReport(6, mfcount);
                    lastMFcount = mfcount;
                    previousTimer4 = millis();
                }
                if ((crimpNext == 1) && (millis() - previousTimer4 >= sysArray[1]))
                {
                    previousTimer4 = millis();
                    digitalWrite(solenoidArray[5], HIGH);
                    if (debug >= 3)
                    {
                        Serial.println("Crimp Cycle | Crimp");
                    }
                    crimpNext = 2;
                }
                // Crimp Reset
                if ((crimpNext == 2) && (millis() - previousTimer4 >= sysArray[2]))
                {
                    if (debug >= 3)
                    {
                        Serial.println("Crimp Cycle | Reset");
                    }
                    previousTimer4 = millis();
                    digitalWrite(solenoidArray[5], LOW);
                    digitalWrite(solenoidArray[4], LOW);
                    digitalWrite(ledArray[4], LOW);
                    crimpNext = 0;
                }
                // Hook Cycle
                hookLoop = digitalRead(sensorArray[2]);
                if ((hookLoop == LOW) && (hookNext == 0))
                {
                    if (logicCount == 0)
                    {
                        precountTime = millis();
                    }
                    logicCount++;
                    if (dispOverride <= 0)
                    {
                        lcd.setCursor(0,1);
                        lcd.print("SC: ");
                        lcd.print(logicCount);
                        lcd.print("   ");
                    }
                    //if ((mpsEnable <= 1) || ((mpsEnable >= 2) && (millis() - previousTimer3 >= sysArray[4])))
                    if ((mpsArray[1] == 0) || ((mpsArray[1] >= 1) && (millis() - previousTimer3 >= sysArray[4])))
                    {
                        if (debug >= 2)
                        {
                            Serial.print(F("Hook Cycle Activated ["));
                            Serial.print(millis() / 1000);
                            Serial.println("]");
                        }
                        digitalWrite(ledArray[2], HIGH);
                        boolean hookCheck;
                        hookCheck = digitalRead(sensorArray[0]);
                        if (hookCheck == HIGH)
                        {
                            errorReport(7, 0);
                            //partError = 1;
                            digitalWrite(ledArray[2], LOW);
                            digitalWrite(ledArray[0], HIGH);
                        }
                        if (hookCheck == LOW)
                        {
                            previousTimer3 = millis();
                            digitalWrite(solenoidArray[1], HIGH);
                            hookNext = 1;
                        }
                    }
                    //if ((mpsEnable >= 2) && (millis() - previousTimer3 < sysArray[5]) && (millis() - previousTimer3 >= sysArray[4]))
                    if ((mpsArray[1] >= 1) && (millis() - previousTimer3 < sysArray[5]) && (millis() - previousTimer3 >= sysArray[4]))
                    {
                        //Check if MPS is enabled.  If so, check value of time sensor triggered.
                        machStop(0);
                        runCheck = 0;
                        previousTimer3 = millis();
                    }
                }
                //Send Head Down
                if ((hookNext == 1) && (millis() - previousTimer3 >= sysArray[0]))
                {
                    previousTimer3 = millis();
                    if (debug >= 3)
                    {
                        Serial.println(F("Hook Cycle | Tool/Head OUT"));
                    }
                    digitalWrite(solenoidArray[2], HIGH);
                    hookNext = 2;
                }
                //Send Strip Off Out / Check Head location
                if (hookNext == 2)
                {
                    int HeadCheckDown = digitalRead(sensorArray[6]);
                    //MPS Disabled
                    //if (((HeadCheckDown == LOW) && (mpsEnable <= 2)) || ((HeadCheckDown == LOW) && (mpsEnable >= 3) && (millis() - previousTimer3 < sysArray[6])))
                    if (((HeadCheckDown == LOW) && (mpsArray[1] <= 2)) || ((HeadCheckDown == LOW) && (mpsArray[1] >= 3) && (millis() - previousTimer3 < sysArray[6])))
                    {
                        digitalWrite(solenoidArray[3], HIGH);
                        hookNext = 3;
                        if (debug >= 3)
                        {
                            Serial.println(F("Hook Cycle | Strip Off OUT"));
                        }
                    }
                    //MPS Setting 5 - Shut down on timer
                    //if ((mpsEnable >= 6) && (millis() - previousTimer3 >= sysArray[6]))
                    if ((mpsArray[1] >= 6) && (millis() - previousTimer3 >= sysArray[6]))
                    {
                        machStop(1);
                        Serial.println(F("Motor stopped due to ERROR[0036]"));
                        hookNext = 0;
                        runCheck = 0;
                    }
                    //if ((HeadCheckDown == LOW) && (mpsEnable >= 3) && (millis() - previousTimer3 >= sysArray[6]))
                    if ((HeadCheckDown == LOW) && (mpsArray[1] >= 3) && (millis() - previousTimer3 >= sysArray[6]))
                    {
                        mfcount++;
                        hookNext = 3;
                        //if (mpsEnable == 4)
                        if (mpsArray[1] == 4)
                        {
                            machStop(1);
                            hookNext - 0;
                            runCheck = 0;
                            Serial.println(F("Motor stopped due to ERROR[0034]"));
                            Serial.print("preTime: ");
                            Serial.print(previousTimer3);
                            Serial.print(" - ");
                            Serial.print("millis(): ");
                            Serial.print(millis());
                            Serial.print(" > ");
                            Serial.print("varTime: ");
                            Serial.println(sysArray[8]);
                            previousTimer3 = millis();
                            //Turn off machine
                        }
                        digitalWrite(solenoidArray[3], HIGH);
                        if (debug >= 3)
                        {
                            Serial.println(F("Hook Cycle | Strip Off OUT"));
                        }
                        Serial.print("Malfunction detected CT[");
                        Serial.print(mfcount);
                        Serial.println("]");
                    }
                }
                //Send Head Up
                if (hookNext == 3)
                {
                    int StripOffCheck = digitalRead(sensorArray[5]);
                    if (StripOffCheck == LOW)
                    {
                        digitalWrite(solenoidArray[2], LOW);
                        digitalWrite(ledArray[3], HIGH);
                        digitalWrite(ledArray[2], LOW);
                        hookNext = 4;
                    }
                }
                // Reset Strip Off / Reset Stopper
                //if (((hookNext == 4) && (mpsEnable < 5)) || ((hookNext == 4) && (mpsEnable >= 5) && (millis() - previousTimer3 < sysArray[6])))
                if (((hookNext == 4) && (mpsArray[1] < 3)) || ((hookNext == 4) && (mpsArray[1] == 4) && (millis() - previousTimer3 < sysArray[6])))
                {
                    int HeadUpCheck = digitalRead(sensorArray[7]);
                    if (HeadUpCheck == LOW)
                    {
                        if (debug >= 3)
                        {
                            Serial.println("Hook Cycle | Reset");
                        }
                        digitalWrite(solenoidArray[1], LOW);
                        digitalWrite(solenoidArray[3], LOW);
                        digitalWrite(ledArray[3], LOW);
                        hookNext = 0;
                    }
                }
                //else if ((hookNext == 4) && (mpsEnable >= 5) && (millis() - previousTimer3 >= sysArray[6]))
                else if ((hookNext == 4) && (mpsArray[1] == 4) && (millis() - previousTimer3 >= sysArray[6]))
                {
                    machStop(1);
                    hookNext = 0;
                    runCheck = 0;
                }
                // End of hook Cycle
                if (logicCount >= 100)
                {
                    TimeKeeper();
                }
            } // end of runcheck 1
        }     // End of If active == true (Else Statement)
    }         // End of SOverride == 0 or 1

    /* Start System Override
     - Trigger Relays individually
     - Record state of relay for display and toggle
  */
    if (sOverride == 2)
    {
        static byte ledStatus;
        if (millis() - previousTimer3 >= LEDSPEED)
        {
            if (ledStatus > (LEDARRAYLENGTH - 1))
            {
                ledStatus = 1;
            }
            setLEDS(ledStatus);
            ledStatus++;
            previousTimer3 = millis();
        }
        lcd.setCursor(0, 1);
        lcd.print("OVERRIDE: ON        ");
        lcd.setCursor(0, 2);
        lcd.print("SYSTEM: Relay ");
        bNextLogic = digitalRead(nextButton);
        if ((bNextLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            buttonPreviousTime = millis();
            rswitch++;
            if (rswitch >= 8)
            {
                rswitch = 0;
            }
            return;
        }
        saveButtonLogic = digitalRead(saveButton);
        if ((saveButtonLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            buttonPreviousTime = millis();
            Override_Trigger(rswitch + 1);
            return;
        }
        bDownLogic = digitalRead(downButton);
        if ((bDownLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            buttonPreviousTime = millis();
            errorReport(8, 0);
            sOverride = 0;
            sysPosition = 0;
            systemReset(1);
        }
        else
        {
            //Get keypad input
            char key;
            key = keypad.getKey();
            if (key)
            {
                switch (key)
                {
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                    return;
                case '#':
                case '*':
                case '0':
                    sOverride = 0;
                    systemReset(1);
                    return;
                default:
                    break;
                }
                int tempb = key - '0';
                //Send keypad input to Override_Trigger function
                Override_Trigger(tempb);
            }
            //Display current selected relay on LCD (For use of manual buttons)
            lcd.setCursor(14, 2);
            lcd.print(rswitch + 1);
        } // End of Else Statment
    }     // End of sOverride2
} //End of LOOP Void

void inactive()
{
    //Trigger intital reset when exiting inactive mode
    sOverride = 0;
    dispOverride = 1;
    railCheckNext = 0;
    digitalWrite(ledArray[0], HIGH);
    digitalWrite(solenoidArray[0], LOW); //Unused
    digitalWrite(solenoidArray[1], LOW); //HookStopper
    digitalWrite(solenoidArray[2], LOW); //Head/Tooling
    digitalWrite(solenoidArray[3], LOW); //StripOff
    digitalWrite(solenoidArray[4], LOW); //CrimpStopper
    digitalWrite(solenoidArray[5], LOW); //Crimp
    digitalWrite(solenoidArray[6], LOW); //Vibrator
    digitalWrite(solenoidArray[7], LOW); //MainAir
    displaySwitch(sysPosition);
}
void displaySwitch(int sysPos)
{
    lcd.setCursor(0, 2);
    lcd.print("Time:");
    switch (sysPos)
    {
    case 0:
        setLEDS(1);
        lcd.setCursor(0, 1);
        lcd.print(F("Hook Cycle Wait     "));
        changetime(sysPos);
        break;
    case 1:
        setLEDS(2);
        lcd.setCursor(0, 1);
        lcd.print(F("Crimp Cycle Wait    "));
        changetime(sysPos);
        break;
    case 2:
        setLEDS(3);
        lcd.setCursor(0, 1);
        lcd.print(F("Crimp Time          "));
        changetime(sysPos);
        break;
    case 3:
        setLEDS(4);
        lcd.setCursor(0, 1);
        lcd.print(F("Vibrator Time     "));
        changetime(sysPos);
        break;
    case 4:
        setLEDS(1);
        lcd.setCursor(0, 1);
        lcd.print(F("Sensor Ignore [MPS] "));
        changetime(sysPos);
        break;
    case 5:
        setLEDS(2);
        lcd.setCursor(0, 1);
        lcd.print(F("Main Cycle [MPS]   "));
        changetime(sysPos);
        break;
    case 6:
        setLEDS(3);
        lcd.setCursor(0, 1);
        lcd.print(F("Head LOC [MPS]     "));
        changetime(sysPos);
        break;
    } //END OF MAIN SWITCH
} // End of Inactive void
//Save trigger function.
/* This function is for saving values from manual button changes.
   For Keypad function see: changetime
*/
void saveTrigger(byte sysPos)
{
    if (sysArray[sysPos] > 5100)
    {
        sysArray[sysPos] = 5100;
        errorReport(10, sysPos);
    }
    eepromWrite(sysPos, sysArray[sysPos]);
    errorReport(9, sysPos + 1);
    digitalWrite(ledArray[0], HIGH);
    delay(200);
    digitalWrite(ledArray[0], LOW);
    delay(200);
    digitalWrite(ledArray[0], HIGH);
    delay(200);
    digitalWrite(ledArray[0], LOW);
}

void Override_Trigger(int RTrigger)
{
    int tempstate = LOW;
    String lcdstate = "OFF";
    if (stateArray[RTrigger] == 1)
    {
        tempstate = LOW;
        lcdstate = "OFF";
        stateArray[RTrigger] = 0;
    }
    else
    {
        stateArray[RTrigger] = 1;
        lcdstate = "ON";
        tempstate = HIGH;
    }
    digitalWrite(solenoidArray[RTrigger - 1], tempstate);
    lcd.setCursor(0, 3);
    lcd.print("Relay ");
    lcd.print(RTrigger);
    lcd.print(" SET TO: ");
    lcd.print(lcdstate);
    lcd.print(" ");
    Serial.print(F("SYSTEM OVERRIDE | Relay "));
    Serial.print(RTrigger);
    Serial.print(" | ");
    Serial.println(lcdstate);
    preLCDClear = millis();
}

//Clear last line of LCD every x(seconds)
void lcdControl()
{
    lcd.setCursor(10, 0);
    lcd.print(millis() / 1000);
    if (millis() - preLCDClear >= lcdClearTime)
    {
        preLCDClear = millis();
        lcd.setCursor(0, 3);
        lcd.print("                    ");
    }
}

//Set one LED to HIGH.
void setLEDS(byte LEDSnumber)
{
    digitalWrite(ledArray[1], LOW);
    digitalWrite(ledArray[2], LOW);
    digitalWrite(ledArray[3], LOW);
    digitalWrite(ledArray[4], LOW);
    //digitalWrite(ledArray[5], LOW);
    if ((LEDSnumber == 0) || (LEDSnumber > LEDARRAYLENGTH))
    {
        return;
    }
    digitalWrite(ledArray[LEDSnumber], HIGH);
}

void machStop(byte airoff)
{
    digitalWrite(solenoidArray[8], HIGH);
    for (byte k; k < 7; k++)
    {
        digitalWrite(solenoidArray[k], LOW);
        delay(1);
    }
    if (airoff >= 1)
    {
        digitalWrite(solenoidArray[7], LOW);
    }
    hookNext = 0;
    railCheckNext = 0;
    return;
}

void recvWithEndMarker()
{
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();

        if (rc != endMarker)
        {
            receivedChars[ndx] = rc;
            ndx++;
            apple = apple += rc;
            if (ndx >= numChars)
            {
                ndx = numChars - 1;
            }
        }
        else
        {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}
void checkData()
{
    if (newData == true)
    {
        if (apple.length() >= 5)
        {
            if (apple.substring(0, 5) == "EREAD")
            {
                int address = firstValue();
                if ((address <= EEPROM.length()) && (address >= 0))
                {
                    byte value = EEPROM.read(address);
                    Serial.print("EEPROM Memory Address: ");
                    Serial.println(address);
                    Serial.print("   Value: ");
                    Serial.println(value);
                }
            }
            if (apple.substring(0, 6) == "EEPROM")
            {
                eepromUpdate();
            }
            if (apple.substring(0, 5) == "DEBUG")
            {
                char voucher = apple.charAt(6);
                byte endingVoucher = voucher - '0';
                if ((endingVoucher >= 0) && (endingVoucher <= 9))
                {
                    debug = endingVoucher;
                    Serial.print("Debug updated to: ");
                    Serial.println(debug);
                    EEPROM.update(DEBUGMEMLOC, debug);
                }
                else
                {
                    Serial.println(F("Debug value not accepted"));
                }
            }
            if ((apple.substring(0, 3) == "PIN") && (sOverride == 2))
            {
                pinUpdate();
            }
            if (apple.substring(0, 10) == "TIMECHANGE")
            {
                ext_timeChange();
            }
            if (apple.substring(0, 7) == "SENWAIT")
            {
                senWaitFunction();
            }
            if (apple.substring(0, 8) == "OVERRIDE")
            {
                if ((sOverride == 0) || (sOverride == 1))
                {
                    sOverride = 2;
                    Serial.println("Override:On");
                }
                else
                {
                    sOverride = 0;
                    Serial.println("Override:Off");
                }
            }
            if (apple.substring(0, 8) == "SENCHECK")
            {
                senBool = !senBool;
                Serial.print("Sensor Auto Updated: ");
                Serial.println(senBool);
            }
            if (apple.substring(0, 6) == "SITREP")
            {
                for (byte k; k < SOLARRAYSIZE; k++)
                {
                    Serial.print("SOL.");
                    Serial.print(k);
                    Serial.print(".");
                    Serial.println(solenoidArray[k]);
                    delay(220);
                }
                for (byte k; k < SENARRAYSIZE; k++)
                {
                    Serial.print("SEN.");
                    Serial.print(k);
                    Serial.print(".");
                    Serial.println(sensorArray[k]);
                    delay(220);
                }
                for (byte k = 0; k < sysLength; k++)
                {
                    byte address = k * 2;
                    byte alpha = EEPROM.read(address);
                    address++;
                    byte beta = EEPROM.read(address);
                    int total = ((alpha + beta) * 10);
                    Serial.print("EMU.");
                    Serial.print(k);
                    Serial.print(".");
                    Serial.println(total);
                    delay(220);
                }
                sensorCheckActivator();
                Serial.println("SITREP COMPLETE");
            }
        }
        newData = false;
        apple = "";
    }
}

void senWaitFunction()
{
    senWait = lastValue();
    Serial.print(F("Sensor Wait Peroid Updated: "));
    Serial.println(senWait);
}

void pinUpdate()
{
    boolean value = LOW;
    int pinAddress = firstValue();
    if (pinAddress >= 64)
    {
        pinAddress = 64;
    }
    byte lastPos = apple.lastIndexOf('.') + 1;
    if (receivedChars[lastPos] == '\0')
    {
        Serial.println("ERROR FOUND");
        return;
    }
    else if (receivedChars[lastPos] == '0')
    {
        value = LOW;
    }
    else if (receivedChars[lastPos] == '1')
    {
        value = HIGH;
    }
    for (byte pinCheck = 0; pinCheck < 10; pinCheck++)
    {
        if (pinAddress == solenoidArray[pinCheck])
        {
            stateArray[pinCheck] = value;
        }
    }
    Serial.print("SUPD [");
    Serial.print(pinAddress);
    Serial.print("] Value: ");
    Serial.println(value);
    digitalWrite(pinAddress, value);
}
void senCheck()
{
    byte indx = 0;
    Serial.print("INL.");
    Serial.print(SENARRAYSIZE);
    Serial.print(".");
    for (byte k = 0; k < SENARRAYSIZE; k++)
    {
        indx++;
        boolean senOutput = digitalRead(sensorArray[k]);
        if (senOutput == HIGH)
        {
            Serial.print("1");
            orchard[k] = 1;
        }
        else
        {
            Serial.print("0");
            orchard[k] = 0;
        }
    }
    orchard[indx] = '\0';
    Serial.println(); //Send the completed serial array
}

void sensorCheckActivator()
{
    boolean carrier;
    if (initial == 1)
    {
        senCheck();
        initial = 0;
    }
    else
    {
        for (byte k = 0; k < SENARRAYSIZE; k++)
        {
            carrier = digitalRead(sensorArray[k]);
            if (carrier == HIGH)
            {
                if (orchard[k] != 1)
                {
                    senCheck();
                }
            }
            if (carrier == LOW)
            {
                if (orchard[k] != 0)
                {
                    senCheck();
                }
            }
        }
    }
}

//Write how long it took to run 100 parts & reset logicCount
void TimeKeeper()
{
    unsigned long tempvar = ((millis() - precountTime) / 1000);
    Serial.print("CTN Run Time: ");
    Serial.println(tempvar);
    if (dispOverride == 0)
    {
        lcd.setCursor(11, 1);
        lcd.print("CTN:");
        lcd.setCursor(15, 1);
        lcd.print(tempvar);
        logicCount = 0;
    }
}

void changetime(int sysPos)
{
    lcd.setCursor(5, 2);
    lcd.print(sysArray[sysPos]);
    lcd.print("      ");
    lcd.setCursor(pos, 2);
    char key = keypad.getKey();
    if (key)
    {
        if ((key == 'A') || (key == 'a'))
        {
            mpsSelection();
            return;
        }
        if (key == 'B')
        {
            byte tempPos = 5; //Temporary lcd location
            jindx = 0;
            lcd.clear();
            lcd.setCursor(6, 0);
            lcd.print("Enter");
            lcd.setCursor(1, 1);
            lcd.print(F("Override Passcode:"));
            boolean complete = false;
            lcd.setCursor(tempPos, 2);
            while (complete == false)
            {
                char key = keypad.getKey();
                if (key)
                {
                    lcd.print(key);
                    tempPos++;
                    lcd.setCursor(tempPos, 2);
                    arraya[jindx++] = key;
                    arraya[jindx];
                    if (key == '*')
                    {
                        int passCheck = atoi(arraya);
                        if ((passCheck == passcode) && (active == 0))
                        {
                            sOverride = 2;
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("Run Time:");
                            complete = true;
                            return;
                        }
                        if ((passCheck == passcode) && (active >= 1))
                        {
                            lcd.clear();
                            errorReport(15, 0);
                        }
                        else
                        {
                            lcd.clear();
                            errorReport(14, 7);
                        }
                        complete = true;
                        return;
                    }
                    if (key == '#')
                    {
                        lcd.clear();
                        errorReport(16, 0);
                        complete = true;
                    }
                }
            }
            preLCDClear = millis();
            jindx = 0;
            return;
        }
        if (key == 'C')
        {
            jindx = 0;
            vectorChange();
            return;
        }
        if (key == 'D')
        {
            switch (dispOverride)
            {
            case 0:
                dispOverride = 1;
                break;

            case 1:
                dispOverride = 0;
                break;
            }
            lcd.clear();
            jindx = 0;
            return;
        }
        lcd.print(key);
        pos++;
        lcd.setCursor(pos, 2);

        arraya[jindx++] = key;
        arraya[jindx];
        if (pos > 20)
        {
            pos = POSDEFAULT;
        }
        if (key == '*')
        {
            int value = atoi(arraya);
            Serial.print("SYSTEM | Keypad Input: ");
            Serial.println(value);
            if ((value == passcode) && (active == 0))
            {
                /* VERY IMPORTANT!  Check to see if active is 0
          so that override isn't turned on while machine running.  */
                sOverride = 2;
                pos = POSDEFAULT;
                lcd.setCursor(pos, 2);
                lcd.print("       ");
                jindx = 0;
                return;
            }
            if (value > 5100)
            {
                value = 5100;
                errorReport(10, sysPos);
            }
            eepromWrite(sysPos, value);
            sysArray[sysPos] = value;
            pos = POSDEFAULT;
            lcd.setCursor(pos, 2);
            lcd.print("      ");
            jindx = 0;
            return;
        }
        if (key == '#')
        {
            pos = POSDEFAULT;
            lcd.setCursor(pos, 2);
            lcd.print("     ");
            jindx = 0;
            return;
        }
    } //End of If(Key)
}
//End of ChangeTime function

void eepromUpdate()
{
    int address = firstValue();
    if (memCheck(address, 10) == false)
    {
        return;
    }
    else
    {
        int eepromValue = lastValue();
        Serial.print("EEPROM.update(");
        Serial.print(address);
        Serial.print(", ");
        Serial.print(eepromValue);
        Serial.println(")");
        EEPROM.update(address, eepromValue);
        memoryLoad();
    }
}

void errorReport(byte errorType, int refID)
{
    lcd.setCursor(0, 3); //Set cursor to last line of LCD
    switch (errorType)
    {
    // Input greater than limit ( eepromWrite Function )
    case 1:
        lcd.print("MEM CORRUPT [");
        lcd.print(refID);
        lcd.print("]");
        Serial.print(F("[REF: 0320] EEPROM Memory corrupted. Address: "));
        Serial.println(refID);
        break;
    // memCheck Function
    case 2:
        Serial.print(F("ALERT: Memory limit reached. ID( "));
        Serial.print(refID);
        Serial.println(" ) [REF: 4320]");
        break;
    // Version control memory corrupted
    case 3:
        Serial.print(F("ALERT: Version control memory location[ "));
        Serial.print(refID);
        Serial.println(" ] is corrupted.");
        break;
    // Runcheck Reset
    case 4:
        lcd.print("Runcheck Reset!");
        Serial.println(F("WARNING: RunCheck reset!"));
        break;
    // Skip crimp cycle
    case 6:
        lcd.print("ALERT: [REF:4455]");
        Serial.print(F("Skipping crimp cycle [REF 4455] [MFC: "));
        Serial.print(refID);
        Serial.println(" ]");
        break;
    // Hook Check failed
    case 7:
        lcd.print("ALERT: Hook Check");
        Serial.println(F("ALERT: Hook check failed. [REF 3294]"));
        break;
    // Override Deactivated
    case 8:
        lcd.print("Override Deactivated");
        Serial.println("SYSTEM OVERRIDE | Deactivated ");
        break;
    // Value Updated
    case 9:
        lcd.print("EE.Update VAR[");
        lcd.print(refID);
        lcd.print("]    ");
        break;
    // Max value hit
    case 10:
        lcd.print("Max Value Hit!");
        Serial.print(F("WARNING: Max value hit when trying to save variable id:"));
        Serial.println(refID);
        break;
    // DEBUG
    case 11:
        Serial.print("DEBUG CODE: ");
        Serial.println(refID);
        break;
    // Vector change
    case 12:
        lcd.print("Loaded VCT");
        lcd.print(refID);
        lcd.print(" timing");
        Serial.print("Loaded VCT");
        Serial.print(refID);
        Serial.print(" settings");
        break;
    // Stop due to MPS
    case 13:
        Serial.print(F("ALERT: Machine stopped due to malfunction. MPS:"));
        Serial.print(mpsArray[refID]);
        Serial.println("[REF 0034]");
        break;
    // Vector invalid input
    case 14:
        lcd.print("INVALID INPUT");
        Serial.print("INVALID INPUT REF: ");
        Serial.println(refID);
        break;
    // Tried to enter override while in active mode
    case 15:
        lcd.print("ERROR:2358");
        Serial.print("ERROR: 2358");
        break;
    // System function termination
    case 16:
        lcd.print(F("Function Terminated"));
        break;
    default:
        Serial.println("PROGRAMMING ERROR");
        break;
    }
    preLCDClear = millis();
}

int firstValue()
{
    char masterArray[numChars];
    byte slaveindx;
    byte value_start = apple.indexOf('.');
    if (debug >= 2)
    {
        Serial.print("V Start: ");
        Serial.println(value_start);
    }
    byte value_end = apple.indexOf('.', value_start + 1);
    if (debug >= 2)
    {
        Serial.print("V End: ");
        Serial.println(value_end);
    }
    for (byte k = value_start + 1; k < value_end; k++)
    {
        masterArray[slaveindx] = receivedChars[k];
        slaveindx++;
    }
    masterArray[slaveindx] = '\0';
    Serial.println(masterArray);
    int value = atoi(masterArray);

    if (debug >= 2)
    {
        Serial.print("fvF firstValue: ");
        Serial.println(value);
    }
    return value;
}

int lastValue()
{
    char masterArray[numChars];
    byte slaveindx = 0;
    byte value_end = apple.lastIndexOf('.');
    if (debug >= 2)
    {
        Serial.print("V End (2): ");
        Serial.println(value_end);
    }
    for (byte k = value_end + 1; k < apple.length(); k++)
    {
        masterArray[slaveindx] = receivedChars[k];
        slaveindx++;
    }
    masterArray[slaveindx] = '\0';
    int lastvalue = atoi(masterArray);
    if (debug >= 2)
    {
        Serial.print("lvF lastValue: ");
        Serial.println(lastvalue);
    }
    return lastvalue;
}

void ext_timeChange()
{
    int sysArrayLoc = firstValue();
    if ((sysArrayLoc < 0) || (sysArrayLoc > sysLength))
    {
        Serial.println(F("Array length exceeded. [REF 8973]"));
        return;
    }
    int timeChangeValue = lastValue();
    //Write data to EEPROM memeory
    eepromWrite(sysArrayLoc, timeChangeValue);
    //Change the time for the appropiate timer
    sysArray[sysArrayLoc] = timeChangeValue;
}

void memoryLoad()
{
    //Load EEPROM Memory
    for (byte k = 0; k < sysLength; k++)
    {
        int memAddress = (((vector * MEMVECTORMULTIPLE) + k) * 2);
        int memBlockOne = EEPROM.read(memAddress);
        Serial.print("X23 Address[ ");
        Serial.print(memAddress);
        Serial.print(" ] Value: ");
        Serial.println(memBlockOne);
        memBlockOne = memBlockOne * 10;
        if ((memBlockOne > 2550) || (memBlockOne < 0))
        {
            Serial.print(F("ERROR | Corrupted memory LOC:"));
            Serial.print(memAddress);
            Serial.print(" Result:");
            Serial.println(memBlockOne);
            lcd.setCursor(0, 3);
            lcd.print("MEMCORE:");
            lcd.print(memAddress);
            break;
        }
        memAddress++;
        int memBlockTwo = EEPROM.read(memAddress);
        Serial.print("X24: Address[ ");
        Serial.print(memAddress);
        Serial.print(" ] Value: ");
        Serial.println(memBlockTwo);
        memBlockTwo = memBlockTwo * 10;
        if ((memBlockTwo > 2550) || (memBlockTwo < 0))
        {
            Serial.print(F("ERROR | Corrupted memory LOC:"));
            Serial.print(memAddress);
            Serial.print(" Result:");
            Serial.println(memBlockTwo);
            lcd.setCursor(0, 3);
            lcd.print("MEMCORE:");
            lcd.print(memAddress);
            break;
        }
        sysArray[k] = memBlockOne + memBlockTwo;
        Serial.print("EEPROM[");
        Serial.print(k);
        Serial.print("]: ");
        Serial.println(sysArray[k]);
        delay(10);
    }
}

void vectorChange()
{
    lcd.setCursor(0, 1);
    lcd.print("Memory Vector:      ");
    pos = POSDEFAULT;
    boolean complete = false;
    while (complete == false)
    {
        char key;
        key = keypad.getKey();
        lcd.setCursor(pos, 2);
        lcd.print(key);
        switch (key)
        {
        case '0':
            vector = 0;
            errorReport(12, vector);
            EEPROM.update(100, 0);
            memoryLoad();
            lcd.setCursor(16, 0);
            lcd.print("VCT0");
            complete = true;
            break;

        case '1':
            vector = 1;
            errorReport(12, vector);
            EEPROM.update(100, 1);
            memoryLoad();
            lcd.setCursor(16, 0);
            lcd.print("VCT1");
            complete = true;
            break;

        case '2':
            vector = 2;
            errorReport(12, vector);
            EEPROM.update(100, 2);
            Serial.println("Vector 2");
            memoryLoad();
            lcd.setCursor(16, 0);
            lcd.print("VCT2");
            complete = true;
            break;

        case '#':
            complete = true;
            return;

        default:
            errorReport(13, 0);
            preLCDClear = millis();
            pos = POSDEFAULT;
            lcd.print("     ");
            break;
        }
    }
    systemReset(0);
}

void systemReset(byte resetVar)
{
    if (resetVar != 0)
    {
        for (byte indx = 0; indx < SOLARRAYSIZE; indx++)
        {
            stateArray[indx] = 0;
            Serial.print("Relay trigger INDEX: ");
            Serial.print(indx);
            Serial.println(" reset. [REF 3305]");
        }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Run Time: ");
    Serial.println("LCD Cleared");
    setLEDS(0);
    //Reset the count after leaving sOverride or inactive mode
    logicCount = 0;
    runCheck = 1;
    //feedNext = 0;
    hookNext = 0;
    crimpNext = 0;
    railCheckNext = 0;
    sOverride = 1; //Exit initial reset
}

// ***** !!!  IMPORTANT FUNCTION  !!!  *****
/* All calls to update eeprom memory, call to this function */
void eepromWrite(byte arrayLoc, int value)
{
    int memAddress = (((vector * MEMVECTORMULTIPLE) + arrayLoc) * 2);
    if (memCheck(memAddress, 12) == true)
    {
        if ((value >= 5100) || (value <= 0))
        {
            errorReport(1, memAddress);
            return;
        }
        int tempValue = value / 10;
        if ((value <= 2550))
        {
            EEPROM.update(memAddress, tempValue);
            Serial.print(F("Updating EEPROM Address ( "));
            Serial.print(memAddress);
            Serial.print(" ) with value [ ");
            Serial.print(tempValue);
            Serial.println(" ]");
            memAddress++;
            if (memCheck(memAddress, 13) == false)
            {
                return;
            }
            EEPROM.update(memAddress, 0);
            Serial.print(F("Updating EEPROM Address ( "));
            Serial.print(memAddress);
            Serial.print(" ) with value [ ");
            Serial.print('0');
            Serial.println(" ]");
        }
        if (value > 2550)
        {
            tempValue = tempValue - 255;
            EEPROM.update(memAddress, tempValue);
            Serial.print(F("Updating EEPROM Address ( "));
            Serial.print(memAddress);
            Serial.print(" ) with value [ ");
            Serial.print(tempValue);
            Serial.println(" ]");
            memAddress++;
            if (memCheck(memAddress, 14) == false)
            {
                return;
            }
            EEPROM.update(memAddress, 255);
            Serial.print(F("Updating EEPROM Address ( "));
            Serial.print(memAddress);
            Serial.print(" ) with value [ ");
            Serial.print("255");
            Serial.println(" ]");
        }
    }
}

boolean memCheck(unsigned int address, byte refID)
{
    if (address > EEPROM.length())
    {
        errorReport(2, refID);
        return false;
    }
    else
    {
        return true;
    }
}

void mpsSelection()
{
    if (debug >= 2)
    {
        Serial.println(F("Triggered MPS Function"));
    }
    sOverride = 0;
    boolean complete = false;
    byte arrayIndex = 0;
    byte formatLCD = 0;
    boolean ledState = HIGH;
    while (complete == false)
    {
        if (millis() - previousTimer4 >= buttonWait)
        {
            ledState = !ledState;
            digitalWrite(ledArray[0], ledState);
            previousTimer4 = millis();
        }
        bNextLogic = digitalRead(nextButton);
        if ((bNextLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            arrayIndex++;
            formatLCD = 0;
            buttonPreviousTime = millis();
        }
        bDownLogic = digitalRead(downButton);
        if ((bDownLogic == LOW) && (millis() - buttonPreviousTime >= buttonWait))
        {
            complete = true;
            buttonPreviousTime = millis();
            systemReset(0);
            return;
        }
        if (arrayIndex == (MPSLENGTH - 1))
        {
            arrayIndex = 0;
        }
        if (formatLCD == 0)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Machine Protection");
            formatLCD = 1;
        }
        lcd.setCursor(0, 1);
        switch (arrayIndex)
        {
        case 0:
            lcd.print("Feed MPS:");
            lcd.print(mpsArray[0]);
            break;
        case 1:
            lcd.print("Inserter MPS:");
            lcd.print(mpsArray[1]);
            break;
        case 2:
            lcd.print("Crimper MPS:");
            lcd.print(mpsArray[2]);
            break;
        default:
            break;
        }

        char key = keypad.getKey();
        if (key)
        {
            if (key == '#')
            {
                complete = true;
                systemReset(0);
                return;
            }
            byte keyValue = key - '0';
            if ((keyValue >= 0) && (keyValue <= 9))
            {
                mpsInput(keyValue, arrayIndex);
                formatLCD = 0;
            }
            else
            {
                errorReport(16, 3);
                complete = true;
            }
        }
    }
}

void mpsInput(byte keyValue, byte arrayIndex)
{
    mpsArray[arrayIndex] = keyValue;
    byte mpsMemoryAddress = arrayIndex + MPSMEMLOC;
    EEPROM.update(mpsMemoryAddress, keyValue);
    lcd.setCursor(0, 3);
    lcd.print("MPS[");
    lcd.print(arrayIndex);
    lcd.print("] Value: ");
    lcd.print(keyValue);
    preLCDClear = millis();
    Serial.print("MPS Array LOC:");
    Serial.println(arrayIndex);
    Serial.print("   Value: ");
    Serial.println(keyValue);
}
