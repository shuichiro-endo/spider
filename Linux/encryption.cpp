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

