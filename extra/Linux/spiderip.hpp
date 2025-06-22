/*
 * Title:  spider spiderip.hpp (Linux)
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
        bool init_flag = false;
        std::string spider_ipv4;
        std::string spider_ipv6_global;
        std::string spider_ipv6_unique_local;
        std::string spider_ipv6_link_local;
        std::string spider_ipv6_link_local_scope_id;

        char addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *addr6_string_pointer = addr6_string;

    public:

    private:

    public:
        Spiderip(std::string spider_ipv4,
                 std::string spider_ipv6_global,
                 std::string spider_ipv6_unique_local,
                 std::string spider_ipv6_link_local);
        ~Spiderip();

        void set_init_flag(bool init_flag);
        bool get_init_flag();

        void set_spider_ipv4(std::string spider_ipv4);
        std::string get_spider_ipv4();

        void set_spider_ipv6_global(std::string spider_ipv6_global);
        std::string get_spider_ipv6_global();

        void set_spider_ipv6_unique_local(std::string spider_ipv6_unique_local);
        std::string get_spider_ipv6_unique_local();

        void set_spider_ipv6_link_local(std::string spider_ipv6_link_local);
        std::string get_spider_ipv6_link_local();

        void set_spider_ipv6_link_local_scope_id(std::string spider_ipv6_link_local_scope_id);
        std::string get_spider_ipv6_link_local_scope_id();
    };
}

#endif /* SPIDERIP_H_ */

