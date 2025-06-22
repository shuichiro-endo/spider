/*
 * Title:  spider pipemanager.cpp (Linux)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "pipemanager.hpp"
#include "pipe.hpp"


namespace spider
{
    Pipemanager::Pipemanager()
    {

    }

    Pipemanager::~Pipemanager()
    {

    }

    std::shared_ptr<Pipe> Pipemanager::get_pipe(uint32_t pipe_id)
    {
        std::shared_ptr<Pipe> pipe = nullptr;
        std::size_t check_pipe_key_count = 0;

        std::unique_lock<std::mutex> lock(pipes_map_mutex);

        check_pipe_key_count = pipes_map.count(pipe_id);
        if(check_pipe_key_count > 0)
        {
            pipe = pipes_map[pipe_id];
        }

        lock.unlock();

        return pipe;
    }

    bool Pipemanager::check_pipe(uint32_t pipe_id)
    {
        bool ret = false;
        std::size_t check_pipe_key_count = 0;

        std::unique_lock<std::mutex> lock(pipes_map_mutex);

        check_pipe_key_count = pipes_map.count(pipe_id);
        if(check_pipe_key_count != 0)
        {
            ret = true;
        }

        lock.unlock();

        return ret;
    }

    int Pipemanager::insert_pipe(uint32_t pipe_id,
                                 std::shared_ptr<Pipe> pipe)
    {
        int ret = -1;
        std::size_t check_pipe_key_count = 0;

        std::unique_lock<std::mutex> lock(pipes_map_mutex);

        check_pipe_key_count = pipes_map.count(pipe_id);
        if(check_pipe_key_count == 0)
        {
            pipe->set_pipe_id(pipe_id);
            pipes_map.insert({pipe_id, pipe});
            ret = 0;
        }

        lock.unlock();

        return ret;
    }

    bool Pipemanager::erase_pipe(uint32_t pipe_id)
    {
        bool ret = false;
        std::size_t check_pipe_key_count = 0;

        std::unique_lock<std::mutex> lock(pipes_map_mutex);

        check_pipe_key_count = pipes_map.count(pipe_id);
        if(check_pipe_key_count > 0)
        {
            ret = true;
            pipes_map.erase(pipe_id);
        }

        lock.unlock();

        return ret;
    }

    void Pipemanager::show_pipes_map()
    {
        std::printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------- pipe --------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("|pipe id   |mode|message|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|forward proxy type|forward proxy auth|forward proxy ip                              |forward proxy ip scope id|forward proxy port|\n");
        std::printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        std::unique_lock<std::mutex> lock_pipes_map(pipes_map_mutex);
        for(auto iterator = pipes_map.begin(); iterator != pipes_map.end(); ++iterator)
        {
            std::string pipe_ip_scope_id;
            std::string pipe_destination_ip_scope_id;
            std::string forward_proxy_ip_scope_id;

            if(if_nametoindex(iterator->second->get_pipe_ip_scope_id().c_str()) > 0)
            {
                pipe_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_pipe_ip_scope_id().c_str()));
            }else
            {
                pipe_ip_scope_id = "";
            }

            if(if_nametoindex(iterator->second->get_pipe_destination_ip_scope_id().c_str()) > 0)
            {
                pipe_destination_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_pipe_destination_ip_scope_id().c_str()));
            }else
            {
                pipe_destination_ip_scope_id = "";
            }

            if(if_nametoindex(iterator->second->get_forward_proxy_ip_scope_id().c_str()) > 0)
            {
                forward_proxy_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_forward_proxy_ip_scope_id().c_str()));
            }else
            {
                forward_proxy_ip_scope_id = "";
            }


            std::printf("|%10u|%c   |%1s      |%-46s|%-10s (%3s)|           %5s|%-46s|%-10s             (%3s)|                %5s|      %5d|%2s                |%1d                 |%-46s|%-10s          (%3s)|             %5s|\n",
                        iterator->second->get_pipe_id(),
                        iterator->second->get_mode(),
                        iterator->second->get_message_mode().c_str(),
                        iterator->second->get_pipe_ip().c_str(),
                        iterator->second->get_pipe_ip_scope_id().c_str(),
                        pipe_ip_scope_id.c_str(),
                        iterator->second->get_pipe_listen_port().c_str(),
                        iterator->second->get_pipe_destination_ip().c_str(),
                        iterator->second->get_pipe_destination_ip_scope_id().c_str(),
                        pipe_destination_ip_scope_id.c_str(),
                        iterator->second->get_pipe_destination_port().c_str(),
                        iterator->second->get_sock(),
                        iterator->second->get_forward_proxy_type().c_str(),
                        iterator->second->get_forward_proxy_authentication(),
                        iterator->second->get_forward_proxy_ip().c_str(),
                        iterator->second->get_forward_proxy_ip_scope_id().c_str(),
                        forward_proxy_ip_scope_id.c_str(),
                        iterator->second->get_forward_proxy_port().c_str());
        }
        lock_pipes_map.unlock();

        std::printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        std::printf("\n");

        return;
    }

    std::string Pipemanager::show_pipes_map_string()
    {
        std::string result = "";


        result += "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------- pipe --------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

        result += "|pipe id   |mode|message|pipe ip                                       |pipe ip scope id|pipe listen port|pipe destination ip                           |pipe destination ip scope id|pipe destination port|pipe socket|forward proxy type|forward proxy auth|forward proxy ip                              |forward proxy ip scope id|forward proxy port|\n";

        result += "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

        std::unique_lock<std::mutex> lock_pipes_map(pipes_map_mutex);
        for(auto iterator = pipes_map.begin(); iterator != pipes_map.end(); ++iterator)
        {
            std::string pipe_ip_scope_id;
            std::string pipe_destination_ip_scope_id;
            std::string forward_proxy_ip_scope_id;

            if(if_nametoindex(iterator->second->get_pipe_ip_scope_id().c_str()) > 0)
            {
                pipe_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_pipe_ip_scope_id().c_str()));
            }else
            {
                pipe_ip_scope_id = "";
            }

            if(if_nametoindex(iterator->second->get_pipe_destination_ip_scope_id().c_str()) > 0)
            {
                pipe_destination_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_pipe_destination_ip_scope_id().c_str()));
            }else
            {
                pipe_destination_ip_scope_id = "";
            }

            if(if_nametoindex(iterator->second->get_forward_proxy_ip_scope_id().c_str()) > 0)
            {
                forward_proxy_ip_scope_id = std::to_string(if_nametoindex(iterator->second->get_forward_proxy_ip_scope_id().c_str()));
            }else
            {
                forward_proxy_ip_scope_id = "";
            }

            std::ostringstream oss;

            oss << "|" << std::right << std::setw(10) << iterator->second->get_pipe_id()
                << "|" << iterator->second->get_mode() << "   "
                << "|" << std::setw(1) << iterator->second->get_message_mode().c_str() << "      "
                << "|" << std::left << std::setw(46) << iterator->second->get_pipe_ip().c_str()
                << "|" << std::left << std::setw(10) << iterator->second->get_pipe_ip_scope_id().c_str() << " (" << std::right << std::setw(3) << pipe_ip_scope_id.c_str() << ")"
                << "|           " << std::right << std::setw(5) << iterator->second->get_pipe_listen_port().c_str()
                << "|" << std::left << std::setw(46) << iterator->second->get_pipe_destination_ip().c_str()
                << "|" << std::left << std::setw(10) << iterator->second->get_pipe_destination_ip_scope_id().c_str() << "             (" << std::right << std::setw(3) << pipe_destination_ip_scope_id.c_str() << ")"
                << "|                " << std::right << std::setw(5) << iterator->second->get_pipe_destination_port().c_str()
                << "|      " << std::right << std::setw(5) << iterator->second->get_sock()
                << "|" << std::setw(2) << iterator->second->get_forward_proxy_type().c_str() << "                "
                << "|" << std::setw(1) << iterator->second->get_forward_proxy_authentication() << "                 "
                << "|" << std::left << std::setw(46) << iterator->second->get_forward_proxy_ip().c_str()
                << "|" << std::left << std::setw(10) << iterator->second->get_forward_proxy_ip_scope_id().c_str() << "          (" << std::right << std::setw(3) << forward_proxy_ip_scope_id.c_str() << ")"
                << "|             " << std::right << std::setw(5) << iterator->second->get_forward_proxy_port().c_str()
                << "|\n";

            std::string tmp = oss.str();
            result += tmp;
        }
        lock_pipes_map.unlock();

        result += "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n";

        return result;
    }

    void Pipemanager::send_routing_message(std::shared_ptr<Routingmessage> routing_message)
    {
        int ret = 0;

        std::unique_lock<std::mutex> lock_pipes_map(pipes_map_mutex);
        for(auto iterator = pipes_map.begin(); iterator != pipes_map.end(); ++iterator)
        {
            if(iterator->second->get_pipe_id() != 0 && iterator->second->get_mode() != 's')
            {
                iterator->second->push_routing_message(routing_message);
            }
        }

        lock_pipes_map.unlock();

        return;
    }
}

