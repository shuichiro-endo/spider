/*
 * Title:  spider socks5messagequeue.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SOCKS5MESSAGEQUEUE_H_
#define SOCKS5MESSAGEQUEUE_H_

#include "spider.hpp"


namespace spider
{
    class Socks5message;

    class Socks5messagequeue
    {
    private:
        std::queue<std::shared_ptr<Socks5message>> queue;
        std::counting_semaphore<SOCKS5_MESSAGE_QUEUE_CAPACITY> token{0};
        std::counting_semaphore<1> guard{1};
        int32_t count = 0;

    public:
        Socks5messagequeue();
        ~Socks5messagequeue();

        void push(std::shared_ptr<Socks5message> message);
        int32_t push_timeout(std::shared_ptr<Socks5message> message,
                            int32_t tv_sec,
                            int32_t tv_usec);

        std::shared_ptr<Socks5message> pop();
        std::shared_ptr<Socks5message> pop_timeout(int32_t tv_sec,
                                                   int32_t tv_usec);
    };
}

#endif /* SOCKS5MESSAGEQUEUE_H_ */

