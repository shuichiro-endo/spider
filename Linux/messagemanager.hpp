/*
 * Title:  spider messagemanager.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include "spider.hpp"


namespace spider
{
    class Clientmanager;
    class Servermanager;
    class Pipemanager;
    class Routingmanager;
    class Routingmessage;
    class Socks5message;
    class Routingmessagequeue;
    class Socks5messagequeue;

    class Messagemanager
    {
    private:
        std::string spider_ip;
        std::shared_ptr<Clientmanager> client_manager;
        std::shared_ptr<Servermanager> server_manager;
        std::shared_ptr<Pipemanager> pipe_manager;
        std::shared_ptr<Routingmanager> routing_manager;
        std::unique_ptr<Routingmessagequeue> routing_messages_queue;
        std::unique_ptr<Socks5messagequeue> socks5_messages_queue;

    public:

    private:
        std::shared_ptr<Routingmessage> pop_routing_message();
        std::shared_ptr<Routingmessage> pop_timeout_routing_message(int32_t tv_sec,
                                                                    int32_t tv_usec);
        std::shared_ptr<Socks5message> pop_socks5_message();
        std::shared_ptr<Socks5message> pop_tieout_socks5_message(int32_t tv_sec,
                                                                 int32_t tv_usec);

    public:
        Messagemanager(std::string spider_ip,
                       std::shared_ptr<Clientmanager> client_manager,
                       std::shared_ptr<Servermanager> server_manager,
                       std::shared_ptr<Pipemanager> pipe_manager,
                       std::shared_ptr<Routingmanager> routing_manager);

        ~Messagemanager();

        void push_routing_message(std::shared_ptr<Routingmessage> routing_message);
        int32_t push_timeout_routing_message(std::shared_ptr<Routingmessage> routing_message,
                                             int32_t tv_sec,
                                             int32_t tv_usec);

        void push_socks5_message(std::shared_ptr<Socks5message> socks5_message);
        int32_t push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                            int32_t tv_sec,
                                            int32_t tv_usec);

        void transfer_routing_message();
        std::shared_ptr<Socks5message> transfer_socks5_message();
    };
}

#endif /* MESSAGEMANAGER_H_ */

