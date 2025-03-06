/*
 * Title:  spider clientmanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#ifndef CLIENTMANAGER_H_
#define CLIENTMANAGER_H_

#include "spider.hpp"


namespace spider
{
    class Client;

    class Clientmanager
    {
    private:
        std::map<std::pair<uint32_t, uint32_t>, std::shared_ptr<Client>> clients_map;

        std::mutex clients_map_mutex;

    public:

    private:

    public:
        Clientmanager();

        ~Clientmanager();

        std::shared_ptr<Client> get_client(uint32_t connection_id,
                                           uint32_t client_id);

        int insert_client(uint32_t connection_id,
                          uint32_t client_id,
                          std::shared_ptr<Client> client);

        bool erase_client(uint32_t connection_id,
                          uint32_t client_id);

        void show_clients_map();

        std::string show_clients_map_string();
    };
}

#endif /* CLIENTMANAGER_H_ */

