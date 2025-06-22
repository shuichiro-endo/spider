/*
 * Title:  spider message.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "spider.hpp"


namespace spider
{
    class Message
    {
    private:

    protected:
        char message_type;      // socks5:s  routing:r
        int32_t data_size = 0;
        char *data = NULL;

    public:

    private:

    protected:

    public:
        Message();

        ~Message();

        void set_message_type(char message_type);
        char get_message_type();

        void set_data_size(int32_t data_size);
        int32_t get_data_size();

        void set_data(char *);
        char *get_data();

        void print_bytes();

        virtual int32_t copy_to_buffer(char *buffer) = 0;
    };
}

#endif /* MESSAGE_H_ */

