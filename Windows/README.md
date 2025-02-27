# spider (Windows)

socks5 proxy tunnel tool

## Installation
### Install dependencies
- visual studio community (Desktop development with C++)
  1. install Desktop development with C++

### Install
1. download files
```
git clone https://github.com/shuichiro-endo/spider.git
```

2. run x64 Native Tools Command Prompt for VS

3. build
```
cd spider/Windows
compile.bat
```
> [!NOTE]
> If you want to display debug messages, please uncomment #define statement (//#define DEBUGPRINT) in spider.hpp.
> 
> And then build it.
- spider.hpp (uncomment #define statement (//#define DEBUGPRINT))
```
#define DEBUGPRINT
```


## Usage
The usage is the same as the Linux version.

### help
> [!NOTE]
> In auto mode with the -r option, routing information is automatically exchanged between spiders.
> 
> In self mode, routing information is not exchanged between spiders, so please set the routing information manually.
>
> If the -r option is not set, it is in auto mode by default.

```
> spider.exe -h

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -:               Windows Ver: 0.12  Author: Shuichiro Endo


usage   : spider.exe [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
        : [-r routing_mode(auto:a self:s)]
        : [-e x(xor encryption)] [-k key(hexstring)]
        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
example : spider.exe -4 192.168.0.10
        : spider.exe -6 2001::xxxx:xxxx:xxxx:xxxx
        : spider.exe -u fd00::xxxx:xxxx:xxxx:xxxx
        : spider.exe -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : spider.exe -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : spider.exe -4 192.168.0.10 -r s
        : spider.exe -4 192.168.0.10 -e x -k deadbeef
        : spider.exe -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a

```

### run
> [!NOTE]
> The -4 and -6 option set the IP address of the network interface.
> 
> Some IP addresses may not work correctly. (e.g. 127.0.0.1, ::1, 0.0.0.0)

```
> spider.exe -4 192.168.0.31 -l fe80::ca60:dc71:546d:8235%14

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -:               Windows Ver: 0.12  Author: Shuichiro Endo


--------------------------------- spider ---------------------------------
 spider ipv4                     : 192.168.0.31
 spider ipv6 link local          : fe80::ca60:dc71:546d:8235
 spider ipv6 link local scope id : 14
 routing mode                    : auto
 xor encryption                  : off
 xor key hex string              :
 aes encryption                  : off
 aes key hex string              :
 aes iv hex string               :
----------------------------- spider command -----------------------------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client udp)
 0: exit
--------------------------------------------------------------------------

command >

```
