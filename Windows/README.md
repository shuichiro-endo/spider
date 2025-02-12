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

## Usage
The usage is the same as the Linux version.

- help
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
        =-    -:                Windows Ver: 0.2  Author: Shuichiro Endo


usage        : spider.exe -i spider_ip
             : [-r routing_mode(auto:a self:s)]
             : [-x (xor encryption)] [-k key(hexstring)]
example      : spider.exe -i 192.168.0.10
             : spider.exe -i 192.168.0.10 -x -k deadbeef
             : spider.exe -i 192.168.0.10 -r s
             : spider.exe -i fe80::xxxx:xxxx:xxxx:xxxx%14

```

- run
> [!NOTE]
> The -i option sets the IP address of the network interface.
> 
> Some IP addresses may not work correctly. (e.g. 127.0.0.1, ::1, 0.0.0.0)

```
> spider.exe -i 192.168.0.31

                 -.                                 _//
           .=  :*=--::                       _/     _//
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///
        :*-  :*-   .:.              _//
        =-    -:                Windows Ver: 0.2  Author: Shuichiro Endo


----------     spider     ----------
 spider ip          : 192.168.0.31
 routing mode       : auto
 xor encryption     : off
 xor key hex string :
---------- spider command ----------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 0: exit
------------------------------------

command >

```
