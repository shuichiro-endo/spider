/*
 * Title:  spider (Linux)
 * Author: Shuichiro Endo
 */

#ifndef SPIDER_H_
#define SPIDER_H_

//#define _DEBUG

#define NODE_BUFFER_SIZE 65535

#define ROUTING_MESSAGE_DATA_SIZE 60000
#define SOCKS5_MESSAGE_DATA_SIZE 60000
#define SHELL_UPLOAD_DOWNLOAD_DATA_SIZE 50000
#define SHELL_RESULT_DATA_SIZE 50000

#define ROUTING_MESSAGE_QUEUE_CAPACITY 100
#define SOCKS5_MESSAGE_QUEUE_CAPACITY 100

#define KEYS_MAP_SIZE 200
#define METRIC_MAX 20   // 0 < METRIC_MAX <= UINT8_MAX(255), UINT8_MAX(255) < delete route
#define DELETE_ROUTE_TIME 5 // 5s

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
#include <csignal>
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
#include <mutex>
#include <queue>
#include <semaphore>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <limits.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


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
    char command[16];
    char file_name[256];
    char file_path[256];
    uint64_t file_size;
    uint64_t data_size;
    char data[SHELL_UPLOAD_DOWNLOAD_DATA_SIZE];
};

#endif /* SPIDER_H_ */

