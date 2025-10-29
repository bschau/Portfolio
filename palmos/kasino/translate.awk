#!/bin/awk -f
#
#	translate.awk
#
#	This script is used to translate .rcp files from locale tokens
#	(@@token@@) to locale text.
#
#	Usage:
#		awk -f translate.awk locale .rcp"
#
#	Versioning:
#
#	1.0	Brian Schau <brian@schau.com>
#		Initial release		
#
BEGIN {
	if (ARGC==3) {
		rules=ARGV[1];
		script=ARGV[2];
	} else {
		print "Usage: awk -f translate.awk locale .rcp" > "/dev/stderr"
		retCode=1
		exit
	}

	FS="\t"
	retCode=0
	delete pairs
	line=0
	lastKey=""
	while ((status=getline < rules)>0) {
		line++

		# find first nonzero length entry
		found=0
		for (idx=1; idx<=NF; idx++) {
			if (length($idx)>0) {
				found=1
				break
			}
		}

		if (found) {
			if (substr($idx, 0, 1)=="#") {
				lastKey=""
			} else {
				if (idx==1) {	# found key
					key=tolower($idx)
					if (key in pairs) {
						print "("rules":"line") Translationkey "key" already set." > "/dev/stderr"
						retCode=1
					}

					text=""
					for (idx++; idx<=NF; idx++) {
						if (length($idx)>0) {
							text=$idx
							break
						}
					}
					pairs[key]=text
					lastKey=key
				} else {	# found more text
					if (length(lastKey)>0) {
						text=pairs[lastKey]
						pairs[lastKey]=(text $idx)
					} else {
						print "("rules":"line") Stray text." > "/dev/stderr"
						retCode=1
					}
				}
			}
		} else
			lastKey=""
	}
	close(rules)

	if (status==0) {
		ARGV[1]=script
		ARGV[2]=""
	} else {
		print "("rules") Getline failed on - file not found?." > "/dev/stderr"
		retCode=1
		exit
	}
}

// {
	outStr=""
	input=$0
	
	while (1) {
		tokenStart=index(input, "@@")
		if (tokenStart==0) {
			if (length(outStr)>0) {
				if (length(input)>0)
					outStr=(outStr input)

				print outStr
			} else
				print $0

			next
		}

		outStr=(outStr substr(input, 1, tokenStart-1))
		input=substr(input, tokenStart+2)
		tokenEnd=index(input, "@@")
		if (tokenEnd==0) {
			print "("FILENAME":"NR") Open key starting at position "tokenStart"." > "/dev/stderr"
			retCode=1
			next
		}

		token=tolower(substr(input, 1, tokenEnd-1))
		if (token in pairs)
			outStr=(outStr pairs[token])
		else {
			print "("FILENAME":"NR") No such translation key "token"." > "/dev/stderr"
			retCode=1
			next
		}

		input=substr(input, tokenEnd+2)
	}
}

END {
	exit retCode
}
