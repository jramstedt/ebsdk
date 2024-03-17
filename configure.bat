@echo off
if exist ebconfig\nt_conf.exe goto doconf
echo Building configuration program
cd ebconfig
cl conf.c /Fent_conf.exe
cd ..
if exist ebconfig\nt_conf.exe goto doconf
echo An error occurred while building nt_conf.exe
echo Unable to continue.
goto end

:doconf

ebconfig\nt_conf.exe

:end
