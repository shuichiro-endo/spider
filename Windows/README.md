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

### Import Default TLS Privatekey and Self Certificate 
1. copy spider/tools/tls directory to windows computer
- spider/tools/tls/pipe_server_https_certificate.pem
- spider/tools/tls/pipe_server_https_privatekey_and_certificate.pfx
- spider/tools/tls/remove_key_and_cert_from_cert_store.ps1
- spider/tools/tls/import_key_pfx_and_cert_pem.ps1

2. run powershell

3. import the default tls privatekey and self certificate
```
.\import_key_pfx_and_cert_pem.ps1
```

### Change TLS Privatekey and Self Certificate (optional)
1. copy spider/tools/tls directory to windows computer
- spider/tools/tls/remove_key_and_cert_from_cert_store.ps1
- spider/tools/tls/generate_and_import_key_pfx_and_cert_cer.ps1

2. run powershell

3. remove the old tls privatekey and self certificate (if the old privatekey and certificate are imported in the certificate store (Cert:\CurrentUser\Root, Cert:\CurrentUser\My))
```
.\remove_key_and_cert_from_cert_store.ps1
```

4. generate and import a new tls privatekey and self certificate
```
.\generate_and_import_key_pfx_and_cert_cer.ps1
```

5. change the old tls privatekey and self certificate of other spider (Linux, Linux_static, Windows, Windows_csharp, Windows_csharp_powershell)

## Usage
The usage is the same as the Linux version.

### help
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
        =-    -:               Windows Ver: 2.6   Author: Shuichiro Endo


usage   : spider.exe
        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
        : [-f config_file]
        : [-d (hide)] [-i pipe_destination_ip] [-p pipe_destination_port] [-m message_mode(default:d http:h https:s)] [-t sleep_ms(0-1000 ms)]
        : [-r routing_mode(auto:a self:s)]
        : [-e x(xor encryption)] [-k key(hexstring)]
        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
        : [-s (prevent spider server startup)]
example : spider.exe
        : spider.exe -4 192.168.0.10
        : spider.exe -6 2001::xxxx:xxxx:xxxx:xxxx
        : spider.exe -u fd00::xxxx:xxxx:xxxx:xxxx
        : spider.exe -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : spider.exe -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : spider.exe -f config_sample.txt
        : spider.exe -d -i 192.168.0.25 -p 1025 -m d
        : spider.exe -4 192.168.0.10 -r s
        : spider.exe -4 192.168.0.10 -e x -k deadbeef
        : spider.exe -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a
        : spider.exe -s

```
#### [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
The -4, -6, -u, and -l option set the ip address of the network interface.

> [!NOTE]
> Some ip addresses may not work correctly. (e.g. 127.0.0.1, ::1, 0.0.0.0)

> [!IMPORTANT]
> If the spiders with the same IP address exist, proper routing will not be performed.
> 
> Please ensure that there are no spiders with the same IP address within the spiders.

#### [-f config_file]
Create nodes configured in the config file at startup.

> [!NOTE]
> [config_sample.txt](https://github.com/shuichiro-endo/spider/blob/main/Windows/config_sample.txt) is a sample.

#### [-d (hide)]
Hide the console window.

> [!CAUTION]
> Be careful, as it will become invisible.
>
> You need to use with pipe client node startup options (-i and -p option).
>
> The "self" of routing mode cannot be used.
> 
> You need to operate from other spider.

#### [-i pipe_destination_ip] [-p pipe_destination_port] [-m message_mode(default:d http:h https:s)] [-t sleep_ms(0-1000 ms)]
Create pipe client node at startup.

Set the ip address and port number of the destination pipe server node.

> [!IMPORTANT]
> In advance, you need to create the pipe server node on the other spider.
>
> The pipe client's message mode must be the same as the message mode of the pipe server to which it will connect.

#### [-r routing_mode(auto:a self:s)]
In auto mode with the -r option, routing information is automatically exchanged between spiders.

In self mode, routing information is not exchanged between spiders, so please set the routing information manually.

If the -r option is not set, it is in auto mode by default.

#### [-e x(xor encryption)] [-k key(hexstring)]
Encrypt SOCKS5 packets between spider client and spider server using xor.

#### [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
Encrypt SOCKS5 packets between spider client and spider server using aes-256-cbc.

[This program (tools/generate_aes_key_hex_string.py)](https://github.com/shuichiro-endo/spider/blob/main/tools/generate_aes_key_hex_string.py) can be used for key generation.

#### [-s (prevent spider server startup)]
Prevent the spider server from starting up.

The spider with this option set prevents the spider server from being generated.

> [!CAUTION]
> Please set this option for the spider that do not start up the spider server.
>
> If this option is not set, there is a risk of being attacked through other spiders.

### run
```
> spider.exe

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -:               Windows Ver: 2.6   Author: Shuichiro Endo


--------------------------------- spider ---------------------------------
 spider ipv4                     : 192.168.0.31
 spider ipv6 link local          : fe80::ca60:dc71:546d:8235
 spider ipv6 link local scope id : 14
 config file                     :
 routing mode                    : auto
 xor encryption                  : off
 xor key hex string              :
 aes encryption                  : off
 aes key hex string              :
 aes iv hex string               :
 prevent spider server startup   : off
----------------------------- spider command -----------------------------
 1: add node (spider pipe)
 2: add node (spider client)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client tcp)
 7: add node (spider client udp)
 8: add node (spider client shell)
 9: close client listener (tcp)
 0: exit
--------------------------------------------------------------------------

command >

```
