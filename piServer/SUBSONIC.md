Subsonic
========

This recipe details how to install the Subsonic Music server (http://www.subsonic.org/).
The server will be installed with HTTPS enabled.
The music served by Subsonic will be stored on a separate USB flash disk.
If you intend to serve Podcasts from your Subsonic server you may want to use another USB flash disk for this purpose.


Requirements
============

* A Raspberry Pi 3 already installed and modified
* Subsonic-6.1.5
* A SSL certificate
* A USB flash disk for the music
* Optionally (but recommended), a USB flash disk for podcasts


Steps
=====

* In the steps below, /some/device should be substituted with the correct block device.
* You must to the steps as root unless otherwise noted.
* Prepare the USB flash disk for music on a linux system:

	mkfs.ext4 -L Music /some/device

* Mount the partition and copy your mp3 files over.
* Optionally (but recommended), prepare the USB flash disk for podcasts on a linux system:

	mkfs.ext4 -L Subsonic /some/device

* Insert the _Music_ and _Subsonic_ devices into your Raspberry Pi 3.
* Add the following to `/etc/fstab`:

LABEL=Subsonic /home/subsonic/tmp ext4 defaults,users,noatime      0 1
LABEL=Music /home/subsonic/Music ext4 defaults,users,noatime      0 1

* Add and prepare the subsonic user:

	useradd -m subsonic
	chsh -s /usr/sbin/nologin subsonic
	mkdir /home/subsonic/Music
	mkdir /home/subsonic/tmp
	mount -a
	mkdir /home/subsonic/tmp/Podcasts
	chown -R subsonic:subsonic /home/subsonic

* Install dependencies:

	apt install openjdk-8-jre

* Install subsonic:

	dpkg -i subsonic-6.1.5.deb

* Add the following to /etc/default/subsonic:

SUBSONIC_ARGS="--max-memory=300"
SUBSONIC_USER="subsonic"

* Reboot your server.
* Now you can log on to your server using port 4040 on the public IP and configure the rest of Subsonic.
* Remember that the music is stored in /home/subsonic/Music and podcasts in /home/subsonic/tmp/Podcasts.


SSL
===

To enable SSL you must first create a suitable pkcs12 file. The pkcs12 file must follow this format:

	certificate-key
	certificate
	intermediates

You must assemble these informations from your existing certificate (using f.ex. OpenSSL) or create a new certificate (Let's Encrypt is fine).
Then:

cat certificate-key certificate intermediates > subsonic.crt
openssl pkcs12 -in subsonic.crt -export -out subsonic.pkcs12
cp subsonic.pkcs12 /etc/ssl
chmod 644 /etc/ssl/subsonic.pkcs12
rm -f subsonic.crt

Make a note of the password you specify to the OpenSSL command - you will need it below!

Then update /etc/default/subsonic:

SUBSONIC_ARGS="--https-port=4443 --port=4040 {Other Args, such as --max-memory}"

Update /usr/share/subsonic/subsonic.sh around line 131, insert:

    -Dsubsonic.ssl.keystore=/etc/ssl/subsonic.pkcs12 \
    -Dsubsonic.ssl.password=...password-from-above... \

Restart Subsonic and log on to http://your-server:4040/ – you should be redirected to https://your-server:4443/.

