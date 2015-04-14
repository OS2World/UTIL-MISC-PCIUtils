@echo off
rem build libpciutils.a, lspci.exe and setpci.exe without using make

rem modify this: wherever gcc is installed..
if .%CCENV%.==.. call Y:\gcc\v3.3.5-ga2\usr\bin\gccenv.cmd
cls

set cflags=-O2 -Wall -W -Wno-parentheses -Wstrict-prototypes -Wmissing-prototypes -Winline

echo *** config.h

rem imitate Configure
copy config.h ..\lib >nul

echo *** lib

rem build library
cd ..\lib
gcc %cflags% -c access.c 
gcc %cflags% -c generic.c 
gcc %cflags% -c dump.c 
gcc %cflags% -c names.c
gcc %cflags% -c filter.c
gcc %cflags% -c os2-device.c
gcc %cflags% -c search_ids.c
gcc %cflags% -c i386-ports.c

if exist libpciutils.a del libpciutils.a
ar rcs libpciutils.a access.o generic.o dump.o names.o filter.o os2-device.o search_ids.o i386-ports.o
ranlib libpciutils.a

echo *** example

rem make simple example
gcc %cflags% example.c libpciutils.a
call lxlite /f+ /zx0 /zs0 /t /b- /cS example.exe > nul

echo *** lspci

rem build the diagnostic programs
cd ..
gcc %cflags% -Ilib lspci.c common.c lib/libpciutils.a

echo *** setpci

gcc %cflags% -Ilib setpci.c common.c lib/libpciutils.a
call lxlite /f+ /zx0 /zs0 /t /b- /cS lspci.exe setpci.exe > nul

cd os2
