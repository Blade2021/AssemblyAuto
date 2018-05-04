:start
echo off
title Checkout Utility
Mode 110,40
cls
echo "              _______   __          ___        ___    ___  |     __         _______       "
echo "   / \   |  |    |     /  \        /    |   | /___/  /     | /  /  \   |  |    |          "
echo "  / - \  |  |    |    |    |      |     |---| |     |      |\  |    |  |  |    |          "
echo " /     \ \__/    |     \__/        \___ |   |  \__/  \___  | \  \__/   \__/    |          "     
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
echo 2 - Pull AssemblyAuto Folder from Server
echo 3 - Get staus of repsoitory
echo 4 - Clone respository [ ! Only execute if needed ! ]
echo.
echo.
set /P o=Choose option:  
if /I "%o%" EQU "1" goto :option1
if /I "%o%" EQU "2" goto :option2
if /I "%o%" EQU "3" goto :option3
if /I "%o%" EQU "4" (
    goto :option4
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
    cd %Homedrive%%Homepath%\Documents\GitHub\AssemblyAuto\
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

:option3
cd %Homedrive%%Homepath%\Documents\GitHub\AssemblyAuto\
echo.
echo.
git status
echo.
goto :menu

:option4
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