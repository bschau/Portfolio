Translate
---------

Translate is used during the build phase of Palm OS programs. It will, based
on language files and the .rcp file generate a new .rcp file with the strings
translated to the selected language.



Install - Windows
-----------------

You need to have tcc for Win32 installed. Get it from:

	http://fabrice.bellard.free.fr/tcc/

You also need to have LMake installed. Get it from:

	http://www.schau.com/gz/linux/lmake/index.html

Then:

	cd Translate-x.y
	lmake

... and follow the instructions.



Install - Linux
---------------

You need to have gcc installed.  Get it from http://gcc.gnu.org/ or from your
vendor.

You also need to have LMake installed. Get it from:

	http://www.schau.com/gz/linux/lmake/index.html

Then:

	cd Translate-x.y
	lmake

... and follow the instructions.

If you are a vim user you can install the trl.vim file to $VIM_HOME/syntax and
then add the following two lines to filetype.vim:

	" Translation
	au BufNewFile,BufRead *.trl	setf trl



Documentation
-------------

Full documentation can be found at:

	http://www.schau.com/



Thanks for evaluating Translate.



Brian Schau <mailto:brian@schau.com>
