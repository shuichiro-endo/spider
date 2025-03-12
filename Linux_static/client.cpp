/*
 * Title:  spider client.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5.hpp"
#include "client.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "encryption.hpp"
#include "caresmanager.hpp"


namespace spider
{
    Client::Client(std::string type,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string client_ip,
                   std::string client_ip_scope_id,
                   std::string client_listen_port,
                   std::string client_port,
                   std::string destination_spider_ip,
                   int32_t client_sock,
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
        this->client_ip_scope_id = client_ip_scope_id;
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
        this->cares_manager = nullptr;
    }

    Client::Client(std::string type,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string client_ip,
                   std::string client_ip_scope_id,
                   std::string client_listen_port,
                   std::string client_port,
                   std::string destination_spider_ip,
                   std::string target_ip,
                   std::string target_port,
                   int32_t client_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   std::shared_ptr<Encryption> encryption,
                   std::shared_ptr<Messagemanager> message_manager,
                   std::shared_ptr<spider::Caresmanager> cares_manager)
    : Node(client_sock, message_manager)
    {
        this->type = type;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->client_ip = client_ip;
        this->client_ip_scope_id = client_ip_scope_id;
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
        this->cares_manager = cares_manager;
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

    void Client::set_client_ip_scope_id(std::string client_ip_scope_id)
    {
        this->client_ip_scope_id = client_ip_scope_id;
    }

    std::string Client::get_client_ip_scope_id()
    {
        return client_ip_scope_id;
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
                                 bool register_server_id_flag)
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

                    recv_message_id = socks5_message->get_message_id();

                    std::memcpy(buffer,
                                socks5_message->get_data(),
                                rec);

#ifdef _DEBUG
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
#ifdef _DEBUG
                    std::printf("[-] recv_message recv error: %d\n",
                           rec);
#endif
                    return -1;
                }
            }else
            {
#ifdef _DEBUG
                std::printf("[-] recv_message unknown message type: %c\n",
                            message_type);
#endif
                return -1;
            }
        }else
        {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
                std::printf("[-] send_message encrypt error: %d\n",
                            ret);
#endif
                return -1;
            }

            data_size = ret;
        }

#ifdef _DEBUG
        print_bytes(buffer, data_size);
#endif

        socks5_message_data = (struct socks5_message_data *)calloc(1,
                                                            sizeof(struct socks5_message_data));

        socks5_message_data->message_type = 's';
        socks5_message_data->message_id = send_message_id;
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
        int nfds = -1;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            nfds = sock + 1;
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(nfds,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[+] forwarder_recv_data select timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
#ifdef _DEBUG
                std::printf("[+] [client -> client] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(sock,
                           buffer,
                           socks5_message_data_max_size,
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
                        std::printf("[-] forwarder_recv_data recv error: %d\n",
                                    errno);
#endif
                        break;
                    }
                }else
                {
#ifdef _DEBUG
                    std::printf("[+] [client -> server] send_message message_id:%u\n",
                                send_message_id);
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0){
                        break;
                    }

                    send_message_id++;
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
        int nfds = -1;
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
            FD_SET(sock, &writefds);
            nfds = sock + 1;
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(nfds,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[+] forwarder_send_data select timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret){
#ifdef _DEBUG
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
                    if(recv_message_id == next_recv_message_id)
                    {
                        len = rec;
                        send_length = 0;
#ifdef _DEBUG
                        std::printf("[+] [client <- client] send message_id:%u\n",
                                    next_recv_message_id);
#endif
                        while(len > 0)
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
                                    std::printf("[-] forwarder_send_data send error: %d\n",
                                                errno);
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
#ifdef _DEBUG
                            std::printf("[+] [client <- client] send message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
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
                                        std::printf("[-] forwarder_send_data send error: %d\n",
                                                    errno);
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

    int32_t Client::forwarder()
    {
        std::thread thread1(&Client::forwarder_recv_data,
                            this);
        std::thread thread2(&Client::forwarder_send_data,
                            this);

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
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();


        // socks SELECTION_REQUEST [client -> client]
#ifdef _DEBUG
        std::printf("[+] [client -> client] recv selection request\n");
#endif
        rec = recv_data(buffer,
                        socks5_message_data_max_size,
                        tv_sec,
                        tv_usec);
        if(rec <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] [client -> client] recv selection request error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef _DEBUG
        std::printf("[+] [client -> client] recv selection request: %d bytes\n",
                    rec);
#endif


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request\n");
#endif

        send_message_id++;
        sen = send_message(buffer,
                           rec,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id = recv_message_id + 1;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif


        // socks SELECTION_RESPONSE [client <- client]
#ifdef _DEBUG
        std::printf("[+] [client <- client] send selection response\n");
#endif
        sen = send_data(buffer,
                        rec,
                        tv_sec,
                        tv_usec);
#ifdef _DEBUG
        std::printf("[+] [client <- client] send selection response: %d bytes\n",
                    sen);
#endif
        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef _DEBUG
            std::printf("[-] socks5server authentication method error\n");
#endif
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST client -> client
#ifdef _DEBUG
            std::printf("[+] [client -> client] recv username password authentication request\n");
#endif
            rec = recv_data(buffer,
                            socks5_message_data_max_size,
                            tv_sec,
                            tv_usec);
            if(ret <= 0)
            {
#ifdef _DEBUG
                std::printf("[-] [client -> client] recv username password authentication request error\n");
#endif
                free(buffer);
                return -1;
            }
#ifdef _DEBUG
            std::printf("[+] [client -> client] recv username password authentication request: %d bytes\n",
                        rec);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request\n");
#endif

            send_message_id++;
            sen = send_message(buffer,
                               rec,
                               tv_sec,
                               tv_usec);

#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- client]
#ifdef _DEBUG
            std::printf("[+] [client <- client] send username password authentication response\n");
#endif
            sen = send_data(buffer,
                            rec,
                            tv_sec,
                            tv_usec);
#ifdef _DEBUG
            std::printf("[+] [client <- client] send username password authentication response: %d bytes\n",
                        sen);
#endif
        }


        // socks SOCKS_REQUEST [client -> client]
#ifdef _DEBUG
        std::printf("[+] [client -> client] recv socks request\n");
#endif
        rec = recv_data(buffer,
                        socks5_message_data_max_size,
                        tv_sec,
                        tv_usec);
        if(rec <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] [client -> client] recv socks request error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef _DEBUG
        std::printf("[+] [client -> client] recv socks request: %d bytes\n",
                    rec);
#endif


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request\n");
#endif

        send_message_id++;
        sen = send_message(buffer,
                           rec,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif


        // socks SOCKS_RESPONSE [client <- client]
#ifdef _DEBUG
        std::printf("[+] [client <- client] send socks response\n");
#endif
        sen = send_data(buffer,
                        rec,
                        tv_sec,
                        tv_usec);
#ifdef _DEBUG
        std::printf("[+] [client <- client] send socks response: %d bytes\n",
                    sen);
#endif


        // forwarder [client <> client <> server <> target]
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder\n");
#endif

        send_message_id++;
        forwarder();

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        free(buffer);
        return -1;
    }

    std::vector<std::string> Client::split_input(const std::string& input)
    {
        std::vector<std::string> tokens;
        std::istringstream stream(input);
        std::string token;

        while(stream >> token)
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    int32_t Client::forwarder_shell_recv_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set readfds;
        int nfds = -1;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;

        std::string result;
        std::string prompt = "\ncommand >";
        std::string input;
        std::vector <std::string> tokens;
        std::string upload_file_name = "";
        std::string upload_file_path = "";
        uint64_t upload_file_size;
        uint64_t upload_file_remaining_size;
        uint64_t read_bytes;
        struct upload_download_data *upload_download_data;


        // output prompt
        std::memset(buffer,
                    0,
                    buffer_max_length);
        std::memcpy(buffer,
                    prompt.c_str(),
                    prompt.size());
        len = prompt.size();
        send_length = 0;

#ifdef _DEBUG
        std::printf("[+] [client <- client] send\n");
#endif
        while(len > 0)
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
                    std::printf("[-] forwarder_shell_recv_data send error: %d\n",
                                errno);
#endif
                    break;
                }
            }
            send_length += sen;
            len -= sen;
        }

        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            nfds = sock + 1;
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(nfds,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[+] forwarder_shell_recv_data select timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
#ifdef _DEBUG
                std::printf("[+] [client -> client] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(sock,
                           buffer,
                           socks5_message_data_max_size,
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
                        std::printf("[-] forwarder_shell_recv_data recv error: %d\n",
                                    errno);
#endif
                        break;
                    }
                }else
                {
                    input = buffer;
                    tokens = split_input(input);

                    if(tokens.size() == 3
                       && tokens[0] == "upload")
                    {
                        std::ifstream upload_file(tokens[1].c_str(),
                                                  std::ios::binary);
                        if(!upload_file.is_open())
                        {
#ifdef _DEBUG
                            std::printf("[-] file open error\n");
#endif

                            std::memset(buffer,
                                        0,
                                        buffer_max_length);

                            result = "[-] file open error";
                            result += prompt;
                            len = result.size();
                            std::memcpy(buffer,
                                        result.c_str(),
                                        result.size());
                            send_length = 0;

#ifdef _DEBUG
                            std::printf("[+] [client <- client] send\n");
#endif
                            while(len > 0)
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
                                        std::printf("[-] forwarder_shell_recv_data send error: %d\n",
                                                    errno);
#endif
                                        break;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            continue;
                        }else
                        {
                            upload_file.seekg(0, std::ios::end);
                            upload_file_size = upload_file.tellg();
                            upload_file_remaining_size = upload_file_size;
                            upload_file.seekg(0, std::ios::beg);

                            std::filesystem::path path(tokens[1].c_str());
                            upload_file_name = path.filename().string();
                            upload_file_path = tokens[2];

                            while(upload_file_remaining_size > 0)
                            {
                                std::memset(buffer,
                                            0,
                                            buffer_max_length);

                                upload_download_data = (struct upload_download_data *)buffer;

                                upload_file.read(upload_download_data->data,
                                                 SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);

                                read_bytes = upload_file.gcount();

                                std::memcpy(upload_download_data->command,
                                            "upload",
                                            sizeof("upload"));

                                if(upload_file_name.size() > 256)
                                {
                                    std::memcpy(upload_download_data->file_name,
                                                upload_file_name.c_str(),
                                                255);
                                }else
                                {
                                    std::memcpy(upload_download_data->file_name,
                                                upload_file_name.c_str(),
                                                upload_file_name.size());
                                }

                                if(upload_file_path.size() > 256)
                                {
                                    std::memcpy(upload_download_data->file_path,
                                                upload_file_path.c_str(),
                                                255);
                                }else
                                {
                                    std::memcpy(upload_download_data->file_path,
                                                upload_file_path.c_str(),
                                                upload_file_path.size());
                                }

                                upload_download_data->file_size = upload_file_size;
                                upload_download_data->data_size = read_bytes;

                                len = sizeof(struct upload_download_data_header) + read_bytes;

#ifdef _DEBUG
                                std::printf("[+] [client -> server] send_message message_id:%u\n",
                                            send_message_id);
#endif

                                sen = send_message(buffer,
                                                   len,
                                                   forwarder_tv_sec,
                                                   forwarder_tv_usec);
                                if(sen <= 0){
                                    break;
                                }

                                send_message_id++;
                                upload_file_remaining_size -= read_bytes;
                            }

                            upload_file.close();

                            if(sen <= 0)
                            {
                                break;
                            }
                        }
                    }else
                    {
#ifdef _DEBUG
                        std::printf("[+] [client -> server] send_message message_id:%u\n",
                                    send_message_id);
#endif
                        sen = send_message(buffer,
                                           rec,
                                           forwarder_tv_sec,
                                           forwarder_tv_usec);
                        if(sen <= 0){
                            break;
                        }

                        send_message_id++;
                    }
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Client::forwarder_shell_send_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set writefds;
        int nfds = -1;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;

        std::string result;
        std::string prompt = "\ncommand >";
        std::string input;
        std::vector <std::string> tokens;
        std::string download_file_name = "";
        std::string download_file_path = "";
        uint64_t download_file_size;
        uint64_t recv_download_file_data_size;
        uint64_t download_file_remaining_size;
        char *download_file_data = NULL;
        bool download_file_flag = false;
        struct upload_download_data *upload_download_data;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(sock, &writefds);
            nfds = sock + 1;
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(nfds,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[+] forwarder_shell_send_data select timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret){
#ifdef _DEBUG
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
                    if(recv_message_id == next_recv_message_id)
                    {
                        next_recv_message_id++;

                        upload_download_data = (struct upload_download_data *)buffer;
                        ret = strncmp(upload_download_data->command,
                                      "download",
                                      sizeof("download"));
                        if(ret == 0)
                        {
                            if(download_file_flag == false)
                            {
                                download_file_flag = true;
                                download_file_path = upload_download_data->file_path;
                                download_file_path += "/";
                                download_file_name = upload_download_data->file_name;
                                download_file_name = download_file_path + download_file_name;
                                download_file_size = upload_download_data->file_size;
                                download_file_data = (char *)calloc(download_file_size,
                                                                    sizeof(char));
                                std::memcpy(download_file_data,
                                            upload_download_data->data,
                                            upload_download_data->data_size);

                                recv_download_file_data_size = upload_download_data->data_size;
                                download_file_remaining_size = download_file_size - recv_download_file_data_size;
                                if(download_file_remaining_size > 0)
                                {
                                    continue;
                                }
                            }else
                            {
                                std::memcpy(download_file_data + recv_download_file_data_size,
                                            upload_download_data->data,
                                            upload_download_data->data_size);

                                recv_download_file_data_size += upload_download_data->data_size;
                                download_file_remaining_size -= upload_download_data->data_size;
                                if(download_file_remaining_size > 0)
                                {
                                    continue;
                                }
                            }

                            std::ofstream output_file(download_file_name.c_str(),
                                                      std::ios::binary);

                            output_file.write(download_file_data,
                                              download_file_size);

                            output_file.close();

                            std::memset(buffer,
                                        0,
                                        buffer_max_length);

                            result = "[+] download file: ";
                            result += download_file_name;
                            result += prompt;

                            std::memcpy(buffer,
                                        result.c_str(),
                                        result.size());

                            len = result.size();
                            send_length = 0;
#ifdef _DEBUG
                            std::printf("[+] [client <- client] send\n");
#endif
                            while(len > 0)
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
                                        std::printf("[-] forwarder_shell_send_data send error: %d\n",
                                                    errno);
#endif
                                        break;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            download_file_flag = false;
                            download_file_name = "";
                            download_file_path = "";
                            download_file_size = 0;
                            recv_download_file_data_size = 0;
                            download_file_remaining_size = 0;
                            free(download_file_data);
                            download_file_data = NULL;

                            if(sen <= 0)
                            {
                                break;
                            }
                        }else
                        {
                            len = rec;
                            send_length = 0;
#ifdef _DEBUG
                            std::printf("[+] [client <- client] send message_id:%u\n",
                                        next_recv_message_id - 1);
#endif
                            while(len > 0)
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
                                        std::printf("[-] forwarder_shell_send_data send error: %d\n",
                                                    errno);
#endif
                                        break;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }
                        }
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
                            next_recv_message_id++;
                            check_msg_count = msgs_map.count(next_recv_message_id);

                            buffer = msg.second;

                            upload_download_data = (struct upload_download_data *)msg.second;
                            ret = strncmp(upload_download_data->command,
                                          "download",
                                          sizeof("download"));
                            if(ret == 0)
                            {
                                if(download_file_flag == false)
                                {
                                    download_file_flag = true;
                                    download_file_path = upload_download_data->file_path;
                                    download_file_path += "/";
                                    download_file_name = upload_download_data->file_name;
                                    download_file_name = download_file_path + download_file_name;
                                    download_file_size = upload_download_data->file_size;
                                    download_file_data = (char *)calloc(download_file_size,
                                                                        sizeof(char));
                                    std::memcpy(download_file_data,
                                                upload_download_data->data,
                                                upload_download_data->data_size);

                                    recv_download_file_data_size = upload_download_data->data_size;
                                    download_file_remaining_size = download_file_size - recv_download_file_data_size;

                                    free(buffer);

                                    if(download_file_remaining_size > 0)
                                    {
                                        continue;
                                    }
                                }else
                                {
                                    std::memcpy(download_file_data + recv_download_file_data_size,
                                                upload_download_data->data,
                                                upload_download_data->data_size);

                                    recv_download_file_data_size += upload_download_data->data_size;
                                    download_file_remaining_size -= upload_download_data->data_size;

                                    free(buffer);

                                    if(download_file_remaining_size > 0)
                                    {
                                        continue;
                                    }
                                }

                                std::ofstream output_file(download_file_name.c_str(),
                                                          std::ios::binary);

                                output_file.write(download_file_data,
                                                  download_file_size);

                                output_file.close();


                                std::memset(buffer,
                                            0,
                                            buffer_max_length);

                                result = "[+] download file: ";
                                result += download_file_name;
                                result += prompt;

                                std::memcpy(buffer,
                                            result.c_str(),
                                            result.size());

                                len = result.size();
                                send_length = 0;
#ifdef _DEBUG
                                std::printf("[+] [client <- client] send\n");
#endif
                                while(len > 0)
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
                                            std::printf("[-] forwarder_shell_send_data send error: %d\n",
                                                        errno);
#endif
                                            break;
                                        }
                                    }
                                    send_length += sen;
                                    len -= sen;
                                }

                                download_file_flag = false;
                                download_file_name = "";
                                download_file_path = "";
                                download_file_size = 0;
                                recv_download_file_data_size = 0;
                                download_file_remaining_size = 0;
                                free(download_file_data);
                                download_file_data = NULL;

                                if(sen <= 0)
                                {
                                    break;
                                }
                            }else
                            {
                                len = msg.first;
                                buffer = msg.second;
                                send_length = 0;
#ifdef _DEBUG
                                std::printf("[+] [client <- client] send message_id:%u\n",
                                            next_recv_message_id - 1);
#endif
                                while(len > 0)
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
                                            std::printf("[-] forwarder_shell_send_data send error: %d\n",
                                                        errno);
#endif
                                            break;
                                        }
                                    }
                                    send_length += sen;
                                    len -= sen;
                                }

                                free(buffer);
                            }

                            buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));
                        }
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

        if(download_file_data != NULL)
        {
            free(download_file_data);
        }

        free(buffer);
        return 0;
    }

    int32_t Client::forwarder_shell()
    {
        std::thread thread1(&Client::forwarder_shell_recv_data,
                            this);
        std::thread thread2(&Client::forwarder_shell_send_data,
                            this);

        thread1.join();
        thread2.join();

        return 0;
    }

    int32_t Client::do_socks5_connection_shell()
    {
        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;
        uint8_t username_length = std::strlen(username);
        uint8_t password_length = std::strlen(password);

        int32_t rec;
        int32_t sen;
        int32_t send_length = 0;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
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

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id = recv_message_id + 1;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif

        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef _DEBUG
            std::printf("[-] authentication method error\n");
#endif
            free(buffer);
            return -1;
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef _DEBUG
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
            *(username_password_authentication_request->uname + username_length) = password_length;
            std::memcpy(username_password_authentication_request->uname + username_length + 1,
                        password,
                        password_length);

            send_length = 3 + username_length + password_length;

            send_message_id++;
            sen = send_message(buffer,
                               send_length,
                               tv_sec,
                               tv_usec);

#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif

            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;

            if(username_password_authentication_response->status != 0x0)
            {
#ifdef _DEBUG
                std::printf("[-] username password authentication error: 0x%02x\n",
                            username_password_authentication_response->status);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request\n");
#endif

        std::memset(buffer,
                    0,
                    buffer_max_length);

        struct socks_request_ipv4 *socks_request_ipv4 = (struct socks_request_ipv4 *)buffer;
        socks_request_ipv4->ver = 0x5;
        socks_request_ipv4->cmd = 0x9;    // SHELL (0x9, original command)
        socks_request_ipv4->rsv = 0x0;
        socks_request_ipv4->atyp = 0x0;   // none
        send_length = sizeof(struct socks_request_ipv4);

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif

        struct socks_response *socks_response = (struct socks_response *)buffer;
        if(socks_response->ver != 0x5
           && socks_response->rep != 0x0
           && socks_response->atyp != 0x0)
        {
#ifdef _DEBUG
            std::printf("[-] socks response error: 0x%02x\n",
                        socks_response->rep);
#endif
            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder_shell\n");
#endif

        send_message_id++;
        forwarder_shell();

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        close(sock);
        free(buffer);
        return -1;
    }

    int32_t Client::forwarder_add_node(std::string config)
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t config_size = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;


        config_size = config.size();
        std::memcpy(buffer,
                    config.c_str(),
                    config_size);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send_message message_id:%u\n",
                    send_message_id);
#endif
        sen = send_message(buffer,
                           config_size,
                           forwarder_tv_sec,
                           forwarder_tv_usec);
        if(sen > 0){
            send_message_id++;

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
                if(recv_message_id == next_recv_message_id)
                {
#ifdef _DEBUG
                    std::printf("[+] [client <- server] recv_message message_id:%u\n",
                                recv_message_id);
                    std::printf("[+] recv_message: %s\n",
                                buffer);
#endif
                }else
                {
#ifdef _DEBUG
                    std::printf("[-] [client <- server] recv_message error message_id:%u\n",
                                recv_message_id);
#endif
                }
            }else
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv_message error\n");
#endif
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Client::do_socks5_connection_add_node(std::string config)
    {
        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;
        uint8_t username_length = std::strlen(username);
        uint8_t password_length = std::strlen(password);

        int32_t rec;
        int32_t sen;
        int32_t send_length = 0;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
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

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id = recv_message_id + 1;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif

        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef _DEBUG
            std::printf("[-] authentication method error\n");
#endif
            free(buffer);
            return -1;
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef _DEBUG
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
            *(username_password_authentication_request->uname + username_length) = password_length;
            std::memcpy(username_password_authentication_request->uname + username_length + 1,
                        password,
                        password_length);

            send_length = 3 + username_length + password_length;

            send_message_id++;
            sen = send_message(buffer,
                               send_length,
                               tv_sec,
                               tv_usec);

#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif

            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;

            if(username_password_authentication_response->status != 0x0)
            {
#ifdef _DEBUG
                std::printf("[-] username password authentication error: 0x%02x\n",
                            username_password_authentication_response->status);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request\n");
#endif

        std::memset(buffer,
                    0,
                    buffer_max_length);

        struct socks_request_ipv4 *socks_request_ipv4 = (struct socks_request_ipv4 *)buffer;
        socks_request_ipv4->ver = 0x5;
        socks_request_ipv4->cmd = 0xa;    // ADD NODE (0xa, original command)
        socks_request_ipv4->rsv = 0x0;
        socks_request_ipv4->atyp = 0x0;   // none
        send_length = sizeof(struct socks_request_ipv4);

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif

        struct socks_response *socks_response = (struct socks_response *)buffer;
        if(socks_response->ver != 0x5
           && socks_response->rep != 0x0
           && socks_response->atyp != 0x0)
        {
#ifdef _DEBUG
            std::printf("[-] socks response error: 0x%02x\n",
                        socks_response->rep);
#endif
            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder_add_node\n");
#endif

        send_message_id++;
        forwarder_add_node(config);

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        close(sock);
        free(buffer);
        return -1;
    }

    int32_t Client::forwarder_show_node()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;


        rec = recv_message(buffer,
                           buffer_max_length,
                           forwarder_tv_sec,
                           forwarder_tv_usec,
                           false);
        if(rec > 0)
        {
            if(recv_message_id == next_recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[+] [client <- server] recv_message message_id:%u\n",
                            recv_message_id);
#endif
                std::printf("%s",
                            buffer);
            }else
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv_message error message_id:%u\n",
                            recv_message_id);
#endif
                std::printf("[-] recv_message error\n");
            }
        }else
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv_message error\n");
#endif
            std::printf("[-] recv_message error\n");
        }

        free(buffer);
        return 0;
    }

    int32_t Client::do_socks5_connection_show_node()
    {
        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;
        uint8_t username_length = std::strlen(username);
        uint8_t password_length = std::strlen(password);

        int32_t rec;
        int32_t sen;
        int32_t send_length = 0;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
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

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id = recv_message_id + 1;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif

        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef _DEBUG
            std::printf("[-] authentication method error\n");
#endif
            free(buffer);
            return -1;
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef _DEBUG
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
            *(username_password_authentication_request->uname + username_length) = password_length;
            std::memcpy(username_password_authentication_request->uname + username_length + 1,
                        password,
                        password_length);

            send_length = 3 + username_length + password_length;

            send_message_id++;
            sen = send_message(buffer,
                               send_length,
                               tv_sec,
                               tv_usec);

#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif

            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;

            if(username_password_authentication_response->status != 0x0)
            {
#ifdef _DEBUG
                std::printf("[-] username password authentication error: 0x%02x\n",
                            username_password_authentication_response->status);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request\n");
#endif

        std::memset(buffer,
                    0,
                    buffer_max_length);

        struct socks_request_ipv4 *socks_request_ipv4 = (struct socks_request_ipv4 *)buffer;
        socks_request_ipv4->ver = 0x5;
        socks_request_ipv4->cmd = 0xb;    // SHOW NODE (0xb, original command)
        socks_request_ipv4->rsv = 0x0;
        socks_request_ipv4->atyp = 0x0;   // none
        send_length = sizeof(struct socks_request_ipv4);

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif

        struct socks_response *socks_response = (struct socks_response *)buffer;
        if(socks_response->ver != 0x5
           && socks_response->rep != 0x0
           && socks_response->atyp != 0x0)
        {
#ifdef _DEBUG
            std::printf("[-] socks response error: 0x%02x\n",
                        socks_response->rep);
#endif
            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder_show_node\n");
#endif

        send_message_id++;
        forwarder_show_node();

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        close(sock);
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
        int nfds = -1;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;

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
            nfds = sock + 1;
            tv.tv_sec = FORWARDER_UDP_TIMEOUT;
            tv.tv_usec = 0;

            ret = select(nfds,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef _DEBUG
                std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
#ifdef _DEBUG
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
                        std::printf("[-] forwarder_recv_data recvfrom error: %d\n",
                                    errno);
#endif
//                        break;
                    }
                }else
                {
#ifdef _DEBUG
                    std::printf("[+] [client -> server] send_message message_id:%u\n",
                                send_message_id);
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0){
//                        break;
                    }

                    send_message_id++;
                }


                FD_ZERO(&writefds);
                FD_SET(sock, &writefds);
                nfds = sock + 1;
                tv.tv_sec = forwarder_tv_sec;
                tv.tv_usec = forwarder_tv_usec;

                ret = select(nfds,
                             NULL,
                             &writefds,
                             NULL,
                             &tv);
                if(ret == 0)
                {
#ifdef _DEBUG
                    std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
//                    break;
                }

                ret = FD_ISSET(sock,
                               &writefds);
                if(ret){
#ifdef _DEBUG
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
                        if(recv_message_id == next_recv_message_id)
                        {
                            len = rec;
                            send_length = 0;
#ifdef _DEBUG
                            std::printf("[+] [client <- client] sendto message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
                            {
                                sen = sendto(sock,
                                             buffer+send_length,
                                             len,
                                             MSG_NOSIGNAL,
                                             client_addr,
                                             client_addr_length);
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
                                        std::printf("[-] forwarder_send_data sendto error: %d\n",
                                                    errno);
#endif
//                                      free(client_addr);
//                                      free(buffer);
//                                      return 0;
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
#ifdef _DEBUG
                                std::printf("[+] [client <- client] sendto message_id:%u\n",
                                            next_recv_message_id);
#endif
                                while(len > 0)
                                {
                                    sen = sendto(sock,
                                                 buffer+send_length,
                                                 len,
                                                 MSG_NOSIGNAL,
                                                 client_addr,
                                                 client_addr_length);
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
                                            std::printf("[-] forwarder_send_data sendto error: %d\n",
                                                        errno);
#endif
//                                          free(client_addr);
//                                          free(buffer);
//                                          return 0;
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

        free(client_addr);
        free(buffer);
        return 0;
    }

    int32_t Client::forwarder_udp(int addr_length)
    {
        std::thread thread(&Client::forwarder_udp_recv_send_data,
                           this,
                           addr_length);

        thread.join();

        return 0;
    }

    int32_t Client::do_socks5_connection_udp()
    {
        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD; // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;
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
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();

        struct sockaddr_in client_addr;
        struct sockaddr_in6 client_addr6;
        int flags = 0;
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


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
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

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send selection request: %d bytes\n",
                    sen);
#endif


        // socks SELECTION_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response\n");
#endif

        rec = recv_message(buffer,
                           sen,
                           tv_sec,
                           tv_usec,
                           true);

        if(rec != sizeof(struct selection_response))
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv selection response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id = recv_message_id + 1;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv selection response: %d bytes\n",
                    rec);
#endif

        struct selection_response *selection_response = (struct selection_response *)buffer;
        if((unsigned char)selection_response->method == 0xFF)
        {
#ifdef _DEBUG
            std::printf("[-] authentication method error\n");
#endif
            free(buffer);
            return -1;
        }

        if(selection_response->method == 0x2)   // USERNAME_PASSWORD_AUTHENTICATION
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef _DEBUG
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
            *(username_password_authentication_request->uname + username_length) = password_length;
            std::memcpy(username_password_authentication_request->uname + username_length + 1,
                        password,
                        password_length);

            send_length = 3 + username_length + password_length;

            send_message_id++;
            sen = send_message(buffer,
                               send_length,
                               tv_sec,
                               tv_usec);

#ifdef _DEBUG
            std::printf("[+] [client -> server] send username password authentication request: %d bytes\n",
                        sen);
#endif


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec,
                               false);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef _DEBUG
            std::printf("[+] [client <- server] recv username password authentication response: %d bytes\n",
                        rec);
#endif

            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;

            if(username_password_authentication_response->status != 0x0)
            {
#ifdef _DEBUG
                std::printf("[-] username password authentication error: 0x%02x\n",
                            username_password_authentication_response->status);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
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

        send_message_id++;
        sen = send_message(buffer,
                           send_length,
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client -> server] send socks request: %d bytes\n",
                    sen);
#endif


        // socks SOCKS_RESPONSE [client <- server]
#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response\n");
#endif

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec,
                           false);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef _DEBUG
        std::printf("[+] [client <- server] recv socks response: %d bytes\n",
                    rec);
#endif

        struct socks_response *socks_response = (struct socks_response *)buffer;
        if(socks_response->ver != 0x5
           && socks_response->rep != 0x0)
        {
#ifdef _DEBUG
            std::printf("[-] socks response error: 0x%02x\n",
                            socks_response->rep);
#endif
            free(buffer);
            return -1;
        }


        if(client_ip.find(":") == std::string::npos)  // ipv4 address
        {
            ret = cares_manager->get_addr_info(client_ip.c_str(),
                                               client_listen_port.c_str(),
                                               AF_INET,
                                               &client_addr,
                                               NULL);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            client_ip.c_str());
#endif
                free(buffer);
                return -1;
            }

            addr_length = client_addr_length;

            sock = socket(AF_INET,
                          SOCK_DGRAM,
                          IPPROTO_UDP);

            flags = fcntl(sock,
                          F_GETFL,
                          0);
            flags &= ~O_NONBLOCK;
            fcntl(sock,
                  F_SETFL,
                  flags);

            // bind
            ret = bind(sock,
                       (struct sockaddr *)&client_addr,
                       sizeof(client_addr));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error: %d\n",
                            errno);
#endif
                close(sock);
                free(buffer);
                return -1;
            }

#ifdef _DEBUG
            std::printf("[+] bind port %d on %s\n",
                        ntohs(client_addr.sin_port),
                        inet_ntoa(client_addr.sin_addr));
#endif
        }else   // ipv6 address
        {
            ret = cares_manager->get_addr_info(client_ip.c_str(),
                                               client_listen_port.c_str(),
                                               AF_INET6,
                                               NULL,
                                               &client_addr6);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            client_ip.c_str());
#endif
                free(buffer);
                return -1;
            }

            if(client_ip_scope_id.size() != 0)
            {
                client_addr6.sin6_scope_id = if_nametoindex(client_ip_scope_id.c_str());
            }

            addr_length = client_addr6_length;

            sock = socket(AF_INET6,
                          SOCK_DGRAM,
                          IPPROTO_UDP);

            flags = fcntl(sock,
                          F_GETFL,
                          0);
            flags &= ~O_NONBLOCK;
            fcntl(sock,
                  F_SETFL,
                  flags);

            // bind
            ret = bind(sock,
                       (struct sockaddr *)&client_addr6,
                       sizeof(client_addr6));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error\n");
#endif
                close(sock);
                free(buffer);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &client_addr6.sin6_addr,
                      client_addr6_string_pointer,
                      INET6_ADDRSTRLEN);

#ifdef _DEBUG
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
        }


        // forwarder [client <> client <> server <> target]
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder_udp\n");
#endif

        send_message_id++;
        forwarder_udp(addr_length);

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        close(sock);
        free(buffer);
        return -1;
    }
}

