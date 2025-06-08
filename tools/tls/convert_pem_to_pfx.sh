#!/bin/bash

rm pipe_server_https_privatekey_and_certificate.pfx

openssl pkcs12 -export -out pipe_server_https_privatekey_and_certificate.pfx -inkey pipe_server_https_privatekey.pem -in pipe_server_https_certificate.pem -passout pass:spider
