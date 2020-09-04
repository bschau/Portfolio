#!/bin/bash
case "$1" in
	"stage1")
		sudo apt -y update
		sudo apt -y install wget libglapi-mesa libxdamage1 libxfixes3 libxcb-glx0 libxcb-dri2-0 libxcb-dri3-0 libxcb-present0 libxcb-sync1 libxshmfence1 libxxf86vm1
		cd ~ && wget -O - "https://www.dropbox.com/download?plat=lnx.x86_64" | tar xzf -
		~/.dropbox-dist/dropboxd
		;;
	"stage2")
		sudo wget -O /usr/local/bin/dropbox "https://www.dropbox.com/download?dl=packages/dropbox.py"
		sudo chmod 755 /usr/local/bin/dropbox

		# Autostart
		srvfile="/tmp/id.$$"
		trap "rm -f $srvfile" 0

		me="$(whoami)"
		cat >$srvfile <<EOF
[Unit]
Description=Dropbox Service
After=network.target

[Service]
ExecStart=/bin/sh -c '/usr/local/bin/dropbox start'
ExecStop=/bin/sh -c '/usr/local/bin/dropbox stop'
PIDFile=${HOME}/.dropbox/dropbox.pid
User=${me}
Group=${me}
Type=forking
Restart=on-failure
RestartSec=5
StartLimitInterval=60s
StartLimitBurst=3

[Install]
WantedBy=multi-user.target
EOF

		sudo cp $srvfile /etc/systemd/system/dropbox.service

		sudo systemctl daemon-reload
		sudo systemctl enable dropbox
		sudo systemctl start dropbox
		dropbox lansync n
		;;

	*)
		echo "Usage: $0 stage1 | stage2" >&2
		echo >&2
		echo "Run stage2 after account has been linked" >&2
		exit 1
		;;
esac
exit 0
