/*
 * Title:  spider encryption.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "encryption.hpp"


namespace spider
{
    Encryption::Encryption(BOOL flag)
    {
        this->flag = flag;
    }

    Encryption::~Encryption()
    {

    }

    void Encryption::set_flag(BOOL flag)
    {
        this->flag = flag;
    }

    BOOL Encryption::get_flag()
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

