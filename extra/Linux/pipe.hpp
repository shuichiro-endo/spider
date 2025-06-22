/*
 * Title:  spider pipe.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef PIPE_H_
#define PIPE_H_

#include "spider.hpp"
#include "node.hpp"


namespace spider
{
    class Spiderip;
    class Routingmanager;
    class Messagemanager;
    class Routingmessage;
    class Routingmessagequeue;

    class Pipe : public Node
    {
    private:
        std::shared_ptr<Spiderip> spider_ip;
        uint32_t pipe_id;
        char mode;                          // client:c server:s
        std::string message_mode;           // default:d http:h https:s
        BIO *bio_tls = NULL;
        SSL *ssl = NULL;
        std::string pipe_ip;
        std::string pipe_ip_scope_id;
        std::string pipe_listen_port;
        std::string pipe_destination_ip;
        std::string pipe_destination_ip_scope_id;
        std::string pipe_destination_port;
        std::string forward_proxy_type;
        int32_t forward_proxy_authentication;
        std::string forward_proxy_ip;
        std::string forward_proxy_ip_scope_id;
        std::string forward_proxy_port;
        std::string forward_proxy_username;
        std::string forward_proxy_password;
        std::string forward_proxy_nthash_hexstring;
        std::string forward_proxy_domainname;
        std::string forward_proxy_workstationname;
        std::string forward_proxy_service_principal_name;
        std::shared_ptr<Routingmanager> routing_manager;
        std::unique_ptr<Routingmessagequeue> routing_messages_queue;

    public:

    private:
        std::shared_ptr<Routingmessage> pop_routing_message();
        std::shared_ptr<Routingmessage> pop_latest_routing_message();

    public:
        Pipe(std::shared_ptr<Spiderip> spider_ip,
             uint32_t pipe_id,
             char mode,
             std::string message_mode,
             std::string pipe_ip,
             std::string pipe_ip_scope_id,
             std::string pipe_destination_ip,
             std::string pipe_destination_ip_scope_id,
             std::string pipe_destination_port,
             int32_t pipe_sock,
             std::shared_ptr<Routingmanager> routing_manager,
             std::shared_ptr<Messagemanager> message_manager);

        Pipe(std::shared_ptr<Spiderip> spider_ip,
             uint32_t pipe_id,
             char mode,
             std::string message_mode,
             std::string pipe_ip,
             std::string pipe_ip_scope_id,
             std::string pipe_listen_port,
             int32_t pipe_sock,
             std::shared_ptr<Routingmanager> routing_manager,
             std::shared_ptr<Messagemanager> message_manager);

        ~Pipe();

        void set_spider_ip(std::shared_ptr<Spiderip> spider_ip);
        std::shared_ptr<Spiderip> get_spider_ip();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        void set_mode(char mode);
        char get_mode();

        void set_message_mode(std::string message_mode);
        std::string get_message_mode();

        void set_bio_tls(BIO *bio_tls);
        BIO *get_bio_tls();

        void set_ssl(SSL *ssl);
        SSL *get_ssl();

        void set_pipe_ip(std::string pipe_ip);
        std::string get_pipe_ip();

        void set_pipe_ip_scope_id(std::string pipe_ip_scope_id);
        std::string get_pipe_ip_scope_id();

        void set_pipe_listen_port(std::string pipe_listen_port);
        std::string get_pipe_listen_port();

        void set_pipe_destination_ip(std::string pipe_destination_ip);
        std::string get_pipe_destination_ip();

        void set_pipe_destination_ip_scope_id(std::string pipe_destination_ip_scope_id);
        std::string get_pipe_destination_ip_scope_id();

        void set_pipe_destination_port(std::string pipe_destination_port);
        std::string get_pipe_destination_port();

        void set_forward_proxy_type(std::string forward_proxy_type);
        std::string get_forward_proxy_type();

        void set_forward_proxy_authentication(int32_t forward_proxy_authentication);
        int32_t get_forward_proxy_authentication();

        void set_forward_proxy_ip(std::string forward_proxy_ip);
        std::string get_forward_proxy_ip();

        void set_forward_proxy_ip_scope_id(std::string forward_proxy_ip_scope_id);
        std::string get_forward_proxy_ip_scope_id();

        void set_forward_proxy_port(std::string forward_proxy_port);
        std::string get_forward_proxy_port();

        void set_forward_proxy_username(std::string forward_proxy_username);
        std::string get_forward_proxy_username();

        void set_forward_proxy_password(std::string forward_proxy_password);
        std::string get_forward_proxy_password();

        void set_forward_proxy_nthash_hexstring(std::string forward_proxy_nthash_hexstring);
        std::string get_forward_proxy_nthash_hexstring();

        void set_forward_proxy_domainname(std::string forward_proxy_domainname);
        std::string get_forward_proxy_domainname();

        void set_forward_proxy_workstationname(std::string forward_proxy_workstationname);
        std::string get_forward_proxy_workstationname();

        void set_forward_proxy_service_principal_name(std::string forward_proxy_service_principal_name);
        std::string get_forward_proxy_service_principal_name();

        void push_routing_message(std::shared_ptr<Routingmessage> routing_message);

        int32_t recv_message();
        int32_t send_routing_message();
        int32_t send_socks5_message();

        int32_t recv_data_tls(char *buffer,
                              int32_t buffer_size,
                              long tv_sec,
                              long tv_usec);

        int32_t send_data_tls(char *buffer,
                              int32_t data_size,
                              long tv_sec,
                              long tv_usec);

        int32_t get_http_header_length(const char* buffer);
        int32_t get_content_length(const char* buffer);
        int32_t do_http_connection_client();
        int32_t do_http_connection_server();
    };
}

#endif /* PIPE_H_ */

