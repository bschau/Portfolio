del /q *~
del /q tttxexport-*.exe
del /q setup.iss
del /q Docs\*~
del /q script\*~
del /q src\*~
del /q src\version.h
del /q Support\*
cd tttxexport
del /q *~
del /q tttxexport.ncb
del /q tttxexport.sln
attrib -H tttxexport.suo
del /q tttxexport.suo
rmdir /q /s Release
cd ..
