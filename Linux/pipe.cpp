/*
 * Title:  spider pipe.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "pipe.hpp"
#include "message.hpp"
#include "routingmessage.hpp"
#include "socks5message.hpp"
#include "routingmessagequeue.hpp"
#include "socks5messagequeue.hpp"
#include "messagemanager.hpp"


namespace spider
{
    Pipe::Pipe(uint32_t pipe_id,
               char mode,
               std::string pipe_ip,
               std::string pipe_destination_ip,
               std::string pipe_destination_port,
               int32_t pipe_sock,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_destination_ip = pipe_destination_ip;
        this->pipe_destination_port = pipe_destination_port;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
    }

    Pipe::Pipe(uint32_t pipe_id,
               char mode,
               std::string pipe_ip,
               std::string pipe_listen_port,
               int32_t pipe_sock,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_listen_port = pipe_listen_port;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
    }

    Pipe::~Pipe()
    {

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
        struct timeval start;
        struct timeval end;
        long tv_sec = 3;
        long tv_usec = 0;
        long t = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                   sizeof(char));

        std::shared_ptr<Routingmessage> routing_message;
        struct routing_message_data *routing_message_data;
        uint16_t routing_message_header_size = sizeof(struct routing_message_data_header);

        std::shared_ptr<Socks5message> socks5_message;
        struct socks5_message_data *socks5_message_data;
        uint16_t socks5_message_header_size = sizeof(struct socks5_message_data_header);

        ret = gettimeofday(&start, NULL);
        if(ret == -1)
        {
#ifdef _DEBUG
            std::printf("[-] recv_message gettimeofday error\n");
#endif
            free(tmp);
            free(buffer);
            return -1;
        }

        while(1)
        {
            ret = gettimeofday(&end, NULL);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] recv_message gettimeofday error\n");
#endif
                free(tmp);
                free(buffer);
                return -1;
            }

            t = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);	// microsecond
            if(t >= (tv_sec * 1000000 + tv_usec))
            {
#ifdef _DEBUG
                printf("[-] recv_message timeout\n");
#endif
                free(tmp);
                free(buffer);
                return 0;
            }

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
            if(ret)
            {
                tmprec = recv(sock,
                              tmp,
                              NODE_BUFFER_SIZE,
                              0);
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

                    if(rec > 0)
                    {
                        if(buffer[0] == 'r')
                        {
                            if(rec < routing_message_header_size)
                            {
                                continue;
                            }else
                            {
                                routing_message_data = (struct routing_message_data *)buffer;
                                if(rec < routing_message_header_size + routing_message_data->data_size)
                                {
                                    continue;
                                }else if(rec == routing_message_header_size + routing_message_data->data_size)
                                {
                                    routing_message = std::make_unique<Routingmessage>(this->get_pipe_id(),
                                                                                       routing_message_data);

                                    std::thread thread_message_manager(&Messagemanager::push_routing_message,
                                                                       message_manager,
                                                                       routing_message);
                                    thread_message_manager.detach();
                                }else
                                {
#ifdef _DEBUG
                                    std::printf("[-] recv_message routing message recv length error: %d\n", rec);
#endif
                                    free(tmp);
                                    free(buffer);
                                    return 0;
                                }
                            }
                        }else if(buffer[0] == 's'){
                            if(rec < socks5_message_header_size)
                            {
                                continue;
                            }else
                            {
                                socks5_message_data = (struct socks5_message_data*)buffer;
                                if(rec < socks5_message_header_size + socks5_message_data->data_size){
                                    continue;
                                }else if(rec == socks5_message_header_size + socks5_message_data->data_size)
                                {
                                    socks5_message = std::make_unique<Socks5message>(socks5_message_data);

                                    std::thread thread_message_manager(&Messagemanager::push_socks5_message,
                                                                       message_manager,
                                                                       socks5_message);
                                    thread_message_manager.detach();

                                }else
                                {
#ifdef _DEBUG
                                    std::printf("[-] recv_message socks5 message recv length error: %d\n", rec);
#endif
                                    free(tmp);
                                    free(buffer);
                                    return 0;;
                                }
                            }
                        }else
                        {
#ifdef _DEBUG
                            std::printf("[-] recv_message message type error: %c\n", buffer[0]);
#endif
                        }
                    }else
                    {
                        continue;
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
        struct timeval start;
        struct timeval end;
        long tv_sec = 3;
        long tv_usec = 0;
        long t = 0;
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

        ret = gettimeofday(&start,
                           NULL);
        if(ret == -1)
        {
#ifdef _DEBUG
            std::printf("[-] send_routing_message gettimeofday error\n");
#endif
            free(buffer);
            return -1;
        }

        while(len > 0)
        {
            ret = gettimeofday(&end,
                               NULL);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] send_routing_message gettimeofday error\n");
#endif
                free(buffer);
                return -1;
            }

            t = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);	// microsecond
            if(t >= (tv_sec * 1000000 + tv_usec))
            {
#ifdef _DEBUG
                std::printf("[-] send_routing_message timeout\n");
#endif
                free(buffer);
                return 0;
            }

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
                           0);
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
        struct timeval start;
        struct timeval end;
        long tv_sec = 3;
        long tv_usec = 0;
        long t = 0;
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

        ret = gettimeofday(&start,
                           NULL);
        if(ret == -1)
        {
#ifdef _DEBUG
            std::printf("[-] send_socks5_message gettimeofday error\n");
#endif
            free(buffer);
            return -1;
        }

        while(len > 0)
        {
            ret = gettimeofday(&end,
                               NULL);
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] send_socks5_message gettimeofday error\n");
#endif
                free(buffer);
                return -1;
            }

            t = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);	// microsecond
            if(t >= (tv_sec * 1000000 + tv_usec))
            {
#ifdef _DEBUG
                std::printf("[-] send_socks5_message timeout\n");
#endif
                free(buffer);
                return 0;
            }

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
                           0);
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
