#!/bin/bash
CC=gcc
CFLAGS="-Wall -O2 -I .. -I ../../common -DLinux"

function doIt
{
	echo "Building lmake"
	echo $*
	$*
}

doIt $CC $CFLAGS -o ../lmake $(ls ../*.c) ../../common/HashTable.c ../../common/Options.c ../../common/StringBuilder.c ../../common/Utils.c
