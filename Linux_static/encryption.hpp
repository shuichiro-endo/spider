/*
 * Title:  spider encryption.hpp (Linux static)
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
        bool flag;

    public:

    private:

    protected:
        char hex_char_to_int(char c);

        void hex_string_to_array(const char *hex_string,
                                 int32_t hex_string_length,
                                 unsigned char *buffer,
                                 int32_t buffer_size);

    public:
        Encryption(bool flag);
        ~Encryption();

        void set_flag(bool flag);
        bool get_flag();

        void print_bytes(char *buffer, int buffer_length) const;

        virtual int32_t encrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) const = 0;
        virtual int32_t decrypt(char *data,
                                int32_t data_size,
                                int32_t buffer_size) const = 0;
    };
}

#endif /* ENCRYPTION_H_ */

