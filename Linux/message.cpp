/*
 * Title:  spider message.cpp (Linux)
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

    void Message::set_data_size(int32_t data_size)
    {
        this->data_size = data_size;
    }

    int32_t Message::get_data_size()
    {
        return data_size;
    }

    void Message::set_data(char *)
    {
        this->data = data;
    }

    char *Message::get_data()
    {
        return data;
    }

    void Message::print_bytes()
    {
        for(int32_t i=0; i<data_size; i++){
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
}

