/*
 * Title:  spider xor.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "encryption.hpp"
#include "xor.hpp"


namespace spider
{
    Xor::Xor(bool flag,
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

            hex_string_to_array(this->xor_key_hex_string.c_str(),
                                this->xor_key_hex_string_size,
                                this->key,
                                this->key_length);
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

    int32_t Xor::encrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size)
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

        return data_size;
    }

    int32_t Xor::decrypt(char *data,
                         int32_t data_size,
                         int32_t buffer_size)
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

        return data_size;
    }
}

