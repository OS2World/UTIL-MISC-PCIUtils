@echo off
cls
call _clean.cmd
call Y:\gcc\v3.3.5-ga2\usr\bin\gccenv.cmd
set path=d:\extra;y:\usr\bin;d:\extra\lxlite;%path%
set comspec=/bin/sh.exe
set ac_executable_extensions=".exe"
set AC_EXEEXT=".exe"
sh -c make
