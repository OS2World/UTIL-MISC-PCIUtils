@echo off
rem build libpciutils.a, lspci.exe and setpci.exe without using make
cls

rem modify this: wherever OpenWATCOM is installed..
if     [%watcom%]==[] call Y:\watcom\setvars.cmd
if not [%watcom%]==[] call %watcom%\setvars.cmd
rem SET INCLUDE=%WATCOM%\H\OS2;%WATCOM%\H;

set cflags=-ox -zq
set linkarg=Option Quiet  System os2v2

rem debug options
rem set cflags=%cflags% -d3
rem set linkarg=%linkarg% Debug All

echo *** config.h

rem imitate Configure
copy config.h ..\lib >nul

echo *** lib

rem build library
cd ..\lib
wcc386 %cflags% access.c
wcc386 %cflags% generic.c
wcc386 %cflags% dump.c
wcc386 %cflags% names.c
wcc386 %cflags% filter.c
wcc386 %cflags% os2-device.c
wcc386 %cflags% search_ids.c
wcc386 %cflags% i386-ports.c

if exist pciutils.lib del pciutils.lib

wlib -q pciutils.lib +access.obj +generic.obj +dump.obj +names.obj +filter.obj +os2-device.obj +search_ids.obj +i386-ports.obj

echo *** example

rem make simple example
wcc386 %cflags% example.c
wlink %linkarg%  file example file pciutils.lib  Option Description 'Example of use (simplistic lister of PCI devices)'
call lxlite /f+ /zx0 /zs0 /t /b- /cS example.exe > nul

echo *** lspci+setpci

rem build the diagnostic programs
cd ..
wcc386 %cflags% -Ilib common.c
wcc386 %cflags% -Ilib lspci.c
wcc386 %cflags% -Ilib setpci.c
wlink %linkarg%  file lspci   file common  file lib/pciutils.lib  Option Description 'lspci: displaying information about all PCI buses in the system and all devices connected to them'
wlink %linkarg%  file setpci  file common  file lib/pciutils.lib  Option Description 'setpci: utility for querying and configuring PCI devices'
call lxlite /f+ /zx0 /zs0 /t /b- /cS lspci.exe setpci.exe > nul

cd os2
