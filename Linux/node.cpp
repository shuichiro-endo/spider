/*
 * Title:  spider node.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "node.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "socks5messagequeue.hpp"


namespace spider
{
    Node::Node(int32_t sock,
              std::shared_ptr<Messagemanager> message_manager)
    {
        this->sock = sock;
        this->message_manager = message_manager;
        this->socks5_messages_queue = std::make_unique<Socks5messagequeue>();
    }

    Node::~Node()
    {

    }

    void Node::set_sock(int32_t sock)
    {
        this->sock = sock;
    }

    int32_t Node::get_sock()
    {
        return sock;
    }

    void Node::set_message_manager(std::shared_ptr<Messagemanager> messagemanager)
    {
        this->message_manager = message_manager;
    }

    std::shared_ptr<Messagemanager> Node::get_message_manager()
    {
        return message_manager;
    }

    void Node::push_socks5_message(std::shared_ptr<Socks5message> socks5_message)
    {

        socks5_messages_queue->push(socks5_message);

        return;
    }

    int32_t Node::push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                              int32_t tv_sec,
                                              int32_t tv_usec)
    {
        int ret = 0;

        ret = socks5_messages_queue->push_timeout(socks5_message,
                                                  tv_sec,
                                                  tv_usec);

        return ret;
    }

    std::shared_ptr<Socks5message> Node::pop_socks5_message()
    {
        std::shared_ptr<Socks5message> socks5_message = socks5_messages_queue->pop();

        return socks5_message;
    }

    std::shared_ptr<Socks5message> Node::pop_timeout_socks5_message(int32_t tv_sec,
                                                                    int32_t tv_usec)
    {
        std::shared_ptr<Socks5message> socks5_message = socks5_messages_queue->pop_timeout(tv_sec,
                                                                                           tv_usec);

        return socks5_message;
    }

    void Node::add_socks5_receive_message_to_map(uint32_t message_id,
                                                 std::shared_ptr<Socks5message> socks5_message)
    {
        std::unique_lock<std::mutex> lock(socks5_receive_messages_map_mutex);

        socks5_receive_messages_map.insert({message_id, socks5_message});

        lock.unlock();

        return;
    }

    std::shared_ptr<Socks5message> Node::get_socks5_receive_message_from_map(uint32_t message_id)
    {
        std::shared_ptr<Socks5message> socks5_message = nullptr;
        std::size_t check_key_count = 0;

        std::unique_lock<std::mutex> lock(socks5_receive_messages_map_mutex);

        check_key_count = socks5_receive_messages_map.count(message_id);
        if(check_key_count > 0)
        {
            socks5_message = socks5_receive_messages_map[message_id];
            socks5_receive_messages_map.erase(message_id);
        }

        lock.unlock();

        return socks5_message;
    }

    int32_t Node::recv_data(char *buffer,
                            int32_t buffer_size,
                            long tv_sec,
                            long tv_usec)
    {
        int ret = 0;
        int32_t rec = 0;
        fd_set readfds;
        int nfds = -1;
        struct timeval tv;


        if(buffer_size > NODE_BUFFER_SIZE)
        {
#ifdef _DEBUG
            std::printf("[-] buffer size error\n");
#endif
            return -1;
        }

        std::memset((char *)buffer,
                    0,
                    buffer_size);

        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock,
                   &readfds);
            nfds = sock + 1;
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(nfds,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[-] recv_data select timeout\n");
#endif
                return -1;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
                rec = recv(sock,
                           buffer,
                           NODE_BUFFER_SIZE,
                           0);
                if(rec <= 0)
                {
                    if(errno == EINTR)
                    {
                        continue;
                    }else if(errno == EAGAIN)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef _DEBUG
                        std::printf("[-] recv error: %d\n",
                                    errno);
#endif
                        return -1;
                    }
                }else
                {
                    break;
                }
            }
        }

        return rec;
    }

    int32_t Node::send_data(char *buffer,
                            int32_t data_size,
                            long tv_sec,
                            long tv_usec)
    {
        int ret = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t len = 0;
        fd_set writefds;
        int nfds = -1;
        struct timeval tv;


        if(data_size > NODE_BUFFER_SIZE)
        {
#ifdef _DEBUG
            std::printf("[-] data size error\n");
#endif
            return -1;
        }

        len = data_size;

        while(len > 0)
        {
            FD_ZERO(&writefds);
            FD_SET(sock,
                   &writefds);
            nfds = sock + 1;
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(nfds,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0){
#ifdef _DEBUG
                std::printf("[-] send_data select timeout\n");
#endif
                return -1;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret)
            {
                sen = send(sock,
                           buffer+send_length,
                           len,
                           MSG_NOSIGNAL);
                if(sen <= 0)
                {
                    if(errno == EINTR)
                    {
                        continue;
                    }else if(errno == EAGAIN)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef _DEBUG
                        std::printf("[-] send error: %d\n",
                                    errno);
#endif
                        return -1;
                    }
                }
                send_length += sen;
                len -= sen;
            }
        }

        return data_size;
    }

    uint32_t Node::generate_random_id()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<uint32_t> distrib(1, UINT32_MAX);

        return distrib(mt);
    }

    void Node::print_bytes(char *buffer, int buffer_length)
    {
        for(int i=0; i<buffer_length; i++){
            if(i != 0 && i%16 == 0){
                std::printf("\n");
            }else if(i%16 == 8){
                std::printf(" ");
            }
            std::printf("%02x ", buffer[i] & 0xff);
        }
        std::printf("\n");

        return;
    }
}

