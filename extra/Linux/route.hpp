/*
 * Title:  spider route.hpp (Linux)
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
        char mode;  // auto:a self:s
        std::string ip;
        uint8_t metric;
        uint32_t pipe_id;
        struct timeval time;

    public:

    private:

    public:
        Route(char mode,
              std::string ip,
              uint8_t metric,
              uint32_t pipe_id);

        ~Route();

        void set_mode(char mode);
        char get_mode();

        void set_ip(std::string ip);
        std::string get_ip();

        void set_metric(uint8_t metric);
        uint8_t get_metric();

        void set_pipe_id(uint32_t pipe_id);
        uint32_t get_pipe_id();

        void set_time(struct timeval time);
        struct timeval get_time();
    };
}

#endif /* ROUTE_H_ */

