/*
 * Title:  spider pipemanager.cpp (Linux)
 * Author: Shuichiro Endo
 */

#ifndef PIPEMANAGER_H_
#define PIPEMANAGER_H_

#include <memory>
#include <mutex>
#include <map>

#include "spider.hpp"


namespace spider
{
    class Pipe;
    class Routingmessage;

    class Pipemanager
    {
    private:
        std::map<uint32_t, std::shared_ptr<Pipe>> pipes_map;

        std::mutex pipes_map_mutex;

    public:

    private:

    public:
        Pipemanager();

        ~Pipemanager();

        std::shared_ptr<Pipe> get_pipe(uint32_t pipe_id);

        bool check_pipe(uint32_t pipe_id);

        int insert_pipe(uint32_t pipe_id,
                        std::shared_ptr<Pipe> pipe);

        bool erase_pipe(uint32_t pipe_id);

        void show_pipes_map();

        void send_routing_message(std::shared_ptr<Routingmessage> routing_message);
    };
}

#endif /* PIPEMANAGER_H_ */

