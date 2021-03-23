Cups
====

This recipe details how to install the Cups printer system so that you can print to a Hewlett Packard printer (my current printer is the HP Envy 4520 AIO.
Cups will be installed so that it can be accessed from the network.
Furthermore, the Cups spool system will be located on a USB flash disk to minimize wear and tear on your SD card.


Requirements
============

* A Raspberry Pi 3 already installed and modified
* A USB flash disk for the spool area.


Steps
=====

* You must do the steps as root unless otherwise noted.
* You must substitute 192.168.1.242 in the steps below with the IP address of your Raspberry Pi.
* Insert the USB flash disk. Use dmesg to reveal information:

[  465.594442] sd 0:0:0:0: [sda] 15633408 512-byte logical blocks: (8.00 GB/7.45 GiB)
[  465.595797] sd 0:0:0:0: [sda] Write Protect is off
[  465.595812] sd 0:0:0:0: [sda] Mode Sense: 43 00 00 00
[  465.596219] sd 0:0:0:0: [sda] Write cache: disabled, read cache: enabled, doesn't support DPO or FUA
[  465.607069]  sda: sda1
[  465.609873] sd 0:0:0:0: [sda] Attached SCSI removable disk

* If you don't get any partition information, use fdisk to repartition the disk - it should contain one partition.
* Create the filesystem:

	mkfs.ext4 -L Cups /dev/sda1

* Install required packages:

	apt -y install cups hplip hpijs-ppds printer-driver-hpijs

* Add yourself to the _lpadmin_ group so that you can administrate printers:

	usermod -aG lpadmin bs

  ...(use your own username :-)
* Stop cups:

	service cups stop

* Move cups spool to USB flash disk:

echo "LABEL=Cups /var/spool/cups ext4 defaults,noatime 0 1" >> /etc/fstab
cd /var/spool/cups
rm -fr *
cd ..
mount -a
chown root:lp cups
cd cups
mkdir tmp
chmod 1700 tmp
chown root:lp tmp

* Open up Cups for remote administration:

	cupsctl --remote-admin
	service cups restart

* Plug your printer into a USB port on your Raspberry Pi, turn on your printer and browse to https://192.168.1.242:631/admin (substitute with your IP-address) and add your USB printer.

When you access the /admin pages your browser will complain that the certificate is self signed and not trusted. Accept and trust the certificate for future reference.

SSL
===

To enable SSL without a self-signed certificate follow these steps. My server is called _tanya.schau.dk_ - please adjust accordingly below:
* (Optional) Remove foreign and self-signed certificates:

	rm /etc/cups/ssl/*

* Copy the server certificate and certificate key to /etc/cups/ssl. Both files must be PEM encoded:

cp tanya.schau.dk.key /etc/cups/ssl
cp tanya.schau.dk.crt /etc/cups/ssl

* Add the following to `/etc/cups/cupsd.conf`:

	ServerAlias *
	CreateSelfSignedCerts no

* Add your IP/Hostname combo (the one matching the certificate) to _/etc/hosts_:

	echo "192.168.1.242    tanya.schau.dk" >> /etc/hosts

* Restart cups:

	sudo service cups restart


