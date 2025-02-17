/*
 * Title:  spider client.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5.hpp"
#include "client.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "encryption.hpp"


namespace spider
{
    Client::Client(std::string type,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string client_ip,
                   std::string client_listen_port,
                   std::string client_port,
                   std::string destination_spider_ip,
                   SOCKET client_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   std::shared_ptr<Encryption> encryption,
                   std::shared_ptr<Messagemanager> message_manager)
    : Node(client_sock, message_manager)
    {
        this->type = type;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->client_ip = client_ip;
        this->client_listen_port = client_listen_port;
        this->client_port = client_port;
        this->destination_spider_ip = destination_spider_ip;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->encryption = encryption;

        this->sock = client_sock;
        this->message_manager = message_manager;
    }

    Client::Client(std::string type,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string client_ip,
                   std::string client_listen_port,
                   std::string client_port,
                   std::string destination_spider_ip,
                   std::string target_ip,
                   std::string target_port,
                   SOCKET client_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   std::shared_ptr<Encryption> encryption,
                   std::shared_ptr<Messagemanager> message_manager)
    : Node(client_sock, message_manager)
    {
        this->type = type;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->client_ip = client_ip;
        this->client_listen_port = client_listen_port;
        this->client_port = client_port;
        this->destination_spider_ip = destination_spider_ip;
        this->target_ip = target_ip;
        this->target_port = target_port;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->encryption = encryption;

        this->sock = client_sock;
        this->message_manager = message_manager;
    }

    Client::~Client()
    {

    }

    void Client::set_type(std::string type)
    {
        this->type = type;
    }

    std::string Client::get_type()
    {
        return type;
    }

    void Client::set_connection_id(uint32_t connection_id)
    {
        this->connection_id = connection_id;
    }

    uint32_t Client::get_connection_id()
    {
        return connection_id;
    }

    void Client::set_client_id(uint32_t client_id)
    {
        this->client_id = client_id;
    }

    uint32_t Client::get_client_id()
    {
        return client_id;
    }

    void Client::set_server_id(uint32_t server_id)
    {
        this->server_id = server_id;
    }

    uint32_t Client::get_server_id()
    {
        return server_id;
    }

    void Client::set_client_ip(std::string client_ip)
    {
        this->client_ip = client_ip;
    }

    std::string Client::get_client_ip()
    {
        return client_ip;
    }

    void Client::set_client_listen_port(std::string client_listen_port)
    {
        this->client_listen_port = client_listen_port;
    }

    std::string Client::get_client_listen_port()
    {
        return client_listen_port;
    }

    void Client::set_client_port(std::string client_port)
    {
        this->client_port = client_port;
    }

    std::string Client::get_client_port()
    {
        return client_port;
    }

    void Client::set_destination_spider_ip(std::string destination_spider_ip)
    {
        this->destination_spider_ip = destination_spider_ip;
    }

    std::string Client::get_destination_spider_ip()
    {
        return destination_spider_ip;
    }

    void Client::set_target_ip(std::string target_ip)
    {
        this->target_ip = target_ip;
    }

    std::string Client::get_target_ip()
    {
        return target_ip;
    }

    void Client::set_target_port(std::string target_port)
    {
        this->target_port = target_port;
    }

    std::string Client::get_target_port()
    {
        return target_port;
    }

    void Client::set_tv_sec(int32_t tv_sec)
    {
        this->tv_sec = tv_sec;
    }

    int32_t Client::get_tv_sec()
    {
        return tv_sec;
    }

    void Client::set_tv_usec(int32_t tv_usec)
    {
        this->tv_usec = tv_usec;
    }

    int32_t Client::get_tv_usec()
    {
        return tv_usec;
    }

    void Client::set_forwarder_tv_sec(int32_t forwarder_tv_sec)
    {
        this->forwarder_tv_sec = forwarder_tv_sec;
    }

    int32_t Client::get_forwarder_tv_sec()
    {
        return forwarder_tv_sec;
    }

    void Client::set_forwarder_tv_usec(int32_t forwarder_tv_usec)
    {
        this->forwarder_tv_usec = forwarder_tv_usec;
    }

    int32_t Client::get_forwarder_tv_usec()
    {
        return forwarder_tv_usec;
    }

    void Client::set_encryption(std::shared_ptr<Encryption> encryption)
    {
        this->encryption = encryption;
    }

    std::shared_ptr<Encryption> Client::get_encryption()
    {
        return encryption;
    }

    int32_t Client::recv_message(char *buffer,
                                 int32_t buffer_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec,
                                 BOOL register_server_id_flag)
    {
        int32_t ret = 0;
        char message_type;
        int32_t rec = 0;
        std::shared_ptr<Socks5message> socks5_message;


        std::memset(buffer,
                    0,
                    buffer_size);

        socks5_message = pop_timeout_socks5_message(tv_sec,
                                                    tv_usec);
        if(socks5_message !=  nullptr)
        {
            message_type = socks5_message->get_message_type();
            if(message_type == 's')     // socks5 message
            {
                rec = socks5_message->get_data_size();
                if(rec >= 0)
                {
                    if(register_server_id_flag)
                    {
                        set_server_id(socks5_message->get_server_id());
                    }

                    std::memcpy(buffer,
                                socks5_message->get_data(),
                                rec);

#ifdef DEBUGPRINT
                    print_bytes(buffer, rec);
#endif

                    if(encryption != nullptr
                       && encryption->get_flag())
                    {
                        ret = encryption->decrypt(buffer,
                                          rec,
                                          buffer_size);
                        if(ret <= 0)
                        {
#ifdef _DEBUG
                            std::printf("[-] recv_message decrypt error: %d\n",
                                        ret);
#endif
                            return -1;
                        }

                        rec = ret;
                    }
                }else
                {
#ifdef DEBUGPRINT
                    printf("[-] recv_message recv error: %d\n",
                           rec);
#endif
                    return -1;
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_message unknown message type: %c\n",
                            message_type);
#endif
                return -1;
            }
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] recv_message error\n");
#endif
            return -1;
        }

        return rec;
    }

    int32_t Client::send_message(char *buffer,
                                 int32_t data_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec)
    {
        int32_t ret = 0;
        int32_t sen = 0;
        std::shared_ptr<Socks5message> socks5_message;
        struct socks5_message_data *socks5_message_data;


        if(data_size > SOCKS5_MESSAGE_DATA_SIZE){
#ifdef DEBUGPRINT
            std::printf("[-] send_message data size error: %d\n",
                        data_size);
#endif
            free(socks5_message_data);
            return -1;
        }

        if(encryption != nullptr
           && encryption->get_flag())
        {
            ret = encryption->encrypt(buffer,
                                      data_size,
                                      SOCKS5_MESSAGE_DATA_SIZE);
            if(ret <= 0)
            {
#ifdef _DEBUG
                std::printf("[-] send_message encrypt error: %d\n",
                            ret);
#endif
                free(socks5_message_data);
                return -1;
            }

            data_size = ret;
        }

#ifdef DEBUGPRINT
        print_bytes(buffer, data_size);
#endif

        socks5_message_data = (struct socks5_message_data *)calloc(1,
                                                            sizeof(struct socks5_message_data));

        socks5_message_data->message_type = 's';
        socks5_message_data->connection_id = connection_id;
        socks5_message_data->client_id = client_id;
        socks5_message_data->server_id = server_id;
        socks5_message_data->source_node_type = 'c';
        memcpy(&socks5_message_data->source_ip, client_ip.c_str(), client_ip.size());
        socks5_message_data->destination_node_type = 's';
        memcpy(&socks5_message_data->destination_ip, destination_spider_ip.c_str(), destination_spider_ip.size());
        socks5_message_data->tv_sec = tv_sec;
        socks5_message_data->tv_usec = tv_usec;
        socks5_message_data->forwarder_tv_sec = forwarder_tv_sec;
        socks5_message_data->forwarder_tv_usec = forwarder_tv_usec;
        socks5_message_data->data_size = data_size;
        memcpy(&socks5_message_data->data, buffer, data_size);

        socks5_message = std::make_shared<Socks5message>(socks5_message_data);

        ret = message_manager->push_timeout_socks5_message(socks5_message,
                                                           tv_sec,
                                                           tv_usec);
        if(ret < 0)
        {
            sen = -1;
        }else
        {
            sen = data_size;
        }

        free(socks5_message_data);
        return sen;
    }

    int32_t Client::forwarder_recv_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        fd_set readfds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(NULL,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_recv_data select timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_recv_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
#ifdef DEBUGPRINT
                std::printf("[+] [client -> client] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(sock,
                           buffer,
                           socks5_message_data_max_size,
                           0);
                if(rec == SOCKET_ERROR)
                {
                    ret =  WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] forwarder_recv_data recv error: %d\n",
                                    ret);
#endif
                        break;
                    }
                }else if(rec > 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [client -> server] send_message\n");
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0){
                        break;
                    }
                }else
                {
                    break;
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Client::forwarder_send_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set writefds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(sock, &writefds);
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_send_data timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR){
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_send_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret){
#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] recv_message\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv_message(buffer,
                                   buffer_max_length,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec,
                                   false);
                if(rec > 0)
                {
                    len = rec;
                    send_length = 0;
#ifdef DEBUGPRINT
                    std::printf("[+] [client <- client] send\n");
#endif
                    while(len > 0)
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
                                std::printf("[-] forwarder_send_data send error: %d\n",
                                            ret);
#endif
                                break;
                            }
                        }
                        send_length += sen;
                        len -= sen;
                    }
                }else
                {
                    break;
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Client::forwarder()
    {
        std::thread thread1(&Client::forwarder_recv_data, this);
        std::thread thread2(&Client::forwarder_send_data, this);

        thread1.join();
        thread2.join();

        return 0;
    }

    int32_t Client::do_socks5_connection()
    {
        int32_t rec;
        int32_t sen;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        // socks SELECTION_REQUEST [client -> client]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> client] recv selection request\n");
#endif
        rec = recv_data(buffer,
                        socks5_message_data_max_size,
                        tv_sec,
                        tv_usec);
        if(rec <= 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> client] recv selection request error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client -> client] recv selection request: %d bytes\n",
                    rec);
#endif


        // socks SELECTION_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send selection request\n");
#endif

        sen = send_message(buffer,
                           rec,
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif


        // socks SELECTION_RESPONSE [client <- client]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- client] send selection response\n");
#endif
        sen = send_data(buffer,
                        rec,
                        tv_sec,
                        tv_usec);
#ifdef DEBUGPRINT
        std::printf("[+] [client <- client] send selection response: %d bytes\n",
                    sen);
#endif
        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks5server authentication method error\n");
#endif
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST client -> client
#ifdef DEBUGPRINT
            std::printf("[+] [client -> client] recv username password authentication request\n");
#endif
            rec = recv_data(buffer,
                            socks5_message_data_max_size,
                            tv_sec,
                            tv_usec);
            if(ret <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client -> client] recv username password authentication request error\n");
#endif
                free(buffer);
                return -1;
            }
#ifdef DEBUGPRINT
            std::printf("[+] [client -> client] recv username password authentication request: %d bytes\n",
                        rec);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] send username password authentication request\n");
#endif

            sen = send_message(buffer,
                               rec,
                               tv_sec,
                               tv_usec);

#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }
#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- client]
#ifdef DEBUGPRINT
            std::printf("[+] [client <- client] send username password authentication response\n");
#endif
            sen = send_data(buffer,
                            rec,
                            tv_sec,
                            tv_usec);
#ifdef DEBUGPRINT
            std::printf("[+] [client <- client] send username password authentication response: %d bytes\n",
                        sen);
#endif
        }


        // socks SOCKS_REQUEST [client -> client]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> client] recv socks request\n");
#endif
        rec = recv_data(buffer,
                        socks5_message_data_max_size,
                        tv_sec,
                        tv_usec);
        if(rec <= 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> client] recv socks request error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client -> client] recv socks request: %d bytes\n",
                    rec);
#endif


        // socks SOCKS_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send socks request\n");
#endif

        sen = send_message(buffer,
                           rec,
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif


        // socks SOCKS_RESPONSE [client <- client]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- client] send socks response\n");
#endif
        sen = send_data(buffer,
                        rec,
                        tv_sec,
                        tv_usec);
#ifdef DEBUGPRINT
        std::printf("[+] [client <- client] send socks response: %d bytes\n",
                    sen);
#endif


        // forwarder [client <> client <> server <> target]
#ifdef DEBUGPRINT
        std::printf("[+] [client <> client <> server <> target] forwarder\n");
#endif
        forwarder();

#ifdef DEBUGPRINT
        printf("[+] worker exit\n");
#endif

        free(buffer);
        return -1;
    }

    int32_t Client::forwarder_udp_recv_send_data(int addr_length)
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set readfds;
        fd_set writefds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;

        struct sockaddr *client_addr;
        int client_addr_length = addr_length;
        if(client_addr_length == sizeof(struct sockaddr_in))    // ipv4
        {
            client_addr = (struct sockaddr *)calloc(1,
                                                    sizeof(struct sockaddr_in));
        }else   // ipv6
        {
            client_addr = (struct sockaddr *)calloc(1,
                                                    sizeof(struct sockaddr_in6));
        }


        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            tv.tv_sec = FORWARDER_UDP_TIMEOUT;
            tv.tv_usec = 0;

            ret = select(NULL,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_udp_recv_send_data select error: 0x%x\n",
                            ret);
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
#ifdef DEBUGPRINT
                std::printf("[+] [client -> client] recvfrom\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recvfrom(sock,
                               buffer,
                               socks5_message_data_max_size,
                               0,
                               client_addr,
                               (socklen_t *)&client_addr_length);
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
                        printf("[-] forwarder_udp_recv_send_data recvfrom error: %d\n",
                               ret);
#endif
//                        break;
                    }
                }else if(rec > 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [client -> server] send_message\n");
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0){
//                        break;
                    }
                }else
                {
//                    break;
                }


                FD_ZERO(&writefds);
                FD_SET(sock, &writefds);
                tv.tv_sec = forwarder_tv_sec;
                tv.tv_usec = forwarder_tv_usec;

                ret = select(NULL,
                             NULL,
                             &writefds,
                             NULL,
                             &tv);
                if(ret == 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
//                    break;
                }else if(ret == SOCKET_ERROR)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] forwarder_udp_recv_send_data select error: 0x%x\n",
                                ret);
#endif
//                    break;
                }

                ret = FD_ISSET(sock,
                               &writefds);
                if(ret){
#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] recv_message\n");
#endif
                    std::memset(buffer,
                                0,
                                buffer_max_length);

                    rec = recv_message(buffer,
                                       buffer_max_length,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec,
                                       false);
                    if(rec > 0)
                    {
                        len = rec;
                        send_length = 0;
#ifdef DEBUGPRINT
                        std::printf("[+] [client <- client] sendto\n");
#endif
                        while(len > 0)
                        {
                            sen = sendto(sock,
                                         buffer+send_length,
                                         len,
                                         0,
                                         client_addr,
                                         client_addr_length);
                            if(sen == SOCKET_ERROR)
                            {
                                ret =  WSAGetLastError();
                                if(ret == WSAEWOULDBLOCK)
                                {
                                    std::this_thread::sleep_for(std::chrono::microseconds(5000));
                                    continue;
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] forwarder_send_data sendto error: %d\n",
                                                ret);
#endif
//                                    free(client_addr);
//                                    free(buffer);
//                                    return 0;
                                }
                            }
                            send_length += sen;
                            len -= sen;
                        }
                    }else
                    {
//                        break;
                    }
                }
            }
        }

        free(client_addr);
        free(buffer);
        return 0;
    }

    int32_t Client::forwarder_udp(int addr_length)
    {
        std::thread thread(&Client::forwarder_udp_recv_send_data, this, addr_length);

        thread.join();

        return 0;
    }

    int32_t Client::do_socks5_connection_udp()
    {
        static char authentication_method = 0x0; // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = "socks5user";
        char password[256] = "supersecretpassword";
        uint8_t username_length = std::strlen(username);
        uint8_t password_length = std::strlen(password);

        uint8_t target_ip_length = target_ip.size();
        uint16_t target_port_tmp = htons((uint16_t)std::atoi(target_port.c_str()));

        int32_t rec;
        int32_t sen;
        int32_t send_length = 0;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;

        struct sockaddr_in client_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 client_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *client_addr_info;
        int family = 0;
        int addr_length = 0;
        int client_addr_length = sizeof(client_addr);
        int client_addr6_length = sizeof(client_addr6);
        char client_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *client_addr6_string_pointer = client_addr6_string;

        std::memset((char *)&client_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&client_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&hints,
                    0,
                    sizeof(struct addrinfo));


        // socks SELECTION_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send selection request\n");
#endif

        std::memset(buffer,
                    0,
                    buffer_max_length);
        struct selection_request *selection_request = (struct selection_request *)buffer;
        selection_request->ver = 0x5;
        selection_request->nmethods = 0x1;
        selection_request->methods[0] = authentication_method;
        send_length = 3;

        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif

        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef DEBUGPRINT
            std::printf("[-] authentication method error\n");
#endif
            free(buffer);
            return -1;
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] send username password authentication request\n");
#endif

            std::memset(buffer,
                        0,
                        buffer_max_length);
            struct username_password_authentication_request *username_password_authentication_request = (struct username_password_authentication_request *)buffer;
            username_password_authentication_request->ver = 0x1;
            username_password_authentication_request->ulen = username_length;
            std::memcpy(username_password_authentication_request->uname,
                        username,
                        username_length);
            username_password_authentication_request->plen = password_length;
            std::memcpy(username_password_authentication_request->passwd,
                        password,
                        password_length);

            send_length = 3 + username_length + password_length;

            sen = send_message(buffer,
                               send_length,
                               tv_sec,
                               tv_usec);

#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }
#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif

            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;

            if(username_password_authentication_response->status != 0x0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] username password authentication error: 0x%02x\n",
                            username_password_authentication_response->status);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send socks request\n");
#endif

        std::memset(buffer,
                    0,
                    buffer_max_length);

        struct socks_request_domainname *socks_request_domainname = (struct socks_request_domainname *)buffer;
        socks_request_domainname->ver = 0x5;
        socks_request_domainname->cmd = 0x8;    // CONNECT UDP (0x8, UDP over TCP, original command)
        socks_request_domainname->rsv = 0x0;
        socks_request_domainname->atyp = 0x3;   // domainname
        socks_request_domainname->dst_addr_len = target_ip_length;
        std::memcpy((char *)&socks_request_domainname->dst_addr,
                    target_ip.c_str(),
                    target_ip_length);
        std::memcpy((char *)&socks_request_domainname->dst_addr+target_ip_length,
                    (char *)&target_port_tmp,
                    2);
        send_length = 5 + target_ip_length + 2;

        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif

        struct socks_response *socks_response = (struct socks_response *)buffer;
        if(socks_response->ver != 0x5
            && socks_response->rep != 0x0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks response error: 0x%02x\n",
                        socks_response->rep);
#endif
            free(buffer);
            return -1;
        }


        if(client_ip.find(":") == std::string::npos)  // ipv4 address
        {
            hints.ai_family = AF_INET;
            ret = getaddrinfo(client_ip.c_str(),
                              client_listen_port.c_str(),
                              &hints,
                              &client_addr_info);
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] cannot resolv the address: %s\n",
                            client_ip.c_str());
#endif
                free(buffer);
                return -1;
            }
        }else   // ipv6 address
        {
            hints.ai_family = AF_INET6;
            ret = getaddrinfo(client_ip.c_str(),
                              client_listen_port.c_str(),
                              &hints,
                              &client_addr_info);
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] cannot resolv the address: %s\n",
                            client_ip.c_str());
#endif
                free(buffer);
                return -1;
            }
        }

        if(client_addr_info->ai_family == AF_INET)
        {
            tmp_ipv4 = (struct sockaddr_in *)client_addr_info->ai_addr;

            family = AF_INET;
            client_addr.sin_family = AF_INET;
            std::memcpy(&client_addr.sin_addr,
                        &tmp_ipv4->sin_addr,
                        sizeof(struct in_addr));
            std::memcpy(&client_addr.sin_port,
                        &tmp_ipv4->sin_port,
                        2);

            freeaddrinfo(client_addr_info);

            addr_length = client_addr_length;

            sock = socket(AF_INET,
                          SOCK_DGRAM,
                          IPPROTO_UDP);
            if(sock == INVALID_SOCKET)
            {
#ifdef DEBUGPRINT
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
#endif
                free(buffer);
                return -1;
            }

            // bind
            ret = bind(sock,
                       (struct sockaddr *)&client_addr,
                       sizeof(client_addr));
            if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[-] bind error\n");
#endif
                closesocket(sock);
                free(buffer);
                return -1;
            }

#ifdef DEBUGPRINT
            std::printf("[+] bind port %d on %s\n",
                        ntohs(client_addr.sin_port),
                        inet_ntoa(client_addr.sin_addr));
#endif
        }else if(client_addr_info->ai_family == AF_INET6)
        {
            tmp_ipv6 = (struct sockaddr_in6 *)client_addr_info->ai_addr;

            family = AF_INET6;
            client_addr6.sin6_family = AF_INET6;
            std::memcpy(&client_addr6.sin6_addr,
                        &tmp_ipv6->sin6_addr,
                        sizeof(struct in6_addr));
            std::memcpy(&client_addr6.sin6_port,
                        &tmp_ipv6->sin6_port,
                        2);
            client_addr6.sin6_scope_id = tmp_ipv6->sin6_scope_id;

            freeaddrinfo(client_addr_info);

            addr_length = client_addr6_length;

            sock = socket(AF_INET6,
                          SOCK_DGRAM,
                          IPPROTO_UDP);
            if(sock == INVALID_SOCKET)
            {
#ifdef DEBUGPRINT
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
#endif
                free(buffer);
                return -1;
            }

            // bind
            ret = bind(sock,
                       (struct sockaddr *)&client_addr6,
                       sizeof(client_addr6));
            if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[-] bind error\n");
#endif
                closesocket(sock);
                free(buffer);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &client_addr6.sin6_addr,
                      client_addr6_string_pointer,
                      INET6_ADDRSTRLEN);

#ifdef DEBUGPRINT
            if(client_addr6.sin6_scope_id > 0)
            {
                std::printf("[+] bind port %d on %s%%%d\n",
                            ntohs(client_addr6.sin6_port),
                            client_addr6_string_pointer,
                            client_addr6.sin6_scope_id);
            }else
            {
                std::printf("[+] bind port %d on %s\n",
                            ntohs(client_addr6.sin6_port),
                            client_addr6_string_pointer);
            }
#endif
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] not implemented\n");
#endif
            freeaddrinfo(client_addr_info);
            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef DEBUGPRINT
        std::printf("[+] [client <> client <> server <> target] forwarder_udp\n");
#endif
        forwarder_udp(addr_length);

#ifdef DEBUGPRINT
        printf("[+] worker exit\n");
#endif

        closesocket(sock);
        free(buffer);
        return -1;
    }
}

