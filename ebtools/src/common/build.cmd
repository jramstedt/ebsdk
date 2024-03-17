@echo off
setlocal
 call ..\..\..\sdkconf.bat
 if "%1" == "DOSUBDIRS" goto DOSUBDIRS

 if NOT EXIST obj md obj
:: common builds done first.
 for %%S in (common gnumake) do call %0 DOSUBDIRS %%S %1 %2 %3 %4

 for %%S in (bit hal hex32 hexpad ic4mat icfmt list makeexp makerom mapcvt) do call %0 DOSUBDIRS %%S %1 %2 %3 %4
 for %%S in (objtest pvc rcsv repeat srec srom strip sysgen uload zip-2.2 unzip-5.32) do call %0 DOSUBDIRS %%S %1 %2 %3 %4

:: utilities that use many other utilities
 for %%S in (hfcomp mini_dbg) do call %0 DOSUBDIRS %%S %1 %2 %3 %4

 goto EXIT

:DOSUBDIRS

 echo Utility %2
 cd %2
 nmake -f makefile.nt %3 %4 %5 %6
 cd ..

:EXIT
endlocal
