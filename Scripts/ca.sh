#!/bin/bash
if test "$(id -u)" != "0"; then
	echo "You must be root" >&2
	exit 1
fi

aliases="/tmp/aliases.$$"
trap "rm -f $aliases" 0

cd /tmp
echo "Adjusting timezone"
cd /etc
rm -f localtime
ln -s /usr/share/zoneinfo/Europe/Copenhagen localtime

echo "Updating system"
apt -y update
apt -y upgrade
apt -y dist-upgrade

echo "Installing programs"
apt -y install python3 python3-pip pylint3 curl git

echo "Upgrading pip3"
pip3 install --upgrade pip

echo "Setting git info"
git config --global user.name "Brian Schau"
git config --global user.email "brian@schau.dk"

echo "Installing nodejs and tools"
curl -sL https://deb.nodesource.com/setup_12.x -o nodesource_setup.sh
chmod 755 nodesource_setup.sh
./nodesource_setup.sh
apt install -y nodejs npm
rm -f nodesource_setup.sh

curl -sL https://dl.yarnpkg.com/debian/pubkey.gpg | apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | tee /etc/apt/sources.list.d/yarn.list
apt update -y
apt install -y yarn

user="cabox"
home="/home/$user"
echo "Setting global npm repository"
sudo -u $user mkdir ${home}/.npm-global
sudo -u $user npm config set prefix ${home}/.npm-global

echo "Updating npm"
sudo -u $user npm install -g npm@latest

echo "Adjusting user profile"
cat > $aliases << BASHRC
alias remtilde='find . -type f -name "*~" -exec rm -f {} \;'
alias sysupd='sudo apt -y update && sudo apt -y upgrade && sudo apt -y dist-upgrade && sudo apt -y autoremove'
alias httphere='python3 -m http.server 3000'
BASHRC

bashrc="/home/cabox/.bashrc"
for i in "$(cat $aliases)"; do
	grep "$i" "$bashrc" >/dev/null 2>&1
	if test $? -eq 1; then
		echo "$i" >> "$bashrc"
	fi
done

grep "export PATH" "$bashrc" >/dev/null 2>&1
if test $? -eq 1; then
	echo "export PATH=\"/home/cabox/.npm-global/bin:$PATH\"" >> "$bashrc"
fi

echo "Cleaning up"
apt -y autoremove

echo "Done - please reboot for changes to take effect!"
exit 0
