#!/bin/bash
M3UDEFS="/home/bs/GDrive/bin/etc/music.m3us"
MUSIC="/home/bs/00-Vault"
KWED="${MUSIC}/Music/Computer/KWED"
OUTPUT="/home/bs/Musica"

if test ! -s "$M3UDEFS"; then
	echo "Not found: $M3UDEFS" >&2
	exit 1
fi

if test ! -d "$MUSIC"; then
	echo "Not found: $MUSIC" >&2
	exit 1
fi

function check
{
	line=0
	errors=0
	declare -A songs
	while read song; do
		line=$(($line+1))

		path="$(echo "$song" | cut -f 2 -d ':' | cut -f 1 -d '|')"
		if test ! -s "$MUSIC/$path"; then
			echo "Not found: ${line}:$song" >&2
			continue
		fi

		real="$(echo "$song" | cut -f 1 -d '|')"
		newname="$(echo "$song" | cut -f 2 -d '|')"
		if test "$newname" = "$real"; then
			key="${real##*/}"
		else
			key="$newname"
		fi

		realkey="$(echo $key | tr '[:upper:]' '[:lower:]' | base64)"
		if test -z "${songs["$realkey"]}"; then
			songs["$realkey"]="${line}:$song"
		else
			target=${songs["$realkey"]}
			echo "${line}:$key exists: $target" >&2
			errors=$(($errors+1))
		fi
	done < $M3UDEFS

	if test $errors -gt 0; then
		echo "Check completed with errors"
		exit 1
	fi
}

function build-layout
{
	mkdir "$OUTPUT"
	while read song; do
		list="$(echo "$song" | cut -f 1 -d ':')"
		trail="$(echo "$song" | cut -f 2 -d ':')"
		sourcefile="$(echo "$trail" | cut -f 1 -d '|')"
		newname="$(echo "$trail" | cut -f 2 -d '|')"

		if test "$newname" = "$sourcefile"; then
			destfile="${sourcefile##*/}"
		else
			destfile="$newname"
		fi

		folder="$OUTPUT/$list"
		if test ! -d "$folder"; then
			echo "Opening new playlist: $list"
			mkdir $folder
		fi

		echo "$MUSIC/$sourcefile -> $list/$destfile"
		cp "$MUSIC/$sourcefile" "$folder/$destfile"
	done < $M3UDEFS
}

function build-list
{
	src="$1"
	list="$2"
	p="$(echo $prefix | sed s/%l/$list/g)"
	{
		echo "#EXTM3U"
		while read file; do
			file="${file#*/}"
			echo "#EXTINF:0,$file"
			echo "${p}${file}"
			echo
		done < "$src"
	} | sed 's/$'"/`echo \\\r`/" > "${list}.m3u"
}

function create-playlists
{
	lists="$(ls $OUTPUT)"
	tmpfile="/tmp/mk-musica.$$"
	trap "rm -f $tmpfile" 0
	for list in $lists; do
		cd $OUTPUT
		if test ! -d "$list"; then
			continue
		fi

		cd "$list"
		echo "* $list"

		find . -type f > $tmpfile
		build-list "$tmpfile" "$list"
	done
}

function create-playlists-lite
{
	mkdir "$OUTPUT"
	while read song; do
		list="${OUTPUT}/$(echo "$song" | cut -f 1 -d ':').m3u"
		trail="$(echo "$song" | cut -f 2 -d ':')"
		name="${trail##*/}"

		if test ! -s "$list"; then
			echo "#EXTM3U" > $list
		fi

		{
			echo "#EXTINF:0,$name"
			echo "${prefix}${trail}"
			echo
		} >> ${list}
	done < $M3UDEFS
}

if test "$1" = "check"; then
	check
elif test "$1" = "build"; then
	if test -e "$OUTPUT"; then
		echo "Already exists: $OUTPUT" >&2
		echo "Please remove and then re-run" >&2
		exit 1
	fi

	if test -z "$2"; then
		prefix=""
	else
		prefix="$2"
	fi
	export prefix

	echo "Validating $M3UDEFS ..."
	check

	echo "Building layout from music.m3us ..."
	build-layout

	echo "Creating playlists ..."
	create-playlists
elif test "$1" = "lite"; then
	if test -e "$OUTPUT"; then
		echo "Already exists: $OUTPUT" >&2
		echo "Please remove and then re-run" >&2
		exit 1
	fi

	if test -z "$2"; then
		prefix=""
	else
		prefix="$2"
	fi
	export prefix

	echo "Validating $M3UDEFS ..."
	check

	echo "Creating playlists ..."
	create-playlists-lite
else
	echo "Usage: $0 build [prefix] | lite [prefix] | check" >&2
	echo >&2
	echo "[prefix], if given, is prefix added to songs on the playlists." >&2
	echo "Use %l in prefix to denote current list." >&2
	exit 1
fi
exit 0
