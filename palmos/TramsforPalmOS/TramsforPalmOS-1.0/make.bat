@echo off

set PKGNAME=TramsforPalmOS
set VERSION=1.0

if "%1" == "clean" goto clean
if "%1" == "archive" goto archive
if "%1" == "release" goto release

echo Targets defined:
echo.
echo	clean
echo	archive
echo	release
goto :eof

:clean
del *~
del %PKGNAME%.zip
rmdir /s /q %PKGNAME%
del dependencies\*~

cd DevConv
call clean.bat
cd ..

del TramsConduit\*~
del TramsConduit\TramsConduit\*~
rmdir /q /s TramsConduit\debug
rmdir /q /s TramsConduit\TramsConduit\Debug
rmdir /q /s TramsConduit\release
rmdir /q /s TramsConduit\TramsConduit\Release
del TramsConduit\TramsConduit\TramsConduit.vcproj.*.user
del TramsConduit\TramsConduit.ncb

del TramsCvt\*~
del TramsCvt\TramsCvt\*~
del TramsCvt\TramsCvt\Properties\*~
del TramsCvt\TramsCvt\Wizard\*~
rmdir /q /s TramsCvt\TramsCvt\bin
rmdir /q /s TramsCvt\TramsCvt\obj

del TramsInst\*~
del TramsInst\Properties\*~
rmdir /q /s TramsInst\bin
rmdir /q /s TramsInst\obj

del TramsData\*~

del prc\*~
del prc\src\*~
del prc\src\Trams.prc
rmdir /q /s prc\src\build
goto :eof

:archive
call make.bat clean
del %PKGNAME%-%VERSION%.exe
cd ..
zip -r %PKGNAME%-%VERSION%.zip %PKGNAME%-%VERSION%/*
cd %PKGNAME%-%VERSION%
goto :eof

:release
iscc setup.iss
:eof
