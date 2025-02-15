/*
 * Title:  spider clientmanager.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "clientmanager.hpp"
#include "client.hpp"


namespace spider
{
    Clientmanager::Clientmanager()
    {

    }

    Clientmanager::~Clientmanager()
    {

    }

    std::shared_ptr<Client> Clientmanager::get_client(uint32_t connection_id,
                                                      uint32_t client_id)
    {
        std::shared_ptr<Client> client = nullptr;
        std::pair<uint32_t, uint32_t> client_key = std::make_pair(connection_id,
                                                                  client_id);
        std::size_t check_client_key_count = 0;

        std::unique_lock<std::mutex> lock(clients_map_mutex);

        check_client_key_count = clients_map.count(client_key);
        if(check_client_key_count > 0)
        {
            client = clients_map[client_key];
        }

        lock.unlock();

        return client;
    }

    int Clientmanager::insert_client(uint32_t connection_id,
                                     uint32_t client_id,
                                     std::shared_ptr<Client> client)
    {
        int ret = -1;

        std::pair<uint32_t, uint32_t> client_key = std::make_pair(connection_id,
                                                                  client_id);
        std::size_t check_client_key_count = 0;

        std::unique_lock<std::mutex> lock(clients_map_mutex);

        check_client_key_count = clients_map.count(client_key);
        if(check_client_key_count == 0)
        {
            client->set_connection_id(connection_id);
            client->set_client_id(client_id);
            clients_map.insert({client_key, client});
            ret = 0;
        }

        lock.unlock();

        return ret;
    }

    bool Clientmanager::erase_client(uint32_t connection_id,
                                     uint32_t client_id)
    {
        bool ret = false;
        std::pair<uint32_t, uint32_t> client_key = std::make_pair(connection_id,
                                                                  client_id);
        std::size_t check_client_key_count = 0;

        std::unique_lock<std::mutex> lock(clients_map_mutex);
        check_client_key_count = clients_map.count(client_key);
        if(check_client_key_count > 0)
        {
            ret = true;
            clients_map.erase(client_key);
        }

        lock.unlock();

        return ret;
    }

    void Clientmanager::show_clients_map()
    {
        std::printf("---------------------------------------------------------------------------------------------------------------------------------------------- client -----------------------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("|type  |connection id|client id |server id |client ip                                     |client listen port|client port|server destination ip                         |target ip                                     |target port|client socket|tv_sec |tv_usec|forwarder_tv_sec|forwarder_tv_usec|\n");
        std::printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        std::unique_lock<std::mutex> lock(clients_map_mutex);
        for(auto iterator = clients_map.begin(); iterator != clients_map.end(); ++iterator)
        {
            std::printf("|%-6s|   %10u|%10u|%10u|%-46s|             %5s|      %5s|%-46s|%-46s|      %5s|        %5d|%7d|%7d|         %7d|          %7d|\n",
                        iterator->second->get_type().c_str(),
                        iterator->second->get_connection_id(),
                        iterator->second->get_client_id(),
                        iterator->second->get_server_id(),
                        iterator->second->get_client_ip().c_str(),
                        iterator->second->get_client_listen_port().c_str(),
                        iterator->second->get_client_port().c_str(),
                        iterator->second->get_server_destination_ip().c_str(),
                        iterator->second->get_target_ip().c_str(),
                        iterator->second->get_target_port().c_str(),
                        iterator->second->get_sock(),
                        iterator->second->get_tv_sec(),
                        iterator->second->get_tv_usec(),
                        iterator->second->get_forwarder_tv_sec(),
                        iterator->second->get_forwarder_tv_usec());
        }
        lock.unlock();

        std::printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("\n");

        return;
    }
}

