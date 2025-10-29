#!/bin/bash
for i in *.jpg; do
	mv $i $i.orig
	echo "Converting $i.orig to $i"
	convert $i.orig -resize 800x480 $i
done
