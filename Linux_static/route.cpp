/*
 * Title:  spider route.cpp (Linux static)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "route.hpp"
#include "pipe.hpp"


namespace spider
{
    Route::Route(char mode,
                 std::string ip,
                 uint8_t metric,
                 uint32_t pipe_id)
    {
        this->mode = mode;
        this->ip = ip;
        this->metric = metric;
        this->pipe_id = pipe_id;

        if(gettimeofday(&this->time, NULL) == -1)
        {
#ifdef _DEBUG
            std::printf("[-] gettimeofday error\n");
#endif
            this->time.tv_sec = 0;
            this->time.tv_usec = 0;
        }
    }

    Route::~Route()
    {

    }

    void Route::set_mode(char mode)
    {
        this->mode = mode;
    }

    char Route::get_mode()
    {
        return mode;
    }

    void Route::set_ip(std::string ip)
    {
        this->ip = ip;
    }

    std::string Route::get_ip()
    {
        return ip;
    }

    void Route::set_metric(uint8_t metric)
    {
        this->metric = metric;
    }

    uint8_t Route::get_metric()
    {
        return metric;
    }

    void Route::set_pipe_id(uint32_t pipe_id)
    {
        this->pipe_id = pipe_id;
    }

    uint32_t Route::get_pipe_id()
    {
        return pipe_id;
    }

    void Route::set_time(struct timeval time)
    {
        this->time = time;
    }

    struct timeval Route::get_time()
    {
        return time;
    }
}

