#!/bin/bash
A="$(basename $(pwd))";
L="files.lst"

rm -f $L
while read entry; do
	echo "${A}/${entry}" >> $L
done < FILELIST
exit 0
