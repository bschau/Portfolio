@echo off
echo Building lmake.exe
tcc -Wall -I .. -I ..\..\common -o ..\lmake.exe ..\Command.c ..\Context.c ..\Expand.c ..\External.c ..\Lexer.c ..\LMake.c ..\LMParse.c ..\Main.c ..\OS.c ..\TIf.c ..\Var.c ..\XUtils.c ..\..\common\HashTable.c ..\..\common\Options.c ..\..\common\StringBuilder.c ..\..\common\Utils.c ..\..\common\WinUtils.c
