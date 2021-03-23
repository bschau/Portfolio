#!/bin/bash
if test "$(id -u)" != "0"; then
	echo "You must run this as root!" >&2
	exit 1
fi

echo "Removing old pi user"
userdel -r pi

echo "Done - now go ahead and tinker with your server ... :-)"
exit 0
