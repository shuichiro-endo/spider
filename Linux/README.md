# spider (Linux)

socks5 proxy tunnel tool

## Installation
### Install dependencies
- g++ (C++20)
- make

### Install
1. download files
```
git clone https://github.com/shuichiro-endo/spider.git
```

2. build
```
cd spider/Linux
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

## Usage
### help
```
> ./spider -h

                 -.                                 _//                   
           .=  :*=--::                       _/     _//                   
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     
        :*-  :*-   .:.              _//                                   
        =-    -:                 Linux Ver: 2.2   Author: Shuichiro Endo  


usage   : ./spider
        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
        : [-f config_file]
        : [-d (daemon)] [-i pipe_destination_ip] [-p pipe_destination_port]
        : [-r routing_mode(auto:a self:s)]
        : [-e x(xor encryption)] [-k key(hexstring)]
        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
        : [-s (prevent spider server startup)]
example : ./spider
        : ./spider -4 192.168.0.10
        : ./spider -6 2001::xxxx:xxxx:xxxx:xxxx
        : ./spider -u fd00::xxxx:xxxx:xxxx:xxxx
        : ./spider -l fe80::xxxx:xxxx:xxxx:xxxx%eth0
        : ./spider -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%eth0
        : ./spider -f config_sample.txt
        : ./spider -d -i 192.168.0.25 -p 1025
        : ./spider -4 192.168.0.10 -r s
        : ./spider -4 192.168.0.10 -e x -k deadbeef
        : ./spider -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a
        : ./spider -s

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
> [config_sample.txt](https://github.com/shuichiro-endo/spider/blob/main/Linux/config_sample.txt) is a sample.

#### [-d (daemon)]
Start as a daemon.

The spider process will stop when it receives the SIGUSR1 signal.

The following commands can be used to stop.
```
ps aux | grep "spider" | grep -v grep | awk '{print $2}' | xargs kill -SIGUSR1
```

> [!CAUTION]
> Be careful, as it will become invisible.
>
> You need to use with pipe client node startup options (-i and -p option).
> 
> The "self" of routing mode cannot be used.
>
> You need to operate from other spider.

#### [-i pipe_destination_ip] [-p pipe_destination_port]
Create pipe client node at startup.

Set the ip address and port number of the destination pipe server node.

> [!IMPORTANT]
> In advance, you need to create the pipe server node on the other spider.

#### [-r routing_mode(auto:a self:s)]
In auto mode with the -r option, routing information is automatically exchanged between spiders.

In self mode, routing information is not exchanged between spiders, so please set the routing information manually. (See [5: edit routing table](https://github.com/shuichiro-endo/spider/tree/main/Linux#5-edit-routing-table))

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
> ./spider

                 -.                                 _//                   
           .=  :*=--::                       _/     _//                   
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     
        :*-  :*-   .:.              _//                                   
        =-    -:                 Linux Ver: 2.2   Author: Shuichiro Endo  


--------------------------------- spider ---------------------------------
 spider ipv4                     : 192.168.0.25
 spider ipv6 unique local        : fd00:abcd:1234:5678::1
 spider ipv6 link local          : fe80::a00:27ff:febe:3a77
 spider ipv6 link local scope id : enp0s3 (2)
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

### 1: add node (spider pipe)
1. pipe (client), self
- ipv4
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:11:14 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:11:14 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:11:14 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > c
pipe ip                                        > 192.168.0.25
pipe destination ip                            > 192.168.0.26
pipe destination port                          > 1026

pipe mode                 : c
pipe ip                   : 192.168.0.25
pipe destination ip       : 192.168.0.26
pipe destination port     : 1026

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:12:27 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:12:27 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:12:27 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > c
pipe ip                                        > fe80::a00:27ff:febe:3a77
pipe destination ip                            > fe80::a00:27ff:fe25:c316
pipe destination port                          > 1026

pipe mode                 : c
pipe ip                   : fe80::a00:27ff:febe:3a77
pipe ip scope id          : enp0s3 (2)
pipe destination ip       : fe80::a00:27ff:fe25:c316
pipe destination port     : 1026

ok? (yes:y no:n quit:q)                        > y

```
2. pipe (server), self
> [!NOTE]
> The pipe server can be connected by only one pipe client at a time.
- ipv4
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:18:56 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:18:56 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:18:56 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > s
pipe listen ip                                 > 192.168.0.25
pipe listen port                               > 1025

pipe mode                 : s
pipe listen ip            : 192.168.0.25
pipe listen port          : 1025

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:19:39 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:19:39 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:19:39 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > s
pipe listen ip                                 > fe80::a00:27ff:febe:3a77
pipe listen port                               > 1025

pipe mode                 : s
pipe listen ip            : fe80::a00:27ff:febe:3a77
pipe listen ip scope id   : enp0s3 (2)
pipe listen port          : 1025

ok? (yes:y no:n quit:q)                        > y

```
3. pipe (client), other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |192.168.0.26                                  |     1|1948536240|Thu Apr  3 09:23:01 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1948536240|Thu Apr  3 09:23:01 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1948536240|Thu Apr  3 09:23:01 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:20:28 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
pipe mode (client:c server:s)                  > c
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
pipe ip                                        > 192.168.0.26
pipe destination ip                            > 192.168.0.27
pipe destination port                          > 1027

pipe mode                 : c
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.26
pipe ip                   : 192.168.0.26
pipe destination ip       : 192.168.0.27
pipe destination port     : 1027

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |192.168.0.26                                  |     1|1948536240|Thu Apr  3 09:24:56 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1948536240|Thu Apr  3 09:24:56 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1948536240|Thu Apr  3 09:24:56 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:20:28 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
pipe mode (client:c server:s)                  > c
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
pipe ip                                        > fe80::a00:27ff:fe25:c316
pipe destination ip                            > fe80::a00:27ff:fe63:4c1f
pipe destination port                          > 1027

pipe mode                 : c
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316
pipe ip                   : fe80::a00:27ff:fe25:c316
pipe destination ip       : fe80::a00:27ff:fe63:4c1f
pipe destination port     : 1027

ok? (yes:y no:n quit:q)                        > y

```
4. pipe (server), other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

> [!NOTE]
> The pipe server can be connected by only one pipe client at a time.
- ipv4
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |192.168.0.26                                  |     1|1948536240|Thu Apr  3 09:28:45 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1948536240|Thu Apr  3 09:28:45 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1948536240|Thu Apr  3 09:28:45 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:20:28 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
pipe mode (client:c server:s)                  > s
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
pipe listen ip                                 > 192.168.0.26
pipe listen port                               > 1026

pipe mode                 : s
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.26
pipe listen ip            : 192.168.0.26
pipe listen port          : 1026

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |192.168.0.26                                  |     1|1948536240|Thu Apr  3 09:30:54 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:20:28 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1948536240|Thu Apr  3 09:30:54 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1948536240|Thu Apr  3 09:30:54 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:20:28 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
pipe mode (client:c server:s)                  > s
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
pipe listen ip                                 > fe80::a00:27ff:fe25:c316
pipe listen port                               > 1026

pipe mode                 : s
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316
pipe listen ip            : fe80::a00:27ff:fe25:c316
pipe listen port          : 1026

ok? (yes:y no:n quit:q)                        > y

```
#### mode (self:s other:o)
Set the type of mode.

If you want to add pipe node to the spider itself, set 's'.

If you want to add pipe node to other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### mode (client:c server:s)
Set the type of the pipe node that you want to create.

If you want to create the pipe client node, set 'c'.

If you want to create the pipe server node, set 's'.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to add the pipe node.

#### pipe listen ip (pipe server)
Set the ip address of the interface that the pipe server node will listen on.

#### pipe listen port (pipe server)
Set the port number that the pipe server node will listen on.

#### pipe ip (pipe client)
Set the ip address of the interface that the pipe client node.

#### pipe destination ip (pipe client)
Set the ip address of the destination pipe server node.

#### pipe destination port (pipe client)
Set the port number of the destination pipe server node.

### 2: add node (spider client)
> [!IMPORTANT]
> This is SOCKS5 connection.
>
> [tcp client] <-TCP-> [socks5 client] <-SOCKS5(TCP)-> [spider client] <-SOCKS5(TCP)-> [spider server] <-TCP-> [destination server]
1. client, self
- ipv4
```
command > 2
[+] add node (spider client)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 08:54:52 2025|
|a   |192.168.0.26                                  |     1| 288260828|Thu Apr  3 08:56:12 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 08:54:52 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 288260828|Thu Apr  3 08:56:12 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 288260828|Thu Apr  3 08:56:12 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 08:54:52 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > 192.168.0.25
client listen port                             > 9050
client destination spider ip                   > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : 192.168.0.25
client listen port           : 9050
client destination spider ip : 192.168.0.26
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 2
[+] add node (spider client)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 08:54:52 2025|
|a   |192.168.0.26                                  |     1| 288260828|Thu Apr  3 08:58:32 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 08:54:52 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 288260828|Thu Apr  3 08:58:32 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 288260828|Thu Apr  3 08:58:32 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 08:54:52 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 9050
client destination spider ip                   > fe80::a00:27ff:fe25:c316
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : fe80::a00:27ff:febe:3a77
client listen ip scope id    : enp0s3 (2)
client listen port           : 9050
client destination spider ip : fe80::a00:27ff:fe25:c316
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
2. client, other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 2
[+] add node (spider client)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:37:13 2025|
|a   |192.168.0.26                                  |     1|1199263702|Thu Apr  3 09:38:06 2025|
|a   |192.168.0.27                                  |     2|1199263702|Thu Apr  3 09:38:06 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:37:13 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1199263702|Thu Apr  3 09:38:06 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1199263702|Thu Apr  3 09:38:06 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1199263702|Thu Apr  3 09:38:06 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:37:13 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
client listen ip                               > 192.168.0.26
client listen port                             > 9050
client destination spider ip                   > 192.168.0.27
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : 192.168.0.25
destination spider ip        : 192.168.0.26
client listen ip             : 192.168.0.26
client listen port           : 9050
client destination spider ip : 192.168.0.27
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 2
[+] add node (spider client)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 09:37:13 2025|
|a   |192.168.0.26                                  |     1|1199263702|Thu Apr  3 09:43:20 2025|
|a   |192.168.0.27                                  |     2|1199263702|Thu Apr  3 09:43:20 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 09:37:13 2025|
|a   |fd00:abcd:1234:5678::2                        |     1|1199263702|Thu Apr  3 09:43:20 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1199263702|Thu Apr  3 09:43:20 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1199263702|Thu Apr  3 09:43:20 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 09:37:13 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
client listen ip                               > fe80::a00:27ff:fe25:c316
client listen port                             > 9050
client destination spider ip                   > fe80::a00:27ff:fe63:4c1f
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : fe80::a00:27ff:febe:3a77
source spider ip scope id    : enp0s3 (2)
destination spider ip        : fe80::a00:27ff:fe25:c316
client listen ip             : fe80::a00:27ff:fe25:c316
client listen port           : 9050
client destination spider ip : fe80::a00:27ff:fe63:4c1f
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
#### mode (self:s other:o)
Set the type of mode.

If you want to add client node to the spider itself, set 's'.

If you want to add client node to other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to add the client node.

#### client listen ip
Set the ip address of the interface that the client node will listen on.

#### client listen port
Set the port number that the client node will listen on.

#### client destination spider ip
Set the ip address of the destination spider.

> [!IMPORTANT]
> The startup location of the spider server node is determined by the 'client destination spider ip' when adding the spider client node.

#### recv/send tv_sec  (timeout 0-60 sec), recv/send tv_usec (timeout 0-1000000 microsec)
Set the timeout for SOCKS5 selection request/response, SOCKS5 username/password authentication request/response, and SOCKS5 request/response.

#### forwarder tv_sec  (timeout 0-3600 sec), forwarder tv_usec (timeout 0-1000000 microsec)
Set the timeout for packet forwarding between the SOCKS5 client and the destination server.

> [!NOTE]
> If you access web sites, set the timeout value shorter. (5s - 30s)
>
> If you access ssh server, set the forwarder timeout value longer. (300s - 3600s)

> [!IMPORTANT]
> The forwarder timeout countdown is reset every time data transfer. In other words, the connection is maintained as long as data transfer continues.

> [!CAUTION]
> If a large number of connections are made in a short period of time from a SOCKS5 client application, the spider will crash. In that case, please adjust the number of connections by using the application option.

### 3: show node information
1. self
```
command > 3
[+] show node information
mode (self:s other:o)                          > s
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|shell |   2431583114|         0|         0|192.168.0.25                                  |             (   )|             10000|           |192.168.0.31                                  |                                              |           |            7|      3|      0|             300|                0|
|udp   |   3646916278|3840635511|2702077304|192.168.0.25                                  |             (   )|             10053|           |192.168.0.26                                  |1.1.1.1                                       |         53|            6|      3|      0|              30|                0|
|socks5|   3744999561|         0|         0|192.168.0.25                                  |             (   )|              9050|           |192.168.0.31                                  |                                              |           |            4|      3|      0|              30|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------
|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|1164494135|c   |192.168.0.25                                  |           (   )|                |192.168.0.26                                  |                       (   )|                 1026|          3|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
```
2. other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 3
[+] show node information
mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.31

mode                      : o
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.31

ok? (yes:y no:n quit:q)                        > y
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|socks5|   2450613389|         0|         0|192.168.0.31                                  |             (   )|              9050|           |192.168.0.26                                  |                                              |           |          248|      3|      0|              30|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------
|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|    664383126| 825471693|1923272466|192.168.0.31                                  |          0|192.168.0.25                                  |           -1|                                              |           |           -1|     10|      0|              10|                0|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  89575302|s   |fe80::ca60:dc71:546d:8235                     |14         (   )|            1031|                                              |                       (   )|                     |        304|
| 333064273|-   |fe80::ca60:dc71:546d:8235                     |14         (   )|                |fe80::a00:27ff:fe25:c316                      |14                     (   )|                44782|        364|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
- ipv6
```
command > 3
[+] show node information
mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316

mode                      : o
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316

ok? (yes:y no:n quit:q)                        > y
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|shell |   3059209999|         0|         0|fe80::a00:27ff:fe25:c316                      |enp0s3       (2  )|             10000|           |fe80::ca60:dc71:546d:8235                     |                                              |           |            7|      3|      0|             600|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------
|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|    144737473|3930263129|3678350255|fe80::a00:27ff:fe25:c316                      |          0|fe80::a00:27ff:febe:3a77                      |           -1|                                              |           |           -1|     10|      0|              10|                0|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| 112844871|c   |fe80::a00:27ff:fe25:c316                      |enp0s3     (  2)|                |fe80::ca60:dc71:546d:8235                     |2                      (   )|                 1031|          6|
| 995249023|s   |192.168.0.26                                  |           (   )|            1026|                                              |                       (   )|                     |          3|
|2201581796|-   |192.168.0.26                                  |           (   )|                |192.168.0.25                                  |                       (   )|                58548|          4|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
#### mode (self:s other:o)
Set the type of mode.

If you want to see the node information of the spider itself, set 's'.

If you want to see the node information of other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to see the node information.

### 4: show routing table
> [!NOTE]
> The default route is not set.

> [!IMPORTANT]
> If the value of the metric is abnormal, the spider with that IP address has been disconnected.
> 
> After disconnection, the value of the metric continues to rise, but routes that exceed the METRIC_MAX (default value = 20) value are automatically deleted.
1. self
```
command > 4
[+] show routing table
mode (self:s other:o)                          > s
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sat Mar 29 13:12:39 2025|
|a   |192.168.0.26                                  |     1|2277811895|Sat Mar 29 13:12:58 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|2277811895|Sat Mar 29 13:12:58 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sat Mar 29 13:12:39 2025|
------------------------------------------------------------------------------------------------

```
2. other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 4
[+] show routing table
mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26

mode                      : o
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.26

ok? (yes:y no:n quit:q)                        > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|a   |192.168.0.25                                  |     1|4188256117|Sat Mar 29 13:13:49 2025|
|-   |192.168.0.26                                  |     0|         0|Sat Mar 29 13:12:14 2025|
|-   |fe80::a00:27ff:fe25:c316                      |     0|         0|Sat Mar 29 13:12:14 2025|
|a   |fe80::a00:27ff:febe:3a77                      |     1|4188256117|Sat Mar 29 13:13:49 2025|
------------------------------------------------------------------------------------------------

```
- ipv6
```
command > 4
[+] show routing table
mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316

mode                      : o
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316

ok? (yes:y no:n quit:q)                        > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|a   |192.168.0.25                                  |     1| 809531751|Sat Mar 29 13:15:56 2025|
|-   |192.168.0.26                                  |     0|         0|Sat Mar 29 13:14:43 2025|
|-   |fe80::a00:27ff:fe25:c316                      |     0|         0|Sat Mar 29 13:14:43 2025|
|a   |fe80::a00:27ff:febe:3a77                      |     1| 809531751|Sat Mar 29 13:15:56 2025|
------------------------------------------------------------------------------------------------

```
#### mode (self:s other:o)
Set the type of mode.

If you want to see the routing table of the spider itself, set 's'.

If you want to see the routing table of other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to see the routing table.

### 5: edit routing table
1. add route
- ipv4
```
command > 5
[+] edit routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  > a
ip address                         > 192.168.0.26
metric (0 < metric <=  20)         > 1
---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| 458672986|c   |192.168.0.25                                  |           (   )|                |192.168.0.26                                  |                       (   )|                 1026|          3|
|1754952154|c   |fe80::a00:27ff:febe:3a77                      |enp0s3     (  2)|                |fe80::a00:27ff:fe25:c316                      |2                      (   )|                10026|          4|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

pipe id                             > 458672986

edit mode       : a
ip address      : 192.168.0.26
metric          : 1
pipe id         :  458672986

ok? (yes:y no:n quit:q)             > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |192.168.0.26                                  |     1| 458672986|Sun Feb 23 18:51:56 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  > 

```
- ipv6
```
command > 5
[+] edit routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |192.168.0.26                                  |     1| 458672986|Sun Feb 23 18:51:56 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  > a
ip address                         > fe80::a00:27ff:fe25:c316
metric (0 < metric <=  20)         > 1
---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| 458672986|c   |192.168.0.25                                  |           (   )|                |192.168.0.26                                  |                       (   )|                 1026|          3|
|1754952154|c   |fe80::a00:27ff:febe:3a77                      |enp0s3     (  2)|                |fe80::a00:27ff:fe25:c316                      |2                      (   )|                10026|          4|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

pipe id                             > 1754952154

edit mode       : a
ip address      : fe80::a00:27ff:fe25:c316
metric          : 1
pipe id         : 1754952154

ok? (yes:y no:n quit:q)             > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |192.168.0.26                                  |     1| 458672986|Sun Feb 23 18:51:56 2025|
|s   |fe80::a00:27ff:fe25:c316                      |     1|1754952154|Sun Feb 23 18:53:07 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  >
```

2. delete route
- ipv4
```
command > 5
[+] edit routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |192.168.0.26                                  |     1| 458672986|Sun Feb 23 18:51:56 2025|
|s   |fe80::a00:27ff:fe25:c316                      |     1|1754952154|Sun Feb 23 18:53:07 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  > d
ip address                         > 192.168.0.26

edit mode       : d
ip address      : 192.168.0.26

ok? (yes:y no:n quit:q)            > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |fe80::a00:27ff:fe25:c316                      |     1|1754952154|Sun Feb 23 18:53:07 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  >

```
- ipv6
```
command > 5
[+] edit routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|s   |fe80::a00:27ff:fe25:c316                      |     1|1754952154|Sun Feb 23 18:53:07 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  > d
ip address                         > fe80::a00:27ff:fe25:c316

edit mode       : d
ip address      : fe80::a00:27ff:fe25:c316

ok? (yes:y no:n quit:q)            > y
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:49:31 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:49:31 2025|
------------------------------------------------------------------------------------------------

edit mode (add:a delete:d quit:q)  >

```
#### edit mode (add:a delete:d quit:q)
Set the type of edit mode.

If you want to add a route to the routing table, set 'a'.

If you want to delete a route from the routing table, set 'd'.

If you want to quit, set 'q'.

#### ip address (add)
Set the ip address of the destination spider that you want to add to the routing table.

#### metric (0 < metric <= 20) (add)
Set the metric (the number of spiders to pass through) to the destination spider.

#### pipe id (add)
Set the pipe id for sending packets to the destination spider.

#### ip address (delete)
Set the ip address of the destination spider that you want to delete from the routing table.

### 6: add node (spider client tcp)
> [!IMPORTANT]
> This is not SOCKS5 connection. (Something like TCP over TCP)
>
> [tcp client] <-TCP-> [spider client] <-SOCKS5(TCP)-> [spider server] <-TCP-> [destination server]

> [!NOTE]
> Please set the 'client listen port' to the destination port of the TCP connection tool. (e.g. curl -v http://192.168.0.25:10000)
1. client tcp, self
- ipv4
```
command > 6
[+] add node (spider client tcp)
[!] This is not SOCKS5 connection. (Something like TCP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 16:32:14 2025|
|a   |192.168.0.26                                  |     1|1846558110|Thu Apr  3 16:41:04 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1846558110|Thu Apr  3 16:41:04 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 16:32:14 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > 192.168.0.25
client listen port                             > 10000
client destination spider ip                   > 192.168.0.26
target ip (ipv4<16, domainname<256, ipv6<46)   > 192.168.0.27
target port                                    > 8080
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : 192.168.0.25
client listen port           : 10000
client destination spider ip : 192.168.0.26
target ip                    : 192.168.0.27
target port                  : 8080
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
```
> curl -v http://192.168.0.25:10000
*   Trying 192.168.0.25:10000...
* Connected to 192.168.0.25 (192.168.0.25) port 10000 (#0)
> GET / HTTP/1.1
> Host: 192.168.0.25:10000
> User-Agent: curl/7.88.1
> Accept: */*
> 
* HTTP 1.0, assume close after body
< HTTP/1.0 200 OK
< Server: SimpleHTTP/0.6 Python/3.11.2
< Date: Thu, 03 Apr 2025 07:46:09 GMT
< Content-type: text/html; charset=utf-8
< Content-Length: 232
< 
<!DOCTYPE HTML>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Directory listing for /</title>
</head>
<body>
<h1>Directory listing for /</h1>
<hr>
<ul>
<li><a href="spider.jpg">spider.jpg</a></li>
</ul>
<hr>
</body>
</html>
* Closing connection 0
```
- ipv6
```
command > 6
[+] add node (spider client tcp)
[!] This is not SOCKS5 connection. (Something like TCP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 16:32:14 2025|
|a   |192.168.0.26                                  |     1|1846558110|Thu Apr  3 16:46:59 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1846558110|Thu Apr  3 16:46:59 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 16:32:14 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 10000
client destination spider ip                   > fe80::a00:27ff:fe25:c316
target ip (ipv4<16, domainname<256, ipv6<46)   > fe80::a00:27ff:fe63:4c1f
target port                                    > 8080
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : fe80::a00:27ff:febe:3a77
client listen ip scope id    : enp0s3 (2)
client listen port           : 10000
client destination spider ip : fe80::a00:27ff:fe25:c316
target ip                    : fe80::a00:27ff:fe63:4c1f
target port                  : 8080
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
```
> curl -v 'http://[fe80::a00:27ff:febe:3a77%2]:10000'
*   Trying [fe80::a00:27ff:febe:3a77]:10000...
* Connected to fe80::a00:27ff:febe:3a77 (fe80::a00:27ff:febe:3a77) port 10000 (#0)
> GET / HTTP/1.1
> Host: [fe80::a00:27ff:febe:3a77]:10000
> User-Agent: curl/7.88.1
> Accept: */*
> 
* HTTP 1.0, assume close after body
< HTTP/1.0 200 OK
< Server: SimpleHTTP/0.6 Python/3.11.2
< Date: Thu, 03 Apr 2025 07:50:28 GMT
< Content-type: text/html; charset=utf-8
< Content-Length: 232
< 
<!DOCTYPE HTML>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Directory listing for /</title>
</head>
<body>
<h1>Directory listing for /</h1>
<hr>
<ul>
<li><a href="spider.jpg">spider.jpg</a></li>
</ul>
<hr>
</body>
</html>
* Closing connection 0

```
2. client tcp, other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 6
[+] add node (spider client tcp)
[!] This is not SOCKS5 connection. (Something like TCP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 16:32:14 2025|
|a   |192.168.0.26                                  |     1|1846558110|Thu Apr  3 16:52:57 2025|
|a   |192.168.0.27                                  |     2|1846558110|Thu Apr  3 16:52:57 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1846558110|Thu Apr  3 16:52:57 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1846558110|Thu Apr  3 16:52:57 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 16:32:14 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
client listen ip                               > 192.168.0.26
client listen port                             > 10000
client destination spider ip                   > 192.168.0.27
target ip (ipv4<16, domainname<256, ipv6<46)   > 192.168.0.31
target port                                    > 8080
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : 192.168.0.25
destination spider ip        : 192.168.0.26
client listen ip             : 192.168.0.26
client listen port           : 10000
client destination spider ip : 192.168.0.27
target ip                    : 192.168.0.31
target port                  : 8080
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 6
[+] add node (spider client tcp)
[!] This is not SOCKS5 connection. (Something like TCP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 16:32:14 2025|
|a   |192.168.0.26                                  |     1|1846558110|Thu Apr  3 17:07:39 2025|
|a   |192.168.0.27                                  |     2|1846558110|Thu Apr  3 17:07:39 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1846558110|Thu Apr  3 17:07:39 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1846558110|Thu Apr  3 17:07:39 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 16:32:14 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
client listen ip                               > fe80::a00:27ff:fe25:c316
client listen port                             > 10000
client destination spider ip                   > fe80::a00:27ff:fe63:4c1f
target ip (ipv4<16, domainname<256, ipv6<46)   > fe80::ca60:dc71:546d:8235
target port                                    > 8080
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : fe80::a00:27ff:febe:3a77
source spider ip scope id    : enp0s3 (2)
destination spider ip        : fe80::a00:27ff:fe25:c316
client listen ip             : fe80::a00:27ff:fe25:c316
client listen port           : 10000
client destination spider ip : fe80::a00:27ff:fe63:4c1f
target ip                    : fe80::ca60:dc71:546d:8235
target port                  : 8080
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
#### mode (self:s other:o)
Set the type of mode.

If you want to add client tcp node to the spider itself, set 's'.

If you want to add client tcp node to other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to add the client tcp node.

#### client listen ip
Set the ip address of the interface that the client tcp node will listen on.

#### client listen port
Set the port number that the client tcp node will listen on.

#### client destination spider ip
Set the ip address of the destination spider.

> [!IMPORTANT]
> The startup location of the spider server node is determined by the 'client destination spider ip' when adding the spider client tcp node.

#### target ip (ipv4<16, domainname<256, ipv6<46)
Set the ip address of the destination server (TCP).

#### target port
Set the port number of the destination server (TCP).

#### recv/send tv_sec  (timeout 0-60 sec), recv/send tv_usec (timeout 0-1000000 microsec)
Set the timeout for SOCKS5 selection request/response, SOCKS5 username/password authentication request/response, and SOCKS5 request/response.

#### forwarder tv_sec  (timeout 0-3600 sec), forwarder tv_usec (timeout 0-1000000 microsec)
Set the timeout for packet forwarding between the SOCKS5 client and the destination server.

### 7: add node (spider client udp)
> [!IMPORTANT]
> This is not SOCKS5 connection. (Something like UDP over TCP)
>
> [udp client] <-UDP-> [spider client] <-SOCKS5(TCP)-> [spider server] <-UDP-> [destination server]
> 
> The server starts when the client is created. Therefore, it is necessary to create a route in advance using pipes.

> [!NOTE]
> Please set the 'client listen port' to the destination port of the UDP connection tool. (e.g. dig @192.168.0.25 -p 10053 google.com +notcp)
> 
> The server startup time is set to 5 minutes (spider.hpp FORWARDER_UDP_TIMEOUT) by default. The timeout period is reset each time communication occurs.
1. client udp, self
- ipv4
```
command > 7
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (Something like UDP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:16:20 2025|
|a   |192.168.0.26                                  |     1|1670893174|Thu Apr  3 17:17:18 2025|
|a   |192.168.0.27                                  |     2|1670893174|Thu Apr  3 17:17:18 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1670893174|Thu Apr  3 17:17:18 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1670893174|Thu Apr  3 17:17:18 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:16:20 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > 192.168.0.25
client listen port                             > 10053
client destination spider ip                   > 192.168.0.26
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : 192.168.0.25
client listen port           : 10053
client destination spider ip : 192.168.0.26
target ip                    : 1.1.1.1
target port                  : 53
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec
FORWARDER_UDP_TIMEOUT        :     300 sec

ok? (yes:y no:n quit:q)                        > y

```
```
> dig @192.168.0.25 -p 10053 google.com +notcp

; <<>> DiG 9.18.24-1-Debian <<>> @192.168.0.25 -p 10053 google.com +notcp
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 39837
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 1232
;; QUESTION SECTION:
;google.com.			IN	A

;; ANSWER SECTION:
google.com.		134	IN	A	142.250.207.110

;; Query time: 19 msec
;; SERVER: 192.168.0.25#10053(192.168.0.25) (UDP)
;; WHEN: Thu Apr 03 17:17:57 JST 2025
;; MSG SIZE  rcvd: 55

```
- ipv6
```
command > 7
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (Something like UDP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:19:45 2025|
|a   |192.168.0.26                                  |     1|1167449020|Thu Apr  3 17:20:33 2025|
|a   |192.168.0.27                                  |     2|1167449020|Thu Apr  3 17:20:33 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1167449020|Thu Apr  3 17:20:33 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1167449020|Thu Apr  3 17:20:33 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:19:45 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 10053
client destination spider ip                   > 192.168.0.26
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : fe80::a00:27ff:febe:3a77
client listen ip scope id    : enp0s3 (2)
client listen port           : 10053
client destination spider ip : 192.168.0.26
target ip                    : 1.1.1.1
target port                  : 53
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec
FORWARDER_UDP_TIMEOUT        :     300 sec

ok? (yes:y no:n quit:q)                        > y

```
```
> dig @fe80::a00:27ff:febe:3a77%2 -p 10053 google.com +notcp

; <<>> DiG 9.18.24-1-Debian <<>> @fe80::a00:27ff:febe:3a77%2 -p 10053 google.com +notcp
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 36798
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 1232
;; QUESTION SECTION:
;google.com.			IN	A

;; ANSWER SECTION:
google.com.		288	IN	A	142.250.206.238

;; Query time: 67 msec
;; SERVER: fe80::a00:27ff:febe:3a77%2#10053(fe80::a00:27ff:febe:3a77%2) (UDP)
;; WHEN: Thu Apr 03 17:21:27 JST 2025
;; MSG SIZE  rcvd: 55

```
2. client, other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 7
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (Something like UDP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:19:45 2025|
|a   |192.168.0.26                                  |     1|1167449020|Thu Apr  3 17:22:28 2025|
|a   |192.168.0.27                                  |     2|1167449020|Thu Apr  3 17:22:28 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1167449020|Thu Apr  3 17:22:28 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1167449020|Thu Apr  3 17:22:28 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:19:45 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
client listen ip                               > 192.168.0.26
client listen port                             > 10053
client destination spider ip                   > 192.168.0.27
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : 192.168.0.25
destination spider ip        : 192.168.0.26
client listen ip             : 192.168.0.26
client listen port           : 10053
client destination spider ip : 192.168.0.27
target ip                    : 1.1.1.1
target port                  : 53
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec
FORWARDER_UDP_TIMEOUT        :     300 sec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 7
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (Something like UDP over TCP)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:19:45 2025|
|a   |192.168.0.26                                  |     1|1167449020|Thu Apr  3 17:24:28 2025|
|a   |192.168.0.27                                  |     2|1167449020|Thu Apr  3 17:24:28 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1|1167449020|Thu Apr  3 17:24:28 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2|1167449020|Thu Apr  3 17:24:28 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:19:45 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
client listen ip                               > fe80::a00:27ff:fe25:c316
client listen port                             > 10053
client destination spider ip                   > 192.168.0.27
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : fe80::a00:27ff:febe:3a77
source spider ip scope id    : enp0s3 (2)
destination spider ip        : fe80::a00:27ff:fe25:c316
client listen ip             : fe80::a00:27ff:fe25:c316
client listen port           : 10053
client destination spider ip : 192.168.0.27
target ip                    : 1.1.1.1
target port                  : 53
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec
FORWARDER_UDP_TIMEOUT        :     300 sec

ok? (yes:y no:n quit:q)                        > y

```
#### mode (self:s other:o)
Set the type of mode.

If you want to add client udp node to the spider itself, set 's'.

If you want to add client udp node to other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to add the client udp node.

#### client listen ip
Set the ip address of the interface that the client udp node will listen on.

#### client listen port
Set the port number that the client udp node will listen on.

#### client destination spider ip
Set the ip address of the destination spider.

> [!IMPORTANT]
> The startup location of the spider server node is determined by the 'client destination spider ip' when adding the spider client udp node.

#### target ip (ipv4<16, domainname<256, ipv6<46)
Set the ip address of the destination server (UDP).

#### target port
Set the port number of the destination server (UDP).

#### recv/send tv_sec  (timeout 0-60 sec), recv/send tv_usec (timeout 0-1000000 microsec)
Set the timeout for SOCKS5 selection request/response, SOCKS5 username/password authentication request/response, and SOCKS5 request/response.

#### forwarder tv_sec  (timeout 0-3600 sec), forwarder tv_usec (timeout 0-1000000 microsec)
Set the timeout for packet forwarding between the SOCKS5 client and the destination server.

### 8: add node (spider client shell)
> [!IMPORTANT]
> This is not SOCKS5 connection.
1. client shell, self
- ipv4
```
command > 8
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |192.168.0.26                                  |     1| 976861421|Thu Apr  3 17:41:15 2025|
|a   |192.168.0.27                                  |     2| 976861421|Thu Apr  3 17:41:15 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 976861421|Thu Apr  3 17:41:15 2025|
|a   |fd00:abcd:1234:5678::3                        |     2| 976861421|Thu Apr  3 17:41:15 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 976861421|Thu Apr  3 17:41:15 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2| 976861421|Thu Apr  3 17:41:15 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:40:53 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > 192.168.0.25
client listen port                             > 10000
client destination spider ip                   > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : 192.168.0.25
client listen port           : 10000
client destination spider ip : 192.168.0.26
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :     300 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
```
> nc 192.168.0.25 10000

command >id
uid=1000(vboxuser) gid=1000(vboxuser) groups=1000(vboxuser),100(users)

command >ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host noprefixroute 
       valid_lft forever preferred_lft forever
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 08:00:27:25:c3:16 brd ff:ff:ff:ff:ff:ff
    inet 192.168.0.26/24 brd 192.168.0.255 scope global dynamic noprefixroute enp0s3
       valid_lft 541sec preferred_lft 541sec
    inet6 fd00:abcd:1234:5678::2/64 scope global 
       valid_lft forever preferred_lft forever
    inet6 fe80::a00:27ff:fe25:c316/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever

command >ls /tmp/spider.jpg 2>&1
ls: cannot access '/tmp/spider.jpg': No such file or directory

command >upload /home/vboxuser/sample/spider.jpg /tmp
[+] upload file: /tmp/spider.jpg
command >ls /tmp/spider.jpg 2>&1
/tmp/spider.jpg

command >download /tmp/spider.jpg /home/vboxuser/Pictures
[+] download file: /home/vboxuser/Pictures/spider.jpg
command >exit

^C
```
> [!CAUTION]
> The file upload and download features are optional.
> 
> The spider cannot upload and download a large file.
- ipv6
```
command > 8
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |192.168.0.26                                  |     1| 976861421|Thu Apr  3 17:43:23 2025|
|a   |192.168.0.27                                  |     2| 976861421|Thu Apr  3 17:43:23 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 976861421|Thu Apr  3 17:43:23 2025|
|a   |fd00:abcd:1234:5678::3                        |     2| 976861421|Thu Apr  3 17:43:23 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 976861421|Thu Apr  3 17:43:23 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2| 976861421|Thu Apr  3 17:43:23 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:40:53 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 10000
client destination spider ip                   > fe80::a00:27ff:fe25:c316
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300 
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : fe80::a00:27ff:febe:3a77
client listen ip scope id    : enp0s3 (2)
client listen port           : 10000
client destination spider ip : fe80::a00:27ff:fe25:c316
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :     300 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
```
> ncat -6 fe80::a00:27ff:febe:3a77%enp0s3 10000

command >id
uid=1000(vboxuser) gid=1000(vboxuser) groups=1000(vboxuser),100(users)

command >ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host noprefixroute 
       valid_lft forever preferred_lft forever
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 08:00:27:25:c3:16 brd ff:ff:ff:ff:ff:ff
    inet 192.168.0.26/24 brd 192.168.0.255 scope global dynamic noprefixroute enp0s3
       valid_lft 404sec preferred_lft 404sec
    inet6 fd00:abcd:1234:5678::2/64 scope global 
       valid_lft forever preferred_lft forever
    inet6 fe80::a00:27ff:fe25:c316/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever

command >ls /tmp/spider.jpg 2>&1
ls: cannot access '/tmp/spider.jpg': No such file or directory

command >upload /home/vboxuser/sample/spider.jpg /tmp
[+] upload file: /tmp/spider.jpg
command >ls /tmp/spider.jpg 2>&1
/tmp/spider.jpg

command >download /tmp/spider.jpg /home/vboxuser/Pictures
[+] download file: /home/vboxuser/Pictures/spider.jpg
command >exit

^C
```
> [!CAUTION]
> The file upload and download features are optional.
> 
> The spider cannot upload and download a large file.
2. client, other (other spider)
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 8
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |192.168.0.26                                  |     1| 976861421|Thu Apr  3 17:45:09 2025|
|a   |192.168.0.27                                  |     2| 976861421|Thu Apr  3 17:45:09 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 976861421|Thu Apr  3 17:45:09 2025|
|a   |fd00:abcd:1234:5678::3                        |     2| 976861421|Thu Apr  3 17:45:09 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 976861421|Thu Apr  3 17:45:09 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2| 976861421|Thu Apr  3 17:45:09 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:40:53 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
client listen ip                               > 192.168.0.26
client listen port                             > 10000
client destination spider ip                   > 192.168.0.27
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : 192.168.0.25
destination spider ip        : 192.168.0.26
client listen ip             : 192.168.0.26
client listen port           : 10000
client destination spider ip : 192.168.0.27
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :     300 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 8
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |192.168.0.26                                  |     1| 976861421|Thu Apr  3 17:49:26 2025|
|a   |192.168.0.27                                  |     2| 976861421|Thu Apr  3 17:49:26 2025|
|-   |fd00:abcd:1234:5678::1                        |     0|         0|Thu Apr  3 17:40:53 2025|
|a   |fd00:abcd:1234:5678::2                        |     1| 976861421|Thu Apr  3 17:49:26 2025|
|a   |fd00:abcd:1234:5678::3                        |     2| 976861421|Thu Apr  3 17:49:26 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 976861421|Thu Apr  3 17:49:26 2025|
|a   |fe80::a00:27ff:fe63:4c1f                      |     2| 976861421|Thu Apr  3 17:49:26 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Thu Apr  3 17:40:53 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > o
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
client listen ip                               > fe80::a00:27ff:fe25:c316
client listen port                             > 10000
client destination spider ip                   > fe80::a00:27ff:fe63:4c1f
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300 
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : fe80::a00:27ff:febe:3a77
source spider ip scope id    : enp0s3 (2)
destination spider ip        : fe80::a00:27ff:fe25:c316
client listen ip             : fe80::a00:27ff:fe25:c316
client listen port           : 10000
client destination spider ip : fe80::a00:27ff:fe63:4c1f
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :     300 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
#### mode (self:s other:o)
Set the type of mode.

If you want to add client shell node to the spider itself, set 's'.

If you want to add client shell node to other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to add the client shell node.

#### client listen ip
Set the ip address of the interface that the client shell node will listen on.

#### client listen port
Set the port number that the client shell node will listen on.

#### client destination spider ip
Set the ip address of the destination spider.

> [!IMPORTANT]
> The startup location of the spider server node is determined by the 'client destination spider ip' when adding the spider client shell node.

#### recv/send tv_sec  (timeout 0-60 sec), recv/send tv_usec (timeout 0-1000000 microsec)
Set the timeout for SOCKS5 selection request/response, SOCKS5 username/password authentication request/response, and SOCKS5 request/response.

#### forwarder tv_sec  (timeout 0-3600 sec), forwarder tv_usec (timeout 0-1000000 microsec)
Set the timeout for packet forwarding between the SOCKS5 client and the destination server.

Set the forwarder timeout value longer. (300s - 3600s)

> [!IMPORTANT]
> The forwarder timeout countdown is reset every time data transfer. In other words, the connection is maintained as long as data transfer continues.

### 9: close client listener (tcp)
1. self
```
command > 9
[+] close client listener (tcp)
mode (self:s other:o)                          > s
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|tcp   |    404378738|         0|         0|fd00:abcd:1234:5678::1                        |             (   )|             10000|           |fd00:abcd:1234:5678::2                        |fd00:abcd:1234:5678::2                        |       8080|            5|      3|      0|              30|                0|
|shell |    794147550|         0|         0|fe80::a00:27ff:febe:3a77                      |enp0s3       (  2)|             20000|           |fe80::a00:27ff:fe25:c316                      |                                              |           |            7|      3|      0|             300|                0|
|socks5|   1399291952|         0|         0|192.168.0.25                                  |             (   )|              9050|           |192.168.0.26                                  |                                              |           |            3|      3|      0|              30|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

connection id                                  > 1399291952

connection id             : 1399291952

ok? (yes:y no:n quit:q)                        > y
[+] close client listener connection_id: 1399291952

```
2. other (other spider)
```
Not implemented.
```

#### mode (self:s other:o)
Set the type of mode.

If you want to close client listener of the spider itself, set 's'.

If you want to close client listener of other spider, set 'o'.

> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.

#### source spider ip (other)
Set the ip address of the spider itself.

#### destination spider ip (other)
Set the ip address of the destination spider that you want to close client listener.

#### connection id
Set the connection id that you want to close client listener.

> [!NOTE]
> It may take some time for client to be able to listen on the same port again. (CLOSE_WAIT timeout)

## Example
![](./imgs/img01.jpg)

1. add spider pipe server node (1)
```
> ./spider -4 192.168.0.25

                 -.                                 _//                   
           .=  :*=--::                       _/     _//                   
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     
        :*-  :*-   .:.              _//                                   
        =-    -:                 Linux Ver: 1.23  Author: Shuichiro Endo  


--------------------------------- spider ---------------------------------
 spider ipv4                     : 192.168.0.25
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
 0: exit
--------------------------------------------------------------------------

command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 18:05:35 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > s
pipe listen ip                                 > 192.168.0.25
pipe listen port                               > 1025

pipe mode                 : s
pipe listen ip            : 192.168.0.25
pipe listen port          : 1025

ok? (yes:y no:n quit:q)                        > y
[+] listening port 1025 on 192.168.0.25

```

2. add spider pipe client node (2)
```
> ./spider -4 192.168.0.26

                 -.                                 _//                   
           .=  :*=--::                       _/     _//                   
      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  
      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     
    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     
  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     
         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     
        :*-  :*-   .:.              _//                                   
        =-    -:                 Linux Ver: 1.23  Author: Shuichiro Endo  


--------------------------------- spider ---------------------------------
 spider ipv4                     : 192.168.0.26
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
 0: exit
--------------------------------------------------------------------------

command > 1
[+] add node (spider pipe)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.26                                  |     0|         0|Thu Apr  3 18:05:54 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
pipe mode (client:c server:s)                  > c
pipe ip                                        > 192.168.0.26
pipe destination ip                            > 192.168.0.25
pipe destination port                          > 1025

pipe mode                 : c
pipe ip                   : 192.168.0.26
pipe destination ip       : 192.168.0.25
pipe destination port     : 1025

ok? (yes:y no:n quit:q)                        > y
[+] connecting to ip:192.168.0.25 port:1025
[+] connected to ip:192.168.0.25 port:1025

```

3. add spider client node (3)
> [!NOTE]
> The startup location of the spider server node is determined by the 'client destination spider ip' when adding the spider client node.
```
command > 2
[+] add node (spider client)
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 18:05:35 2025|
|a   |192.168.0.26                                  |     1|1542109111|Thu Apr  3 18:08:32 2025|
------------------------------------------------------------------------------------------------

mode (self:s other:o)                          > s
client listen ip                               > 192.168.0.25
client listen port                             > 9050
client destination spider ip                   > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip             : 192.168.0.25
client listen port           : 9050
client destination spider ip : 192.168.0.26
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y
[+] listening port 9050 on 192.168.0.25

```

4. confirm node and routing table
- show node information (pc01 192.168.0.25)
```
command > 3
[+] show node information
mode (self:s other:o)                          > s
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|socks5|   3934787980|         0|         0|192.168.0.25                                  |             (   )|              9050|           |192.168.0.26                                  |                                              |           |            6|      3|      0|              30|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------
|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| 862920493|s   |192.168.0.25                                  |           (   )|            1025|                                              |                       (   )|                     |          3|
|1542109111|-   |192.168.0.25                                  |           (   )|                |192.168.0.26                                  |                       (   )|                60590|          4|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
- show routing table (pc01 192.168.0.25)
> [!IMPORTANT]
> Please confirm that there is route information for the destination spider.
```
command > 4
[+] show routing table
mode (self:s other:o)                          > s
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Thu Apr  3 18:05:35 2025|
|a   |192.168.0.26                                  |     1|1542109111|Thu Apr  3 18:10:21 2025|
------------------------------------------------------------------------------------------------

```

5. run socks5 client (e.g. curl)
```
curl -v -x socks5h://192.168.0.25:9050 https://www.google.com
```
