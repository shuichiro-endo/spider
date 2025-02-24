/*
 * Title:  spider spider.cpp (Linux)
 * Author: Shuichiro Endo
 * Ver:    0.8
 */

#include "spider.hpp"
#include "spiderip.hpp"
#include "socks5.hpp"
#include "route.hpp"
#include "message.hpp"
#include "socks5message.hpp"
#include "routingmessage.hpp"
#include "routingmessagequeue.hpp"
#include "socks5messagequeue.hpp"
#include "routingmanager.hpp"
#include "node.hpp"
#include "pipe.hpp"
#include "pipemanager.hpp"
#include "client.hpp"
#include "clientmanager.hpp"
#include "server.hpp"
#include "servermanager.hpp"
#include "messagemanager.hpp"
#include "encryption.hpp"
#include "xor.hpp"
#include "aes.hpp"


namespace spider
{

    static uint32_t generate_random_id();

    static void routing_manager_send_routing_table(std::shared_ptr<Routingmanager> routing_manager);

    static void routing_manager_update_routing_table(std::shared_ptr<Routingmanager> routing_manager);

    static void routing_manager_delete_routing_table(std::shared_ptr<Routingmanager> routing_manager);

    static void routing_manager_worker(std::shared_ptr<Routingmanager> routing_manager);

    static void message_manager_transfer_routing_message(std::shared_ptr<Messagemanager> message_manager);

    static void message_manager_transfer_socks5_message(std::shared_ptr<Spiderip> spider_ip,
                                                        std::shared_ptr<Encryption> encryption,
                                                        std::shared_ptr<Servermanager> server_manager,
                                                        std::shared_ptr<Messagemanager> message_manager);

    static void message_manager_worker(std::shared_ptr<Spiderip> spider_ip,
                                       std::shared_ptr<Encryption> encryption,
                                       std::shared_ptr<Servermanager> server_manager,
                                       std::shared_ptr<Messagemanager> message_manager);

    static void client_worker(std::shared_ptr<Clientmanager> client_manager,
                             std::shared_ptr<Client> client);

    static int listen_client(std::shared_ptr<Clientmanager> client_manager,
                             std::shared_ptr<Messagemanager> message_manager,
                             std::string client_listen_ip,
                             std::string client_listen_ip_scope_id,
                             std::string client_listen_port,
                             std::string destination_spider_ip,
                             int32_t tv_sec,
                             int32_t tv_usec,
                             int32_t forwarder_tv_sec,
                             int32_t forwarder_tv_usec,
                             std::shared_ptr<Encryption> encryption);

    static void add_node_spider_client(std::shared_ptr<Spiderip> spider_ip,
                                       std::shared_ptr<Encryption> encryption,
                                       std::shared_ptr<Clientmanager> client_manager,
                                       std::shared_ptr<Messagemanager> message_manager);

    static void server_worker(std::shared_ptr<Servermanager> server_manager,
                              std::shared_ptr<Server> server,
                              std::shared_ptr<Socks5message> socks5_message);

    static void pipe_recv_message(std::shared_ptr<Pipemanager> pipe_manager,
                                  std::shared_ptr<Pipe> pipe);

    static void pipe_send_routing_message(std::shared_ptr<Pipemanager> pipe_manager,
                                          std::shared_ptr<Pipe> pipe);

    static void pipe_send_socks5_message(std::shared_ptr<Pipemanager> pipe_manager,
                                         std::shared_ptr<Pipe> pipe);

    static void pipe_worker(std::shared_ptr<Pipemanager> pipe_manager,
                           std::shared_ptr<Pipe> pipe);

    static int connect_pipe(std::shared_ptr<Pipemanager> pipe_manager,
                            std::shared_ptr<Messagemanager> message_manager,
                            char mode,
                            std::string pipe_ip,
                            std::string pipe_ip_scope_id,
                            std::string pipe_destination_ip,
                            std::string pipe_destination_port);

    static int listen_pipe(std::shared_ptr<Pipemanager> pipe_manager,
                           std::shared_ptr<Messagemanager> message_manager,
                           char mode,
                           std::string pipe_listen_ip,
                           std::string pipe_listen_ip_scope_id,
                           std::string pipe_listen_port);

    static void add_node_spider_pipe(std::shared_ptr<Spiderip> spider_ip,
                                     std::shared_ptr<Pipemanager> pipe_manager,
                                     std::shared_ptr<Messagemanager> message_manager);

    static void show_node_information(std::shared_ptr<Clientmanager> client_manager,
                                      std::shared_ptr<Servermanager> server_manager,
                                      std::shared_ptr<Pipemanager> pipe_manager);

    static void show_routing_table(std::shared_ptr<Routingmanager> routing_manager);

    static void edit_routing_table(std::shared_ptr<Spiderip> spider_ip,
                                   std::shared_ptr<Pipemanager> pipe_manager,
                                   std::shared_ptr<Routingmanager> routing_manager);

    static void client_udp_workder(std::shared_ptr<Clientmanager> client_manager,
                                   std::shared_ptr<Messagemanager> message_manager,
                                   std::string client_listen_ip,
                                   std::string client_listen_ip_scope_id,
                                   std::string client_listen_port,
                                   std::string destination_spider_ip,
                                   std::string target_ip,
                                   std::string target_port,
                                   int32_t tv_sec,
                                   int32_t tv_usec,
                                   int32_t forwarder_tv_sec,
                                   int32_t forwarder_tv_usec,
                                   std::shared_ptr<Encryption> encryption);

    static void add_node_spider_client_udp(std::shared_ptr<Spiderip> spider_ip,
                                           std::shared_ptr<Encryption> encryption,
                                           std::shared_ptr<Clientmanager> client_manager,
                                           std::shared_ptr<Messagemanager> message_manager);

    static void usage(char *filename);


    static uint32_t generate_random_id()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<uint32_t> distrib(1, UINT32_MAX);

        return distrib(mt);
    }

    static void routing_manager_send_routing_table(std::shared_ptr<Routingmanager> routing_manager)
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // 1s

            routing_manager->send_routing_table();
        }

        return;
    }

    static void routing_manager_update_routing_table(std::shared_ptr<Routingmanager> routing_manager)
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            routing_manager->update_routing_table();
        }

        return;
    }

    static void routing_manager_delete_routing_table(std::shared_ptr<Routingmanager> routing_manager)
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // 1s

            routing_manager->delete_routing_table();
        }

        return;
    }

    static void routing_manager_worker(std::shared_ptr<Routingmanager> routing_manager)
    {
        std::thread routing_manager_send_routing_table_thread(routing_manager_send_routing_table,
                                                              routing_manager);
        routing_manager_send_routing_table_thread.detach();

        std::thread routing_manager_update_routing_table_thread(routing_manager_update_routing_table,
                                                                routing_manager);
        routing_manager_update_routing_table_thread.detach();

        std::thread routing_manager_delete_routing_table_thread(routing_manager_delete_routing_table,
                                                                routing_manager);
        routing_manager_delete_routing_table_thread.detach();

        return;
    }

    static void message_manager_transfer_routing_message(std::shared_ptr<Messagemanager> message_manager)
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            message_manager->transfer_routing_message();
        }

        return;
    }

    static void message_manager_transfer_socks5_message(std::shared_ptr<Spiderip> spider_ip,
                                                        std::shared_ptr<Encryption> encryption,
                                                        std::shared_ptr<Servermanager> server_manager,
                                                        std::shared_ptr<Messagemanager> message_manager)
    {
        int ret = 0;
        std::string spider_ip_server;
        std::shared_ptr<Socks5message> socks5_message = nullptr;
        std::shared_ptr<Server> server;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        uint32_t server_id = 0;

        while(1)
        {
            socks5_message = message_manager->transfer_socks5_message();
            if(socks5_message != nullptr)  // generate server
            {
                if(socks5_message->get_destination_ip().find(":") == std::string::npos) // ipv4
                {
                    spider_ip_server = spider_ip->get_spider_ipv4();
                }else   // ipv6
                {
                    spider_ip_server = spider_ip->get_spider_ipv6();
                }

                connection_id = socks5_message->get_connection_id();
                client_id = socks5_message->get_client_id();
                server = std::make_shared<Server>(connection_id,
                                                  client_id,
                                                  0,
                                                  spider_ip_server,
                                                  "0",
                                                  socks5_message->get_source_ip(),
                                                  -1,
                                                  socks5_message->get_tv_sec(),
                                                  socks5_message->get_tv_usec(),
                                                  socks5_message->get_forwarder_tv_sec(),
                                                  socks5_message->get_forwarder_tv_usec(),
                                                  encryption,
                                                  message_manager);

                do
                {
                    server_id = generate_random_id();
                    ret = server_manager->insert_server(connection_id,
                                                        server_id,
                                                        server);
                }while(ret != 0);

#ifdef _DEBUG
                std::printf("[+] generate server : %u\n", server_id);
#endif
                server->set_server_id(server_id);

                std::thread thread(server_worker,
                                   server_manager,
                                   server,
                                   socks5_message);
                thread.detach();
            }
        }

        return;
    }

    static void message_manager_worker(std::shared_ptr<Spiderip> spider_ip,
                                       std::shared_ptr<Encryption> encryption,
                                       std::shared_ptr<Servermanager> server_manager,
                                       std::shared_ptr<Messagemanager> message_manager)
    {
        std::thread message_manager_transfer_routing_message_thread(message_manager_transfer_routing_message,
                                                                    message_manager);
        message_manager_transfer_routing_message_thread.detach();

        std::thread message_manager_transfer_socks5_message_thread(message_manager_transfer_socks5_message,
                                                                   spider_ip,
                                                                   encryption,
                                                                   server_manager,
                                                                   message_manager);
        message_manager_transfer_socks5_message_thread.detach();

        return;
    }

    static void client_worker(std::shared_ptr<Clientmanager> client_manager,
                              std::shared_ptr<Client> client)
    {
        int32_t ret = 0;

        ret = client->do_socks5_connection();
        if(ret == -1)
        {
            if(client_manager->erase_client(client->get_connection_id(),
                                            client->get_client_id()))
            {
                close(client->get_sock());
            }
        }

        return;
    }

    static int listen_client(std::shared_ptr<Clientmanager> client_manager,
                             std::shared_ptr<Messagemanager> message_manager,
                             std::string client_listen_ip,
                             std::string client_listen_ip_scope_id,
                             std::string client_listen_port,
                             std::string destination_spider_ip,
                             int32_t tv_sec,
                             int32_t tv_usec,
                             int32_t forwarder_tv_sec,
                             int32_t forwarder_tv_usec,
                             std::shared_ptr<Encryption> encryption)
    {
        int ret = 0;
        uint32_t connection_id = 0;
        struct sockaddr_in client_listen_addr, client_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 client_listen_addr6, client_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *client_listen_addr_info;
        int32_t client_listen_sock = -1;
        int32_t client_sock = -1;
        int family = 0;
        int reuse = 1;
        int flags = 0;
        int client_addr_length = sizeof(client_addr);
        int client_addr6_length = sizeof(client_addr6);
        char client_listen_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *client_listen_addr6_string_pointer = client_listen_addr6_string;
        char client_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *client_addr6_string_pointer = client_addr6_string;
        std::shared_ptr<Client> client_listen;
        std::pair<uint32_t, uint32_t> client_listen_key;

        std::memset((char *)&client_listen_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&client_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&client_listen_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&client_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&hints,
                    0,
                    sizeof(struct addrinfo));


        if(client_listen_ip.find(":") == std::string::npos)  // ipv4 address
        {
            hints.ai_family = AF_INET;
            ret = getaddrinfo(client_listen_ip.c_str(),
                              client_listen_port.c_str(),
                              &hints,
                              &client_listen_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }
        }else   // ipv6 address
        {
            hints.ai_family = AF_INET6;
            ret = getaddrinfo(client_listen_ip.c_str(),
                              client_listen_port.c_str(),
                              &hints,
                              &client_listen_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }
        }

        if(client_listen_addr_info->ai_family == AF_INET)
        {
            tmp_ipv4 = (struct sockaddr_in *)client_listen_addr_info->ai_addr;

            family = AF_INET;
            client_listen_addr.sin_family = AF_INET;
            std::memcpy(&client_listen_addr.sin_addr,
                        &tmp_ipv4->sin_addr,
                        sizeof(struct in_addr));
            std::memcpy(&client_listen_addr.sin_port,
                        &tmp_ipv4->sin_port,
                        2);

            freeaddrinfo(client_listen_addr_info);
        }else if(client_listen_addr_info->ai_family == AF_INET6)
        {
            tmp_ipv6 = (struct sockaddr_in6 *)client_listen_addr_info->ai_addr;

            family = AF_INET6;
            client_listen_addr6.sin6_family = AF_INET6;
            std::memcpy(&client_listen_addr6.sin6_addr,
                        &tmp_ipv6->sin6_addr,
                        sizeof(struct in6_addr));
            std::memcpy(&client_listen_addr6.sin6_port,
                        &tmp_ipv6->sin6_port,
                        2);

            if(client_listen_ip_scope_id.size() != 0)
            {
                client_listen_addr6.sin6_scope_id = if_nametoindex(client_listen_ip_scope_id.c_str());
            }

            freeaddrinfo(client_listen_addr_info);
        }else
        {
#ifdef _DEBUG
            std::printf("[-] not implemented\n");
#endif
            freeaddrinfo(client_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            client_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr,
                       sizeof(client_listen_addr));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error\n");
#endif
                close(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
#ifdef _DEBUG
            std::printf("[+] listening port %d on %s\n",
                        ntohs(client_listen_addr.sin_port),
                        inet_ntoa(client_listen_addr.sin_addr));
#endif

            client_listen = std::make_shared<Client>("socks5",
                                                     0,
                                                     0,
                                                     0,
                                                     client_listen_ip,
                                                     "",
                                                     client_listen_port,
                                                     "",
                                                     destination_spider_ip,
                                                     client_listen_sock,
                                                     tv_sec,
                                                     tv_usec,
                                                     forwarder_tv_sec,
                                                     forwarder_tv_usec,
                                                     encryption,
                                                     message_manager);

            do
            {
                connection_id = generate_random_id();
                ret = client_manager->insert_client(connection_id,
                                                    0,
                                                    client_listen);
            }while(ret != 0);

            while(1)
            {
                // accept
                client_sock = accept(client_listen_sock,
                                       (struct sockaddr *)&client_addr,
                                       (socklen_t *)&client_addr_length);
#ifdef _DEBUG
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(client_addr.sin_addr),
                            ntohs(client_addr.sin_port));
#endif

                flags = fcntl(client_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(client_sock,
                      F_SETFL,
                      flags);

                uint32_t client_id = 0;
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                std::string client_port = std::to_string(ntohs(client_addr.sin_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("socks5",
                                                                          connection_id,
                                                                          0,
                                                                          0,
                                                                          client_ip,
                                                                          "",
                                                                          "",
                                                                          client_port,
                                                                          destination_spider_ip,
                                                                          client_sock,
                                                                          tv_sec,
                                                                          tv_usec,
                                                                          forwarder_tv_sec,
                                                                          forwarder_tv_usec,
                                                                          encryption,
                                                                          message_manager);



                do
                {
                    client_id = generate_random_id();
                    ret = client_manager->insert_client(connection_id,
                                                        client_id,
                                                        client);
                }while(ret != 0);

                std::thread thread(client_worker,
                                   client_manager,
                                   client);
                thread.detach();
            }

            client_manager->erase_client(connection_id,
                                         0);
        }else if(family == AF_INET6)
        {
            client_listen_sock = socket(AF_INET6,
                                        SOCK_STREAM,
                                        0);
            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr6,
                       sizeof(client_listen_addr6));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error\n");
#endif
                close(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);

            inet_ntop(AF_INET6,
                      &client_listen_addr6.sin6_addr,
                      client_listen_addr6_string_pointer,
                      INET6_ADDRSTRLEN);

#ifdef _DEBUG
            if(client_listen_addr6.sin6_scope_id > 0)
            {
                std::printf("[+] listening port %d on %s%%%d\n",
                            ntohs(client_listen_addr6.sin6_port),
                            client_listen_addr6_string_pointer,
                            client_listen_addr6.sin6_scope_id);
            }else
            {
                std::printf("[+] listening port %d on %s\n",
                            ntohs(client_listen_addr6.sin6_port),
                            client_listen_addr6_string_pointer);
            }
#endif

            client_listen = std::make_shared<Client>("socks5",
                                                     0,
                                                     0,
                                                     0,
                                                     client_listen_ip,
                                                     client_listen_ip_scope_id,
                                                     client_listen_port,
                                                     "",
                                                     destination_spider_ip,
                                                     client_listen_sock,
                                                     tv_sec,
                                                     tv_usec,
                                                     forwarder_tv_sec,
                                                     forwarder_tv_usec,
                                                     encryption,
                                                     message_manager);

            do
            {
                connection_id = generate_random_id();
                ret = client_manager->insert_client(connection_id,
                                                    0,
                                                    client_listen);
            }while(ret != 0);

            while(1)
            {
                // accept
                client_sock = accept(client_listen_sock,
                                       (struct sockaddr *)&client_addr6,
                                       (socklen_t *)&client_addr6_length);

                inet_ntop(AF_INET6,
                          &client_addr6.sin6_addr,
                          client_addr6_string_pointer,
                          INET6_ADDRSTRLEN);
#ifdef _DEBUG
                if(client_addr6.sin6_scope_id > 0)
                {
                    std::printf("[+] connected from ip:%s%%%d port:%d\n",
                                client_addr6_string_pointer,
                                client_addr6.sin6_scope_id,
                                ntohs(client_addr6.sin6_port));
                }else
                {
                    std::printf("[+] connected from ip:%s port:%d\n",
                                client_addr6_string_pointer,
                                ntohs(client_addr6.sin6_port));
                }
#endif

                flags = fcntl(client_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(client_sock,
                      F_SETFL,
                      flags);

                uint32_t client_id = 0;
                std::string client_ip;
                std::string client_ip_scope_id;
                std::string client_port;
                client_ip = client_addr6_string_pointer;
                client_ip_scope_id = std::to_string(client_addr6.sin6_scope_id);
                client_port = std::to_string(ntohs(client_addr6.sin6_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("socks5",
                                                                          connection_id,
                                                                          0,
                                                                          0,
                                                                          client_ip,
                                                                          client_ip_scope_id,
                                                                          "",
                                                                          client_port,
                                                                          destination_spider_ip,
                                                                          client_sock,
                                                                          tv_sec,
                                                                          tv_usec,
                                                                          forwarder_tv_sec,
                                                                          forwarder_tv_usec,
                                                                          encryption,
                                                                          message_manager);

                do
                {
                    client_id = generate_random_id();
                    ret = client_manager->insert_client(connection_id,
                                                        client_id,
                                                        client);
                }while(ret != 0);

                std::thread thread(client_worker,
                                   client_manager,
                                   client);
                thread.detach();
            }

            client_manager->erase_client(connection_id,
                                         0);
        }

        close(client_listen_sock);
        return 0;
    }

    static void add_node_spider_client(std::shared_ptr<Spiderip> spider_ip,
                                       std::shared_ptr<Encryption> encryption,
                                       std::shared_ptr<Clientmanager> client_manager,
                                       std::shared_ptr<Messagemanager> message_manager)
    {
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string destination_spider_ip;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            std::printf("client listen ip             > ");
            std::cin >> client_listen_ip;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6())
            {
                std::printf("[-] please input spider ipv4 or ipv6\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_scope_id();
            }

            std::printf("client listen port           > ");
            std::cin >> client_listen_port;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("destination spider ip        > ");
            std::cin >> destination_spider_ip;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("recv/send tv_sec  (timeout 0-60 sec)           > ");
            std::cin >> tv_sec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                tv_sec = 3;
            }else if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            std::printf("recv/send tv_usec (timeout 0-1000000 microsec) > ");
            std::cin >> tv_usec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                tv_usec = 0;
            }else if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            std::printf("forwarder tv_sec  (timeout 0-3600 sec)         > ");
            std::cin >> forwarder_tv_sec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                forwarder_tv_sec = 30;
            }else if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            std::printf("forwarder tv_usec (timeout 0-1000000 microsec) > ");
            std::cin >> forwarder_tv_usec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                forwarder_tv_usec = 0;
            }else if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }

            std::printf("\n");
            std::printf("client listen ip          : %s\n", client_listen_ip.c_str());
            if(!client_listen_ip_scope_id.empty())
            {
                std::printf("client listen ip scope id : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
            }
            std::printf("client listen port        : %s\n", client_listen_port.c_str());
            std::printf("destination spider ip     : %s\n", destination_spider_ip.c_str());
            std::printf("recv/send tv_sec          : %7d sec\n", tv_sec);
            std::printf("recv/send tv_usec         : %7d microsec\n", tv_usec);
            std::printf("forwarder_tv_sec          : %7d sec\n", forwarder_tv_sec);
            std::printf("forwarder_tv_usec         : %7d microsec\n", forwarder_tv_usec);
            std::printf("\n");

            std::printf("ok? (yes:y no:n quit:q) > ");
            std::cin >> check;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(check == 'y')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }else if(check == 'n')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(check == 'q'){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }else{
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }

        }

        std::thread thread(listen_client,
                           client_manager,
                           message_manager,
                           client_listen_ip,
                           client_listen_ip_scope_id,
                           client_listen_port,
                           destination_spider_ip,
                           tv_sec,
                           tv_usec,
                           forwarder_tv_sec,
                           forwarder_tv_usec,
                           encryption);
        thread.detach();

        return;
    }

    static void server_worker(std::shared_ptr<Servermanager> server_manager,
                             std::shared_ptr<Server> server,
                             std::shared_ptr<Socks5message> socks5_message)
    {
        int32_t ret = 0;

        ret = server->do_socks5_connection(socks5_message);
        if(ret == -1)
        {
            if(server_manager->erase_server(server->get_connection_id(),
                                            server->get_server_id()))
            {
                close(server->get_sock());
            }
        }

        return;
    }

    static void pipe_recv_message(std::shared_ptr<Pipemanager> pipe_manager,
                                  std::shared_ptr<Pipe> pipe)
    {
        int32_t ret = 0;
        uint32_t pipe_key = pipe->get_pipe_id();

        while(1)
        {
//            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            ret = pipe->recv_message();
            if(ret == -1)
            {
                if(pipe_manager->erase_pipe(pipe_key))
                {
                    close(pipe->get_sock());
                }
                break;
            }
        }
    }

    static void pipe_send_routing_message(std::shared_ptr<Pipemanager> pipe_manager,
                                          std::shared_ptr<Pipe> pipe)
    {
        int32_t ret = 0;
        uint32_t pipe_key = pipe->get_pipe_id();

        while(1)
        {
//            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            ret = pipe->send_routing_message();
            if(ret == -1)
            {
                if(pipe_manager->erase_pipe(pipe_key))
                {
                    close(pipe->get_sock());
                }
                break;
            }
        }
    }

    static void pipe_send_socks5_message(std::shared_ptr<Pipemanager> pipe_manager,
                                         std::shared_ptr<Pipe> pipe)
    {
        int32_t ret = 0;
        uint32_t pipe_key = pipe->get_pipe_id();

        while(1)
        {
//            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            ret = pipe->send_socks5_message();
            if(ret == -1)
            {
                if(pipe_manager->erase_pipe(pipe_key))
                {
                    close(pipe->get_sock());
                }
                break;
            }
        }
    }

    static void pipe_worker(std::shared_ptr<Pipemanager> pipe_manager,
                           std::shared_ptr<Pipe> pipe)
    {

        std::thread pipe_recv_message_thread(pipe_recv_message,
                                             pipe_manager,
                                             pipe);
        pipe_recv_message_thread.detach();

        std::thread pipe_send_routing_message_thread(pipe_send_routing_message,
                                                     pipe_manager,
                                                     pipe);
        pipe_send_routing_message_thread.detach();

        std::thread pipe_send_socks5_message_thread(pipe_send_socks5_message,
                                                    pipe_manager,
                                                    pipe);
        pipe_send_socks5_message_thread.detach();

        return;
    }

    static int connect_pipe(std::shared_ptr<Pipemanager> pipe_manager,
                            std::shared_ptr<Messagemanager> message_manager,
                            char mode,
                            std::string pipe_ip,
                            std::string pipe_ip_scope_id,
                            std::string pipe_destination_ip,
                            std::string pipe_destination_port)
    {
        int ret = 0;
        uint32_t pipe_id = 0;
        struct sockaddr_in pipe_dest_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 pipe_dest_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *pipe_dest_addr_info;
        uint32_t pipe_sock = -1;
        int family = 0;
        int reuse = 1;
        int flags = 0;
        int pipe_dest_addr_length = sizeof(pipe_dest_addr);
        int pipe_dest_addr6_length = sizeof(pipe_dest_addr6);
        char pipe_dest_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *pipe_dest_addr6_string_pointer = pipe_dest_addr6_string;
        std::string pipe_destination_ip_scope_id;
        std::shared_ptr<Pipe> pipe;
        uint32_t pipe_key;

        std::memset((char *)&pipe_dest_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&pipe_dest_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&hints,
                    0,
                    sizeof(struct addrinfo));


        if(pipe_destination_ip.find(":") == std::string::npos)  // ipv4 address
        {
            hints.ai_family = AF_INET;
            ret = getaddrinfo(pipe_destination_ip.c_str(),
                              pipe_destination_port.c_str(),
                              &hints,
                              &pipe_dest_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_destination_ip.c_str());
#endif
                return -1;
            }
        }else   // ipv6 address
        {
            hints.ai_family = AF_INET6;
            ret = getaddrinfo(pipe_destination_ip.c_str(),
                              pipe_destination_port.c_str(),
                              &hints,
                              &pipe_dest_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_destination_ip.c_str());
 #endif
                return -1;
            }
        }

        if(pipe_dest_addr_info->ai_family == AF_INET)
        {
            tmp_ipv4 = (struct sockaddr_in *)pipe_dest_addr_info->ai_addr;

            family = AF_INET;
            pipe_dest_addr.sin_family = AF_INET;
            std::memcpy(&pipe_dest_addr.sin_addr,
                        &tmp_ipv4->sin_addr,
                        sizeof(struct in_addr));
            std::memcpy(&pipe_dest_addr.sin_port,
                        &tmp_ipv4->sin_port,
                        2);

            freeaddrinfo(pipe_dest_addr_info);
        }else if(pipe_dest_addr_info->ai_family == AF_INET6)
        {
            tmp_ipv6 = (struct sockaddr_in6 *)pipe_dest_addr_info->ai_addr;

            family = AF_INET6;
            pipe_dest_addr6.sin6_family = AF_INET6;
            std::memcpy(&pipe_dest_addr6.sin6_addr,
                        &tmp_ipv6->sin6_addr,
                        sizeof(struct in6_addr));
            std::memcpy(&pipe_dest_addr6.sin6_port,
                        &tmp_ipv6->sin6_port,
                        2);

            if(pipe_ip_scope_id.size() > 0)
            {
                pipe_dest_addr6.sin6_scope_id = if_nametoindex(pipe_ip_scope_id.c_str());
            }

            freeaddrinfo(pipe_dest_addr_info);
        }else
        {
#ifdef _DEBUG
            std::printf("[-] not implemented\n");
#endif
            freeaddrinfo(pipe_dest_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            pipe_sock = socket(AF_INET,
                                 SOCK_STREAM,
                                 0);

            flags = fcntl(pipe_sock,
                          F_GETFL,
                          0);
            flags &= ~O_NONBLOCK;
            fcntl(pipe_sock,
                  F_SETFL,
                  flags);

#ifdef _DEBUG
            std::printf("[+] connecting to ip:%s port:%d\n",
                   inet_ntoa(pipe_dest_addr.sin_addr),
                   ntohs(pipe_dest_addr.sin_port));
#endif
            // connect
            ret = connect(pipe_sock,
                          (struct sockaddr *)&pipe_dest_addr,
                          sizeof(pipe_dest_addr));
            if(ret < 0){
#ifdef _DEBUG
                std::printf("[-] connect failed:%d\n", ret);
#endif
                close(pipe_sock);
                return -1;
            }

#ifdef _DEBUG
            std::printf("[+] connected to ip:%s port:%d\n", inet_ntoa(pipe_dest_addr.sin_addr), ntohs(pipe_dest_addr.sin_port));
#endif
        }else if(family == AF_INET6)    // IPv6
        {
            pipe_sock = socket(AF_INET6,
                                 SOCK_STREAM,
                                 0);

            flags = fcntl(pipe_sock,
                          F_GETFL,
                          0);
            flags &= ~O_NONBLOCK;
            fcntl(pipe_sock,
                  F_SETFL,
                  flags);

#ifdef _DEBUG
            inet_ntop(AF_INET6,
                      &pipe_dest_addr6.sin6_addr,
                      pipe_dest_addr6_string_pointer,
                      INET6_ADDRSTRLEN);
            if(pipe_dest_addr6.sin6_scope_id > 0){
                std::printf("[-] connecting to ip:%s%%%d port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            pipe_dest_addr6.sin6_scope_id,
                            ntohs(pipe_dest_addr6.sin6_port));
            }else{
                std::printf("[-] connecting to ip:%s port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            ntohs(pipe_dest_addr6.sin6_port));
            }
#endif
            // connect
            ret = connect(pipe_sock,
                          (struct sockaddr *)&pipe_dest_addr6,
                          sizeof(pipe_dest_addr6));
            if(ret < 0){
#ifdef _DEBUG
                std::printf("[-] connect failed:%d\n", ret);
#endif
                close(pipe_sock);
                return -1;
            }

#ifdef _DEBUG
            inet_ntop(AF_INET6,
                      &pipe_dest_addr6.sin6_addr,
                      pipe_dest_addr6_string_pointer,
                      INET6_ADDRSTRLEN);
            if(pipe_dest_addr6.sin6_scope_id > 0){
                std::printf("[+] connected to ip:%s%%%d port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            pipe_dest_addr6.sin6_scope_id,
                            ntohs(pipe_dest_addr6.sin6_port));
            }else{
                std::printf("[+] connected to ip:%s port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            ntohs(pipe_dest_addr6.sin6_port));
            }
#endif

            pipe_destination_ip_scope_id = std::to_string(pipe_dest_addr6.sin6_scope_id);
        }else
        {
#ifdef _DEBUG
            std::printf("[-] not implemented\n");
#endif
            return -1;
        }

        pipe = std::make_shared<Pipe>(0,
                                      mode,
                                      pipe_ip,
                                      pipe_ip_scope_id,
                                      pipe_destination_ip,
                                      pipe_destination_ip_scope_id,
                                      pipe_destination_port,
                                      pipe_sock,
                                      message_manager);

        do
        {
            pipe_key = generate_random_id();
            ret = pipe_manager->insert_pipe(pipe_key,
                                            pipe);
        }while(ret != 0);

        std::thread thread(pipe_worker,
                           pipe_manager,
                           pipe);
        thread.detach();

        return 0;
    }

    static int listen_pipe(std::shared_ptr<Pipemanager> pipe_manager,
                           std::shared_ptr<Messagemanager> message_manager,
                           char mode,
                           std::string pipe_listen_ip,
                           std::string pipe_listen_ip_scope_id,
                           std::string pipe_listen_port)
    {
        int ret = 0;
        uint32_t pipe_id = 0;
        struct sockaddr_in pipe_listen_addr, pipe_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 pipe_listen_addr6, pipe_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *pipe_listen_addr_info;
        uint32_t pipe_listen_sock = -1;
        uint32_t pipe_sock = -1;
        int family = 0;
        int reuse = 1;
        int flags = 0;
        int pipe_addr_length = sizeof(pipe_addr);
        int pipe_addr6_length = sizeof(pipe_addr6);
        char pipe_listen_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *pipe_listen_addr6_string_pointer = pipe_listen_addr6_string;
        char pipe_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *pipe_addr6_string_pointer = pipe_addr6_string;
        std::shared_ptr<Pipe> pipe_listen;
        uint32_t pipe_listen_key;

        std::memset((char *)&pipe_listen_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&pipe_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&pipe_listen_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&pipe_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&hints,
                    0,
                    sizeof(struct addrinfo));


        if(pipe_listen_ip.find(":") == std::string::npos)  // ipv4 address
        {
            hints.ai_family = AF_INET;
            ret = getaddrinfo(pipe_listen_ip.c_str(),
                              pipe_listen_port.c_str(),
                              &hints,
                              &pipe_listen_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
#endif
                return -1;
            }
        }else   // ipv6 address
        {
            hints.ai_family = AF_INET6;
            ret = getaddrinfo(pipe_listen_ip.c_str(),
                              pipe_listen_port.c_str(),
                              &hints,
                              &pipe_listen_addr_info);
            if(ret != 0)
            {
#ifdef _DEBUG
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
#endif
                return -1;
            }
        }

        if(pipe_listen_addr_info->ai_family == AF_INET)
        {
            tmp_ipv4 = (struct sockaddr_in *)pipe_listen_addr_info->ai_addr;

            family = AF_INET;
            pipe_listen_addr.sin_family = AF_INET;
            std::memcpy(&pipe_listen_addr.sin_addr,
                        &tmp_ipv4->sin_addr,
                        sizeof(struct in_addr));
            std::memcpy(&pipe_listen_addr.sin_port,
                        &tmp_ipv4->sin_port,
                        2);

            freeaddrinfo(pipe_listen_addr_info);
        }else if(pipe_listen_addr_info->ai_family == AF_INET6)
        {
            tmp_ipv6 = (struct sockaddr_in6 *)pipe_listen_addr_info->ai_addr;

            family = AF_INET6;
            pipe_listen_addr6.sin6_family = AF_INET6;
            std::memcpy(&pipe_listen_addr6.sin6_addr,
                        &tmp_ipv6->sin6_addr,
                        sizeof(struct in6_addr));
            std::memcpy(&pipe_listen_addr6.sin6_port,
                        &tmp_ipv6->sin6_port,
                        2);

            if(pipe_listen_ip_scope_id.size() != 0)
            {
                pipe_listen_addr6.sin6_scope_id = if_nametoindex(pipe_listen_ip_scope_id.c_str());
            }

            freeaddrinfo(pipe_listen_addr_info);
        }else
        {
#ifdef _DEBUG
            std::printf("[-] not implemented\n");
#endif
            freeaddrinfo(pipe_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            pipe_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr,
                       sizeof(pipe_listen_addr));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error\n");
#endif
                close(pipe_listen_sock);
                return -1;
            }

            // listen
            listen(pipe_listen_sock,
                   5);
#ifdef _DEBUG
            std::printf("[+] listening port %d on %s\n",
                        ntohs(pipe_listen_addr.sin_port),
                        inet_ntoa(pipe_listen_addr.sin_addr));
#endif

            pipe_listen = std::make_shared<Pipe>(0,
                                                 mode,
                                                 pipe_listen_ip,
                                                 "",
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 message_manager);

            do
            {
                pipe_listen_key = generate_random_id();
                ret = pipe_manager->insert_pipe(pipe_listen_key,
                                                pipe_listen);
            }while(ret != 0);

            while(1)
            {
                // accept
                pipe_sock = accept(pipe_listen_sock,
                                     (struct sockaddr *)&pipe_addr,
                                     (socklen_t *)&pipe_addr_length);

                if(pipe_id != 0 && pipe_manager->check_pipe(pipe_id)){
                    close(pipe_sock);
                    continue;
                }
#ifdef _DEBUG
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(pipe_addr.sin_addr),
                            ntohs(pipe_addr.sin_port));
#endif

                flags = fcntl(pipe_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(pipe_sock,
                      F_SETFL,
                      flags);

                std::string pipe_destination_ip = inet_ntoa(pipe_addr.sin_addr);
                std::string pipe_destination_port = std::to_string(ntohs(pipe_addr.sin_port));
                std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>(0,
                                                                    '-',
                                                                    pipe_listen_ip,
                                                                    "",
                                                                    pipe_destination_ip,
                                                                    "",
                                                                    pipe_destination_port,
                                                                    pipe_sock,
                                                                    message_manager);

                do
                {
                    pipe_id = generate_random_id();
                    ret = pipe_manager->insert_pipe(pipe_id,
                                                    pipe);
                }while(ret != 0);

                std::thread thread(pipe_worker,
                                   pipe_manager,
                                   pipe);
                thread.detach();
            }

            pipe_manager->erase_pipe(pipe_listen_key);
        }else if(family == AF_INET6)
        {
            pipe_listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr6,
                       sizeof(pipe_listen_addr6));
            if(ret == -1)
            {
#ifdef _DEBUG
                std::printf("[-] bind error\n");
#endif
                close(pipe_listen_sock);
                return -1;
            }

            // listen
            listen(pipe_listen_sock,
                   5);

            inet_ntop(AF_INET6,
                      &pipe_listen_addr6.sin6_addr,
                      pipe_listen_addr6_string_pointer,
                      INET6_ADDRSTRLEN);

#ifdef _DEBUG
            if(pipe_listen_addr6.sin6_scope_id > 0)
            {
                std::printf("[+] listening port %d on %s%%%d\n",
                            ntohs(pipe_listen_addr6.sin6_port),
                            pipe_listen_addr6_string_pointer,
                            pipe_listen_addr6.sin6_scope_id);
            }else
            {
                std::printf("[+] listening port %d on %s\n",
                            ntohs(pipe_listen_addr6.sin6_port),
                            pipe_listen_addr6_string_pointer);
            }
#endif

            pipe_listen = std::make_shared<Pipe>(0,
                                                 mode,
                                                 pipe_listen_ip,
                                                 pipe_listen_ip_scope_id,
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 message_manager);

            do
            {
                pipe_listen_key = generate_random_id();
                ret = pipe_manager->insert_pipe(pipe_listen_key,
                                                pipe_listen);
            }while(ret != 0);

            while(1)
            {
                // accept
                pipe_sock = accept(pipe_listen_sock,
                                     (struct sockaddr *)&pipe_addr6,
                                     (socklen_t *)&pipe_addr6_length);

                if(pipe_id != 0
                   && pipe_manager->check_pipe(pipe_id))
                {
                    close(pipe_sock);
                    continue;
                }

                inet_ntop(AF_INET6,
                          &pipe_addr6.sin6_addr,
                          pipe_addr6_string_pointer,
                          INET6_ADDRSTRLEN);
#ifdef _DEBUG
                if(pipe_addr6.sin6_scope_id > 0)
                {
                    std::printf("[+] connected from ip:%s%%%d port:%d\n",
                                pipe_addr6_string_pointer,
                                pipe_addr6.sin6_scope_id,
                                ntohs(pipe_addr6.sin6_port));
                }else
                {
                    std::printf("[+] connected from ip:%s port:%d\n",
                                pipe_addr6_string_pointer,
                                ntohs(pipe_addr6.sin6_port));
                }
#endif

                flags = fcntl(pipe_sock,
                              F_GETFL,
                              0);
                flags &= ~O_NONBLOCK;
                fcntl(pipe_sock,
                      F_SETFL,
                      flags);

                std::string pipe_destination_ip;
                std::string pipe_destination_ip_scope_id;
                std::string pipe_destination_port;
                pipe_destination_ip = pipe_addr6_string_pointer;
                pipe_destination_ip_scope_id = std::to_string(pipe_addr6.sin6_scope_id);
                pipe_destination_port = std::to_string(ntohs(pipe_addr6.sin6_port));
                std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>(0,
                                                                    '-',
                                                                    pipe_listen_ip,
                                                                    pipe_listen_ip_scope_id,
                                                                    pipe_destination_ip,
                                                                    pipe_destination_ip_scope_id,
                                                                    pipe_destination_port,
                                                                    pipe_sock,
                                                                    message_manager);

                do
                {
                    pipe_id = generate_random_id();
                    ret = pipe_manager->insert_pipe(pipe_id,
                                                    pipe);
                }while(ret != 0);

                std::thread thread(pipe_worker,
                                   pipe_manager,
                                   pipe);
                thread.detach();
            }

            pipe_manager->erase_pipe(pipe_listen_key);
        }

        close(pipe_listen_sock);

        return 0;
    }

    static void add_node_spider_pipe(std::shared_ptr<Spiderip> spider_ip,
                                     std::shared_ptr<Pipemanager> pipe_manager,
                                     std::shared_ptr<Messagemanager> message_manager)
    {
        char mode;  // client:c server:s
        std::string pipe_ip;
        std::string pipe_ip_scope_id;
        std::string pipe_destination_ip;
        std::string pipe_destination_port;
        std::string pipe_listen_port;
        char check = 'n';


        while(1)
        {
            std::printf("mode (client:c server:s)    > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 'c')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("pipe ip                     > ");
                std::cin >> pipe_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(pipe_ip != spider_ip->get_spider_ipv4()
                   && pipe_ip != spider_ip->get_spider_ipv6())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(pipe_ip == spider_ip->get_spider_ipv6())
                {
                    pipe_ip_scope_id = spider_ip->get_spider_ipv6_scope_id();
                }

                std::printf("pipe destination ip         > ");
                std::cin >> pipe_destination_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("pipe destination port       > ");
                std::cin >> pipe_destination_port;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("\n");
                std::printf("mode                    : %c\n", mode);
                std::printf("pipe ip                 : %s\n", pipe_ip.c_str());
                if(!pipe_ip_scope_id.empty())
                {
                    std::printf("pipe ip scope id        : %s (%d)\n", pipe_ip_scope_id.c_str(), if_nametoindex(pipe_ip_scope_id.c_str()));
                }
                std::printf("pipe destination ip     : %s\n", pipe_destination_ip.c_str());
                std::printf("pipe destination port   : %s\n", pipe_destination_port.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q) > ");
                std::cin >> check;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'y')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::thread thread(connect_pipe,
                                       pipe_manager,
                                       message_manager,
                                       mode,
                                       pipe_ip,
                                       pipe_ip_scope_id,
                                       pipe_destination_ip,
                                       pipe_destination_port);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q'){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else{
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }

                break;
            }else if(mode == 's')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("pipe listen ip              > ");
                std::cin >> pipe_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(pipe_ip != spider_ip->get_spider_ipv4()
                   && pipe_ip != spider_ip->get_spider_ipv6())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(pipe_ip == spider_ip->get_spider_ipv6())
                {
                    pipe_ip_scope_id = spider_ip->get_spider_ipv6_scope_id();
                }

                std::printf("pipe listen port            > ");
                std::cin >> pipe_listen_port;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("\n");
                std::printf("mode                    : %c\n", mode);
                std::printf("pipe listen ip          : %s\n", pipe_ip.c_str());
                if(!pipe_ip_scope_id.empty())
                {
                    std::printf("pipe listen ip scope id : %s (%d)\n", pipe_ip_scope_id.c_str(), if_nametoindex(pipe_ip_scope_id.c_str()));
                }
                std::printf("pipe listen port        : %s\n", pipe_listen_port.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q) > ");
                std::cin >> check;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'y')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::thread thread(listen_pipe,
                                       pipe_manager,
                                       message_manager,
                                       mode,
                                       pipe_ip,
                                       pipe_ip_scope_id,
                                       pipe_listen_port);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q'){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else{
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }

                break;
            }else{
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }

    static void show_node_information(std::shared_ptr<Clientmanager> client_manager,
                                      std::shared_ptr<Servermanager> server_manager,
                                      std::shared_ptr<Pipemanager> pipe_manager)
    {
        // client
        client_manager->show_clients_map();

        // server
        server_manager->show_servers_map();

        // pipe
        pipe_manager->show_pipes_map();

        return;
    }


    static void show_routing_table(std::shared_ptr<Routingmanager> routing_manager)
    {
        routing_manager->show_routing_table();

        return;
    }

    void edit_routing_table(std::shared_ptr<Spiderip> spider_ip,
                            std::shared_ptr<Pipemanager> pipe_manager,
                            std::shared_ptr<Routingmanager> routing_manager)
    {
        char edit_mode;  // add:a delete:d
        std::string ip;
        uint32_t metric;
        uint32_t pipe_id;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("edit mode (add:a delete:d quit:q)  > ");
            std::cin >> edit_mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(edit_mode == 'a')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("ip address                         > ");
                std::cin >> ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(ip == spider_ip->get_spider_ipv4()
                   || ip == spider_ip->get_spider_ipv6())
                {
                    std::printf("[-] cannot input spider ipv4 and ipv6\n");
                    continue;
                }

                std::printf("metric (0 < metric <= %d)         > ", UINT8_MAX);
                std::cin >> metric;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(metric == 0 || metric > UINT8_MAX)
                {
                    std::printf("[-] input error\n");
                    continue;
                }

                pipe_manager->show_pipes_map();

                std::printf("pipe id                             > ");
                std::cin >> pipe_id;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("\n");
                std::printf("edit mode       : %c\n", edit_mode);
                std::printf("ip address      : %s\n", ip.c_str());
                std::printf("metric          : %u\n", metric);
                std::printf("pipe id         : %10u\n", pipe_id);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)             > ");
                std::cin >> check;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'y')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::shared_ptr<Route> route = std::make_shared<Route>('s',
                                                                           ip,
                                                                           (uint8_t)metric,
                                                                           pipe_id);
                    routing_manager->add_route(route);

                    continue;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q'){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else{
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }

                break;
            }else if(edit_mode == 'd')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("ip address                         > ");
                std::cin >> ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(ip == spider_ip->get_spider_ipv4()
                   || ip == spider_ip->get_spider_ipv6())
                {
                    std::printf("[-] cannot input spider ipv4 and ipv6\n");
                    continue;
                }

                std::printf("\n");
                std::printf("edit mode       : %c\n", edit_mode);
                std::printf("ip address      : %s\n", ip.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)            > ");
                std::cin >> check;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'y')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    routing_manager->delete_route(ip);

                    continue;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q'){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else{
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }

                break;
            }else if(check == 'q'){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }else{
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }

            break;
        }

        return;
    }

    static void client_udp_workder(std::shared_ptr<Clientmanager> client_manager,
                                   std::shared_ptr<Messagemanager> message_manager,
                                   std::string client_listen_ip,
                                   std::string client_listen_ip_scope_id,
                                   std::string client_listen_port,
                                   std::string destination_spider_ip,
                                   std::string target_ip,
                                   std::string target_port,
                                   int32_t tv_sec,
                                   int32_t tv_usec,
                                   int32_t forwarder_tv_sec,
                                   int32_t forwarder_tv_usec,
                                   std::shared_ptr<Encryption> encryption)
    {
        int32_t ret = 0;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        int32_t client_udp_sock = -1;
        std::shared_ptr<Client> client_udp;
        std::pair<uint32_t, uint32_t> client_udp_key;


        client_udp = std::make_shared<Client>("udp",
                                              0,
                                              0,
                                              0,
                                              client_listen_ip,
                                              client_listen_ip_scope_id,
                                              client_listen_port,
                                              "",
                                              destination_spider_ip,
                                              target_ip,
                                              target_port,
                                              client_udp_sock,
                                              tv_sec,
                                              tv_usec,
                                              forwarder_tv_sec,
                                              forwarder_tv_usec,
                                              encryption,
                                              message_manager);

        do
        {
            connection_id = generate_random_id();
            client_id = generate_random_id();
            ret = client_manager->insert_client(connection_id,
                                                client_id,
                                                client_udp);
        }while(ret != 0);

        ret = client_udp->do_socks5_connection_udp();
        if(ret == -1)
        {
            client_manager->erase_client(client_udp->get_connection_id(),
                                         client_udp->get_client_id());
        }

        return;
    }

    static void add_node_spider_client_udp(std::shared_ptr<Spiderip> spider_ip,
                                           std::shared_ptr<Encryption> encryption,
                                           std::shared_ptr<Clientmanager> client_manager,
                                           std::shared_ptr<Messagemanager> message_manager)
    {
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string destination_spider_ip;
        std::string target_ip;      // ipv4, domainname, ipv6
        std::string target_port;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            std::printf("client listen ip                               > ");
            std::cin >> client_listen_ip;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6())
            {
                std::printf("[-] please input spider ipv4 or ipv6\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_scope_id();
            }

            std::printf("client listen port                             > ");
            std::cin >> client_listen_port;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("destination spider ip                          > ");
            std::cin >> destination_spider_ip;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("target ip (ipv4<16, domainname<256, ipv6<46)   > ");
            std::cin >> target_ip;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
/*
            if((destination_spider_ip.find(":") == std::string::npos && target_ip.find(":") != std::string::npos)       // destination_spider_ip:ipv4  target_ip:ipv6
               || (destination_spider_ip.find(":") != std::string::npos && target_ip.find(":") == std::string::npos))   // destination_spider_ip:ipv6  target_ip:ipv4
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
*/
            if(target_ip.size() >= 256)
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("target port                                    > ");
            std::cin >> target_port;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::printf("recv/send tv_sec  (timeout 0-60 sec)           > ");
            std::cin >> tv_sec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                tv_sec = 3;
            }else if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            std::printf("recv/send tv_usec (timeout 0-1000000 microsec) > ");
            std::cin >> tv_usec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                tv_usec = 0;
            }else if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            std::printf("forwarder tv_sec  (timeout 0-3600 sec)         > ");
            std::cin >> forwarder_tv_sec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                forwarder_tv_sec = 30;
            }else if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            std::printf("forwarder tv_usec (timeout 0-1000000 microsec) > ");
            std::cin >> forwarder_tv_usec;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                forwarder_tv_usec = 0;
            }else if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }

            std::printf("\n");
            std::printf("client listen ip          : %s\n", client_listen_ip.c_str());
            if(!client_listen_ip_scope_id.empty())
            {
                std::printf("client listen ip scope id : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
            }
            std::printf("client listen port        : %s\n", client_listen_port.c_str());
            std::printf("destination spider ip     : %s\n", destination_spider_ip.c_str());
            std::printf("target ip                 : %s\n", target_ip.c_str());
            std::printf("target port               : %s\n", target_port.c_str());
            std::printf("recv/send tv_sec          : %7d sec\n", tv_sec);
            std::printf("recv/send tv_usec         : %7d microsec\n", tv_usec);
            std::printf("forwarder_tv_sec          : %7d sec\n", forwarder_tv_sec);
            std::printf("forwarder_tv_usec         : %7d microsec\n", forwarder_tv_usec);
            std::printf("FORWARDER_UDP_TIMEOUT     : %7d sec\n", FORWARDER_UDP_TIMEOUT);
            std::printf("\n");

            std::printf("ok? (yes:y no:n quit:q) > ");
            std::cin >> check;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(check == 'y')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }else if(check == 'n')
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(check == 'q'){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }else{
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }

        }

        std::thread thread(client_udp_workder,
                           client_manager,
                           message_manager,
                           client_listen_ip,
                           client_listen_ip_scope_id,
                           client_listen_port,
                           destination_spider_ip,
                           target_ip,
                           target_port,
                           tv_sec,
                           tv_usec,
                           forwarder_tv_sec,
                           forwarder_tv_usec,
                           encryption);
        thread.detach();

        return;
    }

    static void print_title()
    {
        std::printf("\n");
        std::printf("                 -.                                 _//                   \n");
        std::printf("           .=  :*=--::                       _/     _//                   \n");
        std::printf("      ....  =:::::-==++-::.   _//// _/ _//          _//   _//    _/ _///  \n");
        std::printf("      .:..:--::::..--:..     _//    _/  _// _// _// _// _/   _//  _//     \n");
        std::printf("    .:::==-::.:-===.           _/// _/   _//_//_/   _//_///// _// _//     \n");
        std::printf("  :-.  ::+=-:--=:=*-             _//_// _// _//_/   _//_/         _//     \n");
        std::printf("         -+: ++-  -*-        _// _//_//     _// _// _//  _////   _///     \n");
        std::printf("        :*-  :*-   .:.              _//                                   \n");
        std::printf("        =-    -:                  Linux Ver: 0.8  Author: Shuichiro Endo  \n");
        std::printf("\n");
    }

    static void usage(char *filename)
    {
        std::printf("\n");
        std::printf("usage   : %s [-4 spider_ipv4] [-6 spider_ipv6]\n", filename);
        std::printf("        : [-r routing_mode(auto:a self:s)]\n");
        std::printf("        : [-e x(xor encryption)] [-k key(hexstring)]\n");
        std::printf("        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]\n");
        std::printf("example : %s -4 192.168.0.10\n", filename);
        std::printf("        : %s -6 fe80::xxxx:xxxx:xxxx:xxxx%%eth0\n", filename);
        std::printf("        : %s -4 192.168.0.10 -6 fe80::xxxx:xxxx:xxxx:xxxx%%eth0\n", filename);
        std::printf("        : %s -4 192.168.0.10 -r s\n", filename);
        std::printf("        : %s -4 192.168.0.10 -e x -k deadbeef\n", filename);
        std::printf("        : %s -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a\n", filename);
        std::printf("\n");
    }
}

int main(int argc,
         char **argv)
{
    int opt;
    const char *optstring = "h:4:6:r:e:k:v:";
    opterr = 0;
    std::string spider_ipv4;
    std::string spider_ipv6;
    std::string routing_mode = "a";
    std::string encryption_type;
    std::string key;
    std::string iv;
    bool xor_flag = false;
    bool aes_flag = false;
    std::string xor_key_hex_string;
    std::string aes_key_hex_string;
    std::string aes_iv_hex_string;


    spider::print_title();

    while((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch(opt)
        {
            case 'h':
                spider::usage(argv[0]);
                exit(-1);

            case '4':
                spider_ipv4 = optarg;
                break;

            case '6':
                spider_ipv6 = optarg;
                break;

            case 'r':
                routing_mode = optarg;
                break;

            case 'e':
                encryption_type = optarg;
                break;

            case 'k':
                key = optarg;
                break;

            case 'v':
                iv = optarg;
                break;

            default:
                spider::usage(argv[0]);
                exit(-1);
        }
    }

    if(spider_ipv4.empty()
       && spider_ipv6.empty())
    {
        std::printf("[-] spider ipv4 and ipv6 are empty\n");
        spider::usage(argv[0]);
        exit(-1);
    }

    std::shared_ptr<spider::Spiderip> spider_ip = std::make_shared<spider::Spiderip>(spider_ipv4,
                                                                                     spider_ipv6);

    std::shared_ptr<spider::Encryption> encryption;
    std::shared_ptr<spider::Xor> encryption_xor;
    std::shared_ptr<spider::Aes> encryption_aes;

    if(encryption_type == "x")
    {
        xor_flag = true;
        xor_key_hex_string = key;
        encryption_xor = std::make_shared<spider::Xor>(xor_flag,
                                                       xor_key_hex_string);
        encryption = encryption_xor;
    }else if(encryption_type == "a")
    {
        aes_flag = true;
        aes_key_hex_string = key;
        aes_iv_hex_string = iv;
        encryption_aes = std::make_shared<spider::Aes>(aes_flag,
                                                       aes_key_hex_string,
                                                       aes_iv_hex_string);

        if(encryption_aes->get_flag() == false)
        {
            exit(-1);
        }

        encryption = encryption_aes;
    }else
    {
        encryption = nullptr;
    }

    std::shared_ptr<spider::Clientmanager> client_manager = std::make_shared<spider::Clientmanager>();
    std::shared_ptr<spider::Servermanager> server_manager = std::make_shared<spider::Servermanager>();
    std::shared_ptr<spider::Pipemanager> pipe_manager = std::make_shared<spider::Pipemanager>();
    std::shared_ptr<spider::Routingmanager> routing_manager;
    std::shared_ptr<spider::Messagemanager> message_manager;

    routing_manager = std::make_shared<spider::Routingmanager>(spider_ip,
                                                               pipe_manager,
                                                               message_manager);

    message_manager = std::make_shared<spider::Messagemanager>(spider_ip,
                                                               client_manager,
                                                               server_manager,
                                                               pipe_manager,
                                                               routing_manager);

    if(routing_mode != "s") // auto
    {
        std::thread routing_manager_thread(spider::routing_manager_worker,
                                           routing_manager);
        routing_manager_thread.detach();
    }

    std::thread message_manager_thread(spider::message_manager_worker,
                                       spider_ip,
                                       encryption,
                                       server_manager,
                                       message_manager);
    message_manager_thread.detach();


    int spider_command = 0;

    while(1)
    {
        std::printf("\n");
        std::printf("----------     spider     ----------\n");
        if(!spider_ip->get_spider_ipv4().empty())
        {
            std::printf(" spider ipv4          : %s\n", spider_ip->get_spider_ipv4().c_str());
        }
        if(!spider_ip->get_spider_ipv6().empty())
        {
            std::printf(" spider ipv6          : %s\n", spider_ip->get_spider_ipv6().c_str());
            if(!spider_ip->get_spider_ipv6_scope_id().empty())
            {
                std::printf(" spider ipv6 scope id : %s (%d)\n", spider_ip->get_spider_ipv6_scope_id().c_str(), if_nametoindex(spider_ip->get_spider_ipv6_scope_id().c_str()));
            }
        }
        std::printf(" routing mode         : %s\n", (routing_mode == "s" ? "self" : "auto"));
        std::printf(" xor encryption       : %s\n", (xor_flag ? "on" : "off"));
        std::printf(" xor key hex string   : %s\n", xor_key_hex_string.c_str());
        std::printf(" aes encryption       : %s\n", (aes_flag ? "on" : "off"));
        std::printf(" aes key hex string   : %s\n", aes_key_hex_string.c_str());
        std::printf(" aes iv hex string    : %s\n", aes_iv_hex_string.c_str());
        std::printf("---------- spider command ----------\n");
        std::printf(" %d: add node (spider client)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT);
        std::printf(" %d: add node (spider pipe)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE);
        std::printf(" %d: show node information\n", SPIDER_COMMAND_SHOW_NODE_INFORMATION);
        std::printf(" %d: show routing table\n", SPIDER_COMMAND_SHOW_ROUTING_TABLE);
        std::printf(" %d: edit routing table\n", SPIDER_COMMAND_EDIT_ROUTING_TABLE);
        std::printf(" %d: add node (spider client udp)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP);
        std::printf(" %d: exit\n", SPIDER_COMMAND_EXIT);
        std::printf("------------------------------------\n");
        std::printf("\n");
        std::printf("command > ");

        std::cin >> spider_command;

        if(std::cin.fail())
        {
#ifdef _DEBUG
            std::printf("[-] spider command error\n");
#endif
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch(spider_command)
        {
            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT:
                std::printf("[+] add node (spider client)\n");
                spider::add_node_spider_client(spider_ip,
                                               encryption,
                                               client_manager,
                                               message_manager);
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE:
                std::printf("[+] add node (spider pipe)\n");
                spider::add_node_spider_pipe(spider_ip,
                                             pipe_manager,
                                             message_manager);
                break;

            case SPIDER_COMMAND_SHOW_NODE_INFORMATION:
                std::printf("[+] show node information\n");
                spider::show_node_information(client_manager,
                                              server_manager,
                                              pipe_manager);
                break;

            case SPIDER_COMMAND_SHOW_ROUTING_TABLE:
                std::printf("[+] show routing table\n");
                spider::show_routing_table(routing_manager);
                break;

            case SPIDER_COMMAND_EDIT_ROUTING_TABLE:
                std::printf("[+] edit routing table\n");
                spider::edit_routing_table(spider_ip,
                                           pipe_manager,
                                           routing_manager);
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP:
                std::printf("[+] add node (spider client udp)\n");
                std::printf("[!] This is not SOCKS5 connection. (UDP over TCP)\n");
                spider::add_node_spider_client_udp(spider_ip,
                                                   encryption,
                                                   client_manager,
                                                   message_manager);
                break;

            case SPIDER_COMMAND_EXIT:
                std::printf("[+] exit\n");
                exit(0);
                break;

            default:
                break;
        }
    }

    return 0;
}

