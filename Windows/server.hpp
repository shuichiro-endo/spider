/*
 * Title:  spider server.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "spider.hpp"
#include "node.hpp"


namespace spider
{
    class Spiderip;
    class Encryption;
    class Messagemanager;

    class Server : public Node
    {
    private:
        std::shared_ptr<Spiderip> spider_ip;
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        std::string server_ip;
        std::string server_port;
        std::string client_destination_ip;
        std::string target_ip;
        std::string target_port;
        SOCKET target_sock;
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

        std::string execute_command(const std::string &command);

        std::vector<std::string> split_input(const std::string &input);

        int32_t forwarder_shell();

        int32_t forwarder_udp_recv_send_data(struct sockaddr *target_addr,
                                             int target_addr_lengthh);

        int32_t forwarder_udp(struct sockaddr *target_addr,
                              int target_addr_lengthh);

    public:
        Server(std::shared_ptr<Spiderip> spider_ip,
               uint32_t connection_id,
               uint32_t client_id,
               uint32_t server_id,
               std::string server_ip,
               std::string server_port,
               std::string client_destination_ip,
               SOCKET server_sock,
               int32_t tv_sec,
               int32_t tv_usec,
               int32_t forwarder_tv_sec,
               int32_t forwarder_tv_usec,
               std::shared_ptr<Encryption> encryption,
               std::shared_ptr<Messagemanager> message_manager);

        ~Server();

        void set_spider_ip(std::shared_ptr<Spiderip> spider_ip);
        std::shared_ptr<Spiderip> get_spider_ip();

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

        void set_target_sock(SOCKET target_sock);
        SOCKET get_target_sock();

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

        int32_t do_socks5_connection(std::shared_ptr<Socks5message> socks5_message);
    };
}

#endif /* SERVER_H_ */

