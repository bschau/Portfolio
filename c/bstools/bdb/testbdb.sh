#!/bin/bash
FILE=$0
DB=testdb
PATH=".:$PATH"
export PATH

function replace
{
	echo "Writing:"
	loop=0
	while test $loop -le 100; do
		../bdb -f $FILE $DB replace $loop
		loop=$(($loop+1))
	done
}

function fetch
{
	echo "Reading:"
	loop=0
	while test $loop -le 100; do
		../bdb -f /dev/null $DB fetch $loop
		loop=$(($loop+1))
	done
}

rm -f $DB
time replace
echo
time fetch
echo
ls -l $DB
