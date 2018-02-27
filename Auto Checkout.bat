:start
echo off
title Checkout Utility
Mode 110,40
cls
echo ///////////////////////////////////////////////////////////////
echo ////////////////////////  Auto Checkout  //////////////////////
echo ///////////////////////////////////////////////////////////////
echo.
echo.
echo.
echo This program requires Git to be installed on the computer.  If Git
echo is not currently installed.  Please exit and install now.
echo.
echo !!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!
echo.  
echo This will overwrite any data stored on this computer.
echo Procede with CAUTION!
:menu
echo.
echo Menu:
echo 1 - Checkout AssemblyAuto Folder
echo 2 - Push AssemblyAuto Folder to Server
echo 3 - Pull AssemblyAuto Folder from Server
echo 4 - Get staus of repsoitory
echo 5 - Clone respository [ ! Only execute if needed ! ]
echo.
echo.
set /P o=Choose option:  
if /I "%o%" EQU "1" goto :option1
if /I "%o%" EQU "2" goto :option2
if /I "%o%" EQU "3" goto :option3
if /I "%o%" EQU "4" goto :option4
if /I "%o%" EQU "5" (
    goto :option5
) else (
    goto :start
)

:option1
echo.
echo.
echo This will checkout the AssemblyAuto folder for changes.
echo.
set /P c=Are you sure you want to continue[Y/N]?
if /I "%c%" EQU "y" (
    cls
    cd %Homedrive%%Homepath%\GitHub\AssemblyAuto\
    git checkout master
    echo.
    echo Checkout complete.  Program will now terminate.
    echo.
    pause
    exit
) else (
    goto :exit
)

:option2
cls
echo This will overwrite everything on the server with local changes.
echo Please be sure you want to do this!  This cannot be undone.
echo.
set /P c=Are you sure you want to continue[Y/N]?
if /I "%c%" EQU "y" (
    cls
    cd %Homedrive%%Homepath%\GitHub\AssemblyAuto\
    echo.
    echo Checkout complete.  Program will now terminate.
    pause
    exit
)
if /I "%c%" EQU "N" goto :exit

:option3
echo.
echo.
echo /////////// CAUTION \\\\\\\\\\\\
echo.
echo This will overwrite ALL files locally with files from the server.
echo This cannot be undone!
echo.
set /P c=Are you sure you want to continue[Y/N]? 
if /I "%c%" EQU "y" (
    cd %Homedrive%%Homepath%\Documents\GitHub\AssemblyAuto\
    git fetch --all
    git reset --hard origin/master
    echo.
    git status
    echo.
    echo Checkout complete.  Program will now terminate.
    echo.
    echo.
    pause
    exit
) else (
    goto :exit
)

:option4
cd %Homedrive%%Homepath%\Documents\GitHub\AssemblyAuto\
echo.
echo.
git status
echo.
goto :menu

:option5
echo.
echo.
echo /////////// CAUTION \\\\\\\\\\\\
echo.
echo This will overwrite ALL files locally with files from the server.
echo This cannot be undone!
echo.
set /P c=Are you sure you want to continue[Y/N]? 
if /I "%c%" EQU "y" (
    cd %Homedrive%%Homepath%\Documents\GitHub\AssemblyAuto\
    echo.
    echo.
    git clone https://github.com/Blade2021/AssemblyAuto.git
    echo.
    git status
    echo.
    echo Repository cloned to %Homedrive%%Homepath%\Documents\Github\AssemblyAuto\
    echo This will only pull from "master" branch!
    echo.
) else (
    goto :exit
)
goto :exit

:exit
echo.
echo.
echo Program terminating...
echo.
pause
exit

:checkout
cls
echo Checkout out Git Repository
echo.
cd %Homedrive%%Homepath%\GitHub\AssemblyAuto\
git checkout master
echo.
echo.
pause
goto :exit