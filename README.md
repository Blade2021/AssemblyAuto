# Assembly Auto Repository  [![Build Status](https://travis-ci.com/Blade2021/AssemblyAuto.svg?branch=patch-1.3.2)](https://travis-ci.com/Blade2021/AssemblyAuto)
## Introduction
This repository was made to assist in programming and distribution of a program developed specifically to run an industrial machine.  The program uses a variance between #C/C++.  It is still in alpha mode with no intention of release anytime soon.  

## System Functions
### Machine Protection System (MPS)
MPS was built into the program to help protect the mechanical and eletrical parts of the machine when experiencing an abnormality or malfunction.  MPS will detect malfunctions automatically without any warning or input from the operator by taking very quick measurements of all the sensors connected running them through a function with variables directly adjustable through the main program control.  MPS will decide wether the machine needs to shut down to correct an error or simply just record the error until the next review.

### System Override
System override was built into the program to help debug and process the machine at a more controllable pace.  It enables the operator to activate/deactivate each relay individually to possibly find the problem with the machine wether it be mechanical or electrial; Enabling the technician to pin point any issues at a faster more effecient rate.

## Notes
### Timer Vs Timer-2VAR[EEPROM]
Timer was built as a test script for main functionality.  However Timer-2VAR[EEPROM] was made for a backup sketch in case a user needs to write a bigger number then 255 to the EEPROM.  It separates the variable and writes it to EEPROM.  This version will also use twice the amount of EEPROM memory!

### Main Files
AssemblyAuto.INO
### SubFiles
SerTest.pde

## To-Do
- WatchDog Timer
- MPS(further expansion)
