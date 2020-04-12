@echo off
del processed.png
del doubleloop.tc
java -cp classes com.schau.TrailTools.TrailMaker data\doubleloop.png doubleloop.tc
