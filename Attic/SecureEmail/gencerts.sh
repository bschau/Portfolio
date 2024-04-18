#!/bin/bash
certs="certs"
mkdir -p "$certs"
extfile="$(pwd)/openssl.cnf"
cd "$certs"
CASERIALFILE="${certs}/CA.srl"

function _generate
{
	cert="$1"
	shift
	subject="$1"
	shift
	rest="$*"
	echo "Generating $cert key"
	openssl genrsa -out ${cert}.key 4096

	echo "Generating $cert CSR"
	openssl req -new -key ${cert}.key -out ${cert}.csr -subj "$subject" -passout pass:abcd1234

	echo "Generating intermediate $cert certificate"
	openssl x509 -req -days 3650 -in ${cert}.csr -CA ca.crt -CAkey ca.key -CAcreateserial -CAserial $CASERIALFILE -out ${cert}.crt -addtrust emailProtection -addreject clientAuth -addreject serverAuth -trustout $rest

	echo "Creating $cert PFX archive"
	openssl pkcs12 -export -out ${cert}.pfx -inkey ${cert}.key -in ${cert}.crt -chain -CAfile ca.crt -passout pass:abcd1234

	echo "Creating $cert Thunderbird pem - import pfx, then pem"
	cat ca.crt ${cert}.crt > ${cert}-thunderbird.pem 
}

if test "$1" = "ca"; then
	echo "Generating CA key"
	openssl genrsa -out ca.key 4096 -config ../openssl.cnf # -extensions v3_ca

	echo "Generating CA certificate"
	openssl req -new -x509 -days 3650 -key ca.key -out ca.crt -extensions v3_ca -subj "/emailAddress=ca@schau.dk/C=DK/ST=Copenhagen/L=Copenhagen/O=CA/CN=ca.schau.dk" -passout pass:abcd1234
elif test "$1" = "sign"; then
	_generate sign '/emailAddress=noreply@schau.dk/C=DK/ST=Copenhagen/L=Copenhagen/O=Mail/CN=schau.dk' '-extensions user_crt' "-extfile $extfile"
elif test "$1" = "seal"; then
	_generate seal '/emailAddress=noreply@schau.dk/C=DK/ST=Copenhagen/L=Copenhagen/O=Mail/CN=schau.dk' '-extensions user_crt' "-extfile $extfile"
elif test "$1" = "encrypt"; then
	if test -z "$2"; then
		echo "Missing email address" >&2
		exit 1
	fi
	email="$2"
	cn="$(echo $email | cut -f 2 -d '@')"
	subject="/emailAddress=${email}/C=DK/ST=Copenhagen/L=Copenhagen/O=Mail/CN=${cn}"
	_generate encrypt "$subject" '-extensions user_crt' "-extfile $extfile"
else
	echo "Usage: $0 ca | sign | seal | encrypt email-address" >&2
	exit 1
fi

exit 0
