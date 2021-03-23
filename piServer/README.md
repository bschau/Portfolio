piServer
========

Result:

* Raspbian installed
* SSH enabled and secured
* Fixed IP address configured on ETH0 (builtin network interface)
* Fixed hostname
* A non-standard user installed
* The standard 'pi' user removed
* Various services disabled (notably bluetooth and Avahi)
* Various performance improvements (tmp files in RAM and others)


Requirements
============

Raspbian 2019-04-08 Lite Edition:
	https://www.raspberrypi.org/downloads/raspbian/

Ubuntu 18.04:
	https://www.ubuntu.com/


Steps
=====

* Download Raspbian and "burn" it to a flash drive.
* Unplug and plug the flash drive so that the root and boot partitions on the flash drive is mounted.
* Make a copy of the pi-config.in file:

	cp pi-config.in pi-config

* Make the necessary adjustments to the pi-config file.
* Double-check and triple-check (if necessary) the $ROOTFS and $BOOTFS variables in the pi-config file. If you set this incorrectly, you may damage your system!
* Run the prepare.sh script to perform the changes:

	sudo prepare.sh

* When the script is finished, unplug the flash disk and plug it into your Raspberry Pi. Reboot your Raspberry Pi.
* SSH to your Raspberry Pi:

	ssh pi@_ip-address_

* Run the setup.sh script:

	sudo su -
	cd /root
	./setup.sh

* Answer the various prompts (typically password of the custom user and locale settings).
* Let the script run and reboot your Raspberry Pi when it is done.
* SSH to your Raspberry Pi as the custom user:

	ssh _user_@_ip-address_

* Run the finish.sh script:

	sudo /root/finish.sh

* Reboot your Raspberry Pi.
* Optionally, delete all the files from the /root folder - they're not needed anymore.

