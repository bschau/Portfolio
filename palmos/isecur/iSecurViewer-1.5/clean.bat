@echo off
del *~
del dependencies\*~
del iSecurViewer\*~
del mono\*~
rmdir /s /q iSecurViewer\bin
rmdir /s /q iSecurViewer\obj
del iSecurViewer-*.exe
