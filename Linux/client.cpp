/*
 * Title:  spider client.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5.hpp"
#include "client.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"


namespace spider
{
    Client::Client(uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string client_ip,
                   std::string client_port,
                   std::string server_destination_ip,
                   int32_t client_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   bool xor_flag,
                   std::string xor_key_hex_string,
                   std::shared_ptr<Messagemanager> message_manager)
    : Node(client_sock, message_manager)
    {
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->client_ip = client_ip;
        this->client_port = client_port;
        this->server_destination_ip = server_destination_ip;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->xor_flag = xor_flag;
        this->xor_key_hex_string = xor_key_hex_string;

        this->sock = client_sock;
        this->message_manager = message_manager;
    }

    Client::~Client()
    {

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

    void Client::set_client_port(std::string client_port)
    {
        this->client_port = client_port;
    }

    std::string Client::get_client_port()
    {
        return client_port;
    }

    void Client::set_server_destination_ip(std::string server_destination_ip)
    {
        this->server_destination_ip = server_destination_ip;
    }

    std::string Client::get_server_destination_ip()
    {
        return server_destination_ip;
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

    void Client::set_xor_flag(bool xor_flag)
    {
        this->xor_flag = xor_flag;
    }

    bool Client::get_xor_flag()
    {
        return xor_flag;
    }

    void Client::set_xor_key_hex_string(std::string xor_key_hex_string)
    {
        this->xor_key_hex_string = xor_key_hex_string;
    }

    std::string Client::get_xor_key_hex_string()
    {
        return xor_key_hex_string;
    }

    char Client::hex_char_to_int(char c)
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

    int Client::hex_string_to_array(const char *hex_string,
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


    int32_t Client::encrypt_xor(char *buffer,
                                int32_t data_size)
    {
        int32_t ret = 0;
        int32_t key_length = xor_key_hex_string.size() / 2;
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
            buffer[i] = buffer[i] ^ key[i%key_length];
        }

        free(key);
        return 0;
    }

    int32_t Client::recv_message(char *buffer,
                                 int32_t buffer_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec,
                                 bool register_server_id_flag)
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
                if(rec >= 0)
                {
                    if(register_server_id_flag)
                    {
                        set_server_id(socks5_message->get_server_id());
                    }

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
        socks5_message_data->source_node_type = 'c';
        memcpy(&socks5_message_data->source_ip, client_ip.c_str(), client_ip.size());
        socks5_message_data->destination_node_type = 's';
        memcpy(&socks5_message_data->destination_ip, server_destination_ip.c_str(), server_destination_ip.size());
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
                std::printf("[+] forwarder_recv_data timeout\n");
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
                if(rec > 0)
                {
#ifdef _DEBUG
                    std::printf("[+] [client -> server] send_message\n");
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0){
                        break;
                    }
                }else{
                    break;
                }
            }
        }

        return 0;
    }

    int32_t Client::forwarder_send_data()
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
                std::printf("[+] forwarder_send_data timeout\n");
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
#ifdef _DEBUG
                    std::printf("[+] [client <- client] send\n");
#endif
                    sen = send(sock,
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

            if(rec <= 0)
            {
#ifdef _DEBUG
                std::printf("[-] [client <- server] recv username password authentication response error\n");
#endif
                free(buffer);
                return -1;
            }
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

        if(rec <= 0)
        {
#ifdef _DEBUG
            std::printf("[-] [client <- server] recv socks response error\n");
#endif
            free(buffer);
            return -1;
        }
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
        forwarder();

#ifdef _DEBUG
        printf("[+] worker exit\n");
#endif

        free(buffer);
        return -1;
    }
}

