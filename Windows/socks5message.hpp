/*
 * Title:  spider socks5message.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SOCKS5MESSAGE_H_
#define SOCKS5MESSAGE_H_

#include "spider.hpp"
#include "message.hpp"


namespace spider
{
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
        uint8_t receive_flag;      // received:1
        uint8_t receive_result;    // ok:0  ng:1
        uint8_t command_result;    // ok:0  ng:1

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
                      int32_t data_size,
                      char *data);

        Socks5message(char message_type,
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
                      std::string destination_ip);

        Socks5message(struct spider_message *spider_message);

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

        void set_receive_flag(uint8_t receive_flag);
        uint8_t get_receive_flag();

        void set_receive_result(uint8_t receive_result);
        uint8_t get_receive_result();

        void set_command_result(uint8_t command_result);
        uint8_t get_command_result();

        int32_t copy_to_buffer(char *buffer) override;
    };
}

#endif /* SOCKS5MESSAGE_H_ */

