/*
 * Title:  spider routingmanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "routingmanager.hpp"
#include "route.hpp"
#include "message.hpp"
#include "routingmessagequeue.hpp"
#include "routingmessage.hpp"
#include "pipe.hpp"
#include "pipemanager.hpp"


namespace spider
{
    Routingmanager::Routingmanager(std::string spider_ip,
                                   std::shared_ptr<Pipemanager> pipe_manager,
                                   std::shared_ptr<Messagemanager> message_manager)
    {
        this->spider_ip = spider_ip;
        this->pipe_manager = pipe_manager;
        this->message_manager = message_manager;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();

        this->init_routing_table();
    }

    Routingmanager::~Routingmanager()
    {

    }

    void Routingmanager::set_spider_ip(std::string spider_ip)
    {
        this->spider_ip;
    }

    std::string Routingmanager::get_spider_ip()
    {
        return spider_ip;
    }

    void Routingmanager::init_routing_table()
    {
        // client (self)
        std::shared_ptr<Route> client_route = std::make_shared<Route>('-',
                                                                      'c',
                                                                      spider_ip,
                                                                      0,
                                                                      0);
        this->add_route(client_route);

        // server (self)
        std::shared_ptr<Route> server_route = std::make_shared<Route>('-',
                                                                      's',
                                                                      spider_ip,
                                                                      0,
                                                                      0);
        this->add_route(server_route);
    }

    void Routingmanager::show_routing_table()
    {
        std::printf("------------------------------------------- routing table -------------------------------------------\n");
        std::printf("|mode|type|ip address                                    |metric|pipe id   |time                    |\n");
        std::printf("-----------------------------------------------------------------------------------------------------\n");

        std::unique_lock<std::mutex> lock(routes_map_mutex);

        for(auto iterator = routes_map.begin(); iterator != routes_map.end(); ++iterator)
        {
            struct timeval t = iterator->second->get_time();
            const std::time_t time = (time_t)t.tv_sec;

            std::printf("|%c   |%c   |%-46s|   %3d|%10u|%.24s|\n",
                        iterator->second->get_mode(),
                        iterator->second->get_type(),
                        iterator->second->get_ip().c_str(),
                        iterator->second->get_metric(),
                        iterator->second->get_pipe_id(),
                        ctime(&time));
        }

        lock.unlock();

        std::printf("-----------------------------------------------------------------------------------------------------\n");

        return;
    }

    void Routingmanager::send_routing_table()
    {
        std::shared_ptr<Routingmessage> routing_message = std::make_unique<Routingmessage>();

        struct route_data *route_data;
        uint16_t data_size = 0;
        uint16_t route_data_size = sizeof(struct route_data);
        char *p = routing_message->get_data();

        std::unique_lock<std::mutex> lock(routes_map_mutex);

        for(auto iterator = routes_map.begin(); iterator != routes_map.end(); ++iterator)
        {
            route_data = (struct route_data *)p;
            route_data->type = iterator->second->get_type();
            std::memcpy(&route_data->ip, iterator->second->get_ip().c_str(), iterator->second->get_ip().size());
            route_data->metric = iterator->second->get_metric();

            data_size += route_data_size;
            if(data_size + route_data_size > ROUTING_MESSAGE_DATA_SIZE)
            {
                break;
            }
            p += route_data_size;
        }

        lock.unlock();

        routing_message->set_pipe_id(0);
        routing_message->set_data_size(data_size);

#ifdef DEBUGPRINT
//       routing_message->print_bytes();
#endif

        pipe_manager->send_routing_message(routing_message);

        return;
    }

    void Routingmanager::update_routing_table()
    {
        int ret = 0;
        char message_type;
        std::shared_ptr<Routingmessage> routing_message;
        std::shared_ptr<Route> route_new;
        uint32_t pipe_id;
        char mode;
        char type;
        std::string ip;
        uint8_t metric;
        int data_size;
        char *data;
        struct route_data *route_data;
        int route_data_size = sizeof(struct route_data);


        routing_message = pop_routing_message();
        if(routing_message != nullptr)
        {
            message_type = routing_message->get_message_type();
            if(message_type == 'r')    // routing message
            {
                pipe_id = routing_message->get_pipe_id();

                data_size = routing_message->get_data_size();
                data = routing_message->get_data();

                for(int i = 0; i + route_data_size <= data_size; data += route_data_size, i += route_data_size)
                {
                    route_data = (struct route_data *)data;
                    mode = 'a';
                    type = route_data->type;
                    ip = route_data->ip;
                    if(ip == spider_ip)
                    {
                        continue;
                    }

                    if(route_data->metric < UINT8_MAX)
                    {
                        metric = route_data->metric + 1;    // +1
                    }else
                    {
                        metric = UINT8_MAX;
                    }

                    route_new = std::make_shared<Route>(mode,
                                                        type,
                                                        ip,
                                                        metric,
                                                        pipe_id);

                    ret = update_route(route_new);
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] unknown message type: %c\n",
                            message_type);
#endif
            }
        }

        return;
    }

    void Routingmanager::delete_routing_table()
    {
        int ret = 0;
        struct timeval now;
        long d;
        int count = 0;

        std::unique_lock<std::mutex> lock(routes_map_mutex);

        std::pair<char, std::string> keys[KEYS_MAP_SIZE];

        ret = gettimeofday(&now,
                           NULL);
        if(ret == -1)
        {
#ifdef DEBUGPRINT
            std::printf("[-] delete_routing_table gettimeofday error\n");
#endif
            lock.unlock();
            return;
        }

        for(auto iterator = routes_map.begin(); iterator != routes_map.end(); ++iterator)
        {
            if(iterator->second->get_ip() != spider_ip || iterator->second->get_metric() != 0)
            {
                struct timeval t = iterator->second->get_time();
                d = now.tv_sec - t.tv_sec;

                if(d >= DELETE_ROUTE_TIME)
                {
                    if(count < KEYS_MAP_SIZE)
                    {
                        std::pair<char, std::string> route_key = std::make_pair(iterator->second->get_type(),
                                                                                iterator->second->get_ip().c_str());
                        keys[count] = route_key;
                        count++;
                    }
                }
            }
        }

        for(int i = 0; i < count; i++)
        {
            routes_map.erase(keys[i]);  // delete route
        }

        lock.unlock();

        return;
    }

    void Routingmanager::push_routing_message(std::shared_ptr<Routingmessage> routing_message)
    {
        routing_messages_queue->push(routing_message);

        return;
    }

    std::shared_ptr<Routingmessage> Routingmanager::pop_routing_message()
    {
        std::shared_ptr<Routingmessage> routing_message = routing_messages_queue->pop();

        return routing_message;
    }

    std::shared_ptr<Route> Routingmanager::get_route(char type,
                                                     std::string ip)
    {
        std::shared_ptr<Route> route = nullptr;
        std::pair<char, std::string> route_key = std::make_pair(type,
                                                                ip.c_str());
        std::size_t check_route_key_count = 0;

        std::unique_lock<std::mutex> lock(routes_map_mutex);

        check_route_key_count = routes_map.count(route_key);
        if(check_route_key_count != 0)
        {
            route = routes_map[route_key];
        }

        lock.unlock();

        return route;
    }

    void Routingmanager::add_route(std::shared_ptr<Route> route)
    {
        std::pair<char, std::string> route_key = std::make_pair(route->get_type(),
                                                                route->get_ip().c_str());

        std::unique_lock<std::mutex> lock(routes_map_mutex);
        routes_map.insert({route_key, route});
        lock.unlock();

        return;
    }


    int Routingmanager::update_route(std::shared_ptr<Route> route_new)
    {
        int ret = -1;
        std::shared_ptr<Route> route = nullptr;
        std::pair<char, std::string> route_new_key = std::make_pair(route_new->get_type(),
                                                                    route_new->get_ip().c_str());
        std::size_t check_route_new_key_count = 0;

        std::unique_lock<std::mutex> lock(routes_map_mutex);

        check_route_new_key_count = routes_map.count(route_new_key);
        if(check_route_new_key_count != 0)
        {
            route = routes_map[route_new_key];

            if(route_new->get_metric() <= route->get_metric())
            {
                routes_map[route_new_key] = route_new;
                ret = 0;
            }
        }else
        {
            routes_map[route_new_key] = route_new;
            ret = 0;
        }

        return ret;
    }

    void Routingmanager::delete_route(char type,
                                      std::string ip)
    {
        std::pair<char, std::string> route_key = std::make_pair(type,
                                                                ip.c_str());

        std::unique_lock<std::mutex> lock(routes_map_mutex);
        routes_map.erase(route_key);
        lock.unlock();

        return;
    }

    std::shared_ptr<Pipe> Routingmanager::get_destination_pipe(char type,
                                                               std::string ip)
    {
        std::shared_ptr<Route> route = nullptr;
        uint32_t pipe_id = 0;
        std::shared_ptr<Pipe> pipe = nullptr;

        route = get_route(type,
                          ip);
        if(route != nullptr)
        {
            pipe_id = route->get_pipe_id();
            pipe = pipe_manager->get_pipe(pipe_id);
        }

        return pipe;
    }

    /*
     * Reference:
     * https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows
     */
    int Routingmanager::gettimeofday(timeval *tv, timezone *tz)
    {
        if(tv){
            FILETIME filetime;
            ULARGE_INTEGER x;
            ULONGLONG usec;
            static const ULONGLONG epoch_offset_us = 11644473600000000ULL;

#if _WIN32_WINNT >= WIN32_WINNT_WIN8
            GetSystemTimePreciseAsFileTime(&filetime);
#else
            GetSystemTimeAsFileTime(&filetime);
#endif

            x.LowPart = filetime.dwLowDateTime;
            x.HighPart = filetime.dwHighDateTime;
            usec = x.QuadPart / 10 - epoch_offset_us;
            tv->tv_sec = (long)(usec / 1000000ULL);
            tv->tv_usec = (long)(usec % 1000000ULL);
        }else{
            return -1;
        }

        if(tz){
            TIME_ZONE_INFORMATION timezone;
            GetTimeZoneInformation(&timezone);
            tz->tz_minuteswest = timezone.Bias;
            tz->tz_dsttime = 0;
        }

        return 0;
    }
}

