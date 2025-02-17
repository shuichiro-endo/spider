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
}

