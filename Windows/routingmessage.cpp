/*
 * Title:  spider routingmessage.cpp (Windows)
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
        this->data = (char *)calloc(SPIDER_MESSAGE_DATA_MAX_SIZE,
                                    sizeof(char));
    }

    Routingmessage::Routingmessage(uint32_t pipe_id,
                                   struct spider_message *spider_message)
    {
        this->pipe_id = pipe_id;
        this->message_type = spider_message->header.message_type;
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
            std::printf("[-] routing message data size error: %d\n",
                        this->data_size);
#endif
            this->data_size = SPIDER_MESSAGE_DATA_MAX_SIZE;
            std::memcpy(this->data,
                        spider_message->data,
                        SPIDER_MESSAGE_DATA_MAX_SIZE);
        }
    }

    Routingmessage::~Routingmessage()
    {
        if(this->data != NULL)
        {
            free(data);
        }
    }

    void Routingmessage::set_pipe_id(uint32_t pipe_id)
    {
        this->pipe_id = pipe_id;
    }

    uint32_t Routingmessage::get_pipe_id()
    {
        return pipe_id;
    }

    int32_t Routingmessage::copy_to_buffer(char *buffer)
    {
        int32_t length = 0;

        struct spider_message *spider_message = (struct spider_message *)buffer;
        spider_message->header.message_type = this->message_type;
        spider_message->header.data_size = htonl(this->data_size);
        std::memcpy(spider_message->data,
                    this->data,
                    this->data_size);

        length = sizeof(struct spider_message_header)
               + this->data_size;

        return length;
    }
}

