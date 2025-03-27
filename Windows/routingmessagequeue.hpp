/*
 * Title:  spider routingmessagequeue.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ROUTINGMESSAGEQUEUE_H_
#define ROUTINGMESSAGEQUEUE_H_

#include "spider.hpp"


namespace spider
{
    class Routingmessage;

    class Routingmessagequeue
    {
    private:
        std::queue<std::shared_ptr<Routingmessage>> queue;
        std::counting_semaphore<ROUTING_MESSAGE_QUEUE_CAPACITY> token{0};
        std::counting_semaphore<1> guard{1};
        int32_t count = 0;

    public:
        Routingmessagequeue();
        ~Routingmessagequeue();

        void push(std::shared_ptr<Routingmessage> message);
        int32_t push_timeout(std::shared_ptr<Routingmessage> message,
                             int32_t tv_sec,
                             int32_t tv_usec);

        std::shared_ptr<Routingmessage> pop();
        std::shared_ptr<Routingmessage> pop_timeout(int32_t tv_sec,
                                                    int32_t tv_usec);
    };
}

#endif /* ROUTINGMESSAGEQUEUE_H_ */

