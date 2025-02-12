/*
 * Title:  spider routingmanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef ROUTINGMANAGER_H_
#define ROUTINGMANAGER_H_

#include "spider.hpp"


namespace spider
{
    class Route;
    class Routingmessage;
    class Routingmessagequeue;
    class Pipe;
    class Pipemanager;
    class Messagemanager;

    class Routingmanager
    {
    private:
        std::string spider_ip;
        std::shared_ptr<Pipemanager> pipe_manager;
        std::shared_ptr<Messagemanager> message_manager;
        std::map<std::pair<char, std::string>, std::shared_ptr<Route>> routes_map;
        std::mutex routes_map_mutex;
        std::unique_ptr<Routingmessagequeue> routing_messages_queue;

    public:

    private:
        std::shared_ptr<Routingmessage> pop_routing_message();

    public:
        Routingmanager(std::string spider_ip,
                       std::shared_ptr<Pipemanager> pipe_manager,
                       std::shared_ptr<Messagemanager> message_manager);

        ~Routingmanager();

        void set_spider_ip(std::string spider_ip);
        std::string get_spider_ip();

        void init_routing_table();

        void show_routing_table();

        void send_routing_table();

        void update_routing_table();

        void delete_routing_table();

        void push_routing_message(std::shared_ptr<Routingmessage> routing_message);

        std::shared_ptr<Route> get_route(char type,
                                         std::string ip);

        void add_route(std::shared_ptr<Route> route);

        int update_route(std::shared_ptr<Route> route_new);

        void delete_route(char type,
                          std::string ip);

        std::shared_ptr<Pipe> get_destination_pipe(char type,
                                                   std::string ip);

        int gettimeofday(timeval *tv, timezone *tz);
    };
}

#endif /* ROUTINGMANAGER_H_ */

