/*
 * Title:  spider caresmanager.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "caresmanager.hpp"

namespace spider
{
    Caresmanager::Caresmanager()
    {
        ares_library_init(ARES_LIB_INIT_ALL);

        if(ares_threadsafety())
        {
            memset(&options,
                   0,
                   sizeof(options));
            optmask |= ARES_OPT_EVENT_THREAD;
            options.evsys = ARES_EVSYS_DEFAULT;

            if(ares_init_options(&channel, &options, optmask) == ARES_SUCCESS)
            {
                init_flag = true;
            }else
            {
                std::printf("[-] c-ares inisialization issue\n");
                init_flag = false;
            }
        }else
        {
            std::printf("[-] c-ares not compiled with thread support\n");
            init_flag = false;
        }
    }

    Caresmanager::~Caresmanager()
    {
        std::unique_lock<std::mutex> lock(channel_mutex);
        ares_destroy(channel);
        lock.unlock();

        ares_library_cleanup();
    }

    void Caresmanager::set_init_flag(bool init_flag)
    {
        this->init_flag = init_flag;
    }

    bool Caresmanager::get_init_flag()
    {
        return init_flag;
    }

    void Caresmanager::addrinfo_callback(void *arg,
                                         int status,
                                         int timeouts,
                                         struct ares_addrinfo *result)
    {
        struct addrinfo_callback_arg *addrinfo_callback_arg = (struct addrinfo_callback_arg *)arg;
        addrinfo_callback_arg->ret = -1;

        char addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *addr6_string_pointer = addr6_string;

#ifdef _DEBUG
        std::printf("[+] addrinfo_callback result: %s, timeouts: %d\n",
                    ares_strerror(status),
                    timeouts);
#endif

        if(result)
        {
            struct ares_addrinfo_node *node;

            for(node = result->nodes; node != NULL; node = node->ai_next)
            {
                if(node->ai_family == AF_INET
                   && addrinfo_callback_arg->addr != NULL)
                {
                    const struct sockaddr_in *in_addr = (const struct sockaddr_in *)((void *)node->ai_addr);

                    addrinfo_callback_arg->addr->sin_family = AF_INET;
                    std::memcpy(&addrinfo_callback_arg->addr->sin_addr,
                                &in_addr->sin_addr,
                                sizeof(struct in_addr));
                    std::memcpy(&addrinfo_callback_arg->addr->sin_port,
                                &in_addr->sin_port,
                                2);

#ifdef _DEBUG
                    std::printf("[+] addrinfo_callback ipv4: %s, port: %d\n",
                                inet_ntoa(addrinfo_callback_arg->addr->sin_addr),
                                ntohs(addrinfo_callback_arg->addr->sin_port));
#endif

                    addrinfo_callback_arg->ret = 0;

                    break;
                }else if(node->ai_family == AF_INET6
                         && addrinfo_callback_arg->addr6 != NULL)
                {
                    const struct sockaddr_in6 *in_addr6 = (const struct sockaddr_in6 *)((void *)node->ai_addr);

                    addrinfo_callback_arg->addr6->sin6_family = AF_INET6;
                    memcpy(&addrinfo_callback_arg->addr6->sin6_addr,
                           &in_addr6->sin6_addr,
                           sizeof(struct in6_addr));
                    memcpy(&addrinfo_callback_arg->addr6->sin6_port,
                           &in_addr6->sin6_port,
                           2);
                    addrinfo_callback_arg->addr6->sin6_scope_id = in_addr6->sin6_scope_id;

#ifdef _DEBUG
                    inet_ntop(AF_INET6,
                              &addrinfo_callback_arg->addr6->sin6_addr,
                              addr6_string_pointer,
                              INET6_ADDRSTRLEN);

                    std::printf("[+] addrinfo_callback ipv6: %s, port: %d, scorp_id: %d\n",
                                addr6_string_pointer,
                                ntohs(addrinfo_callback_arg->addr6->sin6_port),
                                addrinfo_callback_arg->addr6->sin6_scope_id);
#endif

                    addrinfo_callback_arg->ret = 0;

                    break;
                }else
                {
                    continue;
                }
            }
        }

        ares_freeaddrinfo(result);
    }

    int32_t Caresmanager::get_addr_info(const char *domainname,
                                        const char *service,
                                        int ai_family,
                                        struct sockaddr_in *addr,
                                        struct sockaddr_in6 *addr6)
    {
        struct ares_addrinfo_hints hints;
        struct addrinfo_callback_arg addrinfo_callback_arg;
        addrinfo_callback_arg.addr = addr;
        addrinfo_callback_arg.addr6 = addr6;
        addrinfo_callback_arg.ret = -1;

        std::unique_lock<std::mutex> lock(channel_mutex);

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = ai_family;
        hints.ai_flags  = ARES_AI_CANONNAME;

        ares_getaddrinfo(channel,
                         domainname,
                         service,
                         &hints,
                         addrinfo_callback,
                         &addrinfo_callback_arg);

        ares_queue_wait_empty(channel,
                              -1);

        lock.unlock();

        return addrinfo_callback_arg.ret;
    }
}

