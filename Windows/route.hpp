/*
 * Title:  spider route.hpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ROUTE_H_
#define ROUTE_H_

#include "spider.hpp"


namespace spider
{
    class Route
    {
    private:
        char type;
        std::string ip;
        uint8_t metric;
        uint32_t pipe_id;
        struct timeval time;

    public:

    private:

    public:
        Route(char type,
              std::string ip,
              uint8_t metric,
              uint32_t pipe_id);

        ~Route();

        void set_type(char type);
        char get_type();

        void set_ip(std::string ip);
        std::string get_ip();

        void set_metric(uint8_t metric);
        uint8_t get_metric();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        void set_time(struct timeval time);
        struct timeval get_time();

        int gettimeofday(timeval *tv, timezone *tz);
    };
}

#endif /* ROUTE_H_ */

