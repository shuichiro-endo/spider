/*
 * Title:  spider (Linux static)
 * Author: Shuichiro Endo
 */

#ifndef SPIDER_H_
#define SPIDER_H_

//#define _DEBUG

#define NODE_BUFFER_SIZE 65535

#define ROUTING_MESSAGE_DATA_SIZE 60000
#define SOCKS5_MESSAGE_DATA_SIZE 60000

#define ROUTING_MESSAGE_QUEUE_CAPACITY 100
#define SOCKS5_MESSAGE_QUEUE_CAPACITY 100

#define KEYS_MAP_SIZE 200
#define DELETE_ROUTE_TIME 5 // 5s

#define FORWARDER_UDP_TIMEOUT 300

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
#include <iostream>
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
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ares.h>

#endif /* SPIDER_H_ */

