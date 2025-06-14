/*
 * Title:  spider pipe.hpp (Linux static)
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
        char message_mode;                  // default:d http:h https:s
        BIO *bio_tls = NULL;
        SSL *ssl = NULL;
        std::string pipe_ip;
        std::string pipe_ip_scope_id;
        std::string pipe_listen_port;
        std::string pipe_destination_ip;
        std::string pipe_destination_ip_scope_id;
        std::string pipe_destination_port;
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
             char message_mode,
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
             char message_mode,
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

        void set_message_mode(char message_mode);
        char get_message_mode();

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

        void push_routing_message(std::shared_ptr<Routingmessage> routing_message);

        int32_t recv_message();
        int32_t send_routing_message();
        int32_t send_socks5_message();

        int32_t get_http_header_length(const char* buffer);
        int32_t get_content_length(const char* buffer);
        int32_t do_http_connection_client();
        int32_t do_http_connection_server();
    };
}

#endif /* PIPE_H_ */

