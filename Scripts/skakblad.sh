#!/bin/bash
url="http://skak.dk/images/skakbladet/seneste.pdf"
latest="$HOME/.skakblad"

if test "$1" = "delayed"; then
	sleep 20
fi

if test -s "$latest"; then
	latest_last_modified="$(cat $latest)"
else
	latest_last_modified=""
fi

if test -z "$latest_last_modified"; then
	latest_last_modified="Mon,1Jan201803:14:15GMT"
fi

last_modified="$(curl --silent --HEAD "$url" | grep Last-Modified | cut -b 16- | tr -d [:space:])"

if test "$latest_last_modified" = "$last_modified"; then
	exit 0
fi

cd $HOME/Desktop
skakblad="skakblad-$(date +'%Y%m%d').pdf"
curl --silent -o "$skakblad" "$url"

if test -s "$skakblad"; then
	echo "$last_modified" > "$latest"
	notify-send -i face-smile "Seneste skakblad er hentet til skrivebordet ..."
else
	notify-send -i face-sad "Kunne ikke hente seneste skakblad"
fi
exit 0
