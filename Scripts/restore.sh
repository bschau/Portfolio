#!/bin/bash
root="/media/bs/DATA"
folders="Movies Vault"

if test -z "$RSYNCACCOUNT"; then
	echo "RSYNCACCOUNT not set" >&2
	exit 1
fi

rsync_flags="-rvz -zz --progress --omit-dir-times --size-only --delete --exclude=.Spotlight-V100 --exclude=.DocumentRevisions-V100 --exclude=.DS_Store --exclude=.fseventsd --exclude=.Trashes --exclude=.TemporaryItems --exclude=.git --exclude=.gitignore --exclude=.gd --exclude=*~"

#
# Sync from remote
#
for fld in $folders; do
	remote="${RSYNCACCOUNT}:${fld}"
	dst="${root}/${fld}"
	echo "Syncing from $remote to $dst..."
	rsync $rsync_flags --rsh=ssh "${remote}/" "${dst}/"
done
exit 0
