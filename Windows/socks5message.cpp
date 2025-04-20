/*
 * Title:  spider socks5message.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5message.hpp"


namespace spider
{
    Socks5message::Socks5message(char message_type,
                                 uint32_t message_id,
                                 uint32_t connection_id,
                                 uint32_t client_id,
                                 uint32_t server_id,
                                 char source_node_type,
                                 std::string source_ip,
                                 char destination_node_type,
                                 std::string destination_ip,
                                 int32_t tv_sec,
                                 int32_t tv_usec,
                                 int32_t forwarder_tv_sec,
                                 int32_t forwarder_tv_usec,
                                 uint16_t data_size,
                                 char *data)
    {
        this->message_type = message_type;
        this->message_id = message_id;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->source_node_type = source_node_type;
        this->source_ip = source_ip;
        this->destination_node_type = destination_node_type;
        this->destination_ip = destination_ip;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->data_size = data_size;

        this->data = (char *)calloc(SOCKS5_MESSAGE_DATA_SIZE,
                                    sizeof(char));
        if(this->data_size <= SOCKS5_MESSAGE_DATA_SIZE){
            std::memcpy(this->data,
                        data,
                        this->data_size);
        }else{
#ifdef DEBUGPRINT
            std::printf("[-] socks5 message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = SOCKS5_MESSAGE_DATA_SIZE;
            std::memcpy(this->data,
                        data,
                        SOCKS5_MESSAGE_DATA_SIZE);
        }
    }

    Socks5message::Socks5message(struct socks5_message_data *socks5_message_data)
    {
        this->message_type = socks5_message_data->message_type;
        this->message_id = ntohl(socks5_message_data->message_id);
        this->connection_id = ntohl(socks5_message_data->connection_id);
        this->client_id = ntohl(socks5_message_data->client_id);
        this->server_id = ntohl(socks5_message_data->server_id);
        this->source_node_type = socks5_message_data->source_node_type;
        this->source_ip = socks5_message_data->source_ip;
        this->destination_node_type = socks5_message_data->destination_node_type;
        this->destination_ip = socks5_message_data->destination_ip;
        this->tv_sec = ntohl(socks5_message_data->tv_sec);
        this->tv_usec = ntohl(socks5_message_data->tv_usec);
        this->forwarder_tv_sec = ntohl(socks5_message_data->forwarder_tv_sec);
        this->forwarder_tv_usec = ntohl(socks5_message_data->forwarder_tv_usec);
        this->data_size = ntohs(socks5_message_data->data_size);

        this->data = (char *)calloc(SOCKS5_MESSAGE_DATA_SIZE,
                                    sizeof(char));
        if(this->data_size <= SOCKS5_MESSAGE_DATA_SIZE){
            std::memcpy(this->data,
                        socks5_message_data->data,
                        this->data_size);
        }else{
#ifdef DEBUGPRINT
            std::printf("[-] socks5 message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = SOCKS5_MESSAGE_DATA_SIZE;
            std::memcpy(this->data,
                        socks5_message_data->data,
                        SOCKS5_MESSAGE_DATA_SIZE);
        }
    }

    Socks5message::~Socks5message()
    {
        free(data);
    }

    void Socks5message::set_message_id(uint32_t message_id)
    {
        this->message_id = message_id;
    }

    uint32_t Socks5message::get_message_id()
    {
        return message_id;
    }

    void Socks5message::set_connection_id(uint32_t connection_id)
    {
        this->connection_id = connection_id;
    }

    uint32_t Socks5message::get_connection_id()
    {
        return connection_id;
    }

    void Socks5message::set_client_id(uint32_t client_id)
    {
        this->client_id = client_id;
    }

    uint32_t Socks5message::get_client_id()
    {
        return client_id;
    }

    void Socks5message::set_server_id(uint32_t server_id)
    {
        this->server_id = server_id;
    }

    uint32_t Socks5message::get_server_id()
    {
        return server_id;
    }

    void Socks5message::set_destination_node_type(char destination_node_type)
    {
        this->destination_node_type = destination_node_type;
    }

    char Socks5message::get_destination_node_type()
    {
        return destination_node_type;
    }

    void Socks5message::set_destination_ip(std::string destination_ip)
    {
        this->destination_ip = destination_ip;
    }

    std::string Socks5message::get_destination_ip()
    {
        return destination_ip;
    }

    void Socks5message::set_source_node_type(char source_node_type)
    {
        this->source_node_type = source_node_type;
    }

    char Socks5message::get_source_node_type()
    {
        return source_node_type;
    }

    void Socks5message::set_source_ip(std::string source_ip)
    {
        this->source_ip = source_ip;
    }

    std::string Socks5message::get_source_ip()
    {
        return source_ip;
    }

    void Socks5message::set_tv_sec(int32_t tv_sec)
    {
        this->tv_sec = tv_sec;
    }

    int32_t Socks5message::get_tv_sec()
    {
        return tv_sec;
    }

    void Socks5message::set_tv_usec(int32_t tv_usec)
    {
        this->tv_usec = tv_usec;
    }

    int32_t Socks5message::get_tv_usec()
    {
        return tv_usec;
    }

    void Socks5message::set_forwarder_tv_sec(int32_t forwarder_tv_sec)
    {
        this->forwarder_tv_sec = forwarder_tv_sec;
    }

    int32_t Socks5message::get_forwarder_tv_sec()
    {
        return forwarder_tv_sec;
    }

    void Socks5message::set_forwarder_tv_usec(int32_t forwarder_tv_usec)
    {
        this->forwarder_tv_usec = forwarder_tv_usec;
    }

    int32_t Socks5message::get_forwarder_tv_usec()
    {
        return forwarder_tv_usec;
    }

    void Socks5message::set_data_size(uint16_t data_size)
    {
        this->data_size = data_size;
    }

    uint16_t Socks5message::get_data_size()
    {
        return data_size;
    }

    void Socks5message::set_data(char *)
    {
        this->data = data;
    }

    char *Socks5message::get_data()
    {
        return data;
    }

    void Socks5message::print_bytes()
    {
        for(int i=0; i<data_size; i++){
            if(i != 0 && i%16 == 0){
                std::printf("\n");
            }else if(i%16 == 8){
                std::printf(" ");
            }
            std::printf("%02x ", data[i] & 0xff);
        }
        std::printf("\n");

        return;
    }

    int32_t Socks5message::copy_to_buffer(char *buffer)
    {
        int32_t length = 0;

        struct socks5_message_data *socks5_message_data = (struct socks5_message_data *)buffer;
        socks5_message_data->message_type = this->message_type;
        socks5_message_data->message_id = htonl(this->message_id);
        socks5_message_data->connection_id = htonl(this->connection_id);
        socks5_message_data->client_id = htonl(this->client_id);
        socks5_message_data->server_id = htonl(this->server_id);
        socks5_message_data->source_node_type = this->source_node_type;
        std::memcpy(socks5_message_data->source_ip, this->source_ip.c_str(), this->source_ip.size());
        socks5_message_data->destination_node_type = this->destination_node_type;
        std::memcpy(socks5_message_data->destination_ip, this->destination_ip.c_str(), this->destination_ip.size());
        socks5_message_data->tv_sec = htonl(this->tv_sec);
        socks5_message_data->tv_usec = htonl(this->tv_usec);
        socks5_message_data->forwarder_tv_sec = htonl(this->forwarder_tv_sec);
        socks5_message_data->forwarder_tv_usec = htonl(this->forwarder_tv_usec);
        socks5_message_data->data_size = htons(this->data_size);
        std::memcpy(socks5_message_data->data, this->data, this->data_size);

        length = sizeof(struct socks5_message_data_header)
               + this->data_size;

        return length;
    }
}

