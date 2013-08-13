@echo off
for %%d in ( *.o *.obj lib\*.obj lib\*.o *.a lib\*.lib lib\*.a *.exe lib\*.exe lib\config.h lib\config.mk *.map lib\*.map *.sym lib\*.sym *.8 update-pciids *.err lib\*.err ) do if exist %%d del %%d
