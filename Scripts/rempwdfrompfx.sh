#!/bin/bash
OUTFILE="export-out.pfx"
if test $# -ne 1; then
	echo "$0 path-to-file.pfx" >&2
	exit 1
fi

PFXIN="$1"
if test ! -s "$PFXIN"; then
	echo "$PFXIN is not a regular pfx file" >&2
	exit 1
fi

if test -e "$OUTFILE"; then
	echo "$OUTFILE already exists" >&2
	exit 1
fi

while :; do
	echo "Enter PFX Password"
	read pass
	set -- $pass
	pass="$1"
	if test -n "$pass"; then
		break
	fi
done

CERTOUT="/tmp/certout.$$"
CAOUT="/tmp/caout.$$"
PRVKEY="/tmp/prvkey.$$"
NEWKEY="/tmp/newkey.$$"
NEWPEM="/tmp/newpem.$$"
trap "rm -f $CERTOUT $CAOUT $PRVKEY $NEWKEY $NEWPEM" 0

temppass="$(openssl rand -base64 12)"

# Extract certificate
openssl pkcs12 -clcerts -nokeys -in $PFXIN -out $CERTOUT -password pass:$pass -passin pass:$pass
if test $? -ne 0; then
	echo "Cannot extract certificate" >&2
	exit 1
fi

# Extract CA
openssl pkcs12 -cacerts -nokeys -in $PFXIN -out $CAOUT -password pass:$pass -passin pass:$pass
if test $? -ne 0; then
	echo "Cannot extract CA certificate" >&2
	exit 1
fi

# Extract private key
openssl pkcs12 -nocerts -in $PFXIN -out $PRVKEY -password pass:$pass -passin pass:$pass -passout pass:$temppass
if test $? -ne 0; then
	echo "Cannot extract the private key" >&2
	exit 1
fi

# Remove passphrase
openssl rsa -in $PRVKEY -out $NEWKEY -passin pass:$temppass
if test $? -ne 0; then
	echo "Cannot remove password from private key" >&2
	exit 1
fi

# Create pem
cat $NEWKEY $CERTOUT $CAOUT > $NEWPEM

# Export to pkcs12
openssl pkcs12 -export -nodes -CAfile $CAOUT -in $NEWPEM -out $OUTFILE -passout pass:
if test $? -ne 0; then
	echo "Cannot export to pkcs12" >&2
	exit 1
fi

exit 0
