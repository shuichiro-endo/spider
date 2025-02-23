/*
 * Title:  spider spiderip.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SPIDERIP_H_
#define SPIDERIP_H_

#include "spider.hpp"


namespace spider
{
    class Spiderip
    {
    private:
        std::string spider_ipv4;
        std::string spider_ipv6;
        std::string spider_ipv6_scope_id;

        char addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *addr6_string_pointer = addr6_string;

    public:

    private:

    public:
        Spiderip(std::string spider_ipv4,
                 std::string spider_ipv6);
        ~Spiderip();

        void set_spider_ipv4(std::string spider_ipv4);
        std::string get_spider_ipv4();

        void set_spider_ipv6(std::string spider_ipv6);
        std::string get_spider_ipv6();

        void set_spider_ipv6_scope_id(std::string spider_ipv6_scope_id);
        std::string get_spider_ipv6_scope_id();
    };
}

#endif /* SPIDERIP_H_ */

