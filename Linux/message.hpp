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
        char message_type;

    public:

    private:

    protected:

    public:
        Message();

        ~Message();

        void set_message_type(char message_type);
        char get_message_type();

        virtual int32_t copy_to_buffer(char *buffer) = 0;
    };
}

#endif /* MESSAGE_H_ */

