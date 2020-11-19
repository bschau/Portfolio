#!/bin/bash
sources="/home/bs/Documents"
dryrun="--dry-run"
slow=""

if test -s "/home/bs/.rsyncrc"; then
	. "/home/bs/.rsyncrc"
else
	echo "/Home/bs/.rsyncrc not found" >&2
	exit 1
fi

if test -z "$RSYNCACCOUNT"; then
	echo "RSYNCACCOUNT not set" >&2
	exit 1
fi

for arg in "$@"; do
	if test "$arg" = "--for-real"; then
		dryrun=""
	elif test "$arg" = "--slow"; then
		slow="yes"
	fi
done

if test -n "$slow"; then
	echo "Running in slow mode."
	mode="--ignore-times"
else
	mode="--omit-dir-times --size-only"
fi

if test -n "$dryrun"; then
	echo "Running in dry-run mode. Invoke this script with the --for-real argument"
	echo "to actually perform the transfers:"
	echo ""
	echo "        $0 --for-real"
	echo ""
	echo ""
	echo "Press enter to continue ..."
	read line
fi

rsync_flags="$dryrun -rvz -zz --progress $mode --delete --exclude=.Spotlight-V100 --exclude=.DocumentRevisions-V100 --exclude=.DS_Store --exclude=.fseventsd --exclude=.Trashes --exclude=.TemporaryItems --exclude=.git --exclude=.gitignore --exclude=.gd --exclude=*~"

#
# Sync to remote
#
for src in $sources; do
	dir="${src##*/}"
	remote="${RSYNCACCOUNT}:${dir}"
	echo "Syncing from $src to $remote ..."
	rsync $rsync_flags --rsh=ssh "${src}/" "${remote}/"
done
exit 0
