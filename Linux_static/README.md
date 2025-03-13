# spider (Linux static)

socks5 proxy tunnel tool

## Installation
### Install dependencies
- g++ (C++20)
- make
- cmake
- [c-ares](https://github.com/c-ares/c-ares) (ares_getaddrinfo function: The alternative to the glibc's getaddrinfo function.)

### Install
#### c-ares
1. download c-ares
```
wget https://github.com/c-ares/c-ares/releases/download/v1.34.4/c-ares-1.34.4.tar.gz
tar xvzf c-ares-1.34.4.tar.gz
```
2. modify ares_getaddrinfo.c and ares_getnameinfo.c ([static linking (glibc getservbyport_r getservbyname_r) #945](https://github.com/c-ares/c-ares/issues/945))
- c-ares-1.34.4/src/lib/ares_getaddrinfo.c (lookup_service function)
```
nano -l c-ares-1.34.4/src/lib/ares_getaddrinfo.c
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
- c-ares-1.34.4/src/lib/ares_getnameinfo.c (lookup_service function)
```
nano -l c-ares-1.34.4/src/lib/ares_getnameinfo.c
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
cd c-ares-1.34.4
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/cares -DCARES_STATIC=On -DCARES_STATIC_PIC=On ..
make
```

#### spider
1. download spider
```
git clone https://github.com/shuichiro-endo/spider.git
```
2. copy c-ares directories to spider directory
- c-ares-1.34.4/include
```
cp -rp c-ares-1.34.4/include spider/Linux_static/
```
- c-ares-1.34.4/build/lib
```
cp -rp c-ares-1.34.4/build/lib spider/Linux_static/
```
3. check Linux_static directory
```
> tree spider/Linux_static
spider/Linux_static
├── Makefile
├── README.md
├── aes.cpp
├── aes.hpp
├── caresmanager.cpp
├── caresmanager.hpp
├── client.cpp
├── client.hpp
├── clientmanager.cpp
├── clientmanager.hpp
├── config_sample.txt
├── encryption.cpp
├── encryption.hpp
├── include
│   ├── CMakeLists.txt
│   ├── Makefile.am
│   ├── Makefile.in
│   ├── ares.h
│   ├── ares_build.h
│   ├── ares_build.h.cmake
│   ├── ares_build.h.in
│   ├── ares_dns.h
│   ├── ares_dns_record.h
│   ├── ares_nameser.h
│   └── ares_version.h
├── lib
│   ├── libcares.a
│   ├── libcares.so -> libcares.so.2
│   ├── libcares.so.2 -> libcares.so.2.19.3
│   └── libcares.so.2.19.3
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
├── socks5.hpp
├── socks5message.cpp
├── socks5message.hpp
├── socks5messagequeue.cpp
├── socks5messagequeue.hpp
├── spider.cpp
├── spider.hpp
├── spidercommand.cpp
├── spidercommand.hpp
├── spiderip.cpp
├── spiderip.hpp
├── xor.cpp
└── xor.hpp

3 directories, 63 files
```
4. build
```
cd spider/Linux_static
make
```
> [!NOTE]
> If you want to display debug messages, please uncomment #define statement (//#define _DEBUG) in spider.hpp.
> 
> And then build it.
- spider.hpp (uncomment #define statement (//#define _DEBUG))
```
#define _DEBUG
```
5. check if it is a statically linked binary
```
> file spider
spider: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=03cbdb3be02d9fee73a2b80e6c56d760eade86a2, for GNU/Linux 4.4.0, not stripped

> ldd spider
not a dynamic executable
```
6. run strip command (optional)
```
> strip spider

> file spider
spider: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=03cbdb3be02d9fee73a2b80e6c56d760eade86a2, for GNU/Linux 4.4.0, stripped

> nm spider
nm: spider: no symbol
```

## Usage
The usage is the same as the Linux version.

