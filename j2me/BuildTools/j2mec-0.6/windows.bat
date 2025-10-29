@echo off
cd src
mkdir j2mec
javac -g:none -d j2mec *.java
cd j2mec
jar cvfm ..\..\j2mec.jar ..\MANIFEST *.class
cd ..
cd ..
