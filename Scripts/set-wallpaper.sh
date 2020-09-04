#!/bin/bash
WALLPAPERS="/home/bs/00-Vault/Wallpapers"

file="$(ls $WALLPAPERS | sort -R | head -n 1)"
uri="file:///${WALLPAPERS}/$file"
gsettings set org.gnome.desktop.background picture-uri "$uri"
exit 0
