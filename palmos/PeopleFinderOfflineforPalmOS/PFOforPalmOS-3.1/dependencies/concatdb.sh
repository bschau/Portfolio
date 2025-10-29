#!/bin/sh
DESKTOP="/cygdrive/c/Documents and Settings/schaub/Desktop"
{
	DB="$DESKTOP/pdadb"

	idx=0

	while test -s "$DB/db_$idx"; do
		cat "$DB/db_$idx"
		idx=$(($idx+1))
		echo -n "." >&2
	done
	echo >&2
} > "$DESKTOP/Peoples.txt"
exit 0
