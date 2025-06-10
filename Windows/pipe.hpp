/*
 * Title:  spider pipe.hpp (Windows)
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
        CtxtHandle *ctxt_handle;
        SecPkgContext_StreamSizes stream_sizes;
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
             SOCKET pipe_sock,
             std::shared_ptr<Routingmanager> routing_manager,
             std::shared_ptr<Messagemanager> message_manager);

        Pipe(std::shared_ptr<Spiderip> spider_ip,
             uint32_t pipe_id,
             char mode,
             char message_mode,
             std::string pipe_ip,
             std::string pipe_ip_scope_id,
             std::string pipe_listen_port,
             SOCKET pipe_sock,
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

        void set_ctxt_handle(CtxtHandle *ctxt_handle);
        CtxtHandle *get_ctxt_handle();

        void set_stream_sizes(SecPkgContext_StreamSizes stream_sizes);
        SecPkgContext_StreamSizes get_stream_sizes();

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
        int32_t encrypt_data_tls(char *input,
                                 int32_t input_length,
                                 char *output,
                                 int32_t output_length);
        int32_t decrypt_data_tls(char *input,
                                 int32_t *input_length,
                                 char *output,
                                 int32_t output_length,
                                 int32_t *decrypted_data_length,
                                 int32_t *remaining_size);
        int32_t do_http_connection_client();
        int32_t do_http_connection_server();
    };
}

#endif /* PIPE_H_ */

