#!/bin/bash
BOOTFS="/media/${USER}/boot"
ROOTFS="/media/${USER}/rootfs"
TMP_IN_MEM="n"
SWAPPINESS="0"

if test -s "pi-config"; then
	. pi-config
else
	echo "pi-config not found" >&2
	exit 1
fi

function _backup
{
	src="$1"
	dirs="${src%/*}"
	if test "$dirs" = "$src"; then
		dirs="."
	fi
	file="${src##*/}"

	dst="${dirs}/orig-${file}"
	if test -e "$dst"; then
		return
	fi

	echo "Creating backup file: $dst"
	cp $src $dst
}

if test "$(id -u)" != "0"; then
	echo "You must run this as root!" >&2
	exit 1
fi

if test ! -d "$BOOTFS/overlays"; then
	echo "$BOOTFS/overlays (from PI boot) not found" >&2
	exit 1
fi

if test ! -d "$ROOTFS/dev"; then
	echo "$ROOTFS/dev (from PI rootfs) not found" >&2
	exit 1
fi

root="$(pwd)"

cd "$BOOTFS"
if test -n "$DT_OVERLAY"; then
	echo "Adjusting boot options"
	_backup config.txt
	grep -v "dtparam=audio=on" orig-config.txt > config.txt
	{
		echo "#dtparam=audio=on"
		echo "dtoverlay=$DT_OVERLAY"
	} >> config.txt
fi

if test "$SSH" = "yes"; then
	echo "Enabling ssh"
	echo ssh > ssh
fi

cd "$ROOTFS/etc"
echo "Adding new aliases.sh file"
echo "alias ll='ls -lpF'" > profile.d/aliases.sh

if test -n "$TIMEZONE"; then
	echo "Setting localtime to $TIMEZONE"
	rm -f localtime
	ln -s /usr/share/zoneinfo/$TIMEZONE localtime
fi

if test "$ETH0" = "yes"; then
	echo "Enabling eth0 with static IP-address"
	_backup dhcpcd.conf
	{
		cat orig-dhcpcd.conf
		echo "interface eth0"
		echo "static ip_address=$IP_ADDRESS"
		echo "static routers=$ROUTERS"
		echo "static domain_name_servers=$DNS"
	} > dhcpcd.conf
fi

if test -s "$root/sshd_config"; then
	echo "Setting new SSHD config"
	_backup ssh/sshd_config
	cp "$root/sshd_config" ssh
elif test "$SSH_USE_SSHD_CONFIG" = "yes"; then
	echo "Using supplied SSHD config"
	_backup ssh/sshd_config
	cp "$root/sshd_config.in" ssh/sshd_config
fi

if test -n "$SSH_PUBLIC_KEY"; then
	echo "Fixing authorized_keys for 'pi' user"
	pissh="$ROOTFS/home/pi/.ssh"
	mkdir -p "$pissh"
	cp "$SSH_PUBLIC_KEY" "$pissh/authorized_keys"
	chmod 600 "$pissh/authorized_keys"
	chmod 700 "$pissh"
	chown -R 1000:1000 "$pissh"
fi

if test "$ENABLE_AVAHI" = "no"; then
	echo "Installing new avahi-daemon defaults file"
	_backup default/avahi-daemon
	{
		echo "AVAHI_DAEMON_DETECT_LOCAL=0"
		echo "AVAHI_DAEMON_START=0"
	} > default/avahi-daemon
fi

if test "$ENABLE_BLUETOOTH" = "no"; then
	echo "Disabling bluetooth"
	_backup default/bluetooth
	grep -v "BLUETOOTH_ENABLED" default/orig-bluetooth > default/bluetooth
	echo "BLUETOOTH_ENABLED=0" >> default/bluetooth
fi

if test -n "$BLACKLIST_MODULES"; then
	for module in $BLACKLIST_MODULES; do
		echo "Disabling module: $module"
		echo "blacklist $module" >> modprobe.d/blacklist.conf
	done
fi

if test "${TMP_AND_LOG_ON_EXTERNAL_DISK}" = "y"; then
	echo "Adding new partitions"
	_backup fstab
	{
	sed s/defaults/defaults,noatime/g orig-fstab
	echo "LABEL=TMP /tmp ext4 defaults,noatime 0 1"
	echo "LABEL=LOG /var/log ext4 defaults,noatime 0 1"
	} > fstab
	rm -fr ../var/tmp
	ln -s /tmp ../var/tmp
elif test "${TMP_IN_MEM}" = "y"; then
	echo "Adding new partitions"
	_backup fstab
	{
	sed s/defaults/defaults,noatime/g orig-fstab
	echo "tmpfs /tmp tmpfs defaults,noatime,nosuid,nodev,noexec,mode=0755,size=${TMP_MEMORY_SIZE}M 0 0"
	echo "tmpfs /var/tmp tmpfs defaults,noatime,nosuid,nodev,noexec,mode=0755,size=${VAR_TMP_MEMORY_SIZE}M 0 0"
	echo "tmpfs /var/log tmpfs defaults,noatime,nosuid,nodev,noexec,mode=0755,size=${VAR_LOG_MEMORY_SIZE}M 0 0"
	} > fstab
fi

if test "$ENABLE_SWAPFILE" = "no"; then
	echo "Preventing auto-swap file"
	_backup init.d/dphys-swapfile
	{
		echo "#!/bin/bash"
		echo "exit 0"
		cat init.d/orig-dphys-swapfile
	} > init.d/dphys-swapfile
fi

if test -n "$SSH_USER"; then
	echo "Enabling $SSH_USER to have sudo access"
	_backup sudoers
	echo "$SSH_USER 	ALL=(ALL:ALL) ALL" >> sudoers
fi

echo "Changing swappiness"
_backup sysctl.conf
{
	grep -v "vm.swappiness=" orig-sysctl.conf
	echo "vm.swappiness=$SWAPPINESS"
} > sysctl.conf

if test -n "$HOSTNAME"; then
	echo "Backing up hosts file"
	_backup hosts
fi

echo "Copying stage files to pi-root"
cp "$root/pi-config" "$ROOTFS/root"
chmod 600 "$ROOTFS/root/pi-config"
cp "$root/setup.sh" "$ROOTFS/root"
chmod 700 "$ROOTFS/root/setup.sh"
cp "$root/finish.sh" "$ROOTFS/root"
chmod 700 "$ROOTFS/root/finish.sh"
if test -n "$SSH_PUBLIC_KEY"; then
	cp "$SSH_PUBLIC_KEY" "$ROOTFS/root"
	key="${SSH_PUBLIC_KEY##*/}"
	chmod 700 "$ROOTFS/root/$key"
fi

echo "Syncing ..."
sync

cd "$root"
umount "$BOOTFS"
umount "$ROOTFS"
echo "Done for now - boot your pi and continue setup on the pi:"
echo
echo "(on pi, as pi user):      sudo /root/setup.sh"
exit 0
