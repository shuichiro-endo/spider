/*
 * Title:  spider caresmanager.hpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"

namespace spider
{
    struct addrinfo_callback_arg
    {
        struct sockaddr_in *addr;
        struct sockaddr_in6 *addr6;
        int32_t ret;
    };

    class Caresmanager
    {
    private:
        bool init_flag = false;
        ares_channel_t *channel = NULL;
        struct ares_options options;
        int optmask = 0;
        std::mutex channel_mutex;

    public:

    private:
        static void addrinfo_callback(void *arg,
                                      int status,
                                      int timeouts,
                                      struct ares_addrinfo *result);

    public:
        Caresmanager();
        ~Caresmanager();

        void set_init_flag(bool init_flag);
        bool get_init_flag();

        int32_t get_addr_info(const char *domainname,
                              const char *service,
                              int ai_family,
                              struct sockaddr_in *addr,
                              struct sockaddr_in6 *addr6);
    };
}

