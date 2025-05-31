/*
 * Title:  spider socks5messagequeue.cpp (Linux)
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

    int32_t Socks5messagequeue::get_count()
    {
        return this->count;
    }

    void Socks5messagequeue::push(std::shared_ptr<Socks5message> message)
    {
        guard.acquire();
        while(this->count >= SOCKS5_MESSAGE_QUEUE_CAPACITY)
        {
            guard.release();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            guard.acquire();
        }
        queue.push(message);
        this->count++;
        token.release();
        guard.release();

        return;
    }

    int32_t Socks5messagequeue::push_timeout(std::shared_ptr<Socks5message> message,
                                             int32_t tv_sec,
                                             int32_t tv_usec)
    {
        int32_t ret = 0;
        int64_t t = (int64_t)tv_sec * 1000000 + (int64_t)tv_usec;
        auto duration = std::chrono::microseconds{t};

        if(guard.try_acquire_for(duration))
        {
            while(this->count >= SOCKS5_MESSAGE_QUEUE_CAPACITY)
            {
                guard.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                guard.acquire();
            }
            queue.push(message);
            this->count++;
            token.release();
            guard.release();
        }else
        {
#ifdef _DEBUG
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
        this->count--;
        guard.release();

        return message;
    }

    std::shared_ptr<Socks5message> Socks5messagequeue::pop_timeout(int32_t tv_sec,
                                                                   int32_t tv_usec)
    {
        std::shared_ptr<Socks5message> message = nullptr;
        int64_t t = (int64_t)tv_sec * 1000000 + (int64_t)tv_usec;
        auto duration = std::chrono::microseconds{t};

        if(token.try_acquire_for(duration))
        {
            guard.acquire();
            message = queue.front();
            queue.pop();
            this->count--;
            guard.release();
        }else
        {
#ifdef _DEBUG
            std::printf("[-] pop timeout\n");
#endif
        }

        return message;
    }
}

