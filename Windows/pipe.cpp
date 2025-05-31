/*
 * Title:  spider pipe.cpp (Windows)
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
               char message_mode,
               std::string pipe_ip,
               std::string pipe_ip_scope_id,
               std::string pipe_destination_ip,
               std::string pipe_destination_ip_scope_id,
               std::string pipe_destination_port,
               SOCKET pipe_sock,
               std::shared_ptr<Routingmanager> routing_manager,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->message_mode = message_mode;
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
               char message_mode,
               std::string pipe_ip,
               std::string pipe_ip_scope_id,
               std::string pipe_listen_port,
               SOCKET pipe_sock,
               std::shared_ptr<Routingmanager> routing_manager,
               std::shared_ptr<Messagemanager> message_manager)
    : Node(pipe_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->pipe_id = pipe_id;
        this->mode = mode;
        this->message_mode = message_mode;
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

    void Pipe::set_message_mode(char message_mode)
    {
        this->message_mode = message_mode;
    }

    char Pipe::get_message_mode()
    {
        return message_mode;
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
        std::shared_ptr<Routingmessage> routing_message_old = nullptr;

        routing_message_old = routing_messages_queue->pop_latest_message();
        routing_messages_queue->push(routing_message);

        return;
    }

    std::shared_ptr<Routingmessage> Pipe::pop_routing_message()
    {
        std::shared_ptr<Routingmessage> routing_message = routing_messages_queue->pop();

        return routing_message;
    }

    std::shared_ptr<Routingmessage> Pipe::pop_latest_routing_message()
    {
        std::shared_ptr<Routingmessage> routing_message = routing_messages_queue->pop_latest_message();

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
#ifdef DEBUGPRINT
                                        std::printf("[-] cannot transfer pipe message\n");
#endif
                                        free(tmp);
                                        free(buffer);
                                        return 0;
                                    }
                                }
                            }else
                            {
#ifdef DEBUGPRINT
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
                std::printf("[+] send_routing_message select timeout\n");
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
                std::printf("[+] send_socks5_message select timeout\n");
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

    int32_t Pipe::get_http_header_length(const char* buffer)
    {
        const char* end = strstr(buffer,
                                 "\r\n\r\n");
        if(end != NULL)
        {
            return end - buffer + 4;
        }

        return 0;
    }

    int32_t Pipe::get_content_length(const char* buffer)
    {
        std::string key = "Content-Length: ";
        const char* start = strstr(buffer,
                                   key.c_str());
        std::string content_length_string = "";
        std::string length_string = "";
        size_t pos = 0;
        size_t end_pos = 0;
        int32_t length = 0;

        if(start != NULL)
        {
            const char* end = strstr(start,
                                     "\r\n");
            if(end != NULL)
            {
                content_length_string = std::string(start,
                                                    end + 2);
                pos = content_length_string.find(key);
                if(pos != std::string::npos)
                {
                    pos += key.length();
                    end_pos = content_length_string.find("\r\n",
                                                         pos);
                    if(end_pos != std::string::npos)
                    {
                        length_string = content_length_string.substr(pos,
                                                                     end_pos - pos);
                        length = (int32_t)std::stoi(length_string);

                        return length;
                    }
                }
            }
        }

        return length;
    }

    int32_t Pipe::do_http_connection_client()
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t tmprec = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t len = 0;
        fd_set readfds;
        fd_set writefds;
        struct timeval tv;
        long tv_sec = 30;
        long tv_usec = 0;

        int32_t buffer_size = NODE_BUFFER_SIZE * 12;
        int32_t buffer_http_header_size = NODE_BUFFER_SIZE;
        int32_t buffer_http_body_size = NODE_BUFFER_SIZE * 11;
        char *buffer = (char *)calloc(buffer_size,
                                      sizeof(char));
        char *buffer_http_header = (char *)calloc(buffer_http_header_size,
                                                  sizeof(char));
        char *buffer_http_body = (char *)calloc(buffer_http_body_size,
                                                sizeof(char));
        int32_t http_header_length = 0;     // start line + headers + empty line
        int32_t http_body_length = 0;       // body
        int32_t http_length = 0;
        int32_t routing_message_count = 0;
        int32_t socks5_message_count = 0;
        std::shared_ptr<Routingmessage> routing_message;
        std::shared_ptr<Socks5message> socks5_message;
        bool recv_http_header_flag = false;
        int32_t total_length = 0;
        int32_t remaining_size = 0;
        char *pos = NULL;
        struct spider_message *spider_message;
        int32_t spider_message_header_size = sizeof(struct spider_message_header);
        std::shared_ptr<Pipe> pipe;


        routing_message = pop_latest_routing_message();
        if(routing_message != nullptr)
        {
            http_body_length = routing_message->copy_to_buffer(buffer_http_body);
            routing_message_count++;
        }

        socks5_message_count = this->socks5_messages_queue->get_count();
        if(socks5_message_count > 10)
        {
            socks5_message_count = 10;
        }

        for(int32_t i=socks5_message_count; i>0; i--)
        {
            socks5_message = pop_socks5_message();
            http_body_length += socks5_message->copy_to_buffer(buffer_http_body + http_body_length);
        }


        if(strstr(pipe_destination_ip.c_str(), ":") == NULL)
        {
            http_header_length = snprintf(buffer_http_header,
                                          NODE_BUFFER_SIZE,
                                          "POST / HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n",
                                          pipe_destination_ip.c_str(),
                                          pipe_destination_port.c_str(),
                                          HTTP_REQUEST_HEADER_USER_AGENT_VALUE,
                                          http_body_length);
        }else
        {
            http_header_length = snprintf(buffer_http_header,
                                          NODE_BUFFER_SIZE,
                                          "POST / HTTP/1.1\r\nHost: [%s]:%s\r\nUser-Agent: %s\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n",
                                          pipe_destination_ip.c_str(),
                                          pipe_destination_port.c_str(),
                                          HTTP_REQUEST_HEADER_USER_AGENT_VALUE,
                                          http_body_length);
        }

        std::memcpy(buffer,
                    buffer_http_header,
                    http_header_length);

        std::memcpy(buffer + http_header_length,
                    buffer_http_body,
                    http_body_length);

        len = http_header_length + http_body_length;
        send_length = 0;

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
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_client select timeout\n");
#endif
                free(buffer);
                free(buffer_http_header);
                free(buffer_http_body);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_client select error:0x%x\n",
                            ret);
#endif
                free(buffer);
                free(buffer_http_header);
                free(buffer_http_body);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret > 0)
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
                        std::printf("[-] do_http_connection_client send error: %d\n",
                                    ret);
#endif
                        free(buffer);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }
                }
                send_length += sen;
                len -= sen;
            }
        }


        std::memset(buffer,
                    0,
                    buffer_size);

        free(buffer_http_header);
        free(buffer_http_body);

        http_header_length = 0;
        http_body_length = 0;


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
                std::printf("[+] do_http_connection_client select timeout\n");
#endif
                free(buffer);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_client select error:0x%x\n",
                            ret);
#endif
                free(buffer);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret > 0)
            {
                if(recv_http_header_flag == false)
                {
                    tmprec = recv(sock,
                                  buffer,
                                  buffer_http_header_size,
                                  0);
                }else
                {
                    tmprec = recv(sock,
                                  buffer + rec,
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
                        std::printf("[-] do_http_connection_client recv error: %d\n",
                                    ret);
#endif
                        free(buffer);
                        return -1;
                    }
                }else
                {
                    rec += tmprec;
                    tmprec = 0;

#ifdef DEBUGPRINT
//                    std::printf("rec: %d\n", rec);
//                    print_bytes(buffer, rec);
#endif

                    if(recv_http_header_flag == false)
                    {
                        recv_http_header_flag = true;

                        http_header_length = get_http_header_length((const char *)buffer);
                        http_body_length = get_content_length((const char *)buffer);
                        total_length = http_header_length + http_body_length;

                        if(total_length == 0)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_client error total_length: %d\n",
                                        total_length);
#endif
                            free(buffer);
                            return -1;
                        }else if(total_length > buffer_size)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_client http size error buffer_size: %d total_length: %d\n",
                                        buffer_size,
                                        total_length);
#endif
                            free(buffer);
                            return -1;
                        }

                        remaining_size = total_length - rec;
                        if(remaining_size > 0)
                        {
                            continue;
                        }
                    }else
                    {
                        remaining_size = total_length - rec;
                        if(remaining_size > 0)
                        {
                            continue;
                        }
                    }

                    pos = buffer + http_header_length;

                    while(pos < buffer + total_length)
                    {
                        spider_message = (struct spider_message *)pos;
                        if(spider_message->header.message_type == 'r')
                        {
                            routing_message = std::make_shared<Routingmessage>(this->get_pipe_id(),
                                                                               spider_message);

                            std::thread thread_message_manager(&Messagemanager::push_routing_message,
                                                               message_manager,
                                                               routing_message);
                            thread_message_manager.detach();

                            pos += spider_message_header_size + routing_message->get_data_size();
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
#ifdef DEBUGPRINT
                                    std::printf("[-] do_http_connection_client cannot transfer pipe message\n");
#endif
                                }
                            }

                            pos += spider_message_header_size + socks5_message->get_data_size();
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_client message type error: %c\n",
                                        spider_message->header.message_type);
#endif
                            break;
                        }
                    }

                    break;
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Pipe::do_http_connection_server()
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t tmprec = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t len = 0;
        fd_set readfds;
        fd_set writefds;
        struct timeval tv;
        long tv_sec = 30;
        long tv_usec = 0;

        int32_t buffer_size = NODE_BUFFER_SIZE * 12;
        int32_t buffer_http_header_size = NODE_BUFFER_SIZE;
        int32_t buffer_http_body_size = NODE_BUFFER_SIZE * 11;
        char *buffer = (char *)calloc(buffer_size,
                                      sizeof(char));
        char *buffer_http_header = NULL;
        char *buffer_http_body = NULL;
        int32_t http_header_length = 0;     // start line + headers + empty line
        int32_t http_body_length = 0;       // body
        int32_t http_length = 0;
        int32_t routing_message_count = 0;
        int32_t socks5_message_count = 0;
        std::shared_ptr<Routingmessage> routing_message;
        std::shared_ptr<Socks5message> socks5_message;
        bool recv_http_header_flag = false;
        int32_t total_length = 0;
        int32_t remaining_size = 0;
        char *pos = NULL;
        struct spider_message *spider_message;
        int32_t spider_message_header_size = sizeof(struct spider_message_header);
        std::shared_ptr<Pipe> pipe;


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
                std::printf("[+] do_http_connection_server select timeout\n");
#endif
                free(buffer);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_server select error:0x%x\n",
                            ret);
#endif
                free(buffer);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret > 0)
            {
                if(recv_http_header_flag == false)
                {
                    tmprec = recv(sock,
                                  buffer,
                                  buffer_http_header_size,
                                  0);
                }else
                {
                    tmprec = recv(sock,
                                  buffer + rec,
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
                        std::printf("[-] do_http_connection_server recv error: %d\n",
                                    ret);
#endif
                        free(buffer);
                        return -1;
                    }
                }else
                {
                    rec += tmprec;
                    tmprec = 0;

#ifdef DEBUGPRINT
//                    std::printf("rec: %d\n", rec);
//                    print_bytes(buffer, rec);
#endif

                    if(recv_http_header_flag == false)
                    {
                        recv_http_header_flag = true;

                        http_header_length = get_http_header_length((const char *)buffer);
                        http_body_length = get_content_length((const char *)buffer);
                        total_length = http_header_length + http_body_length;

                        if(total_length == 0)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_server error total_length: %d\n",
                                        total_length);
#endif
                            free(buffer);
                            return -1;
                        }else if(total_length > buffer_size)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_server http size error buffer_size: %d total_length: %d\n",
                                        buffer_size,
                                        total_length);
#endif
                            free(buffer);
                            return -1;
                        }

                        remaining_size = total_length - rec;
                        if(remaining_size > 0)
                        {
                            continue;
                        }
                    }else
                    {
                        remaining_size = total_length - rec;
                        if(remaining_size > 0)
                        {
                            continue;
                        }
                    }

                    pos = buffer + http_header_length;

                    while(pos < buffer + total_length)
                    {
                        spider_message = (struct spider_message *)pos;
                        if(spider_message->header.message_type == 'r')
                        {
                            routing_message = std::make_shared<Routingmessage>(this->get_pipe_id(),
                                                                               spider_message);

                            std::thread thread_message_manager(&Messagemanager::push_routing_message,
                                                               message_manager,
                                                               routing_message);
                            thread_message_manager.detach();

                            pos += spider_message_header_size + routing_message->get_data_size();
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
#ifdef DEBUGPRINT
                                    std::printf("[-] do_http_connection_server cannot transfer pipe message\n");
#endif
                                }
                            }

                            pos += spider_message_header_size + socks5_message->get_data_size();
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_server message type error: %c\n",
                                        spider_message->header.message_type);
#endif
                            break;
                        }
                    }

                    break;
                }
            }
        }


        std::memset(buffer,
                    0,
                    buffer_size);

        buffer_http_header = (char *)calloc(buffer_http_header_size,
                                            sizeof(char));
        buffer_http_body = (char *)calloc(buffer_http_body_size,
                                          sizeof(char));

        http_header_length = 0;
        http_body_length = 0;


        routing_message = pop_latest_routing_message();
        if(routing_message != nullptr)
        {
            http_body_length = routing_message->copy_to_buffer(buffer_http_body);
            routing_message_count++;
        }

        socks5_message_count = this->socks5_messages_queue->get_count();
        if(socks5_message_count > 10)
        {
            socks5_message_count = 10;
        }

        for(int32_t i=socks5_message_count; i>0; i--)
        {
            socks5_message = pop_socks5_message();
            http_body_length += socks5_message->copy_to_buffer(buffer_http_body + http_body_length);
        }


        http_header_length = snprintf(buffer_http_header,
                                      NODE_BUFFER_SIZE,
                                      "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n",
                                      http_body_length);

        std::memcpy(buffer,
                    buffer_http_header,
                    http_header_length);

        std::memcpy(buffer + http_header_length,
                    buffer_http_body,
                    http_body_length);

        len = http_header_length + http_body_length;
        send_length = 0;

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
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_server select timeout\n");
#endif
                free(buffer);
                free(buffer_http_header);
                free(buffer_http_body);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_server select error:0x%x\n",
                            ret);
#endif
                free(buffer);
                free(buffer_http_header);
                free(buffer_http_body);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret > 0)
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
                        std::printf("[-] do_http_connection_server send error: %d\n",
                                    ret);
#endif
                        free(buffer);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }
                }
                send_length += sen;
                len -= sen;
            }
        }

        free(buffer);
        free(buffer_http_header);
        free(buffer_http_body);
        return 0;
    }
}
