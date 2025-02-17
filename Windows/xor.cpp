/*
 * Title:  spider xor.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "encryption.hpp"
#include "xor.hpp"


namespace spider
{
    Xor::Xor(BOOL flag,
             std::string xor_key_hex_string)
    : Encryption(flag)
    {
        this->xor_key_hex_string = xor_key_hex_string;
        this->xor_key_hex_string_size = xor_key_hex_string.size();

        if(this->xor_key_hex_string_size > 0){
            if(this->xor_key_hex_string_size % 2 == 0){
                this->key_length = this->xor_key_hex_string_size / 2;
            }else{
                this->key_length = this->xor_key_hex_string_size / 2 + 1;
            }

            this->key = (unsigned char *)calloc(this->key_length,
                                                sizeof(char));

            hex_string_to_array();
        }else{
            this->key_length = 0;
            this->key = nullptr;
        }
    }

    Xor::~Xor()
    {
        free(key);
    }

    void Xor::set_xor_key_hex_string(std::string xor_key_hex_string)
    {
        this->xor_key_hex_string = xor_key_hex_string;
    }

    std::string Xor::get_xor_key_hex_string()
    {
        return xor_key_hex_string;
    }

    char Xor::hex_char_to_int(char c)
    {
        char ret = 0;

        if((c >= '0') && (c <= '9'))
        {
            c = c - '0';
        }else if((c >= 'a') && (c <= 'f'))
        {
            c = c + 10 - 'a';
        }else if((c >= 'A') && (c <= 'F'))
        {
            c = c + 10 - 'A';
        }else
        {
            c = 0;
        }

        return c;
    }

    void Xor::hex_string_to_array()
    {
        const char *hex_string = xor_key_hex_string.c_str();
        char tmp1 = 0;
        char tmp2 = 0;
        int32_t length = 0;

        for(int32_t i = 0; i < xor_key_hex_string_size && length < key_length; i += 2)
        {
            tmp1 = hex_char_to_int(hex_string[i]);
            tmp2 = hex_char_to_int(hex_string[i + 1]);

            tmp1 = tmp1 << 4;
            key[length] = (unsigned char)(tmp1 + tmp2);
            length++;
        }
    }


    int32_t Xor::encrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size) const
    {
        if(key_length == 0)
        {
            return 0;
        }else if(key_length > 0)
        {
            for(int32_t i = 0; i < data_size; i++)
            {
                data[i] = data[i] ^ key[i % key_length];
            }
        }else
        {
            return -1;
        }

        return 0;
    }

    int32_t Xor::decrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size) const
    {
        if(key_length == 0)
        {
            return 0;
        }else if(key_length > 0)
        {
            for(int32_t i = 0; i < data_size; i++)
            {
                data[i] = data[i] ^ key[i % key_length];
            }
        }else
        {
            return -1;
        }

        return 0;
    }
}

