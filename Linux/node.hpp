/*
 * Title:  spider node.hpp (Linux)
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
        int32_t sock = 0;
        std::shared_ptr<Messagemanager> message_manager;
        std::unique_ptr<Socks5messagequeue> socks5_messages_queue;
        std::map<uint32_t, std::shared_ptr<Socks5message>> socks5_receive_messages_map;
        std::mutex socks5_receive_messages_map_mutex;

    public:

    private:

    protected:
        std::shared_ptr<Socks5message> pop_socks5_message();
        std::shared_ptr<Socks5message> pop_timeout_socks5_message(int32_t tv_sec,
                                                                  int32_t tv_usec);

        std::shared_ptr<Socks5message> get_socks5_receive_message_from_map(uint32_t message_id);

    public:
        Node(int32_t sock,
             std::shared_ptr<Messagemanager> message_manager);

        ~Node();

        void set_sock(int32_t sock);
        int32_t get_sock();

        void set_message_manager(std::shared_ptr<Messagemanager> messagemanager);
        std::shared_ptr<Messagemanager> get_message_manager();

        void push_socks5_message(std::shared_ptr<Socks5message> socks5_message);
        int32_t push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                            int32_t tv_sec,
                                            int32_t tv_usec);

        void add_socks5_receive_message_to_map(uint32_t message_id,
                                               std::shared_ptr<Socks5message> socks5_message);

        int32_t recv_data(char *buffer,
                          int32_t buffer_size,
                          long tv_sec,
                          long tv_usec);

        int32_t send_data(char *buffer,
                          int32_t data_size,
                          long tv_sec,
                          long tv_usec);

        uint32_t generate_random_id();

        void print_bytes(char *buffer, int buffer_length);
    };
}

#endif /* NODE_H_ */

