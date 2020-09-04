#!/bin/bash
if test "$(hostname)" = "stacy"; then
	PICTURES="/home/bs/MyPictures/Pictures"
else
	PICTURES="/home/bs/00-DATA/Unrecoverable/Pictures"
fi
export PICTURES

{
	find $PICTURES -type f | while read line; do
		exif="$(identify -verbose $line | grep exif:DateTimeOriginal | cut -f 6,7 -d ' ')"
		if test -z "$exif"; then
			echo "MISSING	$line"
		else
			echo "$exif	$line"
		fi
	done
}
exit 0
