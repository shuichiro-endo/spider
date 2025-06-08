#!/bin/bash

rm pipe_server_https_privatekey.pem
rm pipe_server_https_certificate.pem

openssl req -x509 -days 3650 -nodes -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 -subj /CN=spider -outform PEM -keyout pipe_server_https_privatekey.pem -out pipe_server_https_certificate.pem
echo "";

openssl x509 -text -noout -in pipe_server_https_certificate.pem
echo "";

echo -n "const char *pipe_server_https_privatekey = ";
cat pipe_server_https_privatekey.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END PRIVATE KEY-----\\n"\\/"-----END PRIVATE KEY-----\\n";/g'
echo "";

echo -n "const char *pipe_server_https_certificate = ";
cat pipe_server_https_certificate.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END CERTIFICATE-----\\n"\\/"-----END CERTIFICATE-----\\n";/g'
echo "";

