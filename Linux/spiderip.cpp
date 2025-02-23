/*
 * Title:  spider spiderip.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spiderip.hpp"


namespace spider
{
    Spiderip::Spiderip(std::string spider_ipv4,
                       std::string spider_ipv6)
    {
        this->spider_ipv4 = spider_ipv4;
        this->spider_ipv6 = spider_ipv6;

        if(this->spider_ipv6.find("%") != std::string::npos)    // scope_id
        {
            this->spider_ipv6_scope_id = strstr(this->spider_ipv6.c_str(), "%") + 1;
            memcpy(this->addr6_string_pointer,
                   this->spider_ipv6.c_str(),
                   this->spider_ipv6.size() - this->spider_ipv6_scope_id.size() - 1);
            this->spider_ipv6 = this->addr6_string_pointer;
        }
    }

    Spiderip::~Spiderip()
    {

    }

    void Spiderip::set_spider_ipv4(std::string spider_ipv4)
    {
        this->spider_ipv4 = spider_ipv4;
    }

    std::string Spiderip::get_spider_ipv4()
    {
        return spider_ipv4;
    }

    void Spiderip::set_spider_ipv6(std::string spider_ipv6)
    {
        this->spider_ipv6 = spider_ipv6;
    }

    std::string Spiderip::get_spider_ipv6()
    {
        return spider_ipv6;
    }

    void Spiderip::set_spider_ipv6_scope_id(std::string spider_ipv6_scope_id)
    {
        this->spider_ipv6_scope_id = spider_ipv6_scope_id;
    }

    std::string Spiderip::get_spider_ipv6_scope_id()
    {
        return spider_ipv6_scope_id;
    }
}

