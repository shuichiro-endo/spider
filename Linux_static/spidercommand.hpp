/*
 * Title:  spider spidercommand.hpp (Linux static)
 * Author: Shuichiro Endo
 */

#ifndef SPIDERCOMMAND_H_
#define SPIDERCOMMAND_H_

#include "spider.hpp"


namespace spider
{
    class Spiderip;
    class Encryption;
    class Client;
    class Server;
    class Pipe;
    class Clientmanager;
    class Servermanager;
    class Pipemanager;
    class Routingmanager;
    class Messagemanager;
    class Socks5message;
    class Caresmanager;

    class Spidercommand
    {
    private:
        std::shared_ptr<Spiderip> spider_ip;
        std::shared_ptr<Encryption> encryption;
        std::shared_ptr<Clientmanager> client_manager;
        std::shared_ptr<Servermanager> server_manager;
        std::shared_ptr<Pipemanager> pipe_manager;
        std::shared_ptr<Routingmanager> routing_manager;
        std::shared_ptr<Messagemanager> message_manager;
        std::shared_ptr<Caresmanager> cares_manager;

    public:

    private:
        uint32_t generate_random_id();

        void routing_manager_send_routing_table();
        void routing_manager_update_routing_table();
        void routing_manager_delete_routing_table();

        void message_manager_transfer_routing_message();
        void message_manager_transfer_socks5_message();

        void client_worker(std::shared_ptr<Client> client);
        int listen_client(std::string client_listen_ip,
                          std::string client_listen_ip_scope_id,
                          std::string client_listen_port,
                          std::string destination_spider_ip,
                          int32_t tv_sec,
                          int32_t tv_usec,
                          int32_t forwarder_tv_sec,
                          int32_t forwarder_tv_usec);

        void server_worker(std::shared_ptr<Server> server,
                           std::shared_ptr<Socks5message> socks5_message);

        void pipe_recv_message(std::shared_ptr<Pipe> pipe);
        void pipe_send_routing_message(std::shared_ptr<Pipe> pipe);
        void pipe_send_socks5_message(std::shared_ptr<Pipe> pipe);
        void pipe_worker(std::shared_ptr<Pipe> pipe);

        int listen_pipe(char mode,
                        std::string pipe_listen_ip,
                        std::string pipe_listen_ip_scope_id,
                        std::string pipe_listen_port);

        void show_node_information_worker(std::string source_spider_ip,
                                          std::string source_spider_ip_scope_id,
                                          std::string destination_spider_ip);

        void client_udp_workder(std::string client_listen_ip,
                                std::string client_listen_ip_scope_id,
                                std::string client_listen_port,
                                std::string destination_spider_ip,
                                std::string target_ip,
                                std::string target_port,
                                int32_t tv_sec,
                                int32_t tv_usec,
                                int32_t forwarder_tv_sec,
                                int32_t forwarder_tv_usec);

        void client_shell_worker(std::shared_ptr<Client> client);

        int listen_client_shell(std::string client_listen_ip,
                                std::string client_listen_ip_scope_id,
                                std::string client_listen_port,
                                std::string destination_spider_ip,
                                int32_t tv_sec,
                                int32_t tv_usec,
                                int32_t forwarder_tv_sec,
                                int32_t forwarder_tv_usec);

        void add_node_to_destination_spider_worker(std::string config,
                                                   std::string source_spider_ip,
                                                   std::string source_spider_ip_scope_id,
                                                   std::string destination_spider_ip);

        std::string get_line_value(std::string line,
                                   std::string name);

        std::string get_line(char *data,
                             std::size_t data_size,
                             char **line_start,
                             char **line_end);

    public:
        Spidercommand(std::shared_ptr<Spiderip> spider_ip,
                      std::shared_ptr<Encryption> encryption,
                      std::shared_ptr<Clientmanager> client_manager,
                      std::shared_ptr<Servermanager> server_manager,
                      std::shared_ptr<Pipemanager> pipe_manager,
                      std::shared_ptr<Routingmanager> routing_manager,
                      std::shared_ptr<Messagemanager> message_manager,
                      std::shared_ptr<Caresmanager> cares_manager);
        ~Spidercommand();

        void routing_manager_worker();

        void message_manager_worker();

        void add_node_spider_client();

        int connect_pipe(char mode,
                         std::string pipe_ip,
                         std::string pipe_ip_scope_id,
                         std::string pipe_destination_ip,
                         std::string pipe_destination_port);

        void add_node_spider_pipe();

        void show_node_information();

        void show_routing_table();

        void edit_routing_table();

        void add_node_spider_client_udp();

        void add_node_spider_client_shell();

        void add_node_spider_client_to_destination_spider();

        void add_node_spider_pipe_to_destination_spider();

        int32_t read_config(std::vector<char> config);

        int32_t read_config_file(std::string config_file);

    };
}

#endif /* SPIDERCOMMAND_H_ */

