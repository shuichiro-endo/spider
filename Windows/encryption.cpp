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
}

