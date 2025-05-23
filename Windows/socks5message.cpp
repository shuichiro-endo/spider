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
                                 int32_t data_size,
                                 char *data)
    {
        this->message_type = message_type;
        this->receive_flag = 0;
        this->receive_result = 0;
        this->command_result = 0;
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

        if(this->data_size > 0)
        {
            this->data = (char *)calloc(SPIDER_MESSAGE_DATA_MAX_SIZE,
                                        sizeof(char));
        }

        if(this->data_size > 0 &&
           this->data_size <= SPIDER_MESSAGE_DATA_MAX_SIZE)
        {
            std::memcpy(this->data,
                        data,
                        this->data_size);
        }else if(this->data_size > SPIDER_MESSAGE_DATA_MAX_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks5 message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = SPIDER_MESSAGE_DATA_MAX_SIZE;
            std::memcpy(this->data,
                        data,
                        SPIDER_MESSAGE_DATA_MAX_SIZE);
        }
    }

    Socks5message::Socks5message(char message_type,
                                 uint8_t receive_flag,
                                 uint8_t receive_result,
                                 uint8_t command_result,
                                 uint32_t message_id,
                                 uint32_t connection_id,
                                 uint32_t client_id,
                                 uint32_t server_id,
                                 char source_node_type,
                                 std::string source_ip,
                                 char destination_node_type,
                                 std::string destination_ip)
    {
        this->message_type = message_type;
        this->receive_flag = receive_flag;
        this->receive_result = receive_result;
        this->command_result = command_result;
        this->message_id = message_id;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->source_node_type = source_node_type;
        this->source_ip = source_ip;
        this->destination_node_type = destination_node_type;
        this->destination_ip = destination_ip;
        this->tv_sec = 0;
        this->tv_usec = 0;
        this->forwarder_tv_sec = 0;
        this->forwarder_tv_usec = 0;
        this->data_size = 0;
        this->data = NULL;
    }

    Socks5message::Socks5message(struct spider_message *spider_message)
    {
        this->message_type = spider_message->header.message_type;
        this->receive_flag = spider_message->header.receive_flag;
        this->receive_result = spider_message->header.receive_result;
        this->command_result = spider_message->header.command_result;
        this->message_id = ntohl(spider_message->header.message_id);
        this->connection_id = ntohl(spider_message->header.connection_id);
        this->client_id = ntohl(spider_message->header.client_id);
        this->server_id = ntohl(spider_message->header.server_id);
        this->source_node_type = spider_message->header.source_node_type;
        this->source_ip = spider_message->header.source_ip;
        this->destination_node_type = spider_message->header.destination_node_type;
        this->destination_ip = spider_message->header.destination_ip;
        this->tv_sec = ntohl(spider_message->header.tv_sec);
        this->tv_usec = ntohl(spider_message->header.tv_usec);
        this->forwarder_tv_sec = ntohl(spider_message->header.forwarder_tv_sec);
        this->forwarder_tv_usec = ntohl(spider_message->header.forwarder_tv_usec);
        this->data_size = ntohl(spider_message->header.data_size);

        if(this->data_size > 0)
        {
            this->data = (char *)calloc(SPIDER_MESSAGE_DATA_MAX_SIZE,
                                        sizeof(char));
        }

        if(this->data_size > 0 &&
           this->data_size <= SPIDER_MESSAGE_DATA_MAX_SIZE)
        {
            std::memcpy(this->data,
                        spider_message->data,
                        this->data_size);
        }else if(this->data_size > SPIDER_MESSAGE_DATA_MAX_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks5 message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = SPIDER_MESSAGE_DATA_MAX_SIZE;
            std::memcpy(this->data,
                        spider_message->data,
                        SPIDER_MESSAGE_DATA_MAX_SIZE);
        }
    }

    Socks5message::~Socks5message()
    {
        if(data != NULL)
        {
            free(data);
        }
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

    void Socks5message::set_receive_flag(uint8_t receive_flag)
    {
        this->receive_flag = receive_flag;
    }

    uint8_t Socks5message::get_receive_flag()
    {
        return receive_flag;
    }

    void Socks5message::set_receive_result(uint8_t receive_result)
    {
        this->receive_result = receive_result;
    }

    uint8_t Socks5message::get_receive_result()
    {
        return receive_result;
    }

    void Socks5message::set_command_result(uint8_t command_result)
    {
        this->command_result = command_result;
    }

    uint8_t Socks5message::get_command_result()
    {
        return command_result;
    }

    int32_t Socks5message::copy_to_buffer(char *buffer)
    {
        int32_t length = 0;

        struct spider_message *spider_message = (struct spider_message *)buffer;
        spider_message->header.message_type = this->message_type;
        spider_message->header.receive_flag = this->receive_flag;
        spider_message->header.receive_result = this->receive_result;
        spider_message->header.command_result = this->command_result;
        spider_message->header.message_id = htonl(this->message_id);
        spider_message->header.connection_id = htonl(this->connection_id);
        spider_message->header.client_id = htonl(this->client_id);
        spider_message->header.server_id = htonl(this->server_id);
        spider_message->header.source_node_type = this->source_node_type;
        std::memcpy(spider_message->header.source_ip,
                    this->source_ip.c_str(),
                    this->source_ip.size());
        spider_message->header.destination_node_type = this->destination_node_type;
        std::memcpy(spider_message->header.destination_ip,
                    this->destination_ip.c_str(),
                    this->destination_ip.size());
        spider_message->header.tv_sec = htonl(this->tv_sec);
        spider_message->header.tv_usec = htonl(this->tv_usec);
        spider_message->header.forwarder_tv_sec = htonl(this->forwarder_tv_sec);
        spider_message->header.forwarder_tv_usec = htonl(this->forwarder_tv_usec);
        spider_message->header.data_size = htonl(this->data_size);

        if(this->data_size > 0)
        {
            std::memcpy(spider_message->data,
                        this->data,
                        this->data_size);
        }

        length = sizeof(struct spider_message_header)
               + this->data_size;

        return length;
    }
}

