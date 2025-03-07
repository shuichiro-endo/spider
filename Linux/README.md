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
> [!NOTE]
> In auto mode with the -r option, routing information is automatically exchanged between spiders.
> 
> In self mode, routing information is not exchanged between spiders, so please set the routing information manually.
>
> If the -r option is not set, it is in auto mode by default.

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
        =-    -:                  Linux Ver: 1.8  Author: Shuichiro Endo  


usage   : ./spider
        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]
        : [-f config_file]
        : [-i pipe_destination_ip] [-p pipe_destination_port]
        : [-r routing_mode(auto:a self:s)]
        : [-e x(xor encryption)] [-k key(hexstring)]
        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]
example : ./spider
        : ./spider -4 192.168.0.10
        : ./spider -6 2001::xxxx:xxxx:xxxx:xxxx
        : ./spider -u fd00::xxxx:xxxx:xxxx:xxxx
        : ./spider -l fe80::xxxx:xxxx:xxxx:xxxx%eth0
        : ./spider -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%eth0
        : ./spider -f config_sample.txt
        : ./spider -i 192.168.0.25 -p 1025
        : ./spider -4 192.168.0.10 -r s
        : ./spider -4 192.168.0.10 -e x -k deadbeef
        : ./spider -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a

```

### run
> [!NOTE]
> The -4, -6, -u, and -l option set the IP address of the network interface.
> 
> Some IP addresses may not work correctly. (e.g. 127.0.0.1, ::1, 0.0.0.0)

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
        =-    -:                  Linux Ver: 1.8  Author: Shuichiro Endo  


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
----------------------------- spider command -----------------------------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client udp)
 7: add node (spider client shell)
 8: add node (spider client) to destination spider
 9: add node (spider pipe) to destination spider
 0: exit
--------------------------------------------------------------------------

command >

```

### 1: add node (spider client)
> [!NOTE]
> The startup location of the spider server node is determined by the 'destination spider ip' when adding the spider client node.
>
> Please set the forwarder timeout to a larger value. (The default is 30 seconds.)
- ipv4
```
command > 1
[+] add node (spider client)
client listen ip             > 192.168.0.25
client listen port           > 9050
destination spider ip        > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : 192.168.0.25
client listen port        : 9050
destination spider ip     : 192.168.0.26
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :      30 sec
forwarder_tv_usec         :       0 microsec

ok? (yes:y no:n quit:q) > y

```
- ipv6
```
command > 1
[+] add node (spider client)
client listen ip             > fe80::a00:27ff:febe:3a77
client listen port           > 9000
destination spider ip        > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : fe80::a00:27ff:febe:3a77
client listen ip scope id : enp0s3 (2)
client listen port        : 9000
destination spider ip     : 192.168.0.26
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :      30 sec
forwarder_tv_usec         :       0 microsec

ok? (yes:y no:n quit:q) > y

```

### 2: add node (spider pipe)
1. pipe (server)
> [!NOTE]
> The pipe server can be connected by only one pipe client at a time.
- ipv4
```
command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > s
pipe listen ip              > 192.168.0.25
pipe listen port            > 1025

mode                    : s
pipe listen ip          : 192.168.0.25
pipe listen port        : 1025

ok? (yes:y no:n quit:q) > y

```
- ipv6
```
command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > s
pipe listen ip              > fe80::a00:27ff:fe25:c316
pipe listen port            > 1026

mode                    : s
pipe listen ip          : fe80::a00:27ff:fe25:c316
pipe listen ip scope id : enp0s3 (2)
pipe listen port        : 1026

ok? (yes:y no:n quit:q) > y

```

2. pipe (client)
- ipv4
```
command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > c
pipe ip                     > 192.168.0.25
pipe destination ip         > 192.168.0.26
pipe destination port       > 1026

mode                    : c
pipe ip                 : 192.168.0.25
pipe destination ip     : 192.168.0.26
pipe destination port   : 1026

ok? (yes:y no:n quit:q) > y

```
- ipv6
```
command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > c
pipe ip                     > fe80::a00:27ff:febe:3a77
pipe destination ip         > fe80::a00:27ff:fe25:c316
pipe destination port       > 1026

mode                    : c
pipe ip                 : fe80::a00:27ff:febe:3a77
pipe ip scope id        : enp0s3 (2)
pipe destination ip     : fe80::a00:27ff:fe25:c316
pipe destination port   : 1026

ok? (yes:y no:n quit:q) > y

```

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

### 4: show routing table
```
command > 4
[+] show routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:28:42 2025|
|a   |192.168.0.26                                  |     1| 669606159|Sun Feb 23 18:30:30 2025|
|a   |fe80::a00:27ff:fe25:c316                      |     1| 669606159|Sun Feb 23 18:30:30 2025|
|-   |fe80::a00:27ff:febe:3a77                      |     0|         0|Sun Feb 23 18:28:42 2025|
------------------------------------------------------------------------------------------------

```

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
metric (0 < metric <= 255)         > 1
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
metric (0 < metric <= 255)         > 1
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

### 6: add node (spider client udp)
> [!IMPORTANT]
> This is not SOCKS5 connection. (UDP over TCP)
> 
> The server starts when the client is created. Therefore, it is necessary to create a route in advance using pipes.

> [!NOTE]
> Please set the 'client listen port' to the destination port of the UDP connection tool. (e.g. dig @192.168.0.25 -p 10053 google.com +notcp)
> 
> The server startup time is set to 5 minutes (spider.hpp FORWARDER_UDP_TIMEOUT) by default. The timeout period is reset each time communication occurs.
- ipv4
```
command > 6
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (UDP over TCP)
client listen ip                               > 192.168.0.25
client listen port                             > 10053
destination spider ip                          > 192.168.0.26
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : 192.168.0.25
client listen port        : 10053
destination spider ip     : 192.168.0.26
target ip                 : 1.1.1.1
target port               : 53
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :      30 sec
forwarder_tv_usec         :       0 microsec
FORWARDER_UDP_TIMEOUT     :     300 sec

ok? (yes:y no:n quit:q) > y

```
```
> dig @192.168.0.25 -p 10053 google.com +notcp

; <<>> DiG 9.18.24-1-Debian <<>> @192.168.0.25 -p 10053 google.com +notcp
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 4477
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 1232
;; QUESTION SECTION:
;google.com.			IN	A

;; ANSWER SECTION:
google.com.		149	IN	A	142.250.76.142

;; Query time: 28 msec
;; SERVER: 192.168.0.25#10053(192.168.0.25) (UDP)
;; WHEN: Fri Mar 07 08:49:39 JST 2025
;; MSG SIZE  rcvd: 55

```
- ipv6
```
command > 6
[+] add node (spider client udp)
[!] This is not SOCKS5 connection. (UDP over TCP)
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 10053
destination spider ip                          > 192.168.0.26
target ip (ipv4<16, domainname<256, ipv6<46)   > 1.1.1.1
target port                                    > 53
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : fe80::a00:27ff:febe:3a77
client listen ip scope id : enp0s3 (2)
client listen port        : 10053
destination spider ip     : 192.168.0.26
target ip                 : 1.1.1.1
target port               : 53
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :      30 sec
forwarder_tv_usec         :       0 microsec
FORWARDER_UDP_TIMEOUT     :     300 sec

ok? (yes:y no:n quit:q) > y

```
```
dig @fe80::a00:27ff:febe:3a77%2 -p 10053 google.com +notcp

; <<>> DiG 9.18.24-1-Debian <<>> @fe80::a00:27ff:febe:3a77%2 -p 10053 google.com +notcp
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 38501
;; flags: qr rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 1232
;; QUESTION SECTION:
;google.com.			IN	A

;; ANSWER SECTION:
google.com.		265	IN	A	142.250.76.142

;; Query time: 40 msec
;; SERVER: fe80::a00:27ff:febe:3a77%2#10053(fe80::a00:27ff:febe:3a77%2) (UDP)
;; WHEN: Fri Mar 07 08:51:26 JST 2025
;; MSG SIZE  rcvd: 55

```

### 7: add node (spider client shell)
> [!IMPORTANT]
> This is not SOCKS5 connection.
> 
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 7
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
client listen ip                               > 192.168.0.25
client listen port                             > 10000
destination spider ip                          > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : 192.168.0.25
client listen port        : 10000
destination spider ip     : 192.168.0.26
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :     300 sec
forwarder_tv_usec         :       0 microsec

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

command >exit

^C
```
- ipv6
```
command > 7
[+] add node (spider client shell)
[!] This is not SOCKS5 connection.
client listen ip                               > fe80::a00:27ff:febe:3a77
client listen port                             > 10000
destination spider ip                          > fe80::a00:27ff:fe25:c316
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 300 
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : fe80::a00:27ff:febe:3a77
client listen ip scope id : enp0s3 (2)
client listen port        : 10000
destination spider ip     : fe80::a00:27ff:fe25:c316
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :     300 sec
forwarder_tv_usec         :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
```
ncat -6 fe80::a00:27ff:febe:3a77%enp0s3 10000


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

command >exit

^C
```

### 8: add node (spider client) to destination spider
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
- ipv4
```
command > 8
[+] add node (spider client) to destination spider
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
client listen ip                               > 192.168.0.26
client listen port                             > 9050
client destination spider ip                   > 192.168.0.31
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : 192.168.0.25
destination spider ip        : 192.168.0.26
client listen ip             : 192.168.0.26
client listen port           : 9050
client destination spider ip : 192.168.0.31
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 8
[+] add node (spider client) to destination spider
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
client listen ip                               > fe80::a00:27ff:fe25:c316
client listen port                             > 9050
client destination spider ip                   > fe80::ca60:dc71:546d:8235
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

source spider ip             : fe80::a00:27ff:febe:3a77
source spider ip scope id    : enp0s3 (2)
destination spider ip        : fe80::a00:27ff:fe25:c316
client listen ip             : fe80::a00:27ff:fe25:c316
client listen port           : 9050
client destination spider ip : fe80::ca60:dc71:546d:8235
recv/send tv_sec             :       3 sec
recv/send tv_usec            :       0 microsec
forwarder_tv_sec             :      30 sec
forwarder_tv_usec            :       0 microsec

ok? (yes:y no:n quit:q)                        > y

```

### 9: add node (spider pipe) to destination spider
> [!IMPORTANT]
> It is necessary to create a route in advance using pipes.
1. pipe (server)
- ipv4
```
command > 9
[+] add node (spider pipe) to destination spider
mode (client:c server:s)                       > s
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
pipe listen ip                                 > 192.168.0.26
pipe listen port                               > 10026

mode                      : s
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.26
pipe listen ip            : 192.168.0.26
pipe listen port          : 10026

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 9
[+] add node (spider pipe) to destination spider
mode (client:c server:s)                       > s
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
pipe listen ip                                 > fe80::a00:27ff:fe25:c316
pipe listen port                               > 10026

mode                      : s
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316
pipe listen ip            : fe80::a00:27ff:fe25:c316
pipe listen port          : 10026

ok? (yes:y no:n quit:q)                        > y

```
2. pipe (client)
- ipv4
```
command > 9
[+] add node (spider pipe) to destination spider
mode (client:c server:s)                       > c
source spider ip                               > 192.168.0.25
destination spider ip                          > 192.168.0.26
pipe ip                                        > 192.168.0.26
pipe destination ip                            > 192.168.0.31
pipe destination port                          > 1031

mode                      : c
source spider ip          : 192.168.0.25
destination spider ip     : 192.168.0.26
pipe ip                   : 192.168.0.26
pipe destination ip       : 192.168.0.31
pipe destination port     : 1031

ok? (yes:y no:n quit:q)                        > y

```
- ipv6
```
command > 9
[+] add node (spider pipe) to destination spider
mode (client:c server:s)                       > c
source spider ip                               > fe80::a00:27ff:febe:3a77
destination spider ip                          > fe80::a00:27ff:fe25:c316
pipe ip                                        > fe80::a00:27ff:fe25:c316
pipe destination ip                            > fe80::ca60:dc71:546d:8235
pipe destination port                          > 1031

mode                      : c
source spider ip          : fe80::a00:27ff:febe:3a77
source spider ip scope id : enp0s3 (2)
destination spider ip     : fe80::a00:27ff:fe25:c316
pipe ip                   : fe80::a00:27ff:fe25:c316
pipe destination ip       : fe80::ca60:dc71:546d:8235
pipe destination port     : 1031

ok? (yes:y no:n quit:q)                        > y

```

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
        =-    -:                  Linux Ver: 0.8  Author: Shuichiro Endo  


----------     spider     ----------
 spider ipv4          : 192.168.0.25
 routing mode         : auto
 xor encryption       : off
 xor key hex string   : 
 aes encryption       : off
 aes key hex string   : 
 aes iv hex string    : 
---------- spider command ----------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client udp)
 0: exit
------------------------------------

command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > s
pipe listen ip              > 192.168.0.25
pipe listen port            > 1025

mode                    : s
pipe listen ip          : 192.168.0.25
pipe listen port        : 1025

ok? (yes:y no:n quit:q) > y

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
        =-    -:                  Linux Ver: 0.8  Author: Shuichiro Endo  


----------     spider     ----------
 spider ipv4          : 192.168.0.26
 routing mode         : auto
 xor encryption       : off
 xor key hex string   : 
 aes encryption       : off
 aes key hex string   : 
 aes iv hex string    : 
---------- spider command ----------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client udp)
 0: exit
------------------------------------

command > 2
[+] add node (spider pipe)
mode (client:c server:s)    > c
pipe ip                     > 192.168.0.26
pipe destination ip         > 192.168.0.25
pipe destination port       > 1025

mode                    : c
pipe ip                 : 192.168.0.26
pipe destination ip     : 192.168.0.25
pipe destination port   : 1025

ok? (yes:y no:n quit:q) > y

```

3. add spider client node (3)
> [!NOTE]
> The startup location of the spider server node is determined by the 'destination spider ip' when adding the spider client node.
```
----------     spider     ----------
 spider ipv4          : 192.168.0.25
 routing mode         : auto
 xor encryption       : off
 xor key hex string   : 
 aes encryption       : off
 aes key hex string   : 
 aes iv hex string    : 
---------- spider command ----------
 1: add node (spider client)
 2: add node (spider pipe)
 3: show node information
 4: show routing table
 5: edit routing table
 6: add node (spider client udp)
 0: exit
------------------------------------

command > 1
[+] add node (spider client)
client listen ip             > 192.168.0.25
client listen port           > 9050
destination spider ip        > 192.168.0.26
recv/send tv_sec  (timeout 0-60 sec)           > 3
recv/send tv_usec (timeout 0-1000000 microsec) > 0
forwarder tv_sec  (timeout 0-3600 sec)         > 30
forwarder tv_usec (timeout 0-1000000 microsec) > 0

client listen ip          : 192.168.0.25
client listen port        : 9050
destination spider ip     : 192.168.0.26
recv/send tv_sec          :       3 sec
recv/send tv_usec         :       0 microsec
forwarder_tv_sec          :      30 sec
forwarder_tv_usec         :       0 microsec

ok? (yes:y no:n quit:q) > y

```

4. confirm node and routing table
- show node information (pc01 192.168.0.25)
```
command > 3
[+] show node information
-------------------------------------------------------------------------------------------------------------------------------------------------------- client --------------------------------------------------------------------------------------------------------------------------------------------------------
|type  |connection id|client id |server id |client ip                                     |client ip scope id|client listen port|client port|destination spider ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|socks5|   1840561196|         0|         0|192.168.0.25                                  |             (   )|              9050|           |192.168.0.26                                  |                                              |           |            6|      3|      0|              30|                0|
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------- server -----------------------------------------------------------------------------------------------------------------------------------------
|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|target socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------- pipe ------------------------------------------------------------------------------------------------------
|pipe id   |mode|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|1108038504|-   |192.168.0.25                                  |           (   )|                |192.168.0.26                                  |                       (   )|                60886|          4|
|2507956686|s   |192.168.0.25                                  |           (   )|            1025|                                              |                       (   )|                     |          3|
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
- show routing table (pc01 192.168.0.25)
> [!IMPORTANT]
> Please confirm that there is route information for the destination spider.
```
command > 4
[+] show routing table
---------------------------------------- routing  table ----------------------------------------
|mode|ip address                                    |metric|pipe id   |time                    |
------------------------------------------------------------------------------------------------
|-   |192.168.0.25                                  |     0|         0|Sun Feb 23 18:59:30 2025|
|a   |192.168.0.26                                  |     1|1108038504|Sun Feb 23 19:03:37 2025|
------------------------------------------------------------------------------------------------

```

5. run socks5 client (e.g. curl)
```
curl -v -x socks5h://192.168.0.25:9050 https://www.google.com
```
