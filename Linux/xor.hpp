/*
 * Title:  spider xor.hpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef XOR_H_
#define XOR_H_

#include "spider.hpp"


namespace spider
{
    class Encryption;

    class Xor : public Encryption
    {
    private:
        std::string xor_key_hex_string;
        int32_t xor_key_hex_string_size;
        unsigned char *key;
        int32_t key_length;

    public:

    private:

    public:
        Xor(bool flag,
            std::string xor_key_hex_string);
        ~Xor();

        void set_xor_key_hex_string(std::string xor_key_hex_string);
        std::string get_xor_key_hex_string();

        int32_t encrypt(char *data,
                        int32_t data_size,
                        int32_t buffer_size) const override;
        int32_t decrypt(char *data,
                        int32_t data_size,
                        int32_t buffer_size) const override;
    };
}

#endif /* XOR_H_ */

