#!/bin/bash
if test $(id -u) != "0"; then
	echo "Running myself with escalated privileges ..."
	sudo $0 "$HOME/.backup-files.txt" "$HOME/.backup-excludes.txt"
	exit $?
fi

if test -z "$1" -o -z "$2"; then
	echo "Usage: $0 fileslist excludeslist"
	exit 1
fi

fileslist="$1"
excludeslist="$2"

version="$(hostname -s)"
runtime="/home/bs/runtime"
trap 'rm -fr $runtime' 0
mkdir -p $runtime

for user in bs root; do
	echo "Copying crontab for $user ..."
	crontab -u $user -l > $runtime/${user}-crontab
done

echo "Dumping list of installed packages ..."
dpkg-query -l > $runtime/installed-packages.txt

cd /home/bs
tar czvf "${version}.tar.gz" --exclude-from "$excludeslist" -T "$fileslist"
chown bs:bs "${version}.tar.gz"
exit $?
