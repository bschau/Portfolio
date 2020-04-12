@echo off
del lmake\lmake.exe

cd lmake\scripts
call build.bat
cd ..\..
if not exist lmake\lmake.exe goto lmakeerr

cd lcdoc
..\lmake\lmake.exe
if not exist lcdoc.exe goto lcdocerr
cd ..

cd lhd
..\lmake\lmake.exe
if not exist lhd.exe goto lhderr
cd ..

cd lme
..\lmake\lmake.exe
if not exist lme.exe goto lmeerr
cd ..

iscc setup.iss
goto eof

:lhderr
echo Failed to build lhd.exe
goto :eof

:lcdocerr
echo Failed to build lcdoc.exe
goto :eof

:lmakeerr
echo Failed to build lmake.exe
goto :eof

:lmeerr
echo Failed to build lme.exe

:eof
