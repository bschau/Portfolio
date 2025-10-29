#!/bin/bash
REL="english french german"

make distclean
make initall
for r in $REL; do
	echo
	echo
	echo "Cutting $r release"
	make clean
	make initall
	make $r
	zip sit-${r}.zip CHANGELOG README sit.prc
	mv sit-${r}.zip release
done
