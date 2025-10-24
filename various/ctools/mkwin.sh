#!/bin/bash
DOCS="CHANGELOG LICENSE README"

make distclean
make build-win

# Prepare version
set -- $(grep "from bstools" common/common.h)
prog="$(echo $5 | tr -d '";')"
title="$(echo $prog | tr '-' ' ')"
set -- $title
version="$(echo $2 | tr -d 'v')"

cat > sed.script <<EOF
s/@@PROG@@/$prog/g
s/@@TITLE@@/$title/g
s/@@VERSION@@/$version/g
EOF

sed -f sed.script setup.tmpl > setup.iss

# Prepare documents
for i in $DOCS; do
	echo "Converting $i to DOS / Windows format"
	sed 's/$/\r/' $i > ${i}.txt
done

# Compile
iscc setup.iss

# Cleanup
for i in $DOCS; do
	echo "Removing ${i}.txt"
	rm -f ${i}.txt
done

exit 0
