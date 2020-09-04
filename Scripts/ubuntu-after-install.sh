#!/bin/bash
if test "$(id -u)" != "0"; then
	echo "You must be root" >&2
	exit 1
fi

add-apt-repository ppa:zeal-developers/ppa
apt -y update
apt -y upgrade
apt -y dist-upgrade

for p in gnome-tweaks ubuntu-restricted-extras zeal network-manager-openvpn network-manager-openvpn-gnome; do
	apt -y install $p
done

apt -y autoremove

# reset with: gsettings reset org...dash-to-dock extend-height
su -c 'gsettings set org.gnome.shell.extensions.dash-to-dock extend-height false' - bs
su -c 'gsettings set org.gnome.nautilus.preferences use-experimental-views false' - bs
exit 0
