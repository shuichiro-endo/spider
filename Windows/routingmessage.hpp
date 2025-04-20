/*
 * Title:  spider routingmessage.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ROUTINGMESSAGE_H_
#define ROUTINGMESSAGE_H_

#include "spider.hpp"
#include "message.hpp"


namespace spider
{
#pragma pack(push, 1)
    struct routing_message_data
    {
        char message_type;
        char reserved;
        uint16_t data_size;
        char data[ROUTING_MESSAGE_DATA_SIZE];
    };

    struct routing_message_data_header
    {
        char message_type;
        char reserved;
        uint16_t data_size;
    };

    struct route_data
    {
        char ip[INET6_ADDR_STRING_LENGTH + 1];
        uint8_t metric;
    };
#pragma pack(pop)

    class Routingmessage : public Message
    {
    private:
        uint32_t pipe_id;
        uint16_t data_size;
        char *data;

    public:

    private:

    public:
        Routingmessage();

        Routingmessage(uint32_t pipe_id,
                       struct routing_message_data *routing_message_data);

        ~Routingmessage();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        void set_data_size(uint16_t data_size);
        uint16_t get_data_size();

        void set_data(char *);
        char *get_data();

        void print_bytes();

        int32_t copy_to_buffer(char *buffer) override;
    };
}

#endif /* ROUTINGMESSAGE_H_ */

