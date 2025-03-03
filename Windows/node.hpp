/*
 * Title:  spider node.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef NODE_H_
#define NODE_H_

#include "spider.hpp"


namespace spider
{
    class Messagemanager;
    class Socks5message;
    class Socks5messagequeue;

    class Node
    {
    private:

    protected:
        SOCKET sock = INVALID_SOCKET;
        std::shared_ptr<Messagemanager> message_manager;
        std::unique_ptr<Socks5messagequeue> socks5_messages_queue;

    public:

    private:

    protected:
        std::shared_ptr<Socks5message> pop_socks5_message();
        std::shared_ptr<Socks5message> pop_timeout_socks5_message(int32_t tv_sec,
                                                                  int32_t tv_usec);
    public:
        Node(SOCKET sock,
             std::shared_ptr<Messagemanager> message_manager);

        ~Node();

        void set_sock(SOCKET sock);
        SOCKET get_sock();

        void set_message_manager(std::shared_ptr<Messagemanager> messagemanager);
        std::shared_ptr<Messagemanager> get_message_manager();

        void push_socks5_message(std::shared_ptr<Socks5message> message);
        int32_t push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                            int32_t tv_sec,
                                            int32_t tv_usec);

        int32_t recv_data(char *buffer, int32_t buffer_length, long tv_sec, long tv_usec);
        int32_t send_data(char *buffer, int32_t buffer_length, long tv_sec, long tv_usec);

        uint32_t generate_random_id();

        void print_bytes(char *buffer, int buffer_length);

        int gettimeofday(timeval *tv, timezone *tz);
    };
}

#endif /* NODE_H_ */

