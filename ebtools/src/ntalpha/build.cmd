@echo off
setlocal
 call ..\..\..\sdkconf.bat
 if "%1" == "DOSUBDIRS" goto DOSUBDIRS

 if NOT EXIST obj md obj
:: common builds done first.

 for %%S in (gas cat rm wincc winas chmod strip makedepend) do call %0 DOSUBDIRS %%S %1 %2 %3 %4

 goto EXIT

:DOSUBDIRS

 echo Utility %2
 cd %2
 nmake -f makefile.nt %3 %4 %5 %6
 cd ..

:EXIT
endlocal
