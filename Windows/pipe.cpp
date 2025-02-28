/*
 * Title:  spider pipe.cpp (Windows)
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
               std::string pipe_ip_scope_id,
               std::string pipe_destination_ip,
               std::string pipe_destination_ip_scope_id,
               std::string pipe_destination_port,
               SOCKET pipe_sock,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_ip_scope_id = pipe_ip_scope_id;
        this->pipe_destination_ip = pipe_destination_ip;
        this->pipe_destination_ip_scope_id = pipe_destination_ip_scope_id;
        this->pipe_destination_port = pipe_destination_port;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
    }

    Pipe::Pipe(uint32_t pipe_id,
               char mode,
               std::string pipe_ip,
               std::string pipe_ip_scope_id,
               std::string pipe_listen_port,
               SOCKET pipe_sock,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->pipe_ip = pipe_ip;
        this->pipe_ip_scope_id = pipe_ip_scope_id;
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
        struct timeval tv;
        long tv_sec = 3600;
        long tv_usec = 0;
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

        bool recv_message_type_flag = false;
        bool recv_header_flag = false;
        int32_t recv_message_type_size = sizeof(char);
        int32_t recv_header_size = 0;
        int32_t recv_data_size = 0;
        int32_t remaining_size = 0;


        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock,
                   &readfds);
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(NULL,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] recv_message select timeout\n");
#endif
                free(tmp);
                free(buffer);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] recv_message select error:0x%x\n", ret);
#endif
                free(tmp);
                free(buffer);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
                if(recv_message_type_flag == false)
                {
                    tmprec = recv(sock,
                                  tmp,
                                  recv_message_type_size,
                                  0);
                }else
                {
                    tmprec = recv(sock,
                                  tmp,
                                  remaining_size,
                                  0);
                }

                if(tmprec == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] recv_message recv error: %d\n",
                                    ret);
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

#ifdef DEBUGPRINT
//                    std::printf("rec: %d\n", rec);
//                    print_bytes(buffer, rec);
#endif

                    if(recv_message_type_flag == false)
                    {
                        recv_message_type_flag = true;

                        if(buffer[0] == 'r')
                        {
                            remaining_size = routing_message_header_size - rec;
                        }else if(buffer[0] == 's')
                        {
                            remaining_size = socks5_message_header_size - rec;
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] recv_message message type error: %c\n", buffer[0]);
#endif
                            free(tmp);
                            free(buffer);
                            return 0;
                        }

                        continue;
                    }else if(recv_header_flag == false)
                    {
                        if(buffer[0] == 'r')
                        {
                            if(rec < routing_message_header_size)
                            {
                                remaining_size = routing_message_header_size - rec;
                            }else
                            {
                                recv_header_flag = true;

                                routing_message_data = (struct routing_message_data *)buffer;

                                recv_data_size = routing_message_data->data_size;

                                remaining_size = recv_data_size;
                            }

                            continue;
                        }else if(buffer[0] == 's')
                        {
                            if(rec < socks5_message_header_size)
                            {
                                remaining_size = socks5_message_header_size - rec;
                            }else
                            {
                                recv_header_flag = true;

                                socks5_message_data = (struct socks5_message_data*)buffer;

                                recv_data_size = socks5_message_data->data_size;

                                remaining_size = recv_data_size;
                            }

                            continue;
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] recv_message message type error: %c\n", buffer[0]);
#endif
                            free(tmp);
                            free(buffer);
                            return 0;
                        }
                    }else
                    {
                        if(buffer[0] == 'r')
                        {
                            if(rec < routing_message_header_size + recv_data_size)
                            {
                                remaining_size = routing_message_header_size + recv_data_size - rec;
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

                                }
                            }
                        }else if(buffer[0] == 's')
                        {
                            if(rec < socks5_message_header_size + recv_data_size)
                            {
                                remaining_size = socks5_message_header_size + recv_data_size - rec;
                                continue;
                            }else
                            {
                                socks5_message_data = (struct socks5_message_data *)buffer;
                                if(rec < socks5_message_header_size + socks5_message_data->data_size)
                                {
                                    continue;
                                }else if(rec == socks5_message_header_size + socks5_message_data->data_size)
                                {
                                    socks5_message = std::make_unique<Socks5message>(socks5_message_data);

                                    std::thread thread_message_manager(&Messagemanager::push_socks5_message,
                                                                       message_manager,
                                                                       socks5_message);
                                    thread_message_manager.detach();
                                }
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] recv_message message type error: %c\n", buffer[0]);
#endif
                            free(tmp);
                            free(buffer);
                            return 0;
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

#ifdef DEBUGPRINT
//        std::printf("len: %d\n", len);
//        print_bytes(buffer, len);
#endif


        while(len > 0)
        {
            FD_ZERO(&writefds);
            FD_SET(sock,
                   &writefds);
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0){
#ifdef DEBUGPRINT
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
                if(sen == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] send_routing_message send error: %d\n",
                                    ret);
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
        struct timeval tv;
        long tv_sec = 3600;
        long tv_usec = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));

        std::shared_ptr<Socks5message> socks5_message;
        socks5_message = pop_socks5_message();
        length = socks5_message->copy_to_buffer(buffer);
        len = length;

#ifdef DEBUGPRINT
//        std::printf("len: %d\n", len);
//        print_bytes(buffer, len);
#endif


        while(len > 0)
        {
            FD_ZERO(&writefds);
            FD_SET(sock,
                   &writefds);
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0){
#ifdef DEBUGPRINT
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
                if(sen == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] send_socks5_message send error: %d\n",
                                    ret);
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
