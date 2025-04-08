# spider (Windows csharp powershell)

socks5 proxy tunnel tool

## Installation
### Install
1. download files
```
git clone https://github.com/shuichiro-endo/spider.git
```

## Usage
1. run Windows PowerShell

2. run spider.ps1
```
cd .\spider\Windows_csharp_powershell\
.\spider.ps1
```
> [!NOTE]
> It is also possible to download and run it directly.
```
Invoke-Expression ((New-Object Net.WebClient).DownloadString("https://raw.githubusercontent.com/shuichiro-endo/spider/refs/heads/main/Windows_csharp_powershell/spider.ps1"))
```

The usage is the same as the Linux version.

### help
```
> .\spider.ps1 -h

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -: Windows c# powershell Ver: 1.24  Author: Shuichiro Endo


usage   : .\spider.ps1
        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
        : [-f config_file]
        : [-d (hide)] [-i pipe_destination_ip] [-p pipe_destination_port]
        : [-r routing_mode(auto:a self:s)]
        : [-e x(xor encryption)] [-k key(hexstring)]
        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
example : .\spider.ps1
        : .\spider.ps1 -4 192.168.0.10
        : .\spider.ps1 -6 2001::xxxx:xxxx:xxxx:xxxx
        : .\spider.ps1 -u fd00::xxxx:xxxx:xxxx:xxxx
        : .\spider.ps1 -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : .\spider.ps1 -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%14
        : .\spider.ps1 -f C:\Users\test\Desktop\spider\Windows_csharp_powershell\config_sample.txt
        : .\spider.ps1 -d -i 192.168.0.25 -p 1025
        : .\spider.ps1 -4 192.168.0.10 -r s
        : .\spider.ps1 -4 192.168.0.10 -e x -k deadbeef
        : .\spider.ps1 -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a
        
```
#### [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
The -4, -6, -u, and -l option set the ip address of the network interface.

> [!NOTE]
> Some ip addresses may not work correctly. (e.g. 127.0.0.1, ::1, 0.0.0.0)

#### [-f config_file]
Create nodes configured in the config file at startup.

> [!NOTE]
> [config_sample.txt](https://github.com/shuichiro-endo/spider/blob/main/Windows_csharp_powershell/config_sample.txt) is a sample.

#### [-d (hide)]
Hide the console window.

> [!CAUTION]
> Be careful, as it will become invisible.
>
> You need to use with pipe client node startup options (-i and -p option).
> 
> You need to operate from other spider.

#### [-i pipe_destination_ip] [-p pipe_destination_port]
Create pipe client node at startup.

Set the ip address and port number of the destination pipe server node.

> [!IMPORTANT]
> In advance, you need to create the pipe server node on the other spider.

#### [-r routing_mode(auto:a self:s)]
In auto mode with the -r option, routing information is automatically exchanged between spiders.

In self mode, routing information is not exchanged between spiders, so please set the routing information manually.

If the -r option is not set, it is in auto mode by default.

#### [-e x(xor encryption)] [-k key(hexstring)]
Encrypt SOCKS5 packets between spider client and spider server using xor.

#### [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
Encrypt SOCKS5 packets between spider client and spider server using aes-256-cbc.

[This program (tools/generate_aes_key_hex_string.py)](https://github.com/shuichiro-endo/spider/blob/main/tools/generate_aes_key_hex_string.py) can be used for key generation.

### run
```
> .\spider.ps1

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -: Windows c# powershell Ver: 1.24  Author: Shuichiro Endo


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
