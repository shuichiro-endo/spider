/*
 * Title:  spider pipe.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spiderip.hpp"
#include "pipe.hpp"
#include "message.hpp"
#include "routingmessage.hpp"
#include "socks5message.hpp"
#include "routingmessagequeue.hpp"
#include "socks5messagequeue.hpp"
#include "routingmanager.hpp"
#include "messagemanager.hpp"


namespace spider
{
    Pipe::Pipe(std::shared_ptr<Spiderip> spider_ip,
               uint32_t pipe_id,
               char mode,
               std::string pipe_ip,
               std::string pipe_ip_scope_id,
               std::string pipe_destination_ip,
               std::string pipe_destination_ip_scope_id,
               std::string pipe_destination_port,
               int32_t pipe_sock,
               std::shared_ptr<Routingmanager> routing_manager,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_ip_scope_id = pipe_ip_scope_id;
        this->pipe_destination_ip = pipe_destination_ip;
        this->pipe_destination_ip_scope_id = pipe_destination_ip_scope_id;
        this->pipe_destination_port = pipe_destination_port;
        this->routing_manager = routing_manager;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
    }

    Pipe::Pipe(std::shared_ptr<Spiderip> spider_ip,
               uint32_t pipe_id,
               char mode,
               std::string pipe_ip,
               std::string pipe_ip_scope_id,
               std::string pipe_listen_port,
               int32_t pipe_sock,
               std::shared_ptr<Routingmanager> routing_manager,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_ip_scope_id = pipe_ip_scope_id;
        this->pipe_listen_port = pipe_listen_port;
        this->routing_manager = routing_manager;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
    }

    Pipe::~Pipe()
    {

    }

    void Pipe::set_spider_ip(std::shared_ptr<Spiderip> spider_ip)
    {
        this->spider_ip = spider_ip;
    }

    std::shared_ptr<Spiderip> Pipe::get_spider_ip()
    {
        return spider_ip;
    }

    void Pipe::set_pipe_id(uint32_t pipe_id)
    {
        this->pipe_id = pipe_id;
    }

    uint32_t Pipe::get_pipe_id()
    {
        return pipe_id;
    }

    void Pipe::set_mode(char mode)
    {
        this->mode = mode;
    }

    char Pipe::get_mode()
    {
        return mode;
    }

    void Pipe::set_pipe_ip(std::string pipe_ip)
    {
        this->pipe_ip = pipe_ip;
    }

    std::string Pipe::get_pipe_ip()
    {
        return pipe_ip;
    }

    void Pipe::set_pipe_ip_scope_id(std::string pipe_ip_scope_id)
    {
        this->pipe_ip_scope_id = pipe_ip_scope_id;
    }

    std::string Pipe::get_pipe_ip_scope_id()
    {
        return pipe_ip_scope_id;
    }

    void Pipe::set_pipe_listen_port(std::string pipe_listen_port)
    {
        this->pipe_listen_port = pipe_listen_port;
    }

    std::string Pipe::get_pipe_listen_port()
    {
        return pipe_listen_port;
    }

    void Pipe::set_pipe_destination_ip(std::string pipe_destination_ip)
    {
        this->pipe_destination_ip = pipe_destination_ip;
    }

    std::string Pipe::get_pipe_destination_ip()
    {
        return pipe_destination_ip;
    }

    void Pipe::set_pipe_destination_ip_scope_id(std::string pipe_destination_ip_scope_id)
    {
        this->pipe_destination_ip_scope_id = pipe_destination_ip_scope_id;
    }

    std::string Pipe::get_pipe_destination_ip_scope_id()
    {
        return pipe_destination_ip_scope_id;
    }

    void Pipe::set_pipe_destination_port(std::string pipe_destination_port)
    {
        this->pipe_destination_port = pipe_destination_port;
    }

    std::string Pipe::get_pipe_destination_port()
    {
        return pipe_destination_port;
    }

    void Pipe::push_routing_message(std::shared_ptr<Routingmessage> routing_message)
    {
        routing_messages_queue->push(routing_message);
        return;
    }

    std::shared_ptr<Routingmessage> Pipe::pop_routing_message()
    {
        std::shared_ptr<Routingmessage> routing_message = routing_messages_queue->pop();

        return routing_message;
    }

    int32_t Pipe::recv_message()
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t tmprec = 0;
        fd_set readfds;
        int nfds = -1;
        struct timeval tv;
        long tv_sec = 3600;
        long tv_usec = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                   sizeof(char));

        std::shared_ptr<Routingmessage> routing_message;
        std::shared_ptr<Socks5message> socks5_message;
        struct spider_message *spider_message;
        int32_t spider_message_header_size = sizeof(struct spider_message_header);
        bool recv_header_flag = false;
        int32_t recv_data_size = 0;
        int32_t remaining_size = 0;

        std::shared_ptr<Pipe> pipe;
        char destination_node_type;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        uint32_t server_id = 0;
        uint32_t pipe_id = 0;


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
                std::printf("[+] recv_message select timeout\n");
#endif
                free(tmp);
                free(buffer);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret > 0)
            {
                if(recv_header_flag == false)
                {
                    tmprec = recv(sock,
                                  tmp,
                                  spider_message_header_size,
                                  0);
                }else
                {
                    tmprec = recv(sock,
                                  tmp,
                                  remaining_size,
                                  0);
                }

                if(tmprec <= 0)
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
                        std::printf("[-] recv_message recv error: %d\n",
                               errno);
#endif
                        free(tmp);
                        free(buffer);
                        return -1;
                    }
                }else
                {
                    std::memcpy(buffer + rec,
                                tmp,
                                tmprec);
                    rec += tmprec;

                    std::memset(tmp,
                                0,
                                NODE_BUFFER_SIZE);
                    tmprec = 0;

#ifdef _DEBUG
//                    std::printf("rec: %d\n", rec);
//                    print_bytes(buffer, rec);
#endif

                    if(recv_header_flag == false)
                    {
                        if(rec < spider_message_header_size)
                        {
                            remaining_size = spider_message_header_size - rec;
                            continue;
                        }else
                        {
                            recv_header_flag = true;

                            spider_message = (struct spider_message *)buffer;

                            recv_data_size = ntohl(spider_message->header.data_size);

                            remaining_size = recv_data_size;
                            if(remaining_size > 0)
                            {
                                continue;
                            }
                        }
                    }

                    if(recv_header_flag == true)
                    {
                        if(rec < spider_message_header_size + recv_data_size)
                        {
                            remaining_size = spider_message_header_size + recv_data_size - rec;
                            continue;
                        }else
                        {
                            spider_message = (struct spider_message *)buffer;
                            if(spider_message->header.message_type == 'r')
                            {
                                routing_message = std::make_shared<Routingmessage>(this->get_pipe_id(),
                                                                                   spider_message);

                                std::thread thread_message_manager(&Messagemanager::push_routing_message,
                                                                   message_manager,
                                                                   routing_message);
                                thread_message_manager.detach();
                            }else if(spider_message->header.message_type  == 's')
                            {
                                socks5_message = std::make_shared<Socks5message>(spider_message);
                                if(spider_ip->get_spider_ipv4() == socks5_message->get_destination_ip() ||
                                   spider_ip->get_spider_ipv6_global() == socks5_message->get_destination_ip() ||
                                   spider_ip->get_spider_ipv6_unique_local() == socks5_message->get_destination_ip() ||
                                   spider_ip->get_spider_ipv6_link_local() == socks5_message->get_destination_ip())
                                {
                                    std::thread thread_message_manager(&Messagemanager::push_socks5_message,
                                                                       message_manager,
                                                                       socks5_message);
                                    thread_message_manager.detach();
                                }else
                                {
                                    pipe = routing_manager->get_destination_pipe(socks5_message->get_destination_ip());
                                    if(pipe != nullptr)
                                    {
                                        std::thread thread_pipe(&Pipe::push_socks5_message,
                                                                pipe,
                                                                socks5_message);
                                        thread_pipe.detach();
                                    }else
                                    {
#ifdef _DEBUG
                                        std::printf("[-] cannot transfer pipe message\n");
#endif
                                        free(tmp);
                                        free(buffer);
                                        return 0;
                                    }
                                }
                            }else
                            {
#ifdef _DEBUG
                                std::printf("[-] recv_message message type error: %c\n",
                                            spider_message->header.message_type);
#endif
                                free(tmp);
                                free(buffer);
                                return 0;
                            }
                        }
                    }

                    break;
                }
            }
        }

        free(tmp);
        free(buffer);
        return rec;
    }

    int32_t Pipe::send_routing_message()
    {
        int ret = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t length = 0;
        int32_t len = 0;
        fd_set writefds;
        int nfds = -1;
        struct timeval tv;
        long tv_sec = 3600;
        long tv_usec = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));

        std::shared_ptr<Routingmessage> routing_message;
        routing_message = pop_routing_message();
        if(routing_message == nullptr){
            free(buffer);
            return 0;
        }
        length = routing_message->copy_to_buffer(buffer);
        len = length;

#ifdef _DEBUG
//        std::printf("len: %d\n", len);
//        print_bytes(buffer, len);
#endif


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
                std::printf("[I] send_routing_message select timeout\n");
#endif
                free(buffer);
                return 0;
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
                        std::printf("[-] send_routing_message send error: %d\n",
                                    errno);
#endif
                        free(buffer);
                        return -1;
                    }
                }
                send_length += sen;
                len -= sen;
            }
        }

        free(buffer);
        return length;
    }

    int32_t Pipe::send_socks5_message()
    {
        int ret = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t length = 0;
        int32_t len = 0;
        fd_set writefds;
        int nfds = -1;
        struct timeval tv;
        long tv_sec = 3600;
        long tv_usec = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));

        std::shared_ptr<Socks5message> socks5_message;
        socks5_message = pop_socks5_message();
        length = socks5_message->copy_to_buffer(buffer);
        len = length;

#ifdef _DEBUG
//        std::printf("len: %d\n", len);
//        print_bytes(buffer, len);
#endif


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
                std::printf("[I] send_socks5_message select timeout\n");
#endif
                free(buffer);
                return 0;
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
                        std::printf("[-] send_socks5_message send error: %d\n",
                                    errno);
#endif
                        free(buffer);
                        return -1;
                    }
                }
                send_length += sen;
                len -= sen;
            }
        }

        free(buffer);
        return length;
    }
}
