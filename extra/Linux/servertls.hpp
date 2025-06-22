/*
 * Title:  spider servertls.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef SERVERTLS_H_
#define SERVERTLS_H_

namespace spider
{
    const char *pipe_server_https_privatekey = "-----BEGIN PRIVATE KEY-----\n"\
    "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgv8Ec+3gGntf8DJeW\n"\
    "yk3r8MV01TVH08DdUfSEfWeCmS+hRANCAAQZIfYR9kLpiMUlOJLUeqpy6YudRC5J\n"\
    "ZEPeTqts1HPHyjUrs3RXvYTm2n7suBM7GNdknDnnZ5ZiL8BzjwjtFBln\n"\
    "-----END PRIVATE KEY-----\n";

    const char *pipe_server_https_certificate = "-----BEGIN CERTIFICATE-----\n"\
    "MIIBdzCCAR2gAwIBAgIUF+nYzTU8BH9wBNxuz62Q9mK19pEwCgYIKoZIzj0EAwIw\n"\
    "ETEPMA0GA1UEAwwGc3BpZGVyMB4XDTI1MDYwNjIwMTMzM1oXDTM1MDYwNDIwMTMz\n"\
    "M1owETEPMA0GA1UEAwwGc3BpZGVyMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE\n"\
    "GSH2EfZC6YjFJTiS1HqqcumLnUQuSWRD3k6rbNRzx8o1K7N0V72E5tp+7LgTOxjX\n"\
    "ZJw552eWYi/Ac48I7RQZZ6NTMFEwHQYDVR0OBBYEFObZ/+jm7VextuAPS7enG+es\n"\
    "XDNCMB8GA1UdIwQYMBaAFObZ/+jm7VextuAPS7enG+esXDNCMA8GA1UdEwEB/wQF\n"\
    "MAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIhAN1D+PyibCQMNEhrWNUgdHW7VHj9ypbp\n"\
    "C6RZ9KdEkyFPAiB1ObgBDfkWRT9sTjayRSdOoKupY/kMfH3FJBobIqX8mw==\n"\
    "-----END CERTIFICATE-----\n";

    const char *cipher_suite_tls_1_2 = "AESGCM+ECDSA:CHACHA20+ECDSA:+AES256";   // TLS1.2
    const char *cipher_suite_tls_1_3 = "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256";    // TLS1.3
}

#endif /* SERVERTLS_H_ */

