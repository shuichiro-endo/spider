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
               std::string message_mode,
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
               std::string message_mode,
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

    void Pipe::set_message_mode(std::string message_mode)
    {
        this->message_mode = message_mode;
    }

    std::string Pipe::get_message_mode()
    {
        return message_mode;
    }

    void Pipe::set_ctxt_handle(CtxtHandle *ctxt_handle)
    {
        this->ctxt_handle = ctxt_handle;
    }

    CtxtHandle *Pipe::get_ctxt_handle()
    {
        return ctxt_handle;
    }

    void Pipe::set_stream_sizes(SecPkgContext_StreamSizes stream_sizes)
    {
        this->stream_sizes = stream_sizes;
    }

    SecPkgContext_StreamSizes Pipe::get_stream_sizes()
    {
        return stream_sizes;
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

    void Pipe::set_forward_proxy_type(std::string forward_proxy_type)
    {
        this->forward_proxy_type = forward_proxy_type;
    }

    std::string Pipe::get_forward_proxy_type()
    {
        return forward_proxy_type;
    }

    void Pipe::set_forward_proxy_authentication(int32_t forward_proxy_authentication)
    {
        this->forward_proxy_authentication = forward_proxy_authentication;
    }

    int32_t Pipe::get_forward_proxy_authentication()
    {
        return forward_proxy_authentication;
    }

    void Pipe::set_forward_proxy_ip(std::string forward_proxy_ip)
    {
        this->forward_proxy_ip = forward_proxy_ip;
    }

    std::string Pipe::get_forward_proxy_ip()
    {
        return forward_proxy_ip;
    }

    void Pipe::set_forward_proxy_ip_scope_id(std::string forward_proxy_ip_scope_id)
    {
        this->forward_proxy_ip_scope_id = forward_proxy_ip_scope_id;
    }

    std::string Pipe::get_forward_proxy_ip_scope_id()
    {
        return forward_proxy_ip_scope_id;
    }

    void Pipe::set_forward_proxy_port(std::string forward_proxy_port)
    {
        this->forward_proxy_port = forward_proxy_port;
    }

    std::string Pipe::get_forward_proxy_port()
    {
        return forward_proxy_port;
    }

    void Pipe::set_forward_proxy_username(std::string forward_proxy_username)
    {
        this->forward_proxy_username = forward_proxy_username;
    }

    std::string Pipe::get_forward_proxy_username()
    {
        return forward_proxy_username;
    }

    void Pipe::set_forward_proxy_password(std::string forward_proxy_password)
    {
        this->forward_proxy_password = forward_proxy_password;
    }

    std::string Pipe::get_forward_proxy_password()
    {
        return forward_proxy_password;
    }

    void Pipe::set_forward_proxy_nthash_hexstring(std::string forward_proxy_nthash_hexstring)
    {
        this->forward_proxy_nthash_hexstring = forward_proxy_nthash_hexstring;
    }

    std::string Pipe::get_forward_proxy_nthash_hexstring()
    {
        return forward_proxy_nthash_hexstring;
    }

    void Pipe::set_forward_proxy_domainname(std::string forward_proxy_domainname)
    {
        this->forward_proxy_domainname = forward_proxy_domainname;
    }

    std::string Pipe::get_forward_proxy_domainname()
    {
        return forward_proxy_domainname;
    }

    void Pipe::set_forward_proxy_workstationname(std::string forward_proxy_workstationname)
    {
        this->forward_proxy_workstationname = forward_proxy_workstationname;
    }

    std::string Pipe::get_forward_proxy_workstationname()
    {
        return forward_proxy_workstationname;
    }

    void Pipe::set_forward_proxy_service_principal_name(std::string forward_proxy_service_principal_name)
    {
        this->forward_proxy_service_principal_name = forward_proxy_service_principal_name;
    }

    std::string Pipe::get_forward_proxy_service_principal_name()
    {
        return forward_proxy_service_principal_name;
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

    int32_t Pipe::recv_data_tls(char *buffer,
                                int32_t buffer_size,
                                long tv_sec,
                                long tv_usec)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t tmprec = 0;
        int32_t remaining_size = 0;
        int32_t decrypted_data_length = 0;
        fd_set readfds;
        struct timeval tv;

        char *buffer2 = (char *)calloc(NODE_BUFFER_SIZE,
                                       sizeof(char));
        char *tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                   sizeof(char));


        if(buffer_size < NODE_BUFFER_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] recv_data_tls buffer size error\n");
#endif
            free(buffer2);
            free(tmp);
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
                std::printf("[-] recv_data_tls select timeout\n");
#endif
                free(buffer2);
                free(tmp);
                return -1;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_data_tls select error: 0x%x\n",
                            ret);
#endif
                free(buffer2);
                free(tmp);
                return -1;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret > 0)
            {
                if(remaining_size == 0)
                {
                    rec = recv(sock,
                               buffer2,
                               NODE_BUFFER_SIZE,
                               0);
                }else
                {
                    rec = recv(sock,
                               buffer2,
                               remaining_size,
                               0);
                }

                if(rec == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] recv_data_tls recv error: %d\n",
                                    ret);
#endif
                        free(buffer2);
                        free(tmp);
                        return -1;
                    }
                }else if(rec == 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] recv_data_tls recv error: %d\n",
                                rec);
#endif
                    free(buffer2);
                    free(tmp);
                    return -1;
                }else
                {
                    if(tmprec + rec <= buffer_size)
                    {
                        std::memcpy(tmp + tmprec,
                                    buffer2,
                                    rec);
                        tmprec += rec;

                        std::memset(buffer2,
                                    0,
                                    NODE_BUFFER_SIZE);
                        remaining_size = 0;

                        ret = decrypt_data_tls(tmp,
                                               &tmprec,
                                               buffer,
                                               buffer_size,
                                               &decrypted_data_length,
                                               &remaining_size);
                        if(ret == 1)    // SEC_E_INCOMPLETE_MESSAGE
                        {
                            continue;
                        }else if(ret == 2 ||    // SEC_E_DECRYPT_FAILURE
                                 ret < 0)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] recv_data_tls decrypt_data_tls error: %d\n",
                                        ret);
#endif
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }else
                        {
                            break;
                        }
                    }
                }
            }
        }

        free(buffer2);
        free(tmp);
        return decrypted_data_length;
    }

    int32_t Pipe::send_data_tls(char *buffer,
                                int32_t data_size,
                                long tv_sec,
                                long tv_usec)
    {
        int ret = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t len = 0;
        int32_t remaining_size = 0;
        int32_t count = 0;
        int32_t p = 0;
        fd_set writefds;
        struct timeval tv;
        char *buffer2 = (char *)calloc(NODE_BUFFER_SIZE,
                                       sizeof(char));


        if(data_size > NODE_BUFFER_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] send_data_tls data size error\n");
#endif
            free(buffer2);
            return -1;
        }

        remaining_size = data_size;
        count = data_size / (int32_t)stream_sizes.cbMaximumMessage + 1;
        p = 0;

        for(int32_t i=0; i<count; i++)
        {
            if(remaining_size > (int32_t)stream_sizes.cbMaximumMessage)
            {
                len = encrypt_data_tls(buffer + p,
                                       (int32_t)stream_sizes.cbMaximumMessage,
                                       buffer2,
                                       NODE_BUFFER_SIZE);
                if(len < 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] send_data_tls encrypt_data_tls error: %d\n",
                                len);
#endif
                    free(buffer2);
                    return -1;
                }

                p += (int32_t)stream_sizes.cbMaximumMessage;
                remaining_size -= (int32_t)stream_sizes.cbMaximumMessage;
            }else
            {
                len = encrypt_data_tls(buffer + p,
                                       remaining_size,
                                       buffer2,
                                       NODE_BUFFER_SIZE);
                if(len < 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] send_data_tls encrypt_data_tls error: %d\n",
                                len);
#endif
                    free(buffer2);
                    return -1;
                }

                p += remaining_size;
                remaining_size = 0;
            }

            send_length = 0;

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
                    std::printf("[-] send_data_tls select timeout\n");
#endif
                    free(buffer2);
                    return -1;
                }else if(ret == SOCKET_ERROR)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] send_data_tls select error: 0x%0x\n",
                                ret);
#endif
                    free(buffer2);
                    return -1;
                }

                ret = FD_ISSET(sock,
                               &writefds);
                if(ret > 0)
                {
                    sen = send(sock,
                               buffer2+send_length,
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
                            std::printf("[-] send_data_tls send error: %d\n",
                                        ret);
#endif
                            free(buffer2);
                            return -1;
                        }
                    }
                    send_length += sen;
                    len -= sen;
                }
            }
        }

        free(buffer2);
        return data_size;
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

    int32_t Pipe::encrypt_data_tls(char *input,
                                   int32_t input_length,
                                   char *output,
                                   int32_t output_length)
    {
        SecBuffer sec_buffer[4] = {0};
        SecBufferDesc sec_buffer_desc = {0};
        SECURITY_STATUS status;
        int32_t encrypt_data_length = 0;

        if(input_length > (int32_t)stream_sizes.cbMaximumMessage)
        {
#ifdef DEBUGPRINT
            std::printf("[-] input length is too long input_length: %d stream_sizes.cbMaximumMessag: %d\n",
                        input_length,
                        (int32_t)stream_sizes.cbMaximumMessage);
#endif
            return -1;
        }

        if(output_length < (int32_t)stream_sizes.cbHeader + input_length + (int32_t)stream_sizes.cbTrailer)
        {
#ifdef DEBUGPRINT
            std::printf("[-] output length is too small\n");
#endif
            return -1;
        }

        sec_buffer[0].BufferType = SECBUFFER_STREAM_HEADER;
        sec_buffer[0].pvBuffer = output;
        sec_buffer[0].cbBuffer = stream_sizes.cbHeader;
        sec_buffer[1].BufferType = SECBUFFER_DATA;
        sec_buffer[1].pvBuffer = output + stream_sizes.cbHeader;
        sec_buffer[1].cbBuffer = input_length;
        sec_buffer[2].BufferType = SECBUFFER_STREAM_TRAILER;
        sec_buffer[2].pvBuffer = output + stream_sizes.cbHeader + input_length;
        sec_buffer[2].cbBuffer = stream_sizes.cbTrailer;
        sec_buffer[3].BufferType = SECBUFFER_EMPTY;
        sec_buffer[3].pvBuffer = NULL;
        sec_buffer[3].cbBuffer = 0;

        std::memcpy(sec_buffer[1].pvBuffer,
                    input,
                    input_length);

        sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
        sec_buffer_desc.cBuffers = 4;
        sec_buffer_desc.pBuffers = sec_buffer;

        status = EncryptMessage(ctxt_handle,
                                0,
                                &sec_buffer_desc,
                                0);
        if(status != SEC_E_OK)
        {
#ifdef DEBUGPRINT
            std::printf("[-] encrypt_data_tls error: %x\n",
                        status);
#endif
            return -1;
        }

        encrypt_data_length = sec_buffer[0].cbBuffer + sec_buffer[1].cbBuffer + sec_buffer[2].cbBuffer;

        return encrypt_data_length;
    }

    int32_t Pipe::decrypt_data_tls(char *input,
                                   int32_t *input_length,
                                   char *output,
                                   int32_t output_length,
                                   int32_t *decrypted_data_length,
                                   int32_t *remaining_size)
    {
        int message_length = 0;
        SecBuffer sec_buffer[4] = {0};
        SecBufferDesc sec_buffer_desc = {0};
        SECURITY_STATUS status;

        message_length = *input_length - (int32_t)stream_sizes.cbHeader - stream_sizes.cbTrailer;
/*
        if(message_length > (int32_t)stream_sizes.cbMaximumMessage)
        {
#ifdef DEBUGPRINT
            std::printf("[-] message length is too long message_length: %d stream_sizes.cbMaximumMessage: %d\n",
                        message_length,
                        (uint32_t)stream_sizes.cbMaximumMessage);
#endif
            return -1;
        }
*/
        if(output_length < message_length)
        {
#ifdef DEBUGPRINT
            std::printf("[-] output length is too small\n");
#endif
            return -1;
        }

        sec_buffer[0].BufferType = SECBUFFER_DATA;
        sec_buffer[0].pvBuffer = input;
        sec_buffer[0].cbBuffer = *input_length;
        sec_buffer[1].BufferType = SECBUFFER_EMPTY;
        sec_buffer[1].pvBuffer = NULL;
        sec_buffer[1].cbBuffer = 0;
        sec_buffer[2].BufferType = SECBUFFER_EMPTY;
        sec_buffer[2].pvBuffer = NULL;
        sec_buffer[2].cbBuffer = 0;
        sec_buffer[3].BufferType = SECBUFFER_EMPTY;
        sec_buffer[3].pvBuffer = NULL;
        sec_buffer[3].cbBuffer = 0;

        sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
        sec_buffer_desc.cBuffers = 4;
        sec_buffer_desc.pBuffers = sec_buffer;

        while(1)
        {
            status = DecryptMessage(ctxt_handle,
                                    &sec_buffer_desc,
                                    0,
                                    NULL);
            if(status == SEC_E_INCOMPLETE_MESSAGE)
            {
#ifdef DEBUGPRINT
                std::printf("[+] SEC_E_INCOMPLETE_MESSAGE\n");
#endif
                *remaining_size = 0;

                for(int i=0; i<4; i++)
                {
#ifdef DEBUGPRINT
//                std::printf("[+] sec_buffer[%d].BufferType: %d, sec_buffer[%d].cbBuffer: %d\n", i, sec_buffer[i].BufferType, i, sec_buffer[i].cbBuffer);
#endif
                    if(sec_buffer[i].BufferType == SECBUFFER_MISSING)
                    {
                        *remaining_size = sec_buffer[i].cbBuffer;
                        break;
                    }
                }

                return 1;
            }else if(status == SEC_E_DECRYPT_FAILURE)
            {
#ifdef DEBUGPRINT
                std::printf("[+] SEC_E_DECRYPT_FAILURE\n");
#endif
                return 2;
            }else if(status != SEC_E_OK)
            {
#ifdef DEBUGPRINT
                std::printf("[-] DecryptMessage error: %x\n",
                            status);
#endif
                return -1;
            }

            if(sec_buffer[1].BufferType == SECBUFFER_DATA)
            {
                std::memcpy(output + *decrypted_data_length,
                            sec_buffer[1].pvBuffer,
                            sec_buffer[1].cbBuffer);

                *decrypted_data_length += sec_buffer[1].cbBuffer;

                if(sec_buffer[3].cbBuffer != 0)
                {
                    std::memcpy(input,
                                sec_buffer[3].pvBuffer,
                                sec_buffer[3].cbBuffer);

                    *input_length = sec_buffer[3].cbBuffer;

                    sec_buffer[0].BufferType = SECBUFFER_DATA;
                    sec_buffer[0].pvBuffer = input;
                    sec_buffer[0].cbBuffer = *input_length;
                    sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                    sec_buffer[1].pvBuffer = NULL;
                    sec_buffer[1].cbBuffer = 0;
                    sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                    sec_buffer[2].pvBuffer = NULL;
                    sec_buffer[2].cbBuffer = 0;
                    sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                    sec_buffer[3].pvBuffer = NULL;
                    sec_buffer[3].cbBuffer = 0;

                    continue;
                }else
                {
                    break;
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] sec_buffer[1].BufferType error: %x\n",
                            sec_buffer[1].BufferType);
#endif
                return -1;
            }
        }

        return 0;
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
        char *buffer2 = (char *)calloc(buffer_size,
                                       sizeof(char));
        char *tmp = NULL;
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
        BOOL tls_flag = this->message_mode == "s" ? true : false;
        int32_t decrypted_data_length = 0;
        int32_t completed_length = 0;
        int32_t count = 0;
        int32_t p = 0;


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

        if(tls_flag == false)
        {
            std::memcpy(buffer,
                        buffer_http_header,
                        http_header_length);

            std::memcpy(buffer + http_header_length,
                        buffer_http_body,
                        http_body_length);

            len = http_header_length + http_body_length;
            send_length = 0;

#ifdef DEBUGPRINT
//            std::printf("len: %d\n", len);
//            print_bytes(buffer, len);
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
                    free(buffer2);
                    free(buffer_http_header);
                    free(buffer_http_body);
                    return 0;
                }else if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[+] do_http_connection_client select error: 0x%x\n",
                                ret);
#endif
                    free(buffer);
                    free(buffer2);
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
                            free(buffer2);
                            free(buffer_http_header);
                            free(buffer_http_body);
                            return -1;
                        }
                    }
                    send_length += sen;
                    len -= sen;
                }
            }
        }else
        {
            std::memset(buffer,
                        0,
                        buffer_size);

            std::memcpy(buffer2,
                        buffer_http_header,
                        http_header_length);

            std::memcpy(buffer2 + http_header_length,
                        buffer_http_body,
                        http_body_length);

            total_length = http_header_length + http_body_length;
            remaining_size = total_length;
            count = total_length / (int32_t)stream_sizes.cbMaximumMessage + 1;
            p = 0;

            for(int32_t i=0; i<count; i++)
            {
                if(remaining_size > (int32_t)stream_sizes.cbMaximumMessage)
                {
                    len = encrypt_data_tls(buffer2 + p,
                                           (int32_t)stream_sizes.cbMaximumMessage,
                                           buffer,
                                           buffer_size);
                    if(len < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_client encrypt_data_tls error: %d\n",
                                    len);
#endif
                        free(buffer);
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }

                    p += (int32_t)stream_sizes.cbMaximumMessage;
                    remaining_size -= (int32_t)stream_sizes.cbMaximumMessage;
                }else
                {
                    len = encrypt_data_tls(buffer2 + p,
                                           remaining_size,
                                           buffer,
                                           buffer_size);
                    if(len < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_client encrypt_data_tls error: %d\n",
                                    len);
#endif
                        free(buffer);
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }

                    p += remaining_size;
                    remaining_size = 0;
                }

                send_length = 0;

#ifdef DEBUGPRINT
//                std::printf("len: %d\n", len);
//                print_bytes(buffer, len);
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
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return 0;
                    }else if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[+] do_http_connection_client select error: 0x%x\n",
                                    ret);
#endif
                        free(buffer);
                        free(buffer2);
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
                                free(buffer2);
                                free(buffer_http_header);
                                free(buffer_http_body);
                                return -1;
                            }
                        }
                        send_length += sen;
                        len -= sen;
                    }
                }
            }
        }


        free(buffer_http_header);
        free(buffer_http_body);

        std::memset(buffer,
                    0,
                    buffer_size);

        std::memset(buffer2,
                    0,
                    buffer_size);

        tmp = (char *)calloc(buffer_size,
                             sizeof(char));

        http_header_length = 0;
        http_body_length = 0;
        remaining_size = 0;
        tmprec = 0;
        decrypted_data_length = 0;


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
                free(buffer2);
                free(tmp);
                return 0;
            }else if(ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
#ifdef DEBUGPRINT
                std::printf("[+] do_http_connection_client select error:0x%x\n",
                            ret);
#endif
                free(buffer);
                free(buffer2);
                free(tmp);
                return 0;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret > 0)
            {
                if(tls_flag == false)   // http
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
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }else
                    {
                        rec += tmprec;
                        tmprec = 0;

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
                                free(buffer2);
                                free(tmp);
                                return -1;
                            }else if(total_length > buffer_size)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] do_http_connection_client http size error buffer_size: %d total_length: %d\n",
                                            buffer_size,
                                            total_length);
#endif
                                free(buffer);
                                free(buffer2);
                                free(tmp);
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

                        break;
                    }
                }else   // https
                {
                    if(remaining_size == 0)
                    {
                        rec = recv(sock,
                                   buffer2,
                                   buffer_http_header_size,
                                   0);
                    }else
                    {
                        rec = recv(sock,
                                   buffer2,
                                   remaining_size,
                                   0);
                    }

                    if(rec == SOCKET_ERROR)
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
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }else if(rec == 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_client error rec: %d\n",
                                    rec);
#endif
                        free(buffer);
                        free(buffer2);
                        free(tmp);
                        return -1;
                    }else
                    {
                        if(tmprec + rec <= buffer_size)
                        {
                            std::memcpy(tmp + tmprec,
                                        buffer2,
                                        rec);
                            tmprec += rec;

                            std::memset(buffer2,
                                        0,
                                        buffer_size);

                            remaining_size = 0;

                            ret = decrypt_data_tls(tmp,
                                                   &tmprec,
                                                   buffer + completed_length,
                                                   buffer_size - completed_length,
                                                   &decrypted_data_length,
                                                   &remaining_size);
                            if(ret == 1)    // SEC_E_INCOMPLETE_MESSAGE
                            {
                                continue;
                            }else if(ret == 2)  // SEC_E_DECRYPT_FAILURE
                            {
                                decrypted_data_length = 0;
                                tmprec = 0;
                                std::memset(tmp,
                                            0,
                                            buffer_size);
                                continue;
                            }else if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] do_http_connection_client decrypt_data_tls error\n");
#endif
                                free(buffer);
                                free(buffer2);
                                free(tmp);
                                return -1;
                            }else
                            {
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
                                    free(buffer2);
                                    free(tmp);
                                    return -1;
                                }else if(total_length > buffer_size)
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] do_http_connection_client http size error buffer_size: %d total_length: %d\n",
                                                buffer_size,
                                                total_length);
#endif
                                    free(buffer);
                                    free(buffer2);
                                    free(tmp);
                                    return -1;
                                }

                                completed_length += decrypted_data_length;

                                decrypted_data_length = 0;
                                tmprec = 0;
                                std::memset(tmp,
                                            0,
                                            buffer_size);

                                remaining_size = total_length - completed_length;
#ifdef DEBUGPRINT
//                                std::printf("[+] remaining_size: %d\n", remaining_size);
#endif
                                if(remaining_size > 0)
                                {
                                    continue;
                                }

                                break;
                            }
                        }else{
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_client received data size has exceeded the maximum value\n");
#endif
                            free(buffer);
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }
                }
            }
        }

#ifdef DEBUGPRINT
//        std::printf("rec: %d\n", rec);
//        print_bytes(buffer, rec);
#endif

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

        free(buffer);
        free(buffer2);
        free(tmp);
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
        char *buffer2 = (char *)calloc(buffer_size,
                                       sizeof(char));
        char *tmp = (char *)calloc(buffer_size,
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
        BOOL tls_flag = this->message_mode == "s" ? true : false;
        int32_t decrypted_data_length = 0;
        int32_t completed_length = 0;
        int32_t count = 0;
        int32_t p = 0;


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
                if(tls_flag == false)   // http
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
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }else
                    {
                        rec += tmprec;
                        tmprec = 0;

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
                                free(buffer2);
                                free(tmp);
                                return -1;
                            }else if(total_length > buffer_size)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] do_http_connection_server http size error buffer_size: %d total_length: %d\n",
                                            buffer_size,
                                            total_length);
#endif
                                free(buffer);
                                free(buffer2);
                                free(tmp);
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

                        break;
                    }
                }else   // https
                {
                    if(remaining_size == 0)
                    {
                        rec = recv(sock,
                                   buffer2,
                                   buffer_http_header_size,
                                   0);
                    }else
                    {
                        rec = recv(sock,
                                   buffer2,
                                   remaining_size,
                                   0);
                    }

                    if(rec == SOCKET_ERROR)
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
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }else if(rec == 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_server error rec: %d\n",
                                    rec);
#endif
                        free(buffer);
                        free(buffer2);
                        free(tmp);
                        return -1;
                    }else
                    {
                        if(tmprec + rec <= buffer_size)
                        {
                            std::memcpy(tmp + tmprec,
                                        buffer2,
                                        rec);
                            tmprec += rec;

                            std::memset(buffer2,
                                        0,
                                        buffer_size);

                            remaining_size = 0;

                            ret = decrypt_data_tls(tmp,
                                                   &tmprec,
                                                   buffer + completed_length,
                                                   buffer_size - completed_length,
                                                   &decrypted_data_length,
                                                   &remaining_size);
                            if(ret == 1)    // SEC_E_INCOMPLETE_MESSAGE
                            {
                                continue;
                            }else if(ret == 2)  // SEC_E_DECRYPT_FAILURE
                            {
                                decrypted_data_length = 0;
                                tmprec = 0;
                                std::memset(tmp,
                                            0,
                                            buffer_size);
                                continue;
                            }else if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] do_http_connection_server decrypt_data_tls error\n");
#endif
                                free(buffer);
                                free(buffer2);
                                free(tmp);
                                return -1;
                            }else
                            {
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
                                    free(buffer2);
                                    free(tmp);
                                    return -1;
                                }else if(total_length > buffer_size)
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] do_http_connection_server http size error buffer_size: %d total_length: %d\n",
                                                buffer_size,
                                                total_length);
#endif
                                    free(buffer);
                                    free(buffer2);
                                    free(tmp);
                                    return -1;
                                }

                                completed_length += decrypted_data_length;

                                decrypted_data_length = 0;
                                tmprec = 0;
                                std::memset(tmp,
                                            0,
                                            buffer_size);

                                remaining_size = total_length - completed_length;
#ifdef DEBUGPRINT
//                                std::printf("[+] remaining_size: %d\n", remaining_size);
#endif
                                if(remaining_size > 0)
                                {
                                    continue;
                                }

                                break;
                            }
                        }else{
#ifdef DEBUGPRINT
                            std::printf("[-] do_http_connection_server received data size has exceeded the maximum value\n");
#endif
                            free(buffer);
                            free(buffer2);
                            free(tmp);
                            return -1;
                        }
                    }
                }
            }
        }

#ifdef DEBUGPRINT
//        std::printf("rec: %d\n", rec);
//        print_bytes(buffer, rec);
#endif

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


        free(tmp);

        std::memset(buffer,
                    0,
                    buffer_size);

        std::memset(buffer2,
                    0,
                    buffer_size);

        buffer_http_header = (char *)calloc(buffer_http_header_size,
                                            sizeof(char));
        buffer_http_body = (char *)calloc(buffer_http_body_size,
                                          sizeof(char));

        http_header_length = 0;
        http_body_length = 0;
        remaining_size = 0;


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

        if(tls_flag == false)
        {
            std::memcpy(buffer,
                        buffer_http_header,
                        http_header_length);

            std::memcpy(buffer + http_header_length,
                        buffer_http_body,
                        http_body_length);

            len = http_header_length + http_body_length;
            send_length = 0;

#ifdef DEBUGPRINT
//            std::printf("len: %d\n", len);
//            print_bytes(buffer, len);
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
                    free(buffer2);
                    free(buffer_http_header);
                    free(buffer_http_body);
                    return 0;
                }else if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[+] do_http_connection_server select error: 0x%x\n",
                                ret);
#endif
                    free(buffer);
                    free(buffer2);
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
                            free(buffer2);
                            free(buffer_http_header);
                            free(buffer_http_body);
                            return -1;
                        }
                    }
                    send_length += sen;
                    len -= sen;
                }
            }
        }else
        {
            std::memset(buffer,
                        0,
                        buffer_size);

            std::memcpy(buffer2,
                        buffer_http_header,
                        http_header_length);

            std::memcpy(buffer2 + http_header_length,
                        buffer_http_body,
                        http_body_length);

            total_length = http_header_length + http_body_length;
            remaining_size = total_length;
            count = total_length / (int32_t)stream_sizes.cbMaximumMessage + 1;
            p = 0;

            for(int32_t i=0 ; i<count; i++)
            {
                if(remaining_size > (int32_t)stream_sizes.cbMaximumMessage)
                {
                    len = encrypt_data_tls(buffer2 + p,
                                           (int32_t)stream_sizes.cbMaximumMessage,
                                           buffer,
                                           buffer_size);
                    if(len < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_server encrypt_data_tls error: %d\n",
                                    len);
#endif
                        free(buffer);
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }

                    p += (int32_t)stream_sizes.cbMaximumMessage;
                    remaining_size -= (int32_t)stream_sizes.cbMaximumMessage;
                }else
                {
                    len = encrypt_data_tls(buffer2 + p,
                                           remaining_size,
                                           buffer,
                                           buffer_size);
                    if(len < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] do_http_connection_server encrypt_data_tls error: %d\n",
                                    len);
#endif
                        free(buffer);
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return -1;
                    }

                    p += remaining_size;
                    remaining_size = 0;
                }

                send_length = 0;

#ifdef DEBUGPRINT
//                std::printf("len: %d\n", len);
//                print_bytes(buffer, len);
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
                        free(buffer2);
                        free(buffer_http_header);
                        free(buffer_http_body);
                        return 0;
                    }else if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[+] do_http_connection_server select error: 0x%x\n",
                                    ret);
#endif
                        free(buffer);
                        free(buffer2);
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
                                free(buffer2);
                                free(buffer_http_header);
                                free(buffer_http_body);
                                return -1;
                            }
                        }
                        send_length += sen;
                        len -= sen;
                    }
                }
            }

        }

        free(buffer);
        free(buffer2);
        free(buffer_http_header);
        free(buffer_http_body);
        return 0;
    }
}
