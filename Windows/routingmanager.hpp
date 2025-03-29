/*
 * Title:  spider routingmanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ROUTINGMANAGER_H_
#define ROUTINGMANAGER_H_

#include "spider.hpp"


namespace spider
{
    class Spiderip;
    class Route;
    class Routingmessage;
    class Routingmessagequeue;
    class Pipe;
    class Pipemanager;
    class Messagemanager;

    class Routingmanager
    {
    private:
        std::shared_ptr<Spiderip> spider_ip;
        std::shared_ptr<Pipemanager> pipe_manager;
        std::shared_ptr<Messagemanager> message_manager;
        std::map<std::string, std::shared_ptr<Route>> routes_map;
        std::mutex routes_map_mutex;
        std::unique_ptr<Routingmessagequeue> routing_messages_queue;

    public:

    private:
        std::shared_ptr<Routingmessage> pop_routing_message();

    public:
        Routingmanager(std::shared_ptr<Spiderip> spider_ip,
                       std::shared_ptr<Pipemanager> pipe_manager,
                       std::shared_ptr<Messagemanager> message_manager);

        ~Routingmanager();

        void set_spider_ip(std::shared_ptr<Spiderip> spider_ip);
        std::shared_ptr<Spiderip> get_spider_ip();

        void init_routing_table();

        void show_routing_table();

        void send_routing_table();

        std::string show_routing_table_string();

        void update_routing_table();

        void delete_routing_table();

        void push_routing_message(std::shared_ptr<Routingmessage> routing_message);

        std::shared_ptr<Route> get_route(std::string ip);

        void add_route(std::shared_ptr<Route> route);

        int update_route(std::shared_ptr<Route> route_new);

        void delete_route(std::string ip);

        std::shared_ptr<Pipe> get_destination_pipe(std::string ip);

        int gettimeofday(timeval *tv, timezone *tz);
    };
}

#endif /* ROUTINGMANAGER_H_ */

