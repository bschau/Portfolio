@echo off
del DevConv.exe
csc DevConv.cs Currency.cs Country.cs Merger.cs
DevConv.exe ..\prc\src\data ..\TramsData
