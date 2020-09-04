#!/bin/bash
DST="${HOME}/Github"
USERS="bschau"
if test ! -d "$DST"; then
	echo "Directory $DST not found" >&2
	exit 1
fi

if test -n "$SSH_ENV"; then
	if test -z "$(ssh-add -l 2>/dev/null)"; then
		echo "Preloading SSH keys ..."
		ssh-add
	fi
fi

cd "$DST"
out="/tmp/git-dl-out.$$"
repos="/tmp/git-dl-repos.$$"
trap "rm -f $out $repos" 0

rm -f $repos
for user in $USERS; do
	echo "Getting list of Github repositories for $user"
	curl -u $user -s https://api.github.com/user/repos -H Accept: application/vnd.github.v3.full+json > $out

	grep message $out >/dev/null 2>&1
	if test $? -eq 0; then
		cat $out
		exit 1
	fi

	grep ssh_url $out | cut -f 6 -d ' ' | tr -d '",' >> $repos
done

while read repo; do
	cd "$DST"
	git_name="$(echo $repo | cut -f 2 -d '/')"
	folder_name="${git_name%*.git}"
	if test -d "$folder_name"; then
		echo "Project $folder_name already exists - updating"
		cd $folder_name
		git pull origin master
	else
		echo "Creating project: $folder_name"
		git clone $repo
	fi
done < $repos

if test -n "$RSYNCACCOUNT"; then
	cd "$DST"
	rsync_flags="-rvz -zz --progress --omit-dir-times --size-only --delete --exclude=.git --exclude=.gitignore  --exclude=.gd"
	rsync $rsync_flags --rsh=ssh . "${RSYNCACCOUNT}:Github/"
fi

exit 0
