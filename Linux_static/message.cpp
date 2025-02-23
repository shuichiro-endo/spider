/*
 * Title:  spider message.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "message.hpp"


namespace spider
{
    Message::Message()
    {

    }

    Message::~Message()
    {

    }

    void Message::set_message_type(char message_type)
    {
        this->message_type = message_type;
    }

    char Message::get_message_type()
    {
        return message_type;
    }
}

