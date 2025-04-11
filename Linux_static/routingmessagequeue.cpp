/*
 * Title:  spider routingmessagequeue.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "routingmessagequeue.hpp"
#include "routingmessage.hpp"

namespace spider
{
    Routingmessagequeue::Routingmessagequeue()
    {

    }

    Routingmessagequeue::~Routingmessagequeue()
    {

    }

    void Routingmessagequeue::push(std::shared_ptr<Routingmessage> message)
    {
        guard.acquire();
        while(this->count >= ROUTING_MESSAGE_QUEUE_CAPACITY)
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

    int32_t Routingmessagequeue::push_timeout(std::shared_ptr<Routingmessage> message,
                                              int32_t tv_sec,
                                              int32_t tv_usec)
    {
        int32_t ret = 0;
        int64_t t = (int64_t)tv_sec * 1000000 + (int64_t)tv_usec;
        auto duration = std::chrono::microseconds{t};

        if(guard.try_acquire_for(duration))
        {
            while(this->count >= ROUTING_MESSAGE_QUEUE_CAPACITY)
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

    std::shared_ptr<Routingmessage> Routingmessagequeue::pop()
    {
        token.acquire();
        guard.acquire();
        std::shared_ptr<Routingmessage> message = queue.front();
        queue.pop();
        this->count--;
        guard.release();

        return message;
    }

    std::shared_ptr<Routingmessage> Routingmessagequeue::pop_timeout(int32_t tv_sec,
                                                                     int32_t tv_usec)
    {
        std::shared_ptr<Routingmessage> message = nullptr;
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

