/*
 * Title:  spider server.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <cstring>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "spider.hpp"
#include "node.hpp"


namespace spider
{
    class Messagemanager;

    class Server : public Node
    {
    private:
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        std::string server_ip;
        std::string server_port;
        std::string client_destination_ip;
        std::string target_ip;
        std::string target_port;
        int32_t target_sock;
        int32_t tv_sec;
        int32_t tv_usec;
        int32_t forwarder_tv_sec;
        int32_t forwarder_tv_usec;
        bool xor_flag;
        std::string xor_key_hex_string;

    public:

    private:
        char hex_char_to_int(char c);

        int hex_string_to_array(const char *hex_string,
                                int hex_string_length,
                                unsigned char *output,
                                int output_size);

        int32_t encrypt_xor(char *buffer,
                            int32_t data_size);

        int32_t recv_message(char *buffer,
                             int32_t buffer_size,
                             int32_t tv_sec,
                             int32_t tv_usec);

        int32_t send_message(char *buffer,
                             int32_t data_size,
                             int32_t tv_sec,
                             int32_t tv_usec);

        int32_t forwarder_recv_data();

        int32_t forwarder_send_data();

        int32_t forwarder();

        int32_t send_socks_response_ipv4(char *buffer,
                                         int32_t buffer_size,
                                         char ver,
                                         char req,
                                         char rsv,
                                         char atyp);

        int32_t send_socks_response_ipv6(char *buffer,
                                         int32_t buffer_size,
                                         char ver,
                                         char req,
                                         char rsv,
                                         char atyp);

    public:
        Server(uint32_t connection_id,
               uint32_t client_id,
               uint32_t server_id,
               std::string server_ip,
               std::string server_port,
               std::string client_destination_ip,
               int32_t server_sock,
               int32_t tv_sec,
               int32_t tv_usec,
               int32_t forwarder_tv_sec,
               int32_t forwarder_tv_usec,
               bool xor_flag,
               std::string xor_key_hex_string,
               std::shared_ptr<Messagemanager> message_manager);

        ~Server();

        void set_connection_id(uint32_t connection_id);
        uint32_t get_connection_id();

        void set_client_id(uint32_t client_id);
        uint32_t get_client_id();

        void set_server_id(uint32_t server_id);
        uint32_t get_server_id();

        void set_server_ip(std::string server_ip);
        std::string get_server_ip();

        void set_server_port(std::string server_port);
        std::string get_server_port();

        void set_client_destination_ip(std::string client_destination_ip);
        std::string get_client_destination_ip();

        void set_target_ip(std::string target_ip);
        std::string get_target_ip();

        void set_target_port(std::string target_port);
        std::string get_target_port();

        void set_target_sock(int32_t target_sock);
        int32_t get_target_sock();

        void set_tv_sec(int32_t tv_sec);
        int32_t get_tv_sec();

        void set_tv_usec(int32_t tv_usec);
        int32_t get_tv_usec();

        void set_forwarder_tv_sec(int32_t forwarder_tv_sec);
        int32_t get_forwarder_tv_sec();

        void set_forwarder_tv_usec(int32_t forwarder_tv_usec);
        int32_t get_forwarder_tv_usec();

        void set_xor_flag(bool xor_flag);
        bool get_xor_flag();

        void set_xor_key_hex_string(std::string xor_key_hex_string);
        std::string get_xor_key_hex_string();

        int32_t do_socks5_connection(std::shared_ptr<Socks5message> socks5_message);
    };
}

#endif /* SERVER_H_ */

