/*
 * Title:  spider routingmessage.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "routingmessage.hpp"


namespace spider
{
    Routingmessage::Routingmessage()
    {
        this->pipe_id = 0;
        this->message_type = 'r';
        this->data_size = 0;
        this->data = (char *)calloc(ROUTING_MESSAGE_DATA_SIZE,
                                    sizeof(char));
    }

    Routingmessage::Routingmessage(uint32_t pipe_id,
                                   struct routing_message_data *routing_message_data)
    {
        this->pipe_id = pipe_id;
        this->message_type = routing_message_data->message_type;
        this->data_size = ntohs(routing_message_data->data_size);

        this->data = (char *)calloc(ROUTING_MESSAGE_DATA_SIZE,
                                    sizeof(char));
        if(this->data_size <= ROUTING_MESSAGE_DATA_SIZE){
            std::memcpy(this->data,
                        routing_message_data->data,
                        this->data_size);
        }else{
#ifdef _DEBUG
            std::printf("[-] routing message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = ROUTING_MESSAGE_DATA_SIZE;
            std::memcpy(this->data,
                        routing_message_data->data,
                        ROUTING_MESSAGE_DATA_SIZE);
        }
    }

    Routingmessage::~Routingmessage()
    {
        free(data);
    }

    void Routingmessage::set_pipe_id(uint32_t pipe_id)
    {
        this->pipe_id = pipe_id;
    }

    uint32_t Routingmessage::get_pipe_id()
    {
        return pipe_id;
    }

    void Routingmessage::set_data_size(uint16_t data_size)
    {
        this->data_size = data_size;
    }

    uint16_t Routingmessage::get_data_size()
    {
        return data_size;
    }

    void Routingmessage::set_data(char *)
    {
        this->data = data;
    }

    char *Routingmessage::get_data()
    {
        return data;
    }

    void Routingmessage::print_bytes()
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

    int32_t Routingmessage::copy_to_buffer(char *buffer) const
    {
        int32_t length = 0;

        struct routing_message_data *routing_message_data = (struct routing_message_data *)buffer;
        routing_message_data->message_type = this->message_type;
        routing_message_data->data_size = htons(this->data_size);
        std::memcpy(routing_message_data->data, this->data, this->data_size);

        length = sizeof(struct routing_message_data_header)
               + this->data_size;

        return length;
    }
}

