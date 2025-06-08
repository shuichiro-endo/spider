#!/bin/bash

rm pipe_server_https_privatekey.pem
rm pipe_server_https_certificate.pem

openssl pkcs12 -in pipe_server_https_privatekey_and_certificate.pfx -nocerts -nodes -out pipe_server_https_privatekey.pem_bk -passin pass:spider
sed -n '/-----BEGIN PRIVATE KEY-----/,$p' pipe_server_https_privatekey.pem_bk > pipe_server_https_privatekey.pem
rm pipe_server_https_privatekey.pem_bk

echo -n "const char *pipe_server_https_privatekey = ";
cat pipe_server_https_privatekey.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END PRIVATE KEY-----\\n"\\/"-----END PRIVATE KEY-----\\n";/g'
echo "";


openssl pkcs12 -in pipe_server_https_privatekey_and_certificate.pfx -clcerts -nokeys -out pipe_server_https_certificate.pem_bk -passin pass:spider
sed -n '/-----BEGIN CERTIFICATE-----/,$p' pipe_server_https_certificate.pem_bk > pipe_server_https_certificate.pem
rm pipe_server_https_certificate.pem_bk

echo -n "const char *pipe_server_https_certificate = ";
cat pipe_server_https_certificate.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END CERTIFICATE-----\\n"\\/"-----END CERTIFICATE-----\\n";/g'
echo "";
