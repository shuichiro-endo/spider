/*
 * Title:  spider encryption.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include "spider.hpp"


namespace spider
{
    class Encryption
    {
    private:

    protected:
        BOOL flag;

    public:

    private:

    protected:
        char hex_char_to_int(char c);

        void hex_string_to_array(const char *hex_string,
                                 int32_t hex_string_length,
                                 unsigned char *buffer,
                                 int32_t buffer_size);

    public:
        Encryption(BOOL flag);
        ~Encryption();

        void set_flag(BOOL flag);
        BOOL get_flag();

        void print_bytes(char *buffer, int buffer_length);

        virtual int32_t encrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) = 0;
        virtual int32_t decrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) = 0;
    };
}

#endif /* ENCRYPTION_H_ */

