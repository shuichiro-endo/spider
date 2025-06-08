# spider (Linux static)

socks5 proxy tunnel tool

## Installation
### Install dependencies
- g++ (C++20)
- make
- cmake
- [c-ares](https://github.com/c-ares/c-ares) (ares_getaddrinfo function: The alternative to the glibc's getaddrinfo function.)
- [openssl](https://openssl-library.org/)

### Install
#### c-ares
1. download c-ares
```
wget https://github.com/c-ares/c-ares/releases/download/v1.34.5/c-ares-1.34.5.tar.gz
tar xvzf c-ares-1.34.5.tar.gz
```
2. modify ares_getaddrinfo.c and ares_getnameinfo.c ([static linking (glibc getservbyport_r getservbyname_r) #945](https://github.com/c-ares/c-ares/issues/945))
- c-ares-1.34.5/src/lib/ares_getaddrinfo.c (lookup_service function)
```
nano -l c-ares-1.34.5/src/lib/ares_getaddrinfo.c
```
```
static unsigned short lookup_service(const char *service, int flags)
{
  if (service) {
    return (unsigned short)atoi(service);
  }
  return 0;
}
```
- c-ares-1.34.5/src/lib/ares_getnameinfo.c (lookup_service function)
```
nano -l c-ares-1.34.5/src/lib/ares_getnameinfo.c
```
```
static char *lookup_service(unsigned short port, unsigned int flags, char *buf,
                            size_t buflen)
{
  return NULL;
}
```
3. build c-ares
```
cd c-ares-1.34.5
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/cares -DCARES_STATIC=On -DCARES_STATIC_PIC=On ..
make
```

#### openssl
1. download openssl
```
wget https://github.com/openssl/openssl/releases/download/openssl-3.5.0/openssl-3.5.0.tar.gz
tar -xzf openssl-3.5.0.tar.gz
```
2. build
```
cd openssl-3.5.0
./config no-shared
make
```

#### spider
1. download spider
```
git clone https://github.com/shuichiro-endo/spider.git
```
2. copy c-ares directories to spider/Linux_static directory
- c-ares-1.34.5/include
```
cp -rp c-ares-1.34.5/include spider/Linux_static/
```
- c-ares-1.34.5/build/lib
```
cp -rp c-ares-1.34.5/build/lib spider/Linux_static/
```
3. copy openssl libssl.a file and libcrypto.a file to spider/Linux_static/lib directory
```
cp -p openssl-3.5.0/libssl.a spider/Linux_static/lib
cp -p openssl-3.5.0/libcrypto.a spider/Linux_static/lib
```
4. copy openssl include/openssl directory to spider/Linux_static/include directory
```
cp -rp openssl-3.5.0/include/openssl spider/Linux_static/include/
```

5. check Linux_static directory
```
> tree spider/Linux_static
spider/Linux_static
├── aes.cpp
├── aes.hpp
├── caresmanager.cpp
├── caresmanager.hpp
├── client.cpp
├── client.hpp
├── clientmanager.cpp
├── clientmanager.hpp
├── clienttls.hpp
├── config_sample.txt
├── encryption.cpp
├── encryption.hpp
├── include
│   ├── ares_build.h
│   ├── ares_build.h.cmake
│   ├── ares_build.h.in
│   ├── ares_dns.h
│   ├── ares_dns_record.h
│   ├── ares.h
│   ├── ares_nameser.h
│   ├── ares_version.h
│   ├── CMakeLists.txt
│   ├── Makefile.am
│   ├── Makefile.in
│   └── openssl
│       ├── aes.h
│       ├── asn1err.h
│       ├── asn1.h
│       ├── asn1.h.in
│       ├── asn1t.h
│       ├── asn1t.h.in
│       ├── asyncerr.h
│       ├── async.h
│       ├── bioerr.h
│       ├── bio.h
│       ├── bio.h.in
│       ├── blowfish.h
│       ├── bnerr.h
│       ├── bn.h
│       ├── buffererr.h
│       ├── buffer.h
│       ├── byteorder.h
│       ├── camellia.h
│       ├── cast.h
│       ├── cmac.h
│       ├── cmperr.h
│       ├── cmp.h
│       ├── cmp.h.in
│       ├── cmp_util.h
│       ├── cmserr.h
│       ├── cms.h
│       ├── cms.h.in
│       ├── comperr.h
│       ├── comp.h
│       ├── comp.h.in
│       ├── conf_api.h
│       ├── conferr.h
│       ├── conf.h
│       ├── conf.h.in
│       ├── configuration.h
│       ├── configuration.h.in
│       ├── conftypes.h
│       ├── core_dispatch.h
│       ├── core.h
│       ├── core_names.h
│       ├── core_names.h.in
│       ├── core_object.h
│       ├── crmferr.h
│       ├── crmf.h
│       ├── crmf.h.in
│       ├── cryptoerr.h
│       ├── cryptoerr_legacy.h
│       ├── crypto.h
│       ├── crypto.h.in
│       ├── cterr.h
│       ├── ct.h
│       ├── ct.h.in
│       ├── __DECC_INCLUDE_EPILOGUE.H
│       ├── __DECC_INCLUDE_PROLOGUE.H
│       ├── decodererr.h
│       ├── decoder.h
│       ├── des.h
│       ├── dherr.h
│       ├── dh.h
│       ├── dsaerr.h
│       ├── dsa.h
│       ├── dtls1.h
│       ├── ebcdic.h
│       ├── ecdh.h
│       ├── ecdsa.h
│       ├── ecerr.h
│       ├── ec.h
│       ├── encodererr.h
│       ├── encoder.h
│       ├── engineerr.h
│       ├── engine.h
│       ├── e_os2.h
│       ├── e_ostime.h
│       ├── err.h
│       ├── err.h.in
│       ├── esserr.h
│       ├── ess.h
│       ├── ess.h.in
│       ├── evperr.h
│       ├── evp.h
│       ├── fipskey.h
│       ├── fipskey.h.in
│       ├── fips_names.h
│       ├── hmac.h
│       ├── hpke.h
│       ├── httperr.h
│       ├── http.h
│       ├── idea.h
│       ├── indicator.h
│       ├── kdferr.h
│       ├── kdf.h
│       ├── lhash.h
│       ├── lhash.h.in
│       ├── macros.h
│       ├── md2.h
│       ├── md4.h
│       ├── md5.h
│       ├── mdc2.h
│       ├── ml_kem.h
│       ├── modes.h
│       ├── objectserr.h
│       ├── objects.h
│       ├── obj_mac.h
│       ├── ocsperr.h
│       ├── ocsp.h
│       ├── ocsp.h.in
│       ├── opensslconf.h
│       ├── opensslv.h
│       ├── opensslv.h.in
│       ├── ossl_typ.h
│       ├── param_build.h
│       ├── params.h
│       ├── pem2.h
│       ├── pemerr.h
│       ├── pem.h
│       ├── pkcs12err.h
│       ├── pkcs12.h
│       ├── pkcs12.h.in
│       ├── pkcs7err.h
│       ├── pkcs7.h
│       ├── pkcs7.h.in
│       ├── proverr.h
│       ├── provider.h
│       ├── prov_ssl.h
│       ├── quic.h
│       ├── randerr.h
│       ├── rand.h
│       ├── rc2.h
│       ├── rc4.h
│       ├── rc5.h
│       ├── ripemd.h
│       ├── rsaerr.h
│       ├── rsa.h
│       ├── safestack.h
│       ├── safestack.h.in
│       ├── seed.h
│       ├── self_test.h
│       ├── sha.h
│       ├── srp.h
│       ├── srp.h.in
│       ├── srtp.h
│       ├── ssl2.h
│       ├── ssl3.h
│       ├── sslerr.h
│       ├── sslerr_legacy.h
│       ├── ssl.h
│       ├── ssl.h.in
│       ├── stack.h
│       ├── storeerr.h
│       ├── store.h
│       ├── symhacks.h
│       ├── thread.h
│       ├── tls1.h
│       ├── trace.h
│       ├── tserr.h
│       ├── ts.h
│       ├── txt_db.h
│       ├── types.h
│       ├── uierr.h
│       ├── ui.h
│       ├── ui.h.in
│       ├── whrlpool.h
│       ├── x509_acert.h
│       ├── x509_acert.h.in
│       ├── x509err.h
│       ├── x509.h
│       ├── x509.h.in
│       ├── x509v3err.h
│       ├── x509v3.h
│       ├── x509v3.h.in
│       ├── x509_vfy.h
│       └── x509_vfy.h.in
├── lib
│   ├── libcares.a
│   ├── libcares.so -> libcares.so.2
│   ├── libcares.so.2 -> libcares.so.2.19.4
│   ├── libcares.so.2.19.4
│   ├── libcrypto.a
│   └── libssl.a
├── Makefile
├── message.cpp
├── message.hpp
├── messagemanager.cpp
├── messagemanager.hpp
├── node.cpp
├── node.hpp
├── pipe.cpp
├── pipe.hpp
├── pipemanager.cpp
├── pipemanager.hpp
├── pipe_server_https_certificate.pem
├── README.md
├── route.cpp
├── route.hpp
├── routingmanager.cpp
├── routingmanager.hpp
├── routingmessage.cpp
├── routingmessage.hpp
├── routingmessagequeue.cpp
├── routingmessagequeue.hpp
├── server.cpp
├── server.hpp
├── servermanager.cpp
├── servermanager.hpp
├── servertls.hpp
├── socks5.hpp
├── socks5message.cpp
├── socks5message.hpp
├── socks5messagequeue.cpp
├── socks5messagequeue.hpp
├── spidercommand.cpp
├── spidercommand.hpp
├── spider.cpp
├── spider.hpp
├── spiderip.cpp
├── spiderip.hpp
├── xor.cpp
└── xor.hpp

4 directories, 240 files
```
6. build
```
cd spider/Linux_static
make
```
> [!NOTE]
> Please ignore the warning.
> 
> This tool does not use those functions (dlfcn_globallookup, BIO_lookup_ex, BIO_gethostbyname).
```
rm -f spiderip.o encryption.o xor.o aes.o caresmanager.o route.o message.o routingmessagequeue.o socks5messagequeue.o socks5message.o routingmessage.o routingmanager.o node.o pipe.o pipemanager.o client.o clientmanager.o server.o servermanager.o messagemanager.o spidercommand.o spider.o spider
g++ -std=c++20  -I./include -I./include/openssl -I. -c spiderip.cpp encryption.cpp xor.cpp aes.cpp caresmanager.cpp route.cpp message.cpp routingmessagequeue.cpp socks5messagequeue.cpp socks5message.cpp routingmessage.cpp routingmanager.cpp node.cpp pipe.cpp pipemanager.cpp client.cpp clientmanager.cpp server.cpp servermanager.cpp messagemanager.cpp spidercommand.cpp spider.cpp
g++ -std=c++20 -static -o spider spiderip.o encryption.o xor.o aes.o caresmanager.o route.o message.o routingmessagequeue.o socks5messagequeue.o socks5message.o routingmessage.o routingmanager.o node.o pipe.o pipemanager.o client.o clientmanager.o server.o servermanager.o messagemanager.o spidercommand.o spider.o -L./lib  -pthread -lcares -lssl -lcrypto
/usr/bin/ld: ./lib/libcrypto.a(libcrypto-lib-dso_dlfcn.o): in function `dlfcn_globallookup':
dso_dlfcn.c:(.text+0x11): warning: Using 'dlopen' in statically linked applications requires at runtime the shared libraries from the glibc version used for linking
/usr/bin/ld: ./lib/libcrypto.a(libcrypto-lib-bio_addr.o): in function `BIO_lookup_ex':
bio_addr.c:(.text+0xb47): warning: Using 'getaddrinfo' in statically linked applications requires at runtime the shared libraries from the glibc version used for linking
/usr/bin/ld: ./lib/libcrypto.a(libcrypto-lib-bio_sock.o): in function `BIO_gethostbyname':
bio_sock.c:(.text+0x51): warning: Using 'gethostbyname' in statically linked applications requires at runtime the shared libraries from the glibc version used for linking
```
> [!NOTE]
> If you want to display debug messages, please uncomment #define statement (//#define _DEBUG) in spider.hpp.
> 
> And then build it.
- spider.hpp (uncomment #define statement (//#define _DEBUG))
```
#define _DEBUG
```
7. check if it is a statically linked binary
```
> file spider
spider: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=03cbdb3be02d9fee73a2b80e6c56d760eade86a2, for GNU/Linux 4.4.0, not stripped

> ldd spider
not a dynamic executable
```
8. run strip command (optional)
```
> strip spider

> file spider
spider: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=03cbdb3be02d9fee73a2b80e6c56d760eade86a2, for GNU/Linux 4.4.0, stripped

> nm spider
nm: spider: no symbol
```

## Usage
The usage is the same as the Linux version.

