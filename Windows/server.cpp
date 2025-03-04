/*
 * Title:  spider server.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spiderip.hpp"
#include "socks5.hpp"
#include "server.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "encryption.hpp"


namespace spider
{
    Server::Server(std::shared_ptr<Spiderip> spider_ip,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string server_ip,
                   std::string server_port,
                   std::string client_destination_ip,
                   SOCKET server_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   std::shared_ptr<Encryption> encryption,
                   std::shared_ptr<Messagemanager> message_manager)
    : Node(server_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->server_ip = server_ip;
        this->server_port = server_port;
        this->client_destination_ip = client_destination_ip;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->encryption = encryption;

        this->sock = server_sock;
        this->target_sock = INVALID_SOCKET;
        this->message_manager = message_manager;
    }

    Server::~Server()
    {

    }

    void Server::set_spider_ip(std::shared_ptr<Spiderip> spider_ip)
    {
        this->spider_ip = spider_ip;
    }

    std::shared_ptr<Spiderip> Server::get_spider_ip()
    {
        return spider_ip;
    }

    void Server::set_connection_id(uint32_t connection_id)
    {
        this->connection_id = connection_id;
    }

    uint32_t Server::get_connection_id()
    {
        return connection_id;
    }

    void Server::set_client_id(uint32_t client_id)
    {
        this->client_id = client_id;
    }

    uint32_t Server::get_client_id()
    {
        return client_id;
    }

    void Server::set_server_id(uint32_t server_id)
    {
        this->server_id = server_id;
    }

    uint32_t Server::get_server_id()
    {
        return server_id;
    }

    void Server::set_server_ip(std::string server_ip)
    {
        this->server_ip = server_ip;
    }

    std::string Server::get_server_ip()
    {
        return server_ip;
    }

    void Server::set_server_port(std::string server_port)
    {
        this->server_port = server_port;
    }

    std::string Server::get_server_port()
    {
        return server_port;
    }

    void Server::set_client_destination_ip(std::string client_destination_ip)
    {
        this->client_destination_ip = client_destination_ip;
    }

    std::string Server::get_client_destination_ip()
    {
        return client_destination_ip;
    }

    void Server::set_target_ip(std::string target_ip)
    {
        this->target_ip = target_ip;
    }

    std::string Server::get_target_ip()
    {
        return target_ip;
    }

    void Server::set_target_port(std::string target_port)
    {
        this->target_port = target_port;
    }

    std::string Server::get_target_port()
    {
        return target_port;
    }

    void Server::set_target_sock(SOCKET target_sock)
    {
        this->target_sock = target_sock;
    }

    SOCKET Server::get_target_sock()
    {
        return target_sock;
    }

    void Server::set_tv_sec(int32_t tv_sec)
    {
        this->tv_sec = tv_sec;
    }

    int32_t Server::get_tv_sec()
    {
        return tv_sec;
    }

    void Server::set_tv_usec(int32_t tv_usec)
    {
        this->tv_usec = tv_usec;
    }

    int32_t Server::get_tv_usec()
    {
        return tv_usec;
    }

    void Server::set_forwarder_tv_sec(int32_t forwarder_tv_sec)
    {
        this->forwarder_tv_sec = forwarder_tv_sec;
    }

    int32_t Server::get_forwarder_tv_sec()
    {
        return forwarder_tv_sec;
    }

    void Server::set_forwarder_tv_usec(int32_t forwarder_tv_usec)
    {
        this->forwarder_tv_usec = forwarder_tv_usec;
    }

    int32_t Server::get_forwarder_tv_usec()
    {
        return forwarder_tv_usec;
    }

    void Server::set_encryption(std::shared_ptr<Encryption> encryption)
    {
        this->encryption = encryption;
    }

    std::shared_ptr<Encryption> Server::get_encryption()
    {
        return encryption;
    }

    int32_t Server::recv_message(char *buffer,
                                 int32_t buffer_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec)
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
                    recv_message_id = socks5_message->get_message_id();

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
#ifdef DEBUGPRINT
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
                    std::printf("[-] recv_message recv error: %d\n",
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

    int32_t Server::send_message(char *buffer,
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
#ifdef DEBUGPRINT
                std::printf("[-] send_message encrypt error: %d\n",
                            ret);
#endif
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
        socks5_message_data->message_id = send_message_id;
        socks5_message_data->connection_id = connection_id;
        socks5_message_data->client_id = client_id;
        socks5_message_data->server_id = server_id;
        socks5_message_data->source_node_type = 's';
        memcpy(&socks5_message_data->source_ip, server_ip.c_str(), server_ip.size());
        socks5_message_data->destination_node_type = 'c';
        memcpy(&socks5_message_data->destination_ip, client_destination_ip.c_str(), client_destination_ip.size());
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



    int32_t Server::forwarder_recv_data()
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
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(target_sock, &writefds);
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
                std::printf("[+] forwarder_recv_data timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_recv_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &writefds);
            if(ret){
#ifdef DEBUGPRINT
                std::printf("[+] [client -> server] recv_message\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv_message(buffer,
                                   buffer_max_length,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                if(rec > 0)
                {
                    if(recv_message_id == next_recv_message_id)
                    {
                        len = rec;
                        send_length = 0;
#ifdef DEBUGPRINT
                        std::printf("[+] [server -> target] send message_id:%u\n",
                                    next_recv_message_id);
#endif
                        while(len > 0)
                        {
                            sen = send(target_sock,
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
                                    std::printf("[-] forwarder_recv_data send error: %d\n",
                                                ret);
#endif
                                    break;
                                }
                            }
                            send_length += sen;
                            len -= sen;
                        }

                        next_recv_message_id++;
                    }else
                    {
                        msg = std::make_pair(rec,
                                             buffer);
                        msgs_map.insert({recv_message_id, msg});

                        check_msg_count = msgs_map.count(next_recv_message_id);
                        while(check_msg_count > 0)
                        {
                            msg = msgs_map[next_recv_message_id];
                            msgs_map.erase(next_recv_message_id);

                            len = msg.first;
                            buffer = msg.second;
                            send_length = 0;
#ifdef DEBUGPRINT
                            std::printf("[+] [server -> target] send message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
                            {
                                sen = send(target_sock,
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
                                        std::printf("[-] forwarder_recv_data send error: %d\n",
                                                    ret);
#endif
                                        break;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            free(buffer);
                            next_recv_message_id++;
                            check_msg_count = msgs_map.count(next_recv_message_id);
                        }

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                    }
                }else
                {
                    break;
                }
            }
        }

        for(auto it = msgs_map.begin(); it != msgs_map.end();)
        {
            msg = it->second;
            free(msg.second);
            it = msgs_map.erase(it);
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_send_data()
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
            FD_SET(target_sock, &readfds);
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
                std::printf("[+] forwarder_send_data timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_send_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &readfds);
            if(ret)
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server <- target] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(target_sock,
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
                        std::printf("[-] forwarder_send_data recv error: %d\n",
                                    ret);
#endif
                        break;
                    }
                }else if(rec > 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] send_message message_id:%u\n",
                                send_message_id);
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0)
                    {
                        break;
                    }

                    send_message_id++;
                }else
                {
                    break;
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder()
    {
        std::thread thread1(&Server::forwarder_recv_data,
                            this);
        std::thread thread2(&Server::forwarder_send_data,
                            this);

        thread1.join();
        thread2.join();

        return 0;
    }

    int32_t Server::send_socks_response_ipv4(char *buffer,
                                             int32_t buffer_size,
                                             char ver,
                                             char rep,
                                             char rsv,
                                             char atyp)
    {
        int32_t sen = 0;
        std::memset(buffer,
                    0,
                    buffer_size);

        struct socks_response_ipv4 *socks_response_ipv4 = (struct socks_response_ipv4 *)buffer;

        socks_response_ipv4->ver = ver;              // protocol version
        socks_response_ipv4->rep = rep;              // Connection refused
        socks_response_ipv4->rsv = rsv;              // RESERVED
        socks_response_ipv4->atyp = atyp;            // IPv4
        std::memset(socks_response_ipv4->bnd_addr,   // BND.ADDR
                    0,
                    4);
        std::memset(socks_response_ipv4->bnd_port,   // BND.PORT
                    0,
                    2);

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct socks_response_ipv4),
                           tv_sec,
                           tv_usec);

        return sen;
    }

    int32_t Server::send_socks_response_ipv6(char *buffer,
                                             int32_t buffer_size,
                                             char ver,
                                             char rep,
                                             char rsv,
                                             char atyp)
    {
        int32_t sen = 0;
        std::memset(buffer,
                    0,
                    buffer_size);

        struct socks_response_ipv6 *socks_response_ipv6 = (struct socks_response_ipv6 *)buffer;

        socks_response_ipv6->ver = ver;                // protocol version
        socks_response_ipv6->rep = rep;                // Connection refused
        socks_response_ipv6->rsv = rsv;                // RESERVED
        socks_response_ipv6->atyp = atyp;              // IPv6
        std::memset(socks_response_ipv6->bnd_addr,     // BND.ADDR
                    0,
                    16);
        std::memset(socks_response_ipv6->bnd_port,     // BND.PORT
                    0,
                    2);

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct socks_response_ipv6),
                           tv_sec,
                           tv_usec);

        return sen;
    }

    int32_t Server::forwarder_udp_recv_send_data(struct sockaddr *target_addr,
                                                 int target_addr_length)
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
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(target_sock, &writefds);
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
//                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_udp_recv_send_data select error: 0x%x\n",
                            ret);
#endif
//                break;
            }

            ret = FD_ISSET(target_sock,
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
                                   FORWARDER_UDP_TIMEOUT,
                                   0);
                if(rec > 0)
                {
                    if(recv_message_id == next_recv_message_id)
                    {
                        len = rec;
                        send_length = 0;
#ifdef DEBUGPRINT
                        std::printf("[+] [client <- client] sendto message_id:%u\n",
                                    next_recv_message_id);
#endif
                        while(len > 0)
                        {
                            sen = sendto(target_sock,
                                         buffer+send_length,
                                         len,
                                         0,
                                         target_addr,
                                         target_addr_length);
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
//                                  free(buffer);
//                                  return 0;
                                }
                            }
                            send_length += sen;
                            len -= sen;
                        }

                        next_recv_message_id++;
                    }else
                    {
                        msg = std::make_pair(rec,
                                             buffer);
                        msgs_map.insert({recv_message_id, msg});

                        check_msg_count = msgs_map.count(next_recv_message_id);
                        while(check_msg_count > 0)
                        {
                            msg = msgs_map[next_recv_message_id];
                            msgs_map.erase(next_recv_message_id);

                            len = msg.first;
                            buffer = msg.second;
                            send_length = 0;
#ifdef DEBUGPRINT
                            std::printf("[+] [client <- client] sendto message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
                            {
                                sen = sendto(target_sock,
                                             buffer+send_length,
                                             len,
                                             0,
                                             target_addr,
                                             target_addr_length);
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
//                                      free(buffer);
//                                      return 0;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            free(buffer);
                            next_recv_message_id++;
                            check_msg_count = msgs_map.count(next_recv_message_id);
                        }

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                    }
                }else
                {
                    break;
                }


                FD_ZERO(&readfds);
                FD_SET(target_sock, &readfds);
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

                ret = FD_ISSET(target_sock,
                               &readfds);
                if(ret)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [client -> client] recvfrom\n");
#endif
                    std::memset(buffer,
                                0,
                                buffer_max_length);

                    rec = recvfrom(target_sock,
                                   buffer,
                                   socks5_message_data_max_size,
                                   0,
                                   target_addr,
                                   (socklen_t *)&target_addr_length);
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
                            std::printf("[-] forwarder_udp_recv_send_data recvfrom error: %d\n",
                                        ret);
#endif
//                            break;
                        }
                    }else if(rec > 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[+] [client -> server] send_message message_id:%u\n",
                                    send_message_id);
#endif
                        sen = send_message(buffer,
                                           rec,
                                           forwarder_tv_sec,
                                           forwarder_tv_usec);
                        if(sen <= 0){
//                          break;
                        }else
                        {
                            send_message_id++;
                        }
                    }else
                    {
//                        break;
                    }
                }
            }
        }

        for(auto it = msgs_map.begin(); it != msgs_map.end();)
        {
            msg = it->second;
            free(msg.second);
            it = msgs_map.erase(it);
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_udp(struct sockaddr *target_addr,
                                  int target_addr_lengthh)
    {
        std::thread thread(&Server::forwarder_udp_recv_send_data,
                           this,
                           target_addr,
                           target_addr_lengthh);

        thread.join();

        return 0;
    }

    int32_t Server::do_socks5_connection(std::shared_ptr<Socks5message> socks5_message)
    {
        BOOL socks5_connect_flag = true;
        BOOL socks5_connect_udp_flag = false;

        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;

        int32_t rec;
        int32_t sen;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();

        unsigned char method;
        char ver;

        unsigned char ulen = 0;
        unsigned char plen = 0;
        char uname[256] = {0};
        char passwd[256] = {0};

        struct socks_request *socks_request = (struct socks_request *)buffer;
        struct socks_request_ipv4 *socks_request_ipv4;
        struct socks_request_domainname *socks_request_domainname;
        struct socks_request_ipv6 *socks_request_ipv6;

        char atyp;
        char cmd;

        struct sockaddr_in target_addr, *tmp_ipv4;      // IPv4
        struct sockaddr_in6 target_addr6, *tmp_ipv6;    // IPv6
        struct addrinfo hints, *target_host_addr_info;
        int target_addr_length = sizeof(target_addr);
        int target_addr6_length = sizeof(target_addr6);

        std::memset(&target_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset(&target_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset(&hints,
                    0,
                    sizeof(struct addrinfo));

        int family = 0;
        char domainname[256] = {0};
        std::string domainname_string;
        unsigned short domainname_length = 0;

        char target_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *target_addr6_string_pointer = target_addr6_string;

        std::string ipv6_link_local_prefix = "fe80::";


        // socks SELECTION_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv selection request\n");
#endif
        rec = socks5_message->get_data_size();
        if(rec <= 0 || rec > NODE_BUFFER_SIZE){
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] recv selection request error\n");
#endif
            free(buffer);
            return -1;
        }
        std::memcpy(buffer,
                    socks5_message->get_data(),
                    rec);
        recv_message_id = socks5_message->get_message_id();
        next_recv_message_id = recv_message_id + 1;

        if(encryption != nullptr
           && encryption->get_flag())
        {
            ret = encryption->decrypt(buffer,
                                      rec,
                                      buffer_max_length);
            if(ret <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client -> server] selection request decrypt error: %d\n",
                            ret);
#endif
                free(buffer);
                return -1;
            }

            rec = ret;
        }

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv selection request: %d bytes\n",
                    rec);
#endif
        struct selection_request *selection_request = (struct selection_request *)buffer;
        method = 0xFF;
        for(int i=0; i<selection_request->nmethods; i++)
        {
            if(selection_request->methods[i] == authentication_method)  // NO AUTHENTICATION REQUIRED or USERNAME/PASSWORD
            {
#ifdef DEBUGPRINT
                std::printf("[+] selection_request->methods[%d]: %02x\n", i, selection_request->methods[i]);
#endif
                method = selection_request->methods[i];
                break;
            }
        }
        if(method == 0xFF)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> server] selection request method error\n");
#endif
        }
        ver = selection_request->ver;


        // socks SELECTION_RESPONSE [client <- server]
        std::memset(buffer,
                    0,
                    buffer_max_length);
        struct selection_response *selection_response = (struct selection_response *)buffer;
        selection_response->ver = 0x5;          // socks version 5
        selection_response->method = method;    // no authentication required or username/password
        if(ver != 0x5 || authentication_method != method)
        {
            selection_response->method = 0xFF;
        }

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct selection_response),
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] send selection response: %d bytes\n",
                    sen);
#endif

        if(authentication_method != method)
        {
#ifdef DEBUGPRINT
            std::printf("[-] authentication method error: server:0x%x client:0x%x\n",
                        authentication_method,
                        method);
#endif
            free(buffer);
            return -1;
        }


        // socks USERNAME_PASSWORD_AUTHENTICATION [client -> server]
        if(method == 0x2)
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] recv username password authentication request\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client -> server] recv username password authentication request error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] Receive username password authentication request: %d bytes\n",
                        rec);
#endif
            struct username_password_authentication_request_tmp *username_password_authentication_request_tmp = (struct username_password_authentication_request_tmp *)buffer;

            ulen = username_password_authentication_request_tmp->ulen;
            memcpy(uname, &username_password_authentication_request_tmp->uname, ulen);
            memcpy(&plen, &username_password_authentication_request_tmp->uname + ulen, 1);
            memcpy(passwd, &username_password_authentication_request_tmp->uname + ulen + 1, plen);
#ifdef DEBUGPRINT
            std::printf("[+] uname:%s, ulen:%d, passwd:%s, plen:%d\n",
                        uname,
                        ulen,
                        passwd,
                        plen);
#endif
            ver = username_password_authentication_request_tmp->ver;


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
            std::memset(buffer,
                        0,
                        buffer_max_length);
            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;
            username_password_authentication_response->ver = 0x1;

            if(ver == 0x1
               && !strncmp(uname, username, sizeof(username))
               && !strncmp(passwd, password, sizeof(password)))
            {
#ifdef DEBUGPRINT
                std::printf("[+] succeed username password authentication\n");
#endif
                username_password_authentication_response->status = 0x0;

                send_message_id++;
                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] send username password authentication response: %d bytes\n",
                            sen);
#endif
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] fail username password authentication\n");
#endif
                username_password_authentication_response->status = 0xFF;

                send_message_id++;
                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] send selection response: %d bytes\n",
                            sen);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv socks request\n");
#endif
        std::memset(buffer,
                    0,
                    buffer_max_length);

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> server] recv socks request error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv socks request: %d bytes\n",
                    rec);
#endif

        atyp = socks_request->atyp;
        if(atyp != 0x1
           && atyp != 0x3
           && atyp != 0x4)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks request atyp(%d) error\n",
                        atyp);
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x8,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }

        cmd = socks_request->cmd;
        if(cmd != 0x1
           && cmd != 0x8)  // CONNECT (0x1), CONNECT UDP (0x8, UDP over TCP, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks request cmd(%d) error\n",
                        cmd);
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            if(atyp == 0x1
               || atyp == 0x3)  // IPv4
            {
                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);
            }else   // IPv6
            {
                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);
            }

            free(buffer);
            return -1;
        }

        if(socks_request->atyp == 0x1)  // IPv4
        {
            family = AF_INET;
            target_addr.sin_family = AF_INET;
            socks_request_ipv4 = (struct socks_request_ipv4 *)buffer;
            std::memcpy(&target_addr.sin_addr.s_addr,
                        &socks_request_ipv4->dst_addr,
                        4);
            std::memcpy(&target_addr.sin_port,
                        &socks_request_ipv4->dst_port,
                        2);
        }else if(socks_request->atyp == 0x3)    // domain name
        {
            socks_request_domainname = (struct socks_request_domainname*)buffer;
            domainname_length = socks_request_domainname->dst_addr_len;
            memcpy(&domainname,
                   &socks_request_domainname->dst_addr,
                   domainname_length);
            domainname_string = domainname;
#ifdef DEBUGPRINT
            std::printf("[+] domainname:%s, length:%d\n", domainname_string.c_str(), domainname_length);
#endif

            if(domainname_string.find(":") == std::string::npos)   // ipv4 address or domainname
            {
                hints.ai_family = AF_INET;  // IPv4
                ret = getaddrinfo(domainname_string.c_str(),
                                  NULL,
                                  &hints,
                                  &target_host_addr_info);
                if(ret != 0)
                {
                    hints.ai_family = AF_INET6; // IPv6
                    ret = getaddrinfo(domainname_string.c_str(),
                                      NULL,
                                      &hints,
                                      &target_host_addr_info);
                    if(ret != 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] cannot resolv the domain name: %s\n",
                                    domainname_string.c_str());
#endif

                        // socks SOCKS_RESPONSE send error [client <- server]
                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

                        free(buffer);
                        return -1;
                    }
                }
            }else   // ipv6 address
            {
                hints.ai_family = AF_INET6; // IPv6
                ret = getaddrinfo(domainname_string.c_str(),
                                  NULL,
                                  &hints,
                                  &target_host_addr_info);
                if(ret != 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] cannot resolv the domain name: %s\n",
                                domainname_string.c_str());
#endif

                    // socks SOCKS_RESPONSE send error [client <- server]
                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);
                    free(buffer);
                    return -1;
                }
            }

            if(target_host_addr_info->ai_family == AF_INET)
            {
                tmp_ipv4 = (struct sockaddr_in *)target_host_addr_info->ai_addr;

                family = AF_INET;
                target_addr.sin_family = AF_INET;
                std::memcpy(&target_addr.sin_addr,
                            &tmp_ipv4->sin_addr,
                            sizeof(struct in_addr));
                std::memcpy(&target_addr.sin_port,
                            &socks_request_domainname->dst_addr[domainname_length],
                            2);

                freeaddrinfo(target_host_addr_info);
            }else if(target_host_addr_info->ai_family == AF_INET6)
            {
                tmp_ipv6 = (struct sockaddr_in6 *)target_host_addr_info->ai_addr;

                family = AF_INET6;
                target_addr6.sin6_family = AF_INET6;
                memcpy(&target_addr6.sin6_addr,
                       &tmp_ipv6->sin6_addr,
                       sizeof(struct in6_addr));
                memcpy(&target_addr6.sin6_port,
                       &socks_request_domainname->dst_addr[domainname_length],
                       2);

                freeaddrinfo(target_host_addr_info);
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                // socks SOCKS_RESPONSE send error [client <- server]
                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);


                freeaddrinfo(target_host_addr_info);
                free(buffer);
                return -1;
            }
        }else if(socks_request->atyp == 0x4)    // IPv6
        {
            family = AF_INET6;
            target_addr6.sin6_family = AF_INET6;
            socks_request_ipv6 = (struct socks_request_ipv6*)buffer;
            memcpy(&target_addr6.sin6_addr,
                   &socks_request_ipv6->dst_addr,
                   16);
            memcpy(&target_addr6.sin6_port,
                   &socks_request_ipv6->dst_port,
                   2);
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x1,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }


        // socks SOCKS_RESPONSE [client <- server]
        if(atyp == 0x1) // IPv4
        {
            target_ip = inet_ntoa(target_addr.sin_addr);
            target_port = std::to_string(ntohs(target_addr.sin_port));

            if(cmd == 0x1)  // CONNECT
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET,
                                     SOCK_STREAM,
                                     0);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                ret = connect(target_sock,
                              (struct sockaddr *)&target_addr,
                              sizeof(target_addr));
                if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] cannot connect error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    closesocket(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef DEBUGPRINT
                std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x1);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

            }else if(cmd == 0x2)    // BIND
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: BIND\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }else if(cmd == 0x3)    // UDP ASSOCIATE
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                std::printf("[+] ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                target_sock = socket(AF_INET,
                                     SOCK_DGRAM,
                                     IPPROTO_UDP);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x1);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

                socks5_connect_flag = false;
                socks5_connect_udp_flag = true;
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }
        }else if(atyp == 0x3)   // domainname
        {
            if(family == AF_INET)   // IPv4
            {
                target_ip = inet_ntoa(target_addr.sin_addr);
                target_port = std::to_string(ntohs(target_addr.sin_port));

                if(cmd == 0x1)  // CONNECT
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET,
                                         SOCK_STREAM,
                                         0);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr,
                                  sizeof(target_addr));
                    if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] cannot connect error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        closesocket(target_sock);
                        free(buffer);
                        return -1;
                    }
#ifdef DEBUGPRINT
                    std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: BIND\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    target_sock = socket(AF_INET,
                                         SOCK_DGRAM,
                                         IPPROTO_UDP);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    socks5_connect_flag = false;
                    socks5_connect_udp_flag = true;
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x1,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }
            }else if(family == AF_INET6)    // IPv6
            {
                inet_ntop(AF_INET6,
                          &target_addr6.sin6_addr,
                          target_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);

                if(target_addr6.sin6_scope_id > 0)
                {
                    target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                    target_port = std::to_string(ntohs(target_addr6.sin6_port));
                }else
                {
                    target_ip = target_addr6_string_pointer;
                    target_port = std::to_string(ntohs(target_addr6.sin6_port));

                    ret = target_ip.rfind(ipv6_link_local_prefix, 0);
                    if(ret == 0)     // ipv6 link local address
                    {
                        target_addr6.sin6_scope_id = (uint32_t)std::stoi(spider_ip->get_spider_ipv6_link_local_scope_id());
                        target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                    }
                }

                if(cmd == 0x1)  // CONNECT
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET6,
                                         SOCK_STREAM,
                                         0);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr6,
                                  sizeof(target_addr6));
                    if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] cannot connect error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        closesocket(target_sock);
                        free(buffer);
                        return -1;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: BIND\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    target_sock = socket(AF_INET6,
                                         SOCK_DGRAM,
                                         IPPROTO_UDP);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    socks5_connect_flag = false;
                    socks5_connect_udp_flag = true;
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x1,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }
        }else if(atyp == 0x4)   // IPv6
        {
            inet_ntop(AF_INET6,
                      &target_addr6.sin6_addr,
                      target_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

            if(target_addr6.sin6_scope_id > 0)
            {
                target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                target_port = std::to_string(ntohs(target_addr6.sin6_port));
            }else
            {
                target_ip = target_addr6_string_pointer;
                target_port = std::to_string(ntohs(target_addr6.sin6_port));

                ret = target_ip.rfind(ipv6_link_local_prefix, 0);
                if(ret == 0)     // ipv6 link local address
                {
                    target_addr6.sin6_scope_id = (uint32_t)std::stoi(spider_ip->get_spider_ipv6_link_local_scope_id());
                    target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                }
            }

            if(cmd == 0x1)  // CONNECT
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET6,
                                     SOCK_STREAM,
                                     0);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                ret = connect(target_sock,
                              (struct sockaddr *)&target_addr6,
                              sizeof(target_addr6));
                if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    closesocket(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connected ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x4);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif
            }else if(cmd == 0x2)    // BIND
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: BIND\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }else if(cmd == 0x3)    // UDP ASSOCIATE
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                std::printf("[+] ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                target_sock = socket(AF_INET6,
                                     SOCK_DGRAM,
                                     IPPROTO_UDP);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x4);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

                socks5_connect_flag = false;
                socks5_connect_udp_flag = true;
            }else{
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }
        }else{
#ifdef DEBUGPRINT
            std::printf("[-] not implemented\n");
#endif

            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x1,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef DEBUGPRINT
        std::printf("[+] [client <> client <> server <> target] forwarder\n");
#endif

        send_message_id++;
        if(socks5_connect_flag == true)
        {
            ret = forwarder();
        }else if(socks5_connect_udp_flag == true)
        {
            if(family == AF_INET)
            {
                ret = forwarder_udp((sockaddr *)&target_addr, target_addr_length);
            }else if(family == AF_INET6)
            {
                ret = forwarder_udp((sockaddr *)&target_addr6, target_addr6_length);
            }
        }


#ifdef DEBUGPRINT
        std::printf("[+] worker exit\n");
#endif

        closesocket(target_sock);
        free(buffer);
        return -1;
    }
}

