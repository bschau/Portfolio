@echo off
del *~
del dependencies\*~
del pfoc\*~
del pfoc\pfoc\*~
del pfoi\*~
del pfoi\pfoi\*~
del prc\*~
del prc\src\*~
del prc\src\PFO.prc
rmdir /s /q pfoc\pfoc\bin
rmdir /s /q pfoc\pfoc\obj
rmdir /s /q pfoi\pfoi\bin
rmdir /s /q pfoi\pfoi\obj
rmdir /s /q prc\src\build
del PFOForPalmOS-*.exe
