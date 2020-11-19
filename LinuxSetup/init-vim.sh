#!/bin/bash
if test -e "$HOME/.init-vim"; then
	echo "$HOME/.init-vim exists - remove and re-run" >&2
	exit 1
fi

if test ! -d "$HOME/go"; then
	echo "go-lang is not installed in $HOME/go" >&2
	exit 1
fi

if test ! -x /usr/local/bin/vim; then
	echo "No self-compiled vim found in /usr/local/bin" >&2
	exit 1
fi

if test -s "/etc/lsb-release"; then
	. /etc/lsb-release
else
	echo "/etc/lsb-release not found" >&2
	echo "Must run on debian, ubuntu or manjaro" >&2
	exit 1
fi

cd $HOME

function prepare_environment
{
	touch .init-vim

	echo "Removing old .vim folder"
	rm -fr .vim
}

function remove_old_vim
{
	oldvim="$(dpkg -l | grep vim | tr -s ' ' | cut -f 2 -d ' ')"
	echo "Removing old vim installation: $oldvim"
	sudo apt remove -y $oldvim
}

function install_modern_nodejs
{
	echo "Installing nodejs and tools"
	curl -sL https://deb.nodesource.com/setup_12.x -o nodesource_setup.sh
	chmod 755 nodesource_setup.sh
	sudo ./nodesource_setup.sh
	sudo apt install -y nodejs npm
	rm -f nodesource_setup.sh

	curl -sL https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
	echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
	sudo apt update -y
	sudo apt install -y yarn
}

function redirect_npm_packages
{
	mkdir ~/.npm-global
	npm config set prefix '~/.npm-global'
}

ubuntu=""
manjaro=""
if test "$DISTRIB_CODENAME" = "xenial"; then
	ubuntu="yes"
	sudo apt install -y curl
	prepare_environment

	echo "Installing miscallenous tools"
	sudo apt install -y software-properties-common python3 python3-pip pylint3 gcc g++ make ctags

	remove_old_vim
	install_modern_nodejs
	redirect_npm_packages
elif test "$DISTRIB_CODENAME" = "bionic"; then
	ubuntu="yes"
	sudo apt install -y curl
	prepare_environment

	echo "Installing miscallenous tools"
	sudo apt install -y software-properties-common python3 python3-pip pylint3 gcc g++ make ctags

	sudo apt install -y vim-common vim-runtime vim-nox
	install_modern_nodejs
	redirect_npm_packages
elif test "$DISTRIB_CODENAME" = "eoan"; then
	ubuntu="yes"
	sudo apt install -y curl
	prepare_environment

	echo "Installing miscallenous tools"
	sudo apt install -y software-properties-common python python-pip pylint gcc g++ make ctags
	sudo apt install -y vim-common vim-runtime vim-nox
	sudo apt install -y nodejs npm yarn
	redirect_npm_packages
elif test "$DISTRIB_CODENAME" = "focal"; then
	ubuntu="yes"
	sudo apt install -y curl
	prepare_environment

	echo "Installing miscallenous tools"
	sudo apt install -y software-properties-common python3 python3-pip pylint gcc g++ make ctags
	sudo apt install -y nodejs npm yarn
	redirect_npm_packages
elif test "$DISTRIB_CODENAME" = "groovy"; then
	ubuntu="yes"
	sudo apt install -y curl
	prepare_environment

	echo "Installing miscallenous tools"
	sudo apt install -y software-properties-common python3 python3-pip pylint gcc g++ make universal-ctags
	sudo apt install -y nodejs npm yarn
	redirect_npm_packages
elif test "$DISTRIB_CODENAME" = "Lysia"; then
	manjaro="yes"
	sudo pacman -S --noconfirm curl
	prepare_environment

	sudo pacman -S --noconfirm nodejs npm yarn
	redirect_npm_packages
else
{
	echo "Unsupported OS - must be one of:"
	echo
	echo "Ubuntu 16.04 - Xenial"
	echo "Ubuntu 18.04 - Bionic"
	echo "Ubuntu 19.10 - Eoan"
	echo "Ubuntu 20.04 - Focal"
	echo "Ubuntu 20.10 - Groovy"
	echo "Manjaro 20 - Lysia"
} >&2
	exit 1
fi

echo "Installing go language server"
GO111MODULE=on
export GO111MODULE
go get golang.org/x/tools/gopls@latest

echo "Updating npm"
npm install -g npm@latest

echo "Installing global npm packages"
npm install -g typescript
npm install -g tserver

echo "Installing gruvbox theme"
mkdir -p $HOME/.vim/pack/default/start
cd $HOME/.vim/pack/default/start
git clone https://github.com/morhetz/gruvbox.git

echo "Installing vim-fugitive"
mkdir -p $HOME/.vim/pack/tpope/start
cd $HOME/.vim/pack/tpope/start
git clone https://tpope.io/vim/fugitive.git
vim -u NONE -c "helptags fugitive/doc" -c q

echo "Installing CoC"
mkdir -p $HOME/.vim/pack/coc/start
cd $HOME/.vim/pack/coc/start
curl --fail -L https://github.com/neoclide/coc.nvim/archive/release.tar.gz | tar xzfv -

if test "$ubuntu" = "yes"; then
	echo "Autoremoving unused packages"
	sudo apt autoremove -y
fi

echo "Attempting to CocInstall - press enter to continue. Exit vim when done."
read a
vim -c "CocInstall coc-json" -c "CocInstall coc-tsserver" -c "CocInstall coc-html" -c "CocInstall coc-css" -c "CocInstall coc-python" -c "CocInstall coc-go"
# you can have up to 10 -c commands
exit 0
