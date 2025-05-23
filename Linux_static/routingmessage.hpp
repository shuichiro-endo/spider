/*
 * Title:  spider routingmessage.hpp (Linux static)
 * Author: Shuichiro Endo
 */

#ifndef ROUTINGMESSAGE_H_
#define ROUTINGMESSAGE_H_

#include "spider.hpp"
#include "message.hpp"


namespace spider
{
    class Routingmessage : public Message
    {
    private:
        uint32_t pipe_id;

    public:

    private:

    public:
        Routingmessage();

        Routingmessage(uint32_t pipe_id,
                       struct spider_message *spider_message);

        ~Routingmessage();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        int32_t copy_to_buffer(char *buffer) override;
    };
}

#endif /* ROUTINGMESSAGE_H_ */

