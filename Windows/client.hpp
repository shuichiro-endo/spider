/*
 * Title:  spider client.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "spider.hpp"
#include "node.hpp"


namespace spider
{
    class Encryption;
    class Messagemanager;

    class Client : public Node
    {
    private:
        std::string type;
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        std::string client_ip;
        std::string client_ip_scope_id;
        std::string client_listen_port;
        std::string client_port;
        std::string destination_spider_ip;
        std::string target_ip;
        std::string target_port;
        int32_t tv_sec;
        int32_t tv_usec;
        int32_t forwarder_tv_sec;
        int32_t forwarder_tv_usec;
        std::shared_ptr<Encryption> encryption;
        uint32_t recv_message_id = 0;
        uint32_t next_recv_message_id = 0;
        uint32_t send_message_id = 0;

    public:

    private:
        int32_t recv_message(char *buffer,
                             int32_t buffer_size,
                             int32_t tv_sec,
                             int32_t tv_usec,
                             BOOL register_server_id_flag);

        int32_t send_message(char *buffer,
                             int32_t data_size,
                             int32_t tv_sec,
                             int32_t tv_usec);

        int32_t forwarder_recv_data();

        int32_t forwarder_send_data();

        int32_t forwarder();

        int32_t forwarder_udp_recv_send_data(int addr_length);

        int32_t forwarder_udp(int addr_length);

    public:
        Client(std::string type,
               uint32_t connection_id,
               uint32_t client_id,
               uint32_t server_id,
               std::string client_ip,
               std::string client_ip_scope_id,
               std::string client_listen_port,
               std::string client_port,
               std::string destination_spider_ip,
               SOCKET client_sock,
               int32_t tv_sec,
               int32_t tv_usec,
               int32_t forwarder_tv_sec,
               int32_t forwarder_tv_usec,
               std::shared_ptr<Encryption> encryption,
               std::shared_ptr<Messagemanager> message_manager);

        Client(std::string type,
               uint32_t connection_id,
               uint32_t client_id,
               uint32_t server_id,
               std::string client_ip,
               std::string client_ip_scope_id,
               std::string client_listen_port,
               std::string client_port,
               std::string destination_spider_ip,
               std::string target_ip,
               std::string target_port,
               SOCKET client_sock,
               int32_t tv_sec,
               int32_t tv_usec,
               int32_t forwarder_tv_sec,
               int32_t forwarder_tv_usec,
               std::shared_ptr<Encryption> encryption,
               std::shared_ptr<Messagemanager> message_manager);

        ~Client();

        void set_type(std::string type);
        std::string get_type();

        void set_connection_id(uint32_t connection_id);
        uint32_t get_connection_id();

        void set_client_id(uint32_t client_id);
        uint32_t get_client_id();

        void set_server_id(uint32_t server_id);
        uint32_t get_server_id();

        void set_client_ip(std::string client_ip);
        std::string get_client_ip();

        void set_client_ip_scope_id(std::string client_ip_scope_id);
        std::string get_client_ip_scope_id();

        void set_client_listen_port(std::string client_listen_port);
        std::string get_client_listen_port();

        void set_client_port(std::string client_port);
        std::string get_client_port();

        void set_destination_spider_ip(std::string destination_spider_ip);
        std::string get_destination_spider_ip();

        void set_target_ip(std::string target_ip);
        std::string get_target_ip();

        void set_target_port(std::string target_port);
        std::string get_target_port();

        void set_tv_sec(int32_t tv_sec);
        int32_t get_tv_sec();

        void set_tv_usec(int32_t tv_usec);
        int32_t get_tv_usec();

        void set_forwarder_tv_sec(int32_t forwarder_tv_sec);
        int32_t get_forwarder_tv_sec();

        void set_forwarder_tv_usec(int32_t forwarder_tv_usec);
        int32_t get_forwarder_tv_usec();

        void set_encryption(std::shared_ptr<Encryption> encryption);
        std::shared_ptr<Encryption> get_encryption();

        int32_t do_socks5_connection();

        int32_t do_socks5_connection_shell();

        int32_t do_socks5_connection_udp();
    };
}

#endif /* CLIENT_H_ */

