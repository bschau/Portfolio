#!/bin/bash
function _cp
{
	echo "* $1"
	dst="$share/$2"
	mkdir -p "$dst"
	cp "$1" "$dst"
}

share="$HOME/.local/share"
for app in bitwarden set-wallpaper; do
	echo "Installing $app"
	_cp ${app}.desktop applications
	_cp ${app}.png icons
done
echo "Done"
exit 0
