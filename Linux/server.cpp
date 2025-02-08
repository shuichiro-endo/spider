/*
 * Title:  spider server.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5.hpp"
#include "server.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"


namespace spider
{
    Server::Server(uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string server_ip,
                   std::string server_port,
                   std::string client_destination_ip,
                   int32_t server_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   bool xor_flag,
                   std::string xor_key_hex_string,
                   std::shared_ptr<Messagemanager> message_manager)
    : Node(server_sock, message_manager)
    {
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
        this->xor_flag = xor_flag;
        this->xor_key_hex_string = xor_key_hex_string;

        this->sock = server_sock;
        this->message_manager = message_manager;
    }

    Server::~Server()
    {

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

    void Server::set_xor_flag(bool xor_flag)
    {
        this->xor_flag = xor_flag;
    }

    bool Server::get_xor_flag()
    {
        return xor_flag;
    }

    void Server::set_xor_key_hex_string(std::string xor_key_hex_string)
    {
        this->xor_key_hex_string = xor_key_hex_string;
    }

    std::string Server::get_xor_key_hex_string()
    {
        return xor_key_hex_string;
    }

    char Server::hex_char_to_int(char c)
    {
        char ret = 0;

        if((c >= '0') && (c <= '9'))
        {
            ret = c - '0';
        }else if((c >= 'a') && (c <= 'f'))
        {
            ret = c + 10 - 'a';
        }else if((c >= 'A') && (c <= 'F'))
        {
            ret = c + 10 - 'A';
        }else
        {
            ret = -1;
        }

        return ret;
    }

    int Server::hex_string_to_array(const char *hex_string,
                                    int hex_string_length,
                                    unsigned char *output,
                                    int output_size)
    {
        char tmp1 = 0;
        char tmp2 = 0;
        int output_length = 0;

        if(hex_string_length % 2 != 0){
#ifdef _DEBUG
            std::printf("[-] hex string length error\n");
#endif
            return -1;
        }

        if(hex_string_length / 2 > output_size){
#ifdef _DEBUG
            std::printf("[-] hex string length error\n");
#endif
            return -1;
        }

        for(int i = 0; i < hex_string_length; i += 2){
            tmp1 = hex_char_to_int(hex_string[i]);
            tmp2 = hex_char_to_int(hex_string[i + 1]);

            if(tmp1 == -1 || tmp2 == -1)
            {
#ifdef _DEBUG
                std::printf("[-] hex_char_to_int error\n");
#endif
                return -1;
            }

            tmp1 = tmp1 << 4;
            output[output_length] = (unsigned char)(tmp1 + tmp2);
            output_length++;
        }

        return output_length;
    }


    int32_t Server::encrypt_xor(char *buffer,
                                int32_t data_size)
    {
        int32_t ret = 0;
        int32_t key_length = xor_key_hex_string.size() * 2 + 1;
        unsigned char *key = (unsigned char *)calloc(key_length,
                                                     sizeof(char));

        ret = hex_string_to_array(xor_key_hex_string.c_str(),
                                 xor_key_hex_string.size(),
                                 key,
                                 key_length);
        if(ret < 0)
        {
#ifdef _DEBUG
            std::printf("[-] hex_string_to_array error\n");
#endif
            free(key);
            return -1;
        }

        for(int32_t i = 0; i < data_size; i++)
        {
            buffer[i] = buffer[i] ^ key[i % key_length];
        }

        free(key);
        return 0;
    }

    int32_t Server::recv_message(char *buffer,
                                 int32_t buffer_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec)
    {
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
                if(rec > 0)
                {
                    std::memcpy(buffer,
                                socks5_message->get_data(),
                                rec);

#ifdef _DEBUG
                    print_bytes(buffer, rec);
#endif

                    if(xor_flag)
                    {
                        encrypt_xor(buffer,
                                    rec);
                    }
                }else
                {
#ifdef _DEBUG
                    printf("[-] recv_message recv error: %d\n",
                           rec);
#endif
                    return -1;
                }
            }else
            {
#ifdef _DEBUG
                std::printf("recv_message unknown message type: %c\n",
                            message_type);
#endif
                return -1;
            }
        }else
        {
#ifdef _DEBUG
            std::printf("recv_message error\n");
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
#ifdef _DEBUG
            std::printf("send_message data size error: %d\n",
                        data_size);
#endif
            free(socks5_message_data);
            return -1;
        }

        if(xor_flag)
        {
            encrypt_xor(buffer,
                        data_size);
        }

#ifdef _DEBUG
        print_bytes(buffer, data_size);
#endif

        socks5_message_data = (struct socks5_message_data *)calloc(1,
                                                                   sizeof(struct socks5_message_data));
        socks5_message_data->message_type = 's';
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
        fd_set writefds;
        int nfds = -1;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(target_sock, &writefds);
            nfds = target_sock + 1;
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
                std::printf("[+] forwarder_recv_data timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &writefds);
            if(ret){
#ifdef _DEBUG
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
#ifdef _DEBUG
                    std::printf("[+] [server -> target] send\n");
#endif
                    sen = send(target_sock,
                               buffer,
                               rec,
                               0);
                    if(sen <= 0)
                    {
                        break;
                    }
                }else
                {
                    break;
                }
            }
        }

        return 0;
    }

    int32_t Server::forwarder_send_data()
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
            FD_SET(target_sock, &readfds);
            nfds = target_sock + 1;
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
                std::printf("[+] forwarder_send_data timeout\n");
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &readfds);
            if(ret)
            {
#ifdef _DEBUG
                std::printf("[+] [server <- target] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(target_sock,
                           buffer,
                           socks5_message_data_max_size,
                           0);
                if(rec > 0)
                {
#ifdef _DEBUG
                    std::printf("[+] [client <- server] send_message\n");
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0)
                    {
                        break;
                    }
                }else{
                    break;
                }
            }
        }

        return 0;
    }

    int32_t Server::forwarder()
    {
        std::thread thread1(&Server::forwarder_recv_data, this);
        std::thread thread2(&Server::forwarder_send_data, this);

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

        sen = send_message(buffer,
                           sizeof(struct socks_response_ipv6),
                           tv_sec,
                           tv_usec);

        return sen;
    }

    int32_t Server::do_socks5_connection(std::shared_ptr<Socks5message> socks5_message)
    {
        static char authentication_method = 0x0; // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = "socks5user";
        char password[256] = "supersecretpassword";

        int32_t rec;
        int32_t sen;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;

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

        struct sockaddr_in target_addr, *tmp_ipv4;  // IPv4
        std::memset(&target_addr,
                    0,
                    sizeof(struct sockaddr_in));

        struct sockaddr_in6 target_addr6, *tmp_ipv6;	// IPv6
        std::memset(&target_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        struct addrinfo hints, *target_host_addr_info;
        std::memset(&hints,
                    0,
                    sizeof(struct addrinfo));

        int family = 0;
        char domainname[256] = {0};
        std::string domainname_string;
        unsigned short domainname_length = 0;

        char target_addr6_string[INET6_ADDRSTRLEN +1 ] = {0};
        char *target_addr6_string_pointer = target_addr6_string;
        int flags = 0;


        // socks SELECTION_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] recv selection request\n");
#endif
        rec = socks5_message->get_data_size();
        if(rec <= 0 || rec > NODE_BUFFER_SIZE){
#ifdef _DEBUG
            std::printf("[+] [client -> server] recv selection request error\n");
#endif
            free(buffer);
            return -1;
        }
        std::memcpy(buffer, socks5_message->get_data(), rec);

        if(xor_flag)
        {
            encrypt_xor(buffer,
                        rec);
        }

#ifdef _DEBUG
        std::printf("[+] [client -> server] recv selection request: %d bytes\n",
                    rec);
#endif
        struct selection_request *selection_request = (struct selection_request *)buffer;
        method = 0xFF;
        for(int i=0; i<selection_request->nmethods; i++)
        {
            if(selection_request->methods[i] == authentication_method)  // NO AUTHENTICATION REQUIRED or USERNAME/PASSWORD
            {
#ifdef _DEBUG
                std::printf("[+] selection_request->methods[%d]: %02x\n", i, selection_request->methods[i]);
#endif
                method = selection_request->methods[i];
                break;
            }
        }
        if(method == 0xFF)
        {
#ifdef _DEBUG
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

        sen = send_message(buffer,
                           sizeof(struct selection_response),
                           tv_sec,
                           tv_usec);

#ifdef _DEBUG
        std::printf("[+] [client <- server] send selection response: %d bytes\n",
                    sen);
#endif

        if(authentication_method != method)
        {
#ifdef _DEBUG
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
#ifdef _DEBUG
            std::printf("[+] [client -> server] recv username password authentication request\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec);

            if(rec <= 0)
            {
#ifdef _DEBUG
                std::printf("[-] [client -> server] recv username password authentication request error\n");
#endif
                free(buffer);
                return -1;
            }
#ifdef _DEBUG
            std::printf("[+] [client -> server] Receive username password authentication request: %d bytes\n",
                        rec);
#endif
            struct username_password_authentication_request_tmp *username_password_authentication_request_tmp = (struct username_password_authentication_request_tmp *)buffer;

            ulen = username_password_authentication_request_tmp->ulen;
            memcpy(uname, &username_password_authentication_request_tmp->uname, ulen);
            memcpy(&plen, &username_password_authentication_request_tmp->uname + ulen, 1);
            memcpy(passwd, &username_password_authentication_request_tmp->uname + ulen + 1, plen);
#ifdef _DEBUG
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
#ifdef _DEBUG
                std::printf("[+] succeed username password authentication\n");
#endif
                username_password_authentication_response->status = 0x0;

                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef _DEBUG
                std::printf("[+] [client <- server] send username password authentication response: %d bytes\n",
                            sen);
#endif
            }else
            {
#ifdef _DEBUG
                std::printf("[-] fail username password authentication\n");
#endif
                username_password_authentication_response->status = 0xFF;

                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef _DEBUG
                std::printf("[+] [client <- server] send selection response: %d bytes\n",
                            sen);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef _DEBUG
        std::printf("[+] [client -> server] recv socks request\n");
#endif
        std::memset(buffer,
                    0,
                    buffer_max_length);

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec);

        if(rec <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] [client -> server] recv socks request error\n");
#endif
            free(buffer);
            return -1;
        }
#ifdef _DEBUG
        std::printf("[+] [client -> server] recv socks request: %d bytes\n",
                    rec);
#endif

        atyp = socks_request->atyp;
        if(atyp != 0x1
           && atyp != 0x3
           && atyp != 0x4)
        {
#ifdef _DEBUG
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
        if(cmd != 0x1)  // CONNECT only
        {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
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

#ifdef _DEBUG
            std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                        target_ip.c_str(),
                        target_port.c_str());
#endif

            if(cmd == 0x1)  // CONNECT
            {
#ifdef _DEBUG
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET,
                                       SOCK_STREAM,
                                       0);

                flags = fcntl(target_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(target_sock,
                      F_SETFL,
                      flags);

                ret = connect(target_sock,
                              (struct sockaddr *)&target_addr,
                              sizeof(target_addr));
                if(ret < 0)
                {
#ifdef _DEBUG
                    std::printf("[-] [server <- target] cannot connect errno: %d\n", ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef _DEBUG
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    close(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef _DEBUG
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

#ifdef _DEBUG
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

            }else if(cmd == 0x2)    // BIND
            {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
            }else
            {
#ifdef _DEBUG
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

#ifdef _DEBUG
                std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                if(cmd == 0x1)  // CONNECT
                {
#ifdef _DEBUG
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET,
                                           SOCK_STREAM,
                                           0);

                    flags = fcntl(target_sock,
                                  F_GETFL,
                                  0);
                    flags &= ~O_NONBLOCK;
                    fcntl(target_sock,
                          F_SETFL,
                          flags);

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr,
                                  sizeof(target_addr));
                    if(ret < 0)
                    {
#ifdef _DEBUG
                        std::printf("[-] [server <- target] cannot connect errno: %d\n", ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef _DEBUG
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        close(target_sock);
                        free(buffer);
                        return -1;
                    }
#ifdef _DEBUG
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

#ifdef _DEBUG
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
                }else
                {
#ifdef _DEBUG
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
                          INET6_ADDRSTRLEN);

                if(target_addr6.sin6_scope_id > 0)
                {
                    target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                    target_port = ntohs(target_addr6.sin6_port);
                }else
                {
                    target_ip = target_addr6_string_pointer;
                    target_port = ntohs(target_addr6.sin6_port);
                }

#ifdef _DEBUG
                std::printf("[+] [server -> target] Connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                if(cmd == 0x1)  // CONNECT
                {
#ifdef _DEBUG
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET6,
                                           SOCK_STREAM,
                                           0);

                    flags = fcntl(target_sock,
                                  F_GETFL,
                                  0);
                    flags &= ~O_NONBLOCK;
                    fcntl(target_sock,
                          F_SETFL,
                          flags);

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr6,
                                  sizeof(target_addr6));
                    if(ret < 0){
#ifdef _DEBUG
                        std::printf("[-] [server <- target] cannot connect errno: %d\n", ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef _DEBUG
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        close(target_sock);
                        free(buffer);
                        return -1;
                    }

#ifdef _DEBUG
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

#ifdef _DEBUG
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
                }else
                {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
                      INET6_ADDRSTRLEN);

            if(target_addr6.sin6_scope_id > 0)
            {
                target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                target_port = ntohs(target_addr6.sin6_port);
            }else
            {
                target_ip = target_addr6_string_pointer;
                target_port = ntohs(target_addr6.sin6_port);
            }


#ifdef _DEBUG
            std::printf("[+] [server -> target] Connecting ip: %s port: %s\n",
                        target_ip.c_str(),
                        target_port.c_str());
#endif

            if(cmd == 0x1)  // CONNECT
            {
#ifdef _DEBUG
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET6,
                                       SOCK_STREAM,
                                       0);

                flags = fcntl(target_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(target_sock,
                      F_SETFL,
                      flags);

                ret = connect(target_sock, (struct sockaddr *)&target_addr6, sizeof(target_addr6));
                if(ret < 0){
#ifdef _DEBUG
                    std::printf("[-] [server <- target] cannot connect errno: %d\n", ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef _DEBUG
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    close(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef _DEBUG
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

#ifdef _DEBUG
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif
            }else if(cmd == 0x2)    // BIND
            {
#ifdef _DEBUG
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
#ifdef _DEBUG
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
            }else{
#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
        std::printf("[+] [client <> client <> server <> target] forwarder\n");
#endif
        ret = forwarder();

#ifdef _DEBUG
        std::printf("[+] worker exit\n");
#endif

        close(target_sock);
        free(buffer);
        return -1;
    }
}

