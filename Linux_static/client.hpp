/*
 * Title:  spider client.hpp (Linux static)
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
    class Caresmanager;

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
        std::shared_ptr<spider::Caresmanager> cares_manager;
        uint32_t recv_message_id = 0;
        uint32_t next_recv_message_id = 0;
        uint32_t send_message_id = 0;

    public:

    private:
        int32_t recv_message(char *buffer,
                             int32_t buffer_size,
                             int32_t tv_sec,
                             int32_t tv_usec,
                             bool register_server_id_flag);

        int32_t send_message(char *buffer,
                             int32_t data_size,
                             int32_t tv_sec,
                             int32_t tv_usec);

        int32_t forwarder_recv_data();

        int32_t forwarder_send_data();

        int32_t forwarder();

        uint64_t htonll(uint64_t x);

        uint64_t ntohll(uint64_t x);

        std::vector<std::string> split_input(const std::string& input);

        int32_t forwarder_shell_recv_data();

        int32_t forwarder_shell_send_data();

        int32_t forwarder_shell();

        int32_t forwarder_add_node(std::string config);

        int32_t forwarder_show_node();

        int32_t forwarder_show_route();

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
               int32_t client_sock,
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
               int32_t client_sock,
               int32_t tv_sec,
               int32_t tv_usec,
               int32_t forwarder_tv_sec,
               int32_t forwarder_tv_usec,
               std::shared_ptr<Encryption> encryption,
               std::shared_ptr<Messagemanager> message_manager,
               std::shared_ptr<spider::Caresmanager> cares_manager);

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

        int32_t do_socks5_connection_add_node(std::string config);

        int32_t do_socks5_connection_show_node();

        int32_t do_socks5_connection_show_route();

        int32_t do_socks5_connection_udp();
    };
}

#endif /* CLIENT_H_ */

