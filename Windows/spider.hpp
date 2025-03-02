/*
 * Title:  spider (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SPIDER_H_
#define SPIDER_H_

//#define DEBUGPRINT

#define NODE_BUFFER_SIZE 65535

#define ROUTING_MESSAGE_DATA_SIZE 60000
#define SOCKS5_MESSAGE_DATA_SIZE 60000

#define ROUTING_MESSAGE_QUEUE_CAPACITY 100
#define SOCKS5_MESSAGE_QUEUE_CAPACITY 100

#define KEYS_MAP_SIZE 200
#define DELETE_ROUTE_TIME 5 // 5s

#define INET6_ADDR_STRING_LENGTH 46

#define FORWARDER_UDP_TIMEOUT 300

#define SOCKS5_AUTHENTICATION_METHOD 0x0; // 0x0:No Authentication Required  0x2:Username/Password Authentication
#define SOCKS5_USERNAME "socks5user"
#define SOCKS5_PASSWORD "supersecretpassword"

#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT      1
#define SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE        2
#define SPIDER_COMMAND_SHOW_NODE_INFORMATION       3
#define SPIDER_COMMAND_SHOW_ROUTING_TABLE          4
#define SPIDER_COMMAND_EDIT_ROUTING_TABLE          5
#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP  6
#define SPIDER_COMMAND_EXIT                        0


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdint.h>
#include <process.h>
#include <iostream>
#include <limits>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <map>
#include <queue>
#include <semaphore>
#include <stdint.h>
#include <stddef.h>
#include <iphlpapi.h>
#include <netioapi.h>


#pragma comment(lib, "ws2_32.lib")      // Winsock Library
#pragma comment(lib, "iphlpapi.lib")    // IP Helper Library


namespace spider
{
    struct timezone
    {
        int tz_minuteswest;
        int tz_dsttime;
    };
}

#endif /* SPIDER_H_ */

