#!/bin/bash
if test "$(id -u)" != "0"; then
	echo "You must run this as root!" >&2
	exit 1
fi

if test -s pi-config; then
	. pi-config
else
	echo "pi-config not found" >&2
	exit 1
fi

if test -n "$HOSTNAME"; then
	hostnamectl set-hostname "$HOSTNAME"
	{
		cat /etc/orig-hosts | grep -v raspberrypi
		echo "127.0.1.1 $HOSTNAME"
	} > /etc/hosts
fi

if test -n "$SSH_USER"; then
	echo "Adding user: $SSH_USER"
	useradd -m $SSH_USER
	chsh -s /bin/bash $SSH_USER
	passwd $SSH_USER
	if test -n "$SSH_PUBLIC_KEY"; then
		mkdir /home/$SSH_USER/.ssh
		key="${SSH_PUBLIC_KEY##*/}"
		cp "$key" /home/bs/.ssh/authorized_keys
		chown -R $SSH_USER:$SSH_USER /home/$SSH_USER/.ssh
		chmod 700 /home/$SSH_USER/.ssh
		chmod 600 /home/$SSH_USER/.ssh/authorized_keys
	else
		echo "No SSH public key given - you will not be able to SSH to the server"
	fi
fi

echo "Updating repositories and adding new packages"
apt -y update
apt -y upgrade
apt -y dist-upgrade
apt -y autoremove

if test -n "$EXTRA_PACKAGES"; then
	for p in $EXTRA_PACKAGES; do
		echo "Installing: $p"
		apt -y install $p
	done
fi

if test "$ENABLE_AVAHI" = "no"; then
	echo "Disabling Avahi"
	update-rc.d avahi-daemon disable
fi

if test "$ENABLE_BLUETOOTH" = "no"; then
	echo "Disabling bluetooth"
	update-rc.d bluetooth disable
fi

if test -n "$DISABLE_SERVICES"; then
	for s in $DISABLE_SERVICES; do
		echo "Disabling $s"
		update-rc.d $s disable
	done
fi

echo "Reconfiguring locales - this will open a gui"
echo "Enter to continue"
read a
dpkg-reconfigure locales

echo "Removing execute bit on /usr/sbin/rfkill"
chmod -x /usr/sbin/rfkill

echo "Done - run finish.sh as user $SSH_USER after reboot:"
echo
echo "    sudo /root/finish.sh"
echo "Enter to continue"
read a
reboot
exit 0
