#!/bin/bash
for i in *.jpg; do
	mv $i $i.orig
	echo "Converting $i.orig to $i"
	convert $i.orig -resize 200x300 $i
done
