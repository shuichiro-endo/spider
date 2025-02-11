/*
 * Title:  spider socks5messagequeue.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "socks5messagequeue.hpp"
#include "socks5message.hpp"

namespace spider
{
    Socks5messagequeue::Socks5messagequeue()
    {

    }

    Socks5messagequeue::~Socks5messagequeue()
    {

    }

    void Socks5messagequeue::push(std::shared_ptr<Socks5message> message)
    {
        guard.acquire();
        queue.push(message);
        guard.release();

        token.release();
    }

    int32_t Socks5messagequeue::push_timeout(std::shared_ptr<Socks5message> message,
                                             int32_t tv_sec,
                                             int32_t tv_usec)
    {
        int32_t ret = 0;
        int32_t t = tv_sec * 1000000 + tv_usec;
        auto duration = std::chrono::microseconds{t};

        if(guard.try_acquire_for(duration))
        {
            queue.push(message);
            guard.release();
            token.release();
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] push timeout\n");
#endif
            ret = -1;
        }

        return ret;
    }

    std::shared_ptr<Socks5message> Socks5messagequeue::pop()
    {
        token.acquire();

        guard.acquire();
        std::shared_ptr<Socks5message> message = queue.front();
        queue.pop();
        guard.release();

        return message;
    }

    std::shared_ptr<Socks5message> Socks5messagequeue::pop_timeout(int32_t tv_sec,
                                                                   int32_t tv_usec)
    {
        std::shared_ptr<Socks5message> message = nullptr;
        int32_t t = tv_sec * 1000000 + tv_usec;
        auto duration = std::chrono::microseconds{t};

        if(token.try_acquire_for(duration))
        {
            guard.acquire();
            message = queue.front();
            queue.pop();
            guard.release();
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] pop timeout\n");
#endif
        }

        return message;
    }
}

