/*
 * Title:  spider socks5message.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef SOCKS5MESSAGE_H_
#define SOCKS5MESSAGE_H_

#include "spider.hpp"
#include "message.hpp"


namespace spider
{
    struct socks5_message_data
    {
        char message_type;
        char reserved1;
        char reserved2;
        char reserved3;
        uint32_t message_id;
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        char source_node_type;
        char reserved4;
        char source_ip[INET6_ADDRSTRLEN + 1];
        char reserved5;
        char destination_node_type;
        char reserved6;
        char destination_ip[INET6_ADDRSTRLEN + 1];
        char reserved7;
        int32_t tv_sec;
        int32_t tv_usec;
        int32_t forwarder_tv_sec;
        int32_t forwarder_tv_usec;
        uint16_t data_size;
        char data[SOCKS5_MESSAGE_DATA_SIZE];
    };

    struct socks5_message_data_header
    {
        char message_type;
        char reserved1;
        char reserved2;
        char reserved3;
        uint32_t message_id;
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        char source_node_type;
        char reserved4;
        char source_ip[INET6_ADDRSTRLEN + 1];
        char reserved5;
        char destination_node_type;
        char reserved6;
        char destination_ip[INET6_ADDRSTRLEN + 1];
        char reserved7;
        int32_t tv_sec;
        int32_t tv_usec;
        int32_t forwarder_tv_sec;
        int32_t forwarder_tv_usec;
        uint16_t data_size;
    };

    class Socks5message : public Message
    {
    private:
        uint32_t message_id;
        uint32_t connection_id;
        uint32_t client_id;
        uint32_t server_id;
        char source_node_type;
        std::string source_ip;
        char destination_node_type;
        std::string destination_ip;
        int32_t tv_sec;
        int32_t tv_usec;
        int32_t forwarder_tv_sec;
        int32_t forwarder_tv_usec;
        uint16_t data_size;
        char *data;

    public:

    private:

    public:
        Socks5message(char message_type,
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
                      char *data);

        Socks5message(struct socks5_message_data *socks5_message_data);

        ~Socks5message();

        void set_message_id(uint32_t message_id);
        uint32_t get_message_id();

        void set_connection_id(uint32_t connection_id);
        uint32_t get_connection_id();

        void set_client_id(uint32_t client_id);
        uint32_t get_client_id();

        void set_server_id(uint32_t server_id);
        uint32_t get_server_id();

        void set_source_node_type(char source_node);
        char get_source_node_type();

        void set_source_ip(std::string source_ip);
        std::string get_source_ip();

        void set_destination_node_type(char destination_node);
        char get_destination_node_type();

        void set_destination_ip(std::string destination_ip);
        std::string get_destination_ip();

        void set_tv_sec(int32_t tv_sec);
        int32_t get_tv_sec();

        void set_tv_usec(int32_t tv_usec);
        int32_t get_tv_usec();

        void set_forwarder_tv_sec(int32_t forwarder_tv_sec);
        int32_t get_forwarder_tv_sec();

        void set_forwarder_tv_usec(int32_t forwarder_tv_usec);
        int32_t get_forwarder_tv_usec();

        void set_data_size(uint16_t data_size);
        uint16_t get_data_size();

        void set_data(char *);
        char *get_data();

        void print_bytes();

        int32_t copy_to_buffer(char *buffer) const override;
    };
}

#endif /* SOCKS5MESSAGE_H_ */

