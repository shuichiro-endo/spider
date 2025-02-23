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
    class Messagemanager;
    class Routingmessage;
    class Routingmessagequeue;

    class Pipe : public Node
    {
    private:
        uint32_t pipe_id;
        char mode;                          // client:c server:s
        std::string pipe_ip;
        std::string pipe_ip_scope_id;
        std::string pipe_listen_port;
        std::string pipe_destination_ip;
        std::string pipe_destination_ip_scope_id;
        std::string pipe_destination_port;
        std::unique_ptr<Routingmessagequeue> routing_messages_queue;

    public:

    private:
        std::shared_ptr<Routingmessage> pop_routing_message();

    public:
        Pipe(uint32_t pipe_id,
             char mode,
             std::string pipe_ip,
             std::string pipe_ip_scope_id,
             std::string pipe_destination_ip,
             std::string pipe_destination_ip_scope_id,
             std::string pipe_destination_port,
             int32_t pipe_sock,
             std::shared_ptr<Messagemanager> message_manager);

        Pipe(uint32_t pipe_id,
             char mode,
             std::string pipe_ip,
             std::string pipe_ip_scope_id,
             std::string pipe_listen_port,
             int32_t pipe_sock,
             std::shared_ptr<Messagemanager> message_manager);

        ~Pipe();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        void set_mode(char mode);
        char get_mode();

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
    };
}

#endif /* PIPE_H_ */

