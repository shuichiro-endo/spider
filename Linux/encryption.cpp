/*
 * Title:  spider encryption.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "encryption.hpp"


namespace spider
{
    Encryption::Encryption(bool flag)
    {
        this->flag = flag;
    }

    Encryption::~Encryption()
    {

    }

    void Encryption::set_flag(bool flag)
    {
        this->flag = flag;
    }

    bool Encryption::get_flag()
    {
        return flag;
    }

    char Encryption::hex_char_to_int(char c)
    {
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

    void Encryption::hex_string_to_array(const char *hex_string,
                                         int32_t hex_string_length,
                                         unsigned char *buffer,
                                         int32_t buffer_size)
    {
        char tmp1 = 0;
        char tmp2 = 0;
        int32_t length = 0;


        for(int32_t i = 0; i < hex_string_length && length < buffer_size; i += 2)
        {
            tmp1 = hex_char_to_int(hex_string[i]);
            tmp2 = hex_char_to_int(hex_string[i + 1]);

            tmp1 = tmp1 << 4;
            buffer[length] = (unsigned char)(tmp1 + tmp2);
            length++;
        }
    }

    void Encryption::print_bytes(char *buffer, int buffer_length) const
    {
        for(int i=0; i<buffer_length; i++){
            if(i != 0 && i%16 == 0){
                std::printf("\n");
            }else if(i%16 == 8){
                std::printf(" ");
            }
            std::printf("%02x ", buffer[i] & 0xff);
        }
        std::printf("\n");

        return;
    }
}

