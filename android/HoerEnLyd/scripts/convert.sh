#!/bin/bash
for i in *.wav; do
	echo "Converting $i"
	o="${i%*.wav}"
	ffmpeg -i $i ${o}.mp3
done
exit 0
