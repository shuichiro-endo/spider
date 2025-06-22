/*
 * Title:  spider (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SPIDER_H_
#define SPIDER_H_

//#define DEBUGPRINT

#define SECURITY_WIN32

#define NODE_BUFFER_SIZE 72000
#define SPIDER_MESSAGE_DATA_SIZE 65536
#define SPIDER_MESSAGE_DATA_MAX_SIZE 65552        // 65536 + 16 (AES padding)
#define SHELL_UPLOAD_DOWNLOAD_DATA_SIZE 50000
#define SHELL_RESULT_DATA_SIZE 50000

#define ROUTING_MESSAGE_QUEUE_CAPACITY 100
#define SOCKS5_MESSAGE_QUEUE_CAPACITY 100

#define KEYS_MAP_SIZE 200
#define METRIC_MAX 20   // 0 < METRIC_MAX <= UINT8_MAX(255), METRIC_MAX < delete route
#define DELETE_ROUTE_TIME 5 // 5s

#define INET6_ADDR_STRING_LENGTH 46

#define PIPE_MESSAGE_MODE_HTTP_SLEEP 300    // 300 ms
#define HTTP_REQUEST_HEADER_USER_AGENT_VALUE "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36 Edg/127.0.2651.74"

#define CERT_NAME_MAX_SIZE 1024

#define FORWARDER_UDP_TIMEOUT 300

#define SHOW_NODE_INFORMATION_WORKER_TV_SEC 10
#define SHOW_NODE_INFORMATION_WORKER_TV_USEC 0
#define SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC 10
#define SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC 0

#define SHOW_ROUTING_TABLE_WORKER_TV_SEC 10
#define SHOW_ROUTING_TABLE_WORKER_TV_USEC 0
#define SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_SEC 10
#define SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_USEC 0

#define ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_SEC 10
#define ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_USEC 0
#define ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_SEC 60
#define ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_USEC 0

#define SOCKS5_AUTHENTICATION_METHOD 0x0; // 0x0:No Authentication Required  0x2:Username/Password Authentication
#define SOCKS5_USERNAME "socks5user"
#define SOCKS5_PASSWORD "supersecretpassword"

#define SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE                         1
#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT                       2
#define SPIDER_COMMAND_SHOW_NODE_INFORMATION                        3
#define SPIDER_COMMAND_SHOW_ROUTING_TABLE                           4
#define SPIDER_COMMAND_EDIT_ROUTING_TABLE                           5
#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP                   6
#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP                   7
#define SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL                 8
#define SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP                    9
#define SPIDER_COMMAND_EXIT                                         0


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
#include <iomanip>
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>
#include <filesystem>
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
#include <Security.h>
#include <schannel.h>
#include <wincrypt.h>
#include <ncrypt.h>
#include <sspi.h>


#pragma comment(lib, "user32.lib")      // User32 Library
#pragma comment(lib, "ws2_32.lib")      // Winsock Library
#pragma comment(lib, "iphlpapi.lib")    // IP Helper Library
#pragma comment(lib, "secur32.lib")     // Secur32 Library
#pragma comment(lib, "crypt32.lib")     // Crypt32 Library
#pragma comment(lib, "ncrypt.lib")      // Ncrypt Library
#pragma comment(lib, "advapi32.lib")    // Advapi32 Library


namespace spider
{
    struct timezone
    {
        int tz_minuteswest;
        int tz_dsttime;
    };
}


#pragma pack(push, 1)
struct spider_message_header
{
    char message_type;
    uint8_t receive_flag;
    uint8_t receive_result;
    uint8_t command_result;
    uint32_t message_id;
    uint32_t connection_id;
    uint32_t client_id;
    uint32_t server_id;
    char source_node_type;
    char reserved1;
    char source_ip[INET6_ADDR_STRING_LENGTH + 1];
    char reserved2;
    char destination_node_type;
    char reserved3;
    char destination_ip[INET6_ADDR_STRING_LENGTH + 1];
    char reserved4;
    int32_t tv_sec;
    int32_t tv_usec;
    int32_t forwarder_tv_sec;
    int32_t forwarder_tv_usec;
    int32_t data_size;
    char reserved5;
    char reserved6;
    char reserved7;
    char reserved8;
};

struct spider_message
{
    spider_message_header header;
    char data[SPIDER_MESSAGE_DATA_MAX_SIZE];
};

struct route_data
{
    char ip[INET6_ADDR_STRING_LENGTH + 1];
    uint8_t metric;
};

struct upload_download_data_header
{
    char command[16];
    char file_name[256];
    char file_path[256];
    uint64_t file_size;
    uint64_t data_size;
};

struct upload_download_data
{
    upload_download_data_header header;
    char data[SHELL_UPLOAD_DOWNLOAD_DATA_SIZE];
};
#pragma pack(pop)

#endif /* SPIDER_H_ */

