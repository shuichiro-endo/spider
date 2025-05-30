/*
 * Title:  spider spiderip.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spiderip.hpp"


namespace spider
{
    Spiderip::Spiderip(std::string spider_ipv4,
                       std::string spider_ipv6_global,
                       std::string spider_ipv6_unique_local,
                       std::string spider_ipv6_link_local)
    {
        this->spider_ipv4 = spider_ipv4;
        this->spider_ipv6_global = spider_ipv6_global;
        this->spider_ipv6_unique_local = spider_ipv6_unique_local;
        this->spider_ipv6_link_local = spider_ipv6_link_local;

        if(!spider_ipv6_link_local.empty())    // scope_id
        {
            if(spider_ipv6_link_local.find("%") == std::string::npos)
            {
                std::printf("[-] ipv6 link local address does not include a scope id\n");

                init_flag = false;
            }else
            {
                this->spider_ipv6_link_local_scope_id = strstr(this->spider_ipv6_link_local.c_str(), "%") + 1;
                if(this->spider_ipv6_link_local_scope_id.empty())
                {
                    std::printf("[-] ipv6 link local address does not include a scope id\n");

                    init_flag = false;
                }else
                {
                    memcpy(this->addr6_string_pointer,
                           this->spider_ipv6_link_local.c_str(),
                           this->spider_ipv6_link_local.size() - this->spider_ipv6_link_local_scope_id.size() - 1);
                    this->spider_ipv6_link_local = this->addr6_string_pointer;

                    init_flag = true;
                }
            }
        }else
        {
            init_flag = true;
        }
    }

    Spiderip::~Spiderip()
    {

    }

    void Spiderip::set_init_flag(bool init_flag)
    {
        this->init_flag = init_flag;
    }

    bool Spiderip::get_init_flag()
    {
        return init_flag;
    }

    void Spiderip::set_spider_ipv4(std::string spider_ipv4)
    {
        this->spider_ipv4 = spider_ipv4;
    }

    std::string Spiderip::get_spider_ipv4()
    {
        return spider_ipv4;
    }

    void Spiderip::set_spider_ipv6_global(std::string spider_ipv6_global)
    {
        this->spider_ipv6_global = spider_ipv6_global;
    }

    std::string Spiderip::get_spider_ipv6_global()
    {
        return spider_ipv6_global;
    }

    void Spiderip::set_spider_ipv6_unique_local(std::string spider_ipv6_unique_local)
    {
        this->spider_ipv6_unique_local = spider_ipv6_unique_local;
    }

    std::string Spiderip::get_spider_ipv6_unique_local()
    {
        return spider_ipv6_unique_local;
    }

    void Spiderip::set_spider_ipv6_link_local(std::string spider_ipv6_link_local)
    {
        this->spider_ipv6_link_local = spider_ipv6_link_local;
    }

    std::string Spiderip::get_spider_ipv6_link_local()
    {
        return spider_ipv6_link_local;
    }

    void Spiderip::set_spider_ipv6_link_local_scope_id(std::string spider_ipv6_link_local_scope_id)
    {
        this->spider_ipv6_link_local_scope_id = spider_ipv6_link_local_scope_id;
    }

    std::string Spiderip::get_spider_ipv6_link_local_scope_id()
    {
        return spider_ipv6_link_local_scope_id;
    }
}

