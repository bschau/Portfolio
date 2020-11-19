#!/bin/bash
if test "$(id -u)" != "0"; then
	echo "You must be root" >&2
	exit 1
fi

cd /usr/src
if test -d rtw88; then
	cd rtw88
	echo "Updating ..."
	git pull
else
	echo "Cloning ..."
	git clone https://github.com/lwfinger/rtw88.git
	cd rtw88
fi
echo "Building ..."
make

echo "Installing ..."
make install

echo "Please reboot for new modules to be loaded."
exit 0
