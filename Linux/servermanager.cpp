/*
 * Title:  spider servermanager.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "servermanager.hpp"
#include "server.hpp"


namespace spider
{
    Servermanager::Servermanager()
    {

    }

    Servermanager::~Servermanager()
    {

    }

    std::shared_ptr<Server> Servermanager::get_server(uint32_t connection_id,
                                                      uint32_t server_id)
    {
        std::shared_ptr<Server> server = nullptr;
        std::pair<uint32_t, uint32_t> server_key = std::make_pair(connection_id,
                                                                  server_id);
        std::size_t check_server_key_count = 0;

        std::unique_lock<std::mutex> lock(servers_map_mutex);

        check_server_key_count = servers_map.count(server_key);
        if(check_server_key_count > 0)
        {
            server = servers_map[server_key];
        }

        lock.unlock();

        return server;
    }

    int Servermanager::insert_server(uint32_t connection_id,
                                     uint32_t server_id,
                                     std::shared_ptr<Server> server)
    {
        int ret = -1;

        std::pair<uint32_t, uint32_t> server_key = std::make_pair(connection_id,
                                                                  server_id);
        std::size_t check_server_key_count = 0;

        std::unique_lock<std::mutex> lock(servers_map_mutex);

        check_server_key_count = servers_map.count(server_key);
        if(check_server_key_count == 0)
        {
            server->set_connection_id(connection_id);
            server->set_server_id(server_id);
            servers_map.insert({server_key, server});
            ret = 0;
        }

        lock.unlock();

        return ret;
    }

    bool Servermanager::erase_server(uint32_t connection_id,
                                     uint32_t server_id)
    {
        bool ret = false;
        std::pair<uint32_t, uint32_t> server_key = std::make_pair(connection_id,
                                                                  server_id);

        std::size_t check_server_key_count = 0;

        std::unique_lock<std::mutex> lock(servers_map_mutex);

        check_server_key_count = servers_map.count(server_key);
        if(check_server_key_count > 0)
        {
            ret = true;
            servers_map.erase(server_key);
        }

        lock.unlock();

        return ret;
    }

    void Servermanager::show_servers_map()
    {
        std::printf("--------------------------------------------------------------------------------------------------------------------------------- server ----------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("|connection id|client id |server id |server ip                                     |server port|client destination ip                         |server socket|target ip                                     |target port|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|\n");
        std::printf("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        std::unique_lock<std::mutex> lock_servers_map(servers_map_mutex);
        for(auto iterator = servers_map.begin(); iterator != servers_map.end(); ++iterator)
        {
            std::printf("|   %10u|%10u|%10u|%-46s|      %5s|%-46s|        %5d|%-46s|%7d|%7d|         %7d|          %7d|\n",
                        iterator->second->get_connection_id(),
                        iterator->second->get_client_id(),
                        iterator->second->get_server_id(),
                        iterator->second->get_server_ip().c_str(),
                        iterator->second->get_server_port().c_str(),
                        iterator->second->get_client_destination_ip().c_str(),
                        iterator->second->get_sock(),
                        iterator->second->get_target_ip().c_str(),
                        iterator->second->get_target_port().c_str(),
                        iterator->second->get_tv_sec(),
                        iterator->second->get_tv_usec(),
                        iterator->second->get_forwarder_tv_sec(),
                        iterator->second->get_forwarder_tv_usec());
        }
        lock_servers_map.unlock();

        std::printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("\n");

        return;
    }
}

