#!/bin/bash

echo -n "const char *pipe_server_https_privatekey = ";
cat pipe_server_https_privatekey.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END PRIVATE KEY-----\\n"\\/"-----END PRIVATE KEY-----\\n";/g'
echo "";

echo -n "const char *pipe_server_https_certificate = ";
cat pipe_server_https_certificate.pem | sed -e 's/^/"/g' -e 's/$/\\n"\\/g' -e 's/"-----END CERTIFICATE-----\\n"\\/"-----END CERTIFICATE-----\\n";/g'
echo "";
