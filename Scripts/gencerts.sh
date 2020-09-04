#!/bin/sh
cd $HOME
echo "Cleaning up"
rm -fr certs
mkdir certs
cd certs

echo "Generating CA key"
openssl genrsa -out ca.key 4096 -extensions v3_ca

echo "Generating CA certificate"
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt -extensions v3_ca -subj "/emailAddress=info@esignatur.dk/C=DK/ST=Copenhagen/L=Copenhagen/O=CA/CN=ca.esignatur.dk" -passout pass:abcd1234

echo "Generating intermediate key"
openssl genrsa -out ia.key 4096

echo "Generating CSR"
openssl req -new -key ia.key -out ia.csr -subj "/emailAddress=noreply@mail.esignatur.dk/C=DK/ST=Copenhagen/L=Copenhagen/O=Mail/CN=mail.esignatur.dk" -passout pass:abcd1234

echo "Generating intermediate certificate"
openssl x509 -req -days 3650 -in ia.csr -CA ca.crt -CAkey ca.key -set_serial 1 -out ia.crt -addtrust emailProtection -addreject clientAuth -addreject serverAuth -trustout

echo "Creating PFX archive"
openssl pkcs12 -export -out noreply.pfx -inkey ia.key -in ia.crt -chain -CAfile ca.crt -passout pass:abcd1234

echo "Doing Thunderbird pem - import pfx, then pem"
cat ca.crt ia.crt > thunderbird.pem 
exit 0
