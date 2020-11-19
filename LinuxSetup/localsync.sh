#!/bin/bash
vault="/home/bs/00-Vault"
remote="/media/bs/VaultBackup/00-Vault"
dryrun="--dry-run"
slow=""

if test ! -d "${remote}"; then
	echo "${remote} seems not to be mounted?" >&2
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
# Sync to SD Card
#
echo "Synchronizing $vault to $remote"
rsync $rsync_flags "$vault/" "$remote/"
exit 0
