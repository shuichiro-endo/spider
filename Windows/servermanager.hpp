/*
 * Title:  spider servermanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef SERVERMANAGER_H_
#define SERVERMANAGER_H_

#include "spider.hpp"


namespace spider
{
    class Server;

    class Servermanager
    {
    private:
        std::map<std::pair<uint32_t, uint32_t>, std::shared_ptr<Server>> servers_map;

        std::mutex servers_map_mutex;

    public:

    private:

    public:
        Servermanager();

        ~Servermanager();

        std::shared_ptr<Server> get_server(uint32_t connection_id,
                                           uint32_t server_id);

        int insert_server(uint32_t connection_id,
                          uint32_t server_id,
                          std::shared_ptr<Server> server);

        bool erase_server(uint32_t connection_id,
                          uint32_t server_id);

        void show_servers_map();

        std::string show_servers_map_string();
    };
}

#endif /* SERVERMANAGER_H_ */

