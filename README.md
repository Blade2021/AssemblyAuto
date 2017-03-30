# Assembly Auto Repository  
##Introduction
This repository was made to assist in programming and distribution of a program developed specifically to run an industrial machine.  The program uses a variance between #C/C++.  It is still in alpha mode with no intention of release anytime soon.  

## Main Files
AssemblyAuto.INO

## SubFiles
root.PDE

## Notes
### System Override
System override was built into the program to help debug and process the machine at a more controllable pace.  It enables the operator to activate/deactivate each relay individually to possibly find the problem with the machine wether it be mechanical or electrical.  

### Timer Vs Timer-2VAR[EEPROM]
Timer was built as a test script for main functionality.  However Timer-2VAR[EEPROM] was made for a backup sketch in case a user needs to write a bigger number then 255 to the EEPROM.  It separates the variable and writes it to EEPROM.  This version will also use twice the amount of EEPROM memory!
