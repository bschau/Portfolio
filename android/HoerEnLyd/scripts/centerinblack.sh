#!/bin/bash
for i in *.jpg; do
	echo $i
	php -f centerinblack.php $i
done
