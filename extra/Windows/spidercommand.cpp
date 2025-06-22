/*
 * Title:  spider spidercommand.cpp (Windows)
 * Author: Shuichiro Endo
 */

#define NOMINMAX

#include "spider.hpp"
#include "spiderip.hpp"
#include "spidercommand.hpp"
#include "route.hpp"
#include "socks5message.hpp"
#include "client.hpp"
#include "server.hpp"
#include "pipe.hpp"
#include "clientmanager.hpp"
#include "servermanager.hpp"
#include "pipemanager.hpp"
#include "routingmanager.hpp"
#include "messagemanager.hpp"
#include "encryption.hpp"
#include "clienttls.hpp"
#include "servertls.hpp"
#include "basic.hpp"
#include "digest.hpp"
#include "ntlm.hpp"
#include "ntlmv2.hpp"
#include "spnego.hpp"


namespace spider
{
    Spidercommand::Spidercommand(std::shared_ptr<Spiderip> spider_ip,
                                 std::shared_ptr<Encryption> encryption,
                                 std::shared_ptr<Clientmanager> client_manager,
                                 std::shared_ptr<Servermanager> server_manager,
                                 std::shared_ptr<Pipemanager> pipe_manager,
                                 std::shared_ptr<Routingmanager> routing_manager,
                                 std::shared_ptr<Messagemanager> message_manager)
    {
        this->spider_ip = spider_ip;
        this->encryption = encryption;
        this->client_manager = client_manager;
        this->server_manager = server_manager;
        this->pipe_manager = pipe_manager;
        this->routing_manager = routing_manager;
        this->message_manager = message_manager;
    }

    Spidercommand::~Spidercommand()
    {

    }

    uint32_t Spidercommand::generate_random_id()
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<uint32_t> distrib(1, UINT32_MAX);

        return distrib(mt);
    }

    void Spidercommand::routing_manager_send_routing_table()
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // 1s

            routing_manager->send_routing_table();
        }

        return;
    }

    void Spidercommand::routing_manager_update_routing_table()
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            routing_manager->update_routing_table();
        }

        return;
    }

    void Spidercommand::routing_manager_delete_routing_table()
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 500ms

            routing_manager->delete_routing_table();
        }

        return;
    }

    void Spidercommand::routing_manager_worker()
    {
        std::thread routing_manager_send_routing_table_thread(&Spidercommand::routing_manager_send_routing_table,
                                                              this);
        routing_manager_send_routing_table_thread.detach();

        std::thread routing_manager_update_routing_table_thread(&Spidercommand::routing_manager_update_routing_table,
                                                    this);
        routing_manager_update_routing_table_thread.detach();

        std::thread routing_manager_delete_routing_table_thread(&Spidercommand::routing_manager_delete_routing_table,
                                                                this);
        routing_manager_delete_routing_table_thread.detach();

        return;
    }

    void Spidercommand::message_manager_transfer_routing_message()
    {
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5000));  // 5000µs

            message_manager->transfer_routing_message();
        }

        return;
    }

    void Spidercommand::message_manager_transfer_socks5_message(BOOL prevent_spider_server_startup_flag)
    {
        int ret = 0;
        std::shared_ptr<Socks5message> socks5_message = nullptr;
        std::shared_ptr<Server> server;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        uint32_t server_id = 0;

        while(1)
        {
            socks5_message = message_manager->transfer_socks5_message();
            if(socks5_message != nullptr
               && prevent_spider_server_startup_flag == false)  // generate server
            {
                connection_id = socks5_message->get_connection_id();
                client_id = socks5_message->get_client_id();
                server = std::make_shared<Server>(spider_ip,
                                                  connection_id,
                                                  client_id,
                                                  0,
                                                  socks5_message->get_destination_ip(),
                                                  "0",
                                                  socks5_message->get_source_ip(),
                                                  INVALID_SOCKET,
                                                  socks5_message->get_tv_sec(),
                                                  socks5_message->get_tv_usec(),
                                                  socks5_message->get_forwarder_tv_sec(),
                                                  socks5_message->get_forwarder_tv_usec(),
                                                  encryption,
                                                  client_manager,
                                                  server_manager,
                                                  pipe_manager,
                                                  routing_manager,
                                                  message_manager,
                                                  this);

                do
                {
                    server_id = generate_random_id();
                    ret = server_manager->insert_server(connection_id,
                                                        server_id,
                                                        server);
                }while(ret != 0);

#ifdef DEBUGPRINT
                std::printf("[+] generate server : %u\n", server_id);
#endif
                server->set_server_id(server_id);

                std::thread thread(&Spidercommand::server_worker,
                                   this,
                                   server,
                                   socks5_message);
                thread.detach();
            }
        }

        return;
    }

    void Spidercommand::message_manager_worker(BOOL prevent_spider_server_startup_flag)
    {
        std::thread message_manager_transfer_routing_message_thread(&Spidercommand::message_manager_transfer_routing_message,
                                                                    this);
        message_manager_transfer_routing_message_thread.detach();

        std::thread message_manager_transfer_socks5_message_thread(&Spidercommand::message_manager_transfer_socks5_message,
                                                                   this,
                                                                   prevent_spider_server_startup_flag);
        message_manager_transfer_socks5_message_thread.detach();

        return;
    }

    void Spidercommand::client_worker(std::shared_ptr<Client> client)
    {
        int32_t ret = 0;

        ret = client->do_socks5_connection();
        if(ret == -1)
        {
            if(client_manager->erase_client(client->get_connection_id(),
                                            client->get_client_id()))
            {
                closesocket(client->get_sock());
            }
        }

        return;
    }

    int Spidercommand::listen_client(std::string client_listen_ip,
                                     std::string client_listen_ip_scope_id,
                                     std::string client_listen_port,
                                     std::string destination_spider_ip,
                                     int32_t tv_sec,
                                     int32_t tv_usec,
                                     int32_t forwarder_tv_sec,
                                     int32_t forwarder_tv_usec)
    {
        int ret = 0;
        uint32_t connection_id = 0;
        struct sockaddr_in client_listen_addr, client_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 client_listen_addr6, client_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *client_listen_addr_info;
        SOCKET client_listen_sock = INVALID_SOCKET;
        SOCKET client_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int client_addr_length = sizeof(client_addr);
        int client_addr6_length = sizeof(client_addr6);
        char client_listen_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *client_listen_addr6_string_pointer = client_listen_addr6_string;
        char client_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                client_listen_addr6.sin6_scope_id = (uint32_t)std::stoi(client_listen_ip_scope_id);
            }

            freeaddrinfo(client_listen_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(client_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            client_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr,
                       sizeof(client_listen_addr));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            std::printf("[+] listening port %d on %s\n",
                        ntohs(client_listen_addr.sin_port),
                        inet_ntoa(client_listen_addr.sin_addr));

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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_length)) != INVALID_SOCKET)
            {
                // accept
#ifdef DEBUGPRINT
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(client_addr.sin_addr),
                            ntohs(client_addr.sin_port));
#endif

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

                std::thread thread(&Spidercommand::client_worker,
                                   this,
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
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr6,
                       sizeof(client_listen_addr6));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &client_listen_addr6.sin6_addr,
                      client_listen_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr6, (socklen_t *)&client_addr6_length)) != INVALID_SOCKET)
            {
                // accept
                inet_ntop(AF_INET6,
                          &client_addr6.sin6_addr,
                          client_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
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

                std::thread thread(&Spidercommand::client_worker,
                                   this,
                                   client);
                thread.detach();
            }

            client_manager->erase_client(connection_id,
                                         0);
        }

        closesocket(client_listen_sock);
        return 0;
    }

    void Spidercommand::add_node_spider_client()
    {
        std::string config = "";
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string client_destination_spider_ip;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                if(!client_listen_ip_scope_id.empty())
                {
                    std::printf("client listen ip scope id    : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
                }
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    std::thread thread(&Spidercommand::listen_client,
                                       this,
                                       client_listen_ip,
                                       client_listen_ip_scope_id,
                                       client_listen_port,
                                       client_destination_spider_ip,
                                       tv_sec,
                                       tv_usec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    thread.detach();

                    std::this_thread::sleep_for(std::chrono::seconds(2));  // 2s

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client listen ip                               > ");
                std::cin >> client_listen_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                std::printf("source spider ip             : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id    : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip        : %s\n", destination_spider_ip.c_str());
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    config = "";
                    config += "[client]\n";

                    config += "client_listen_ip:";
                    config += client_listen_ip;
                    config += "\n";

                    config += "client_listen_port:";
                    config += client_listen_port;
                    config += "\n";

                    config += "client_destination_spider_ip:";
                    config += client_destination_spider_ip;
                    config += "\n";

                    config += "tv_sec:";
                    config += std::to_string(tv_sec);
                    config += "\n";

                    config += "tv_usec:";
                    config += std::to_string(tv_usec);
                    config += "\n";

                    config += "forwarder_tv_sec:";
                    config += std::to_string(forwarder_tv_sec);
                    config += "\n";

                    config += "forwarder_tv_usec:";
                    config += std::to_string(forwarder_tv_usec);
                    config += "\n";

                    std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                       this,
                                       config,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }

    void Spidercommand::server_worker(std::shared_ptr<Server> server,
                                      std::shared_ptr<Socks5message> socks5_message)
    {
        int32_t ret = 0;

        ret = server->do_socks5_connection(socks5_message);
        if(ret == -1)
        {
            if(server_manager->erase_server(server->get_connection_id(),
                                            server->get_server_id()))
            {
                closesocket(server->get_sock());
            }
        }

        return;
    }

    void Spidercommand::pipe_recv_message(std::shared_ptr<Pipe> pipe)
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
                    closesocket(pipe->get_sock());
                }
                break;
            }
        }
    }

    void Spidercommand::pipe_send_routing_message(std::shared_ptr<Pipe> pipe)
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
                    closesocket(pipe->get_sock());
                }
                break;
            }
        }
    }

    void Spidercommand::pipe_send_socks5_message(std::shared_ptr<Pipe> pipe)
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
                    closesocket(pipe->get_sock());
                }
                break;
            }
        }
    }

    void Spidercommand::pipe_worker(std::shared_ptr<Pipe> pipe)
    {

        std::thread pipe_recv_message_thread(&Spidercommand::pipe_recv_message,
                                             this,
                                             pipe);
        pipe_recv_message_thread.detach();

        std::thread pipe_send_routing_message_thread(&Spidercommand::pipe_send_routing_message,
                                                     this,
                                                     pipe);
        pipe_send_routing_message_thread.detach();

        std::thread pipe_send_socks5_message_thread(&Spidercommand::pipe_send_socks5_message,
                                                    this,
                                                    pipe);
        pipe_send_socks5_message_thread.detach();

        return;
    }

    int Spidercommand::connect_pipe(char mode,
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
        SOCKET pipe_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int pipe_dest_addr_length = sizeof(pipe_dest_addr);
        int pipe_dest_addr6_length = sizeof(pipe_dest_addr6);
        char pipe_dest_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_destination_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_destination_ip.c_str());
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
                pipe_dest_addr6.sin6_scope_id = (uint32_t)std::stoi(pipe_ip_scope_id);
            }

            freeaddrinfo(pipe_dest_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(pipe_dest_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            pipe_sock = socket(AF_INET,
                               SOCK_STREAM,
                               0);
            if(pipe_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            std::printf("[+] connecting to ip:%s port:%d\n",
                        inet_ntoa(pipe_dest_addr.sin_addr),
                        ntohs(pipe_dest_addr.sin_port));

            // connect
            ret = connect(pipe_sock,
                          (struct sockaddr *)&pipe_dest_addr,
                          sizeof(pipe_dest_addr));
            if(ret < 0){
                std::printf("[-] connect failed:%d\n",
                            ret);
                closesocket(pipe_sock);
                return -1;
            }

            std::printf("[+] connected to ip:%s port:%d\n",
                        inet_ntoa(pipe_dest_addr.sin_addr),
                        ntohs(pipe_dest_addr.sin_port));
        }else if(family == AF_INET6)    // IPv6
        {
            pipe_sock = socket(AF_INET6,
                               SOCK_STREAM,
                               0);
            if(pipe_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            inet_ntop(AF_INET6,
                      &pipe_dest_addr6.sin6_addr,
                      pipe_dest_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);
            if(pipe_dest_addr6.sin6_scope_id > 0){
                std::printf("[+] connecting to ip:%s%%%d port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            pipe_dest_addr6.sin6_scope_id,
                            ntohs(pipe_dest_addr6.sin6_port));
            }else{
                std::printf("[+] connecting to ip:%s port:%d\n",
                            pipe_dest_addr6_string_pointer,
                            ntohs(pipe_dest_addr6.sin6_port));
            }

            // connect
            ret = connect(pipe_sock,
                          (struct sockaddr *)&pipe_dest_addr6,
                          sizeof(pipe_dest_addr6));
            if(ret != 0){
                std::printf("[-] connect failed: %d\n",
                            WSAGetLastError());
                closesocket(pipe_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &pipe_dest_addr6.sin6_addr,
                      pipe_dest_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);
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

            pipe_destination_ip_scope_id = std::to_string(pipe_dest_addr6.sin6_scope_id);
        }else
        {
            std::printf("[-] not implemented\n");
            return -1;
        }

        pipe = std::make_shared<Pipe>(spider_ip,
                                      0,
                                      mode,
                                      "d",
                                      pipe_ip,
                                      pipe_ip_scope_id,
                                      pipe_destination_ip,
                                      pipe_destination_ip_scope_id,
                                      pipe_destination_port,
                                      pipe_sock,
                                      routing_manager,
                                      message_manager);

        do
        {
            pipe_key = generate_random_id();
            ret = pipe_manager->insert_pipe(pipe_key,
                                            pipe);
        }while(ret != 0);

        std::thread thread(&Spidercommand::pipe_worker,
                           this,
                           pipe);
        thread.detach();

        return 0;
    }

    int Spidercommand::listen_pipe(char mode,
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
        SOCKET pipe_listen_sock = INVALID_SOCKET;
        SOCKET pipe_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int pipe_addr_length = sizeof(pipe_addr);
        int pipe_addr6_length = sizeof(pipe_addr6);
        char pipe_listen_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *pipe_listen_addr6_string_pointer = pipe_listen_addr6_string;
        char pipe_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
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
                pipe_listen_addr6.sin6_scope_id = (uint32_t)std::stoi(pipe_listen_ip_scope_id);
            }

            freeaddrinfo(pipe_listen_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(pipe_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            pipe_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            if(pipe_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr,
                       sizeof(pipe_listen_addr));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            // listen
            ret = listen(pipe_listen_sock,
                         5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            std::printf("[+] listening port %d on %s\n",
                        ntohs(pipe_listen_addr.sin_port),
                        inet_ntoa(pipe_listen_addr.sin_addr));

            pipe_listen = std::make_shared<Pipe>(spider_ip,
                                                 0,
                                                 mode,
                                                 "d",
                                                 pipe_listen_ip,
                                                 "",
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 routing_manager,
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
                    closesocket(pipe_sock);
                    continue;
                }
#ifdef DEBUGPRINT
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(pipe_addr.sin_addr),
                            ntohs(pipe_addr.sin_port));
#endif

                std::string pipe_destination_ip = inet_ntoa(pipe_addr.sin_addr);
                std::string pipe_destination_port = std::to_string(ntohs(pipe_addr.sin_port));
                std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>(spider_ip,
                                                                    0,
                                                                    '-',
                                                                    "d",
                                                                    pipe_listen_ip,
                                                                    "",
                                                                    pipe_destination_ip,
                                                                    "",
                                                                    pipe_destination_port,
                                                                    pipe_sock,
                                                                    routing_manager,
                                                                    message_manager);

                do
                {
                    pipe_id = generate_random_id();
                    ret = pipe_manager->insert_pipe(pipe_id,
                                                    pipe);
                }while(ret != 0);

                std::thread thread(&Spidercommand::pipe_worker,
                                   this,
                                   pipe);
                thread.detach();
            }

            pipe_manager->erase_pipe(pipe_listen_key);
        }else if(family == AF_INET6)
        {
            pipe_listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
            if(pipe_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr6,
                       sizeof(pipe_listen_addr6));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            // listen
            ret = listen(pipe_listen_sock,
                         5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &pipe_listen_addr6.sin6_addr,
                      pipe_listen_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

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

            pipe_listen = std::make_shared<Pipe>(spider_ip,
                                                 0,
                                                 mode,
                                                 "d",
                                                 pipe_listen_ip,
                                                 pipe_listen_ip_scope_id,
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 routing_manager,
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
                   && pipe_manager->check_pipe(pipe_id)){
                    closesocket(pipe_sock);
                    continue;
                }

                inet_ntop(AF_INET6,
                          &pipe_addr6.sin6_addr,
                          pipe_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
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

                std::string pipe_destination_ip;
                std::string pipe_destination_ip_scope_id;
                std::string pipe_destination_port;
                pipe_destination_ip = pipe_addr6_string_pointer;
                pipe_destination_ip_scope_id = std::to_string(pipe_addr6.sin6_scope_id);
                pipe_destination_port = std::to_string(ntohs(pipe_addr6.sin6_port));
                std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>(spider_ip,
                                                                    0,
                                                                    '-',
                                                                    "d",
                                                                    pipe_listen_ip,
                                                                    pipe_listen_ip_scope_id,
                                                                    pipe_destination_ip,
                                                                    pipe_destination_ip_scope_id,
                                                                    pipe_destination_port,
                                                                    pipe_sock,
                                                                    routing_manager,
                                                                    message_manager);

                do
                {
                    pipe_id = generate_random_id();
                    ret = pipe_manager->insert_pipe(pipe_id,
                                                    pipe);
                }while(ret != 0);

                std::thread thread(&Spidercommand::pipe_worker,
                                   this,
                                   pipe);
                thread.detach();
            }

            pipe_manager->erase_pipe(pipe_listen_key);
        }

        closesocket(pipe_listen_sock);

        return 0;
    }

    void Spidercommand::free_all_buffers(SecBufferDesc *sec_buffer_desc)
    {
        for(unsigned long i=0; i<sec_buffer_desc->cBuffers; i++)
        {
            if(sec_buffer_desc->pBuffers[i].pvBuffer != NULL)
            {
                FreeContextBuffer(sec_buffer_desc->pBuffers[i].pvBuffer);
            }
        }
    }

    PCCERT_CONTEXT Spidercommand::find_certificate(const HCERTSTORE h_cert_store,
                                                   const char* cert_search_string)
    {
        BOOL ret_b = false;
        PCCERT_CONTEXT cert = NULL;
        DWORD type = CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG;
        char cert_name[CERT_NAME_MAX_SIZE] = {0};

        while(1)
        {
            cert = CertEnumCertificatesInStore(h_cert_store,
                                               cert);
            if(cert == NULL)
            {
                break;
            }

            ret_b = CertGetNameStringA(cert,
                                       CERT_NAME_RDN_TYPE,
                                       0,
                                       &type,
                                       cert_name,
                                       CERT_NAME_MAX_SIZE);
            if(ret_b == FALSE)
            {
                CertFreeCertificateContext(cert);
                cert = NULL;
                break;
            }

            if((strncmp(cert_name, cert_search_string, CERT_NAME_MAX_SIZE) == 0) &&
               (cert->dwCertEncodingType == X509_ASN_ENCODING))
            {
                break;
            }
        }

        return cert;
    }

    int Spidercommand::forward_proxy_authentication_no(std::shared_ptr<Pipe> pipe)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        long tv_sec = 30;
        long tv_usec = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *http_request = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
        int32_t http_request_length = 0;
        std::string forward_proxy_type = pipe->get_forward_proxy_type();
        std::string pipe_destination_ip = pipe->get_pipe_destination_ip();
        std::string pipe_destination_port = pipe->get_pipe_destination_port();


        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nAccept: */*\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nUser-Agent: %s\r\nAccept: */*\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            rec = pipe->recv_data(buffer,
                                  NODE_BUFFER_SIZE,
                                  tv_sec,
                                  tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no recv_data error\n");
#endif
                goto error;
            }

            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no strncmp error\n");
#endif
                goto error;
            }
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            rec = pipe->recv_data_tls(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no recv_data_tls error\n");
#endif
                goto error;
            }

            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_no strncmp error\n");
#endif
                goto error;
            }
        }

        free(buffer);
        free(http_request);
        return 0;

error:
        free(buffer);
        free(http_request);
        return -1;
    }

    int Spidercommand::forward_proxy_authentication_basic(std::shared_ptr<Pipe> pipe)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        long tv_sec = 30;
        long tv_usec = 0;
        int32_t http_request_length = 0;

        int32_t length = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *http_request = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
        char *proxy_b64_credential = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));

        std::string forward_proxy_type = pipe->get_forward_proxy_type();
        std::string pipe_destination_ip = pipe->get_pipe_destination_ip();
        std::string pipe_destination_port = pipe->get_pipe_destination_port();
        std::string forward_proxy_username = pipe->get_forward_proxy_username();
        std::string forward_proxy_password = pipe->get_forward_proxy_password();

        std::unique_ptr<Basic> basic = std::make_unique<Basic>(forward_proxy_username,
                                                               forward_proxy_password);


        ret = basic->get_base64_credential(proxy_b64_credential,
                                           NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_basic get_base64_credential error\n");
#endif
            goto error;
        }

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Authorization: Basic %s\r\nUser-Agent: %s\r\nAccept: */*\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           proxy_b64_credential,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nProxy-Authorization: Basic %s\r\nUser-Agent: %s\r\nAccept: */*\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           proxy_b64_credential,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            rec = pipe->recv_data(buffer,
                                  NODE_BUFFER_SIZE,
                                  tv_sec,
                                  tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic recv_data error\n");
#endif
                goto error;
            }

            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic strncmp error\n");
#endif
                goto error;
            }
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            rec = pipe->recv_data_tls(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic recv_data_tls error\n");
#endif
                goto error;
            }

            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_basic strncmp error\n");
#endif
                goto error;
            }
        }

        free(buffer);
        free(http_request);
        free(proxy_b64_credential);
        return 0;

error:
        free(buffer);
        free(http_request);
        free(proxy_b64_credential);
        return -1;
    }

    int Spidercommand::forward_proxy_authentication_digest(std::shared_ptr<Pipe> pipe)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        long tv_sec = 30;
        long tv_usec = 0;
        int http_request_length = 0;
        int length = 0;
        int count = 0;
        int check = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *http_request = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));

        std::string forward_proxy_type = pipe->get_forward_proxy_type();
        std::string pipe_destination_ip = pipe->get_pipe_destination_ip();
        std::string pipe_destination_port = pipe->get_pipe_destination_port();
        std::string forward_proxy_username = pipe->get_forward_proxy_username();
        std::string forward_proxy_password = pipe->get_forward_proxy_password();

        std::unique_ptr<Digest> digest = std::make_unique<Digest>(forward_proxy_username,
                                                                  forward_proxy_password,
                                                                  pipe_destination_ip,
                                                                  pipe_destination_port);


        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data(buffer,
                                  NODE_BUFFER_SIZE,
                                  tv_sec,
                                  tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest recv_data error\n");
#endif
                goto error;
            }
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data_tls(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest recv_data_tls error\n");
#endif
                goto error;
            }
        }

        ret = strncmp(buffer,
                      "HTTP/1.1 407 Proxy Authentication Required\r\n",
                      strlen("HTTP/1.1 407 Proxy Authentication Required\r\n"));
        if(ret != 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_digest strncmp error\n");
#endif
            goto error;
        }

        ret = digest->get_digest_response(buffer);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_digest get_digest_response error\n");
#endif
            goto error;
        }

        std::memset(http_request,
                    0,
                    NODE_BUFFER_SIZE);

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", cnonce=\"%s\", nc=%s, qop=%s, response=\"%s\"\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           digest->get_digest_param()->username,
                                           digest->get_digest_param()->realm,
                                           digest->get_digest_param()->nonce,
                                           digest->get_digest_param()->uri,
                                           digest->get_digest_param()->cnonce,
                                           digest->get_digest_param()->nc,
                                           digest->get_digest_param()->qop,
                                           digest->get_digest_param()->response_hash,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nProxy-Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", cnonce=\"%s\", nc=%s, qop=%s, response=\"%s\"\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           digest->get_digest_param()->username,
                                           digest->get_digest_param()->realm,
                                           digest->get_digest_param()->nonce,
                                           digest->get_digest_param()->uri,
                                           digest->get_digest_param()->cnonce,
                                           digest->get_digest_param()->nc,
                                           digest->get_digest_param()->qop,
                                           digest->get_digest_param()->response_hash,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }


        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_digest recv_data error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data_tls(buffer,
                                          NODE_BUFFER_SIZE,
                                          tv_sec,
                                          tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_digest recv_data_tls error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }

        if(check == 1)
        {
            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_digest strncmp error\n");
#endif
                goto error;
            }
        }

        free(buffer);
        free(http_request);
        return 0;

error:
        free(buffer);
        free(http_request);
        return -1;
    }

    int Spidercommand::forward_proxy_authentication_ntlmv2(std::shared_ptr<Pipe> pipe)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        long tv_sec = 30;
        long tv_usec = 0;
        int http_request_length = 0;
        int count = 0;
        int check = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *http_request = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
        char *ntlm_b64 = (char *)calloc(NODE_BUFFER_SIZE,
                                        sizeof(char));

        std::string forward_proxy_type = pipe->get_forward_proxy_type();
        std::string pipe_destination_ip = pipe->get_pipe_destination_ip();
        std::string pipe_destination_port = pipe->get_pipe_destination_port();
        std::string forward_proxy_username = pipe->get_forward_proxy_username();
        std::string forward_proxy_password = pipe->get_forward_proxy_password();
        std::string forward_proxy_nthash_hexstring = pipe->get_forward_proxy_nthash_hexstring();
        std::string forward_proxy_domainname = pipe->get_forward_proxy_domainname();
        std::string forward_proxy_workstationname = pipe->get_forward_proxy_workstationname();

        std::unique_ptr<Ntlmv2> ntlmv2 = std::make_unique<Ntlmv2>(forward_proxy_username,
                                                                  forward_proxy_password,
                                                                  forward_proxy_nthash_hexstring,
                                                                  forward_proxy_domainname,
                                                                  forward_proxy_workstationname);


        ret = ntlmv2->generate_base64_negotiate_message(ntlm_b64,
                                                        NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_ntlmv2 generate_base64_negotiate_message error\n");
#endif
            goto error;
        }

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Authorization: NTLM %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           ntlm_b64,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);

        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nProxy-Authorization: NTLM %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           ntlm_b64,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data(buffer,
                                  NODE_BUFFER_SIZE,
                                  tv_sec,
                                  tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 recv_data error\n");
#endif
                goto error;
            }
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data_tls(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 recv_data_tls error\n");
#endif
                goto error;
            }
        }

        ret = strncmp(buffer,
                      "HTTP/1.1 407 Proxy Authentication Required\r\n",
                      strlen("HTTP/1.1 407 Proxy Authentication Required\r\n"));
        if(ret != 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_ntlmv2 strncmp error\n");
#endif
            goto error;
        }

        std::memset(ntlm_b64,
                    0,
                    NODE_BUFFER_SIZE);

        ret = ntlmv2->generate_base64_authenticate_message(buffer,
                                                           ntlm_b64,
                                                           NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_ntlmv2 generate_base64_authenticate_message error\n");
#endif
            goto error;
        }

        std::memset(http_request,
                    0,
                    NODE_BUFFER_SIZE);

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Authorization: NTLM %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           ntlm_b64,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nProxy-Authorization: NTLM %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           ntlm_b64,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_ntlmv2 recv_data error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data_tls(buffer,
                                          NODE_BUFFER_SIZE,
                                          tv_sec,
                                          tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_ntlmv2 recv_data_tls error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }

        if(check == 1)
        {
            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_ntlmv2 strncmp error\n");
#endif
                goto error;
            }
        }

        free(buffer);
        free(http_request);
        free(ntlm_b64);
        return 0;

error:
        free(buffer);
        free(http_request);
        free(ntlm_b64);
        return -1;
    }

    int Spidercommand::forward_proxy_authentication_spnego_kerberos(std::shared_ptr<Pipe> pipe)
    {
        int ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        long tv_sec = 30;
        long tv_usec = 0;
        int http_request_length = 0;
        int count = 0;
        int check = 0;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        char *http_request = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
        char *b64_kerberos_token = (char *)calloc(NODE_BUFFER_SIZE,
                                                  sizeof(char));

        std::string forward_proxy_type = pipe->get_forward_proxy_type();
        std::string pipe_destination_ip = pipe->get_pipe_destination_ip();
        std::string pipe_destination_port = pipe->get_pipe_destination_port();
        std::string forward_proxy_service_principal_name = pipe->get_forward_proxy_service_principal_name();

        std::unique_ptr<Spnego> spnego = std::make_unique<Spnego>(forward_proxy_service_principal_name);

        ret = spnego->generate_base64_kerberos_token(b64_kerberos_token,
                                                     NODE_BUFFER_SIZE);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_spnego_kerberos generate_base64_kerberos_token error\n");
#endif
            goto error;
        }

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data(buffer,
                                  NODE_BUFFER_SIZE,
                                  tv_sec,
                                  tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos recv_data error\n");
#endif
                goto error;
            }
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 407 Proxy Authentication Required)
            rec = pipe->recv_data_tls(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
            if(rec <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos recv_data_tls error\n");
#endif
                goto error;
            }
        }

        ret = strncmp(buffer,
                      "HTTP/1.1 407 Proxy Authentication Required\r\n",
                      strlen("HTTP/1.1 407 Proxy Authentication Required\r\n"));
        if(ret != 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_spnego_kerberos strncmp error\n");
#endif
            goto error;
        }

        ret = spnego->check_http_header(buffer);
        if(ret < 0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] forward_proxy_authentication_spnego_kerberos check_http_header error\n");
#endif
            goto error;
        }

        std::memset(http_request,
                    0,
                    NODE_BUFFER_SIZE);

        if(pipe_destination_ip.find(":") == std::string::npos)  // no ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Authorization: Negotiate %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           b64_kerberos_token,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }else   // ipv6 address
        {
            http_request_length = snprintf(http_request,
                                           NODE_BUFFER_SIZE,
                                           "CONNECT [%s]:%s HTTP/1.1\r\nHost: [%s]:%s\r\nProxy-Authorization: Negotiate %s\r\nUser-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n\r\n",
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           pipe_destination_ip.c_str(),
                                           pipe_destination_port.c_str(),
                                           b64_kerberos_token,
                                           HTTP_REQUEST_HEADER_USER_AGENT_VALUE);
        }

        if(forward_proxy_type == "h" ||
           forward_proxy_type == "hs")
        {
            // HTTP Request
            sen = pipe->send_data(http_request,
                                  http_request_length,
                                  tv_sec,
                                  tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos send_data error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data(buffer,
                                      NODE_BUFFER_SIZE,
                                      tv_sec,
                                      tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_spnego_kerberos recv_data error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }else
        {
            // HTTP Request
            sen = pipe->send_data_tls(http_request,
                                      http_request_length,
                                      tv_sec,
                                      tv_usec);
            if(sen <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos send_data_tls error\n");
#endif
                goto error;
            }

            // HTTP Response (HTTP/1.1 200 Connection established)
            count = 0;
            check = 0;

            do
            {
                count++;

                std::memset(buffer,
                            0,
                            NODE_BUFFER_SIZE);

                rec = pipe->recv_data_tls(buffer,
                                          NODE_BUFFER_SIZE,
                                          tv_sec,
                                          tv_usec);
                if(rec <= 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] forward_proxy_authentication_spnego_kerberos recv_data_tls error\n");
#endif
                    goto error;
                }

                ret = strncmp(buffer,
                              "HTTP/1.1",
                              strlen("HTTP/1.1"));
                if(ret == 0)
                {
                    check = 1;
                    break;
                }
            }while(count < 2);
        }

        if(check == 1)
        {
            ret = strncmp(buffer,
                          "HTTP/1.1 200 Connection established\r\n",
                          strlen("HTTP/1.1 200 Connection established\r\n"));
            if(ret != 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] forward_proxy_authentication_spnego_kerberos strncmp error\n");
#endif
                goto error;
            }
        }

        free(buffer);
        free(http_request);
        free(b64_kerberos_token);
        return 0;

error:
        free(buffer);
        free(http_request);
        free(b64_kerberos_token);
        return -1;
    }

    int Spidercommand::connect_pipe_http(char mode,
                                         BOOL forward_proxy_flag,
                                         std::string forward_proxy_type,
                                         int32_t forward_proxy_authentication,
                                         std::string forward_proxy_ip,
                                         std::string forward_proxy_port,
                                         std::string forward_proxy_username,
                                         std::string forward_proxy_password,
                                         std::string forward_proxy_nthash_hexstring,
                                         std::string forward_proxy_domainname,
                                         std::string forward_proxy_workstationname,
                                         std::string forward_proxy_service_principal_name,
                                         BOOL tls_flag,
                                         std::string message_mode,
                                         std::string pipe_ip,
                                         std::string pipe_ip_scope_id,
                                         std::string pipe_destination_ip,
                                         std::string pipe_destination_port,
                                         int32_t sleep_ms)
    {
        int ret = 0;
        uint32_t pipe_id = 0;
        struct sockaddr_in pipe_dest_addr;
        struct sockaddr_in forward_proxy_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 pipe_dest_addr6;
        struct sockaddr_in6 forward_proxy_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *pipe_dest_addr_info;
        struct addrinfo *forward_proxy_addr_info;
        SOCKET pipe_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int pipe_dest_addr_length = sizeof(pipe_dest_addr);
        int forward_proxy_addr_length = sizeof(forward_proxy_addr);
        int pipe_dest_addr6_length = sizeof(pipe_dest_addr6);
        int forward_proxy_addr6_length = sizeof(forward_proxy_addr6);
        char pipe_dest_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char forward_proxy_addr6_string[INET6_ADDRSTRLEN + 1] = {0};
        char *pipe_dest_addr6_string_pointer = pipe_dest_addr6_string;
        char *forward_proxy_addr6_string_pointer = forward_proxy_addr6_string;
        std::string pipe_destination_ip_scope_id;
        std::string forward_proxy_ip_scope_id;
        std::shared_ptr<Pipe> pipe = nullptr;
        uint32_t pipe_key = 0;
        int32_t error_count = 0;


        std::memset((char *)&pipe_dest_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&forward_proxy_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset((char *)&pipe_dest_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&forward_proxy_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset((char *)&hints,
                    0,
                    sizeof(struct addrinfo));


        if(forward_proxy_flag == true)
        {
            if(forward_proxy_ip.find(":") == std::string::npos)  // ipv4 address
            {
                hints.ai_family = AF_INET;
                ret = getaddrinfo(forward_proxy_ip.c_str(),
                                  forward_proxy_port.c_str(),
                                  &hints,
                                  &forward_proxy_addr_info);
                if(ret != 0)
                {
                    std::printf("[-] cannot resolv the address: %s\n",
                                forward_proxy_ip.c_str());
                    return -1;
                }
            }else   // ipv6 address
            {
                hints.ai_family = AF_INET6;
                ret = getaddrinfo(forward_proxy_ip.c_str(),
                                  forward_proxy_port.c_str(),
                                  &hints,
                                  &forward_proxy_addr_info);
                if(ret != 0)
                {
                    std::printf("[-] cannot resolv the address: %s\n",
                                forward_proxy_ip.c_str());
                    return -1;
                }
            }

            if(forward_proxy_addr_info->ai_family == AF_INET)
            {
                tmp_ipv4 = (struct sockaddr_in *)forward_proxy_addr_info->ai_addr;

                family = AF_INET;
                forward_proxy_addr.sin_family = AF_INET;
                std::memcpy(&forward_proxy_addr.sin_addr,
                            &tmp_ipv4->sin_addr,
                            sizeof(struct in_addr));
                std::memcpy(&forward_proxy_addr.sin_port,
                            &tmp_ipv4->sin_port,
                            2);

                freeaddrinfo(forward_proxy_addr_info);
            }else if(forward_proxy_addr_info->ai_family == AF_INET6)
            {
                tmp_ipv6 = (struct sockaddr_in6 *)forward_proxy_addr_info->ai_addr;

                family = AF_INET6;
                forward_proxy_addr6.sin6_family = AF_INET6;
                std::memcpy(&forward_proxy_addr6.sin6_addr,
                            &tmp_ipv6->sin6_addr,
                            sizeof(struct in6_addr));
                std::memcpy(&forward_proxy_addr6.sin6_port,
                            &tmp_ipv6->sin6_port,
                            2);

                if(pipe_ip_scope_id.size() > 0)
                {
                    forward_proxy_addr6.sin6_scope_id = (uint32_t)std::stoi(pipe_ip_scope_id);
                }

                freeaddrinfo(forward_proxy_addr_info);
            }else
            {
                std::printf("[-] not implemented\n");
                freeaddrinfo(forward_proxy_addr_info);
                return -1;
            }

            if(family == AF_INET)   // IPv4
            {
                while(1)
                {
                    pipe_sock = socket(AF_INET,
                                       SOCK_STREAM,
                                       0);
                    if(pipe_sock == INVALID_SOCKET)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] socket error: %d\n",
                                    WSAGetLastError());
#endif
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] connecting to ip:%s port:%d\n",
                                inet_ntoa(forward_proxy_addr.sin_addr),
                                ntohs(forward_proxy_addr.sin_port));
#endif

                    // connect
                    ret = connect(pipe_sock,
                                  (struct sockaddr *)&forward_proxy_addr,
                                  sizeof(forward_proxy_addr));
                    if(ret < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] connect failed:%d\n",
                                    ret);
#endif
                        closesocket(pipe_sock);
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] connected to ip:%s port:%d\n",
                                inet_ntoa(forward_proxy_addr.sin_addr),
                                ntohs(forward_proxy_addr.sin_port));
#endif

                    if(pipe == nullptr)
                    {
                        pipe = std::make_shared<Pipe>(spider_ip,
                                                      0,
                                                      mode,
                                                      message_mode,
                                                      pipe_ip,
                                                      pipe_ip_scope_id,
                                                      pipe_destination_ip,
                                                      pipe_destination_ip_scope_id,
                                                      pipe_destination_port,
                                                      pipe_sock,
                                                      routing_manager,
                                                      message_manager);

                        do
                        {
                            pipe_key = generate_random_id();
                            ret = pipe_manager->insert_pipe(pipe_key,
                                                            pipe);
                        }while(ret != 0);
                    }else
                    {
                        pipe->set_sock(pipe_sock);
                    }


                    SCHANNEL_CRED schannel_cred;
                    CredHandle cred_handle;
                    CtxtHandle ctxt_handle;
                    SecPkgContext_StreamSizes stream_sizes;
                    SECURITY_STATUS status;
                    unsigned long f_context_req;
                    unsigned long f_context_attr;
                    TimeStamp ts_expiry;
                    SecBufferDesc input_sec_buffer_desc;
                    SecBufferDesc output_sec_buffer_desc;
                    SecBuffer input_sec_buffer[4];
                    SecBuffer output_sec_buffer[3];
                    BOOL init_flag = true;
                    BOOL error_flag = false;
                    char *buffer = NULL;
                    char *tmp = NULL;
                    int32_t rec = 0;
                    int32_t tmprec = 0;
                    int32_t sen = 0;
                    unsigned long tv_sec = 30;
                    unsigned long tv_usec = 0;


                    if(forward_proxy_type == "h" ||
                       forward_proxy_type == "hs")
                    {
                        // forward proxy
                        if(forward_proxy_authentication == 0)   // no
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);

                            ret = forward_proxy_authentication_no(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_no error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);

                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 1) // basic
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_basic(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_basic error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 2) // digest
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_digest(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_digest error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 3) // ntlmv2
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);
                            pipe->set_forward_proxy_nthash_hexstring(forward_proxy_nthash_hexstring);
                            pipe->set_forward_proxy_domainname(forward_proxy_domainname);
                            pipe->set_forward_proxy_workstationname(forward_proxy_workstationname);

                            ret = forward_proxy_authentication_ntlmv2(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_ntlmv2 error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 4) // spnego(kerberos)
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_service_principal_name(forward_proxy_service_principal_name);

                            ret = forward_proxy_authentication_spnego_kerberos(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_spnego_kerberos error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] Not implemented\n");
#endif
                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }


                        // TLS
                        if(tls_flag == true)
                        {
                            std::memset(&schannel_cred,
                                        0,
                                        sizeof(schannel_cred));
                            schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                            schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                            schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                            status = AcquireCredentialsHandleA(NULL,
                                                               const_cast<LPSTR>(UNISP_NAME_A),
                                                               SECPKG_CRED_OUTBOUND,
                                                               NULL,
                                                               &schannel_cred,
                                                               NULL,
                                                               NULL,
                                                               &cred_handle,
                                                               NULL);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                            status);
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            std::memset(&ctxt_handle,
                                        0,
                                        sizeof(ctxt_handle));

                            f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                            output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                            output_sec_buffer[0].pvBuffer = NULL;
                            output_sec_buffer[0].cbBuffer = 0;
                            output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                            output_sec_buffer[1].pvBuffer = NULL;
                            output_sec_buffer[1].cbBuffer = 0;
                            output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                            output_sec_buffer[2].pvBuffer = NULL;
                            output_sec_buffer[2].cbBuffer = 0;
                            output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                            output_sec_buffer_desc.cBuffers = 3;
                            output_sec_buffer_desc.pBuffers = output_sec_buffer;

                            buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));
                            tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                                 sizeof(char));

                            while(1)
                            {
                                if(init_flag == true)
                                {
                                    init_flag = false;

                                    status = InitializeSecurityContext(&cred_handle,
                                                                       NULL,
                                                                       target_server_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       NULL,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }else
                                {
                                    input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                    input_sec_buffer[0].pvBuffer = buffer;
                                    input_sec_buffer[0].cbBuffer = rec;
                                    input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[1].pvBuffer = NULL;
                                    input_sec_buffer[1].cbBuffer = 0;
                                    input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[2].pvBuffer = NULL;
                                    input_sec_buffer[2].cbBuffer = 0;
                                    input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[3].pvBuffer = NULL;
                                    input_sec_buffer[3].cbBuffer = 0;
                                    input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                    input_sec_buffer_desc.cBuffers = 4;
                                    input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                    status = InitializeSecurityContext(&cred_handle,
                                                                       &ctxt_handle,
                                                                       target_server_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       &input_sec_buffer_desc,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }

                                if(status == SEC_E_OK)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_CONTINUE_NEEDED)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_I_COMPLETE_NEEDED)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] CompleteAuthToken error: %x\n",
                                                    status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        printf("[-] CompleteAuthToken error: %x\n",
                                               status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                                {
                                    tmprec = pipe->recv_data(tmp,
                                                             NODE_BUFFER_SIZE,
                                                             tv_sec,
                                                             tv_usec);
                                    if(tmprec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }

                                    if(rec + tmprec <= NODE_BUFFER_SIZE)
                                    {
                                        std::memcpy(buffer + rec,
                                                    tmp,
                                                    tmprec);
                                        rec += tmprec;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                    rec + tmprec);
#endif
                                        error_flag = true;
                                        break;
                                    }
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] InitializeSecurityContext error:%x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                            }

                            free_all_buffers(&output_sec_buffer_desc);
                            free(buffer);
                            free(tmp);

                            if(error_flag == true)
                            {
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            status = QueryContextAttributes(&ctxt_handle,
                                                            SECPKG_ATTR_STREAM_SIZES,
                                                            &stream_sizes);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] QueryContextAttributes error:%x\n",
                                            status);
#endif
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            pipe->set_ctxt_handle(&ctxt_handle);
                            pipe->set_stream_sizes(stream_sizes);
                        }
                    }else if(forward_proxy_type == "s")
                    {
                        // TLS
                        if(tls_flag == true)
                        {
                            std::memset(&schannel_cred,
                                        0,
                                        sizeof(schannel_cred));
                            schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                            schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                            schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                            status = AcquireCredentialsHandleA(NULL,
                                                               const_cast<LPSTR>(UNISP_NAME_A),
                                                               SECPKG_CRED_OUTBOUND,
                                                               NULL,
                                                               &schannel_cred,
                                                               NULL,
                                                               NULL,
                                                               &cred_handle,
                                                               NULL);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                            status);
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            std::memset(&ctxt_handle,
                                        0,
                                        sizeof(ctxt_handle));

                            f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                            output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                            output_sec_buffer[0].pvBuffer = NULL;
                            output_sec_buffer[0].cbBuffer = 0;
                            output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                            output_sec_buffer[1].pvBuffer = NULL;
                            output_sec_buffer[1].cbBuffer = 0;
                            output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                            output_sec_buffer[2].pvBuffer = NULL;
                            output_sec_buffer[2].cbBuffer = 0;
                            output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                            output_sec_buffer_desc.cBuffers = 3;
                            output_sec_buffer_desc.pBuffers = output_sec_buffer;

                            buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));
                            tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                                 sizeof(char));

                            while(1)
                            {
                                if(init_flag == true)
                                {
                                    init_flag = false;
                                    status = InitializeSecurityContext(&cred_handle,
                                                                       NULL,
                                                                       target_proxy_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       NULL,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }else
                                {
                                    input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                    input_sec_buffer[0].pvBuffer = buffer;
                                    input_sec_buffer[0].cbBuffer = rec;
                                    input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[1].pvBuffer = NULL;
                                    input_sec_buffer[1].cbBuffer = 0;
                                    input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[2].pvBuffer = NULL;
                                    input_sec_buffer[2].cbBuffer = 0;
                                    input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[3].pvBuffer = NULL;
                                    input_sec_buffer[3].cbBuffer = 0;
                                    input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                    input_sec_buffer_desc.cBuffers = 4;
                                    input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                    status = InitializeSecurityContext(&cred_handle,
                                                                       &ctxt_handle,
                                                                       target_proxy_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       &input_sec_buffer_desc,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }

                                if(status == SEC_E_OK)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_CONTINUE_NEEDED)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_I_COMPLETE_NEEDED)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] CompleteAuthToken error: %x\n",
                                                    status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        printf("[-] CompleteAuthToken error: %x\n",
                                               status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                                {
                                    tmprec = pipe->recv_data(tmp,
                                                             NODE_BUFFER_SIZE,
                                                             tv_sec,
                                                             tv_usec);
                                    if(tmprec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }

                                    if(rec + tmprec <= NODE_BUFFER_SIZE)
                                    {
                                        std::memcpy(buffer + rec,
                                                    tmp,
                                                    tmprec);
                                        rec += tmprec;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                    rec + tmprec);
#endif
                                        error_flag = true;
                                        break;
                                    }
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] InitializeSecurityContext error:%x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                            }

                            free_all_buffers(&output_sec_buffer_desc);
                            free(buffer);
                            free(tmp);

                            if(error_flag == true)
                            {
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            status = QueryContextAttributes(&ctxt_handle,
                                                            SECPKG_ATTR_STREAM_SIZES,
                                                            &stream_sizes);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] QueryContextAttributes error:%x\n",
                                            status);
#endif
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            pipe->set_ctxt_handle(&ctxt_handle);
                            pipe->set_stream_sizes(stream_sizes);
                        }


                        // forward proxy
                        if(forward_proxy_authentication == 0)   // no
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);

                            ret = forward_proxy_authentication_no(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_no error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 1) // basic
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_basic(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_basic error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 2) // digest
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_digest(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_digest error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 3) // ntlmv2
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);
                            pipe->set_forward_proxy_nthash_hexstring(forward_proxy_nthash_hexstring);
                            pipe->set_forward_proxy_domainname(forward_proxy_domainname);
                            pipe->set_forward_proxy_workstationname(forward_proxy_workstationname);

                            ret = forward_proxy_authentication_ntlmv2(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_ntlmv2 error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 4) // spnego(kerberos)
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_service_principal_name(forward_proxy_service_principal_name);

                            ret = forward_proxy_authentication_spnego_kerberos(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_spnego_kerberos error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] Not implemented\n");
#endif
                            pipe->set_ctxt_handle(NULL);
                            pipe->set_stream_sizes({0});

                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }
                    }


                    // http connection
                    ret = pipe->do_http_connection_client();
                    if(ret < 0)
                    {
                        error_count++;
                    }else
                    {
                        error_count = 0;    // reset
                    }

                    if(tls_flag == true)
                    {
                        pipe->set_ctxt_handle(NULL);
                        pipe->set_stream_sizes({0});

                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);
                    }

                    pipe->set_sock(-1);
                    closesocket(pipe_sock);

                    if(error_count >= 10)
                    {
                        std::printf("[-] connect_pipe_http error\n");
                        std::printf("[-] close pipe client: %u\n",
                                    pipe_key);
                        pipe_manager->erase_pipe(pipe_key);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
                }
            }else if(family == AF_INET6)    // IPv6
            {
                while(1)
                {
                    pipe_sock = socket(AF_INET6,
                                       SOCK_STREAM,
                                       0);
                    if(pipe_sock == INVALID_SOCKET)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] socket error: %d\n",
                                    WSAGetLastError());
#endif
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

                    inet_ntop(AF_INET6,
                              &forward_proxy_addr6.sin6_addr,
                              forward_proxy_addr6_string_pointer,
                              INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
                    if(forward_proxy_addr6.sin6_scope_id > 0)
                    {
                        std::printf("[+] connecting to ip:%s%%%d port:%d\n",
                                    forward_proxy_addr6_string_pointer,
                                    forward_proxy_addr6.sin6_scope_id,
                                    ntohs(forward_proxy_addr6.sin6_port));
                    }else
                    {
                        std::printf("[+] connecting to ip:%s port:%d\n",
                                    forward_proxy_addr6_string_pointer,
                                    ntohs(forward_proxy_addr6.sin6_port));
                    }
#endif

                    // connect
                    ret = connect(pipe_sock,
                                  (struct sockaddr *)&forward_proxy_addr6,
                                  sizeof(forward_proxy_addr6));
                    if(ret != 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] connect failed: %d\n",
                                    WSAGetLastError());
#endif
                        closesocket(pipe_sock);
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

                    inet_ntop(AF_INET6,
                              &forward_proxy_addr6.sin6_addr,
                              forward_proxy_addr6_string_pointer,
                              INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
                    if(forward_proxy_addr6.sin6_scope_id > 0)
                    {
                        std::printf("[+] connected to ip:%s%%%d port:%d\n",
                                    forward_proxy_addr6_string_pointer,
                                    forward_proxy_addr6.sin6_scope_id,
                                    ntohs(forward_proxy_addr6.sin6_port));
                    }else
                    {
                        std::printf("[+] connected to ip:%s port:%d\n",
                                    forward_proxy_addr6_string_pointer,
                                    ntohs(forward_proxy_addr6.sin6_port));
                    }
#endif

                    forward_proxy_ip_scope_id = std::to_string(forward_proxy_addr6.sin6_scope_id);

                    if(pipe == nullptr)
                    {
                        pipe = std::make_shared<Pipe>(spider_ip,
                                                      0,
                                                      mode,
                                                      message_mode,
                                                      pipe_ip,
                                                      pipe_ip_scope_id,
                                                      pipe_destination_ip,
                                                      pipe_destination_ip_scope_id,
                                                      pipe_destination_port,
                                                      pipe_sock,
                                                      routing_manager,
                                                      message_manager);

                        do
                        {
                            pipe_key = generate_random_id();
                            ret = pipe_manager->insert_pipe(pipe_key,
                                                            pipe);
                        }while(ret != 0);
                    }else
                    {
                        pipe->set_sock(pipe_sock);
                    }


                    SCHANNEL_CRED schannel_cred;
                    CredHandle cred_handle;
                    CtxtHandle ctxt_handle;
                    SecPkgContext_StreamSizes stream_sizes;
                    SECURITY_STATUS status;
                    unsigned long f_context_req;
                    unsigned long f_context_attr;
                    TimeStamp ts_expiry;
                    SecBufferDesc input_sec_buffer_desc;
                    SecBufferDesc output_sec_buffer_desc;
                    SecBuffer input_sec_buffer[4];
                    SecBuffer output_sec_buffer[3];
                    BOOL init_flag = true;
                    BOOL error_flag = false;
                    char *buffer = NULL;
                    char *tmp = NULL;
                    int32_t rec = 0;
                    int32_t tmprec = 0;
                    int32_t sen = 0;
                    unsigned long tv_sec = 30;
                    unsigned long tv_usec = 0;


                    if(forward_proxy_type == "h" ||
                       forward_proxy_type == "hs")
                    {
                        // forward proxy
                        if(forward_proxy_authentication == 0)   // no
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);

                            ret = forward_proxy_authentication_no(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_no error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 1) // basic
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_basic(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_basic error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }


                        }else if(forward_proxy_authentication == 2) // digest
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_digest(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_digest error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 3) // ntlmv2
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);
                            pipe->set_forward_proxy_nthash_hexstring(forward_proxy_nthash_hexstring);
                            pipe->set_forward_proxy_domainname(forward_proxy_domainname);
                            pipe->set_forward_proxy_workstationname(forward_proxy_workstationname);

                            ret = forward_proxy_authentication_ntlmv2(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_ntlmv2 error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 4) // spnego(kerberos)
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_service_principal_name(forward_proxy_service_principal_name);

                            ret = forward_proxy_authentication_spnego_kerberos(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_spnego_kerberos error\n");
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] Not implemented\n");
#endif
                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }


                        // TLS
                        if(tls_flag == true)
                        {
                            std::memset(&schannel_cred,
                                        0,
                                        sizeof(schannel_cred));
                            schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                            schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                            schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                            status = AcquireCredentialsHandleA(NULL,
                                                               const_cast<LPSTR>(UNISP_NAME_A),
                                                               SECPKG_CRED_OUTBOUND,
                                                               NULL,
                                                               &schannel_cred,
                                                               NULL,
                                                               NULL,
                                                               &cred_handle,
                                                               NULL);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                            status);
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            std::memset(&ctxt_handle,
                                        0,
                                        sizeof(ctxt_handle));

                            f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                            output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                            output_sec_buffer[0].pvBuffer = NULL;
                            output_sec_buffer[0].cbBuffer = 0;
                            output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                            output_sec_buffer[1].pvBuffer = NULL;
                            output_sec_buffer[1].cbBuffer = 0;
                            output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                            output_sec_buffer[2].pvBuffer = NULL;
                            output_sec_buffer[2].cbBuffer = 0;
                            output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                            output_sec_buffer_desc.cBuffers = 3;
                            output_sec_buffer_desc.pBuffers = output_sec_buffer;

                            buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));
                            tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                                 sizeof(char));

                            while(1)
                            {
                                if(init_flag == true)
                                {
                                    init_flag = false;
                                    status = InitializeSecurityContext(&cred_handle,
                                                                       NULL,
                                                                       target_server_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       NULL,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }else
                                {
                                    input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                    input_sec_buffer[0].pvBuffer = buffer;
                                    input_sec_buffer[0].cbBuffer = rec;
                                    input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[1].pvBuffer = NULL;
                                    input_sec_buffer[1].cbBuffer = 0;
                                    input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[2].pvBuffer = NULL;
                                    input_sec_buffer[2].cbBuffer = 0;
                                    input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[3].pvBuffer = NULL;
                                    input_sec_buffer[3].cbBuffer = 0;
                                    input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                    input_sec_buffer_desc.cBuffers = 4;
                                    input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                    status = InitializeSecurityContext(&cred_handle,
                                                                       &ctxt_handle,
                                                                       target_server_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       &input_sec_buffer_desc,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }

                                if(status == SEC_E_OK)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_CONTINUE_NEEDED)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_I_COMPLETE_NEEDED)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] CompleteAuthToken error: %x\n",
                                                    status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        printf("[-] CompleteAuthToken error: %x\n",
                                               status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                                {
                                    tmprec = pipe->recv_data(tmp,
                                                             NODE_BUFFER_SIZE,
                                                             tv_sec,
                                                             tv_usec);
                                    if(tmprec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }

                                    if(rec + tmprec <= NODE_BUFFER_SIZE)
                                    {
                                        std::memcpy(buffer + rec,
                                                    tmp,
                                                    tmprec);
                                        rec += tmprec;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                    rec + tmprec);
#endif
                                        error_flag = true;
                                        break;
                                    }
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] InitializeSecurityContext error:%x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                            }

                            free_all_buffers(&output_sec_buffer_desc);
                            free(buffer);
                            free(tmp);

                            if(error_flag == true)
                            {
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            status = QueryContextAttributes(&ctxt_handle,
                                                            SECPKG_ATTR_STREAM_SIZES,
                                                            &stream_sizes);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] QueryContextAttributes error:%x\n",
                                            status);
#endif
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            pipe->set_ctxt_handle(&ctxt_handle);
                            pipe->set_stream_sizes(stream_sizes);
                        }
                    }else if(forward_proxy_type == "s")
                    {
                        // TLS
                        if(tls_flag == true)
                        {
                            std::memset(&schannel_cred,
                                        0,
                                        sizeof(schannel_cred));
                            schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                            schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                            schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                            status = AcquireCredentialsHandleA(NULL,
                                                               const_cast<LPSTR>(UNISP_NAME_A),
                                                               SECPKG_CRED_OUTBOUND,
                                                               NULL,
                                                               &schannel_cred,
                                                               NULL,
                                                               NULL,
                                                               &cred_handle,
                                                               NULL);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                            status);
#endif
                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            std::memset(&ctxt_handle,
                                        0,
                                        sizeof(ctxt_handle));

                            f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                            output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                            output_sec_buffer[0].pvBuffer = NULL;
                            output_sec_buffer[0].cbBuffer = 0;
                            output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                            output_sec_buffer[1].pvBuffer = NULL;
                            output_sec_buffer[1].cbBuffer = 0;
                            output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                            output_sec_buffer[2].pvBuffer = NULL;
                            output_sec_buffer[2].cbBuffer = 0;
                            output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                            output_sec_buffer_desc.cBuffers = 3;
                            output_sec_buffer_desc.pBuffers = output_sec_buffer;

                            buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                    sizeof(char));
                            tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                                 sizeof(char));

                            while(1)
                            {
                                if(init_flag == true)
                                {
                                    init_flag = false;
                                    status = InitializeSecurityContext(&cred_handle,
                                                                       NULL,
                                                                       target_proxy_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       NULL,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }else
                                {
                                    input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                    input_sec_buffer[0].pvBuffer = buffer;
                                    input_sec_buffer[0].cbBuffer = rec;
                                    input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[1].pvBuffer = NULL;
                                    input_sec_buffer[1].cbBuffer = 0;
                                    input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[2].pvBuffer = NULL;
                                    input_sec_buffer[2].cbBuffer = 0;
                                    input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                    input_sec_buffer[3].pvBuffer = NULL;
                                    input_sec_buffer[3].cbBuffer = 0;
                                    input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                    input_sec_buffer_desc.cBuffers = 4;
                                    input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                    status = InitializeSecurityContext(&cred_handle,
                                                                       &ctxt_handle,
                                                                       target_proxy_name,
                                                                       f_context_req,
                                                                       0,
                                                                       0,
                                                                       &input_sec_buffer_desc,
                                                                       0,
                                                                       &ctxt_handle,
                                                                       &output_sec_buffer_desc,
                                                                       &f_context_attr,
                                                                       &ts_expiry);
                                }

                                if(status == SEC_E_OK)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_CONTINUE_NEEDED)
                                {
                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_I_COMPLETE_NEEDED)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] CompleteAuthToken error: %x\n",
                                                    status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    break;
                                }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                                {
                                    status = CompleteAuthToken(&ctxt_handle,
                                                               &output_sec_buffer_desc);
                                    if(status != SEC_E_OK)
                                    {
#ifdef DEBUGPRINT
                                        printf("[-] CompleteAuthToken error: %x\n",
                                               status);
#endif
                                        error_flag = true;
                                        break;
                                    }

                                    if(output_sec_buffer[0].cbBuffer > 0)
                                    {
                                        sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                              output_sec_buffer[0].cbBuffer,
                                                              tv_sec,
                                                              tv_usec);
                                        if(sen <= 0)
                                        {
                                            error_flag = true;
                                            break;
                                        }
                                    }

                                    free_all_buffers(&output_sec_buffer_desc);

                                    rec = pipe->recv_data(buffer,
                                                          NODE_BUFFER_SIZE,
                                                          tv_sec,
                                                          tv_usec);
                                    if(rec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                                {
                                    tmprec = pipe->recv_data(tmp,
                                                             NODE_BUFFER_SIZE,
                                                             tv_sec,
                                                             tv_usec);
                                    if(tmprec <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }

                                    if(rec + tmprec <= NODE_BUFFER_SIZE)
                                    {
                                        std::memcpy(buffer + rec,
                                                    tmp,
                                                    tmprec);
                                        rec += tmprec;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                    rec + tmprec);
#endif
                                        error_flag = true;
                                        break;
                                    }
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] InitializeSecurityContext error:%x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                            }

                            free_all_buffers(&output_sec_buffer_desc);
                            free(buffer);
                            free(tmp);

                            if(error_flag == true)
                            {
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            status = QueryContextAttributes(&ctxt_handle,
                                                            SECPKG_ATTR_STREAM_SIZES,
                                                            &stream_sizes);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] QueryContextAttributes error:%x\n",
                                            status);
#endif
                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }

                            pipe->set_ctxt_handle(&ctxt_handle);
                            pipe->set_stream_sizes(stream_sizes);
                        }


                        // forward proxy
                        if(forward_proxy_authentication == 0)   // no
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);

                            ret = forward_proxy_authentication_no(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_no error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 1) // basic
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_basic(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_basic error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }


                        }else if(forward_proxy_authentication == 2) // digest
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);

                            ret = forward_proxy_authentication_digest(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_digest error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 3) // ntlmv2
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_username(forward_proxy_username);
                            pipe->set_forward_proxy_password(forward_proxy_password);
                            pipe->set_forward_proxy_nthash_hexstring(forward_proxy_nthash_hexstring);
                            pipe->set_forward_proxy_domainname(forward_proxy_domainname);
                            pipe->set_forward_proxy_workstationname(forward_proxy_workstationname);

                            ret = forward_proxy_authentication_ntlmv2(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_ntlmv2 error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else if(forward_proxy_authentication == 4) // spnego(kerberos)
                        {
                            pipe->set_forward_proxy_type(forward_proxy_type);
                            pipe->set_forward_proxy_authentication(forward_proxy_authentication);
                            pipe->set_forward_proxy_ip(forward_proxy_ip);
                            pipe->set_forward_proxy_ip_scope_id(forward_proxy_ip_scope_id);
                            pipe->set_forward_proxy_port(forward_proxy_port);
                            pipe->set_forward_proxy_service_principal_name(forward_proxy_service_principal_name);

                            ret = forward_proxy_authentication_spnego_kerberos(pipe);
                            if(ret < 0)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] forward_proxy_authentication_spnego_kerberos error\n");
#endif
                                pipe->set_ctxt_handle(NULL);
                                pipe->set_stream_sizes({0});

                                DeleteSecurityContext(&ctxt_handle);
                                FreeCredentialsHandle(&cred_handle);

                                pipe->set_sock(-1);
                                closesocket(pipe_sock);
                                pipe_manager->erase_pipe(pipe_key);
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] Not implemented\n");
#endif
                            pipe->set_ctxt_handle(NULL);
                            pipe->set_stream_sizes({0});

                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }
                    }


                    // http connection
                    ret = pipe->do_http_connection_client();
                    if(ret < 0)
                    {
                        error_count++;
                    }else
                    {
                        error_count = 0;    // reset
                    }

                    if(tls_flag == true)
                    {
                        pipe->set_ctxt_handle(NULL);
                        pipe->set_stream_sizes({0});

                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);
                    }

                    pipe->set_sock(-1);
                    closesocket(pipe_sock);

                    if(error_count >= 10)
                    {
                        std::printf("[-] connect_pipe_http error\n");
                        std::printf("[-] close pipe client: %u\n",
                                    pipe_key);
                        pipe_manager->erase_pipe(pipe_key);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
                }
            }else
            {
                std::printf("[-] not implemented\n");
                return -1;
            }
        }else
        {
            if(pipe_destination_ip.find(":") == std::string::npos)  // ipv4 address
            {
                hints.ai_family = AF_INET;
                ret = getaddrinfo(pipe_destination_ip.c_str(),
                                  pipe_destination_port.c_str(),
                                  &hints,
                                  &pipe_dest_addr_info);
                if(ret != 0)
                {
                    std::printf("[-] cannot resolv the address: %s\n",
                                pipe_destination_ip.c_str());
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
                    std::printf("[-] cannot resolv the address: %s\n",
                                pipe_destination_ip.c_str());
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
                    pipe_dest_addr6.sin6_scope_id = (uint32_t)std::stoi(pipe_ip_scope_id);
                }

                freeaddrinfo(pipe_dest_addr_info);
            }else
            {
                std::printf("[-] not implemented\n");
                freeaddrinfo(pipe_dest_addr_info);
                return -1;
            }

            if(family == AF_INET)   // IPv4
            {
                while(1)
                {
                    pipe_sock = socket(AF_INET,
                                       SOCK_STREAM,
                                       0);
                    if(pipe_sock == INVALID_SOCKET)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] socket error: %d\n",
                                    WSAGetLastError());
#endif
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] connecting to ip:%s port:%d\n",
                                inet_ntoa(pipe_dest_addr.sin_addr),
                                ntohs(pipe_dest_addr.sin_port));
#endif

                    // connect
                    ret = connect(pipe_sock,
                                  (struct sockaddr *)&pipe_dest_addr,
                                  sizeof(pipe_dest_addr));
                    if(ret < 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] connect failed:%d\n",
                                    ret);
#endif
                        closesocket(pipe_sock);
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] connected to ip:%s port:%d\n",
                                inet_ntoa(pipe_dest_addr.sin_addr),
                                ntohs(pipe_dest_addr.sin_port));
#endif

                    if(pipe == nullptr)
                    {
                        pipe = std::make_shared<Pipe>(spider_ip,
                                                      0,
                                                      mode,
                                                      message_mode,
                                                      pipe_ip,
                                                      pipe_ip_scope_id,
                                                      pipe_destination_ip,
                                                      pipe_destination_ip_scope_id,
                                                      pipe_destination_port,
                                                      pipe_sock,
                                                      routing_manager,
                                                      message_manager);

                        do
                        {
                            pipe_key = generate_random_id();
                            ret = pipe_manager->insert_pipe(pipe_key,
                                                            pipe);
                        }while(ret != 0);
                    }else
                    {
                        pipe->set_sock(pipe_sock);
                    }


                    // TLS
                    SCHANNEL_CRED schannel_cred;
                    CredHandle cred_handle;
                    CtxtHandle ctxt_handle;
                    SecPkgContext_StreamSizes stream_sizes;
                    SECURITY_STATUS status;
                    unsigned long f_context_req;
                    unsigned long f_context_attr;
                    TimeStamp ts_expiry;
                    SecBufferDesc input_sec_buffer_desc;
                    SecBufferDesc output_sec_buffer_desc;
                    SecBuffer input_sec_buffer[4];
                    SecBuffer output_sec_buffer[3];
                    BOOL init_flag = true;
                    BOOL error_flag = false;
                    char *buffer = NULL;
                    char *tmp = NULL;
                    int32_t rec = 0;
                    int32_t tmprec = 0;
                    int32_t sen = 0;
                    unsigned long tv_sec = 30;
                    unsigned long tv_usec = 0;

                    if(tls_flag == true)
                    {
                        std::memset(&schannel_cred,
                                    0,
                                    sizeof(schannel_cred));
                        schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                        schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                        schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                        status = AcquireCredentialsHandleA(NULL,
                                                           const_cast<LPSTR>(UNISP_NAME_A),
                                                           SECPKG_CRED_OUTBOUND,
                                                           NULL,
                                                           &schannel_cred,
                                                           NULL,
                                                           NULL,
                                                           &cred_handle,
                                                           NULL);
                        if(status != SEC_E_OK)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                        status);
#endif
                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        std::memset(&ctxt_handle,
                                    0,
                                    sizeof(ctxt_handle));

                        f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                        output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                        output_sec_buffer[0].pvBuffer = NULL;
                        output_sec_buffer[0].cbBuffer = 0;
                        output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                        output_sec_buffer[1].pvBuffer = NULL;
                        output_sec_buffer[1].cbBuffer = 0;
                        output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                        output_sec_buffer[2].pvBuffer = NULL;
                        output_sec_buffer[2].cbBuffer = 0;
                        output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                        output_sec_buffer_desc.cBuffers = 3;
                        output_sec_buffer_desc.pBuffers = output_sec_buffer;

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                        tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                             sizeof(char));

                        while(1)
                        {
                            if(init_flag == true)
                            {
                                init_flag = false;
                                status = InitializeSecurityContext(&cred_handle,
                                                                   NULL,
                                                                   target_server_name,
                                                                   f_context_req,
                                                                   0,
                                                                   0,
                                                                   NULL,
                                                                   0,
                                                                   &ctxt_handle,
                                                                   &output_sec_buffer_desc,
                                                                   &f_context_attr,
                                                                   &ts_expiry);
                            }else
                            {
                                input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                input_sec_buffer[0].pvBuffer = buffer;
                                input_sec_buffer[0].cbBuffer = rec;
                                input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[1].pvBuffer = NULL;
                                input_sec_buffer[1].cbBuffer = 0;
                                input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[2].pvBuffer = NULL;
                                input_sec_buffer[2].cbBuffer = 0;
                                input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[3].pvBuffer = NULL;
                                input_sec_buffer[3].cbBuffer = 0;
                                input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                input_sec_buffer_desc.cBuffers = 4;
                                input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                status = InitializeSecurityContext(&cred_handle,
                                                                   &ctxt_handle,
                                                                   target_server_name,
                                                                   f_context_req,
                                                                   0,
                                                                   0,
                                                                   &input_sec_buffer_desc,
                                                                   0,
                                                                   &ctxt_handle,
                                                                   &output_sec_buffer_desc,
                                                                   &f_context_attr,
                                                                   &ts_expiry);
                            }

                            if(status == SEC_E_OK)
                            {
                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                break;
                            }else if(status == SEC_I_CONTINUE_NEEDED)
                            {
                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                free_all_buffers(&output_sec_buffer_desc);

                                rec = pipe->recv_data(buffer,
                                                      NODE_BUFFER_SIZE,
                                                      tv_sec,
                                                      tv_usec);
                                if(rec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }else if(status == SEC_I_COMPLETE_NEEDED)
                            {
                                status = CompleteAuthToken(&ctxt_handle,
                                                           &output_sec_buffer_desc);
                                if(status != SEC_E_OK)
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] CompleteAuthToken error: %x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                break;
                            }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                            {
                                status = CompleteAuthToken(&ctxt_handle,
                                                           &output_sec_buffer_desc);
                                if(status != SEC_E_OK)
                                {
#ifdef DEBUGPRINT
                                    printf("[-] CompleteAuthToken error: %x\n",
                                           status);
#endif
                                    error_flag = true;
                                    break;
                                }

                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                free_all_buffers(&output_sec_buffer_desc);

                                rec = pipe->recv_data(buffer,
                                                      NODE_BUFFER_SIZE,
                                                      tv_sec,
                                                      tv_usec);
                                if(rec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                            {
                                tmprec = pipe->recv_data(tmp,
                                                         NODE_BUFFER_SIZE,
                                                         tv_sec,
                                                         tv_usec);
                                if(tmprec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }

                                if(rec + tmprec <= NODE_BUFFER_SIZE)
                                {
                                    std::memcpy(buffer + rec,
                                                tmp,
                                                tmprec);
                                    rec += tmprec;
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                rec + tmprec);
#endif
                                    error_flag = true;
                                    break;
                                }
                            }else
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] InitializeSecurityContext error:%x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                        }

                        free_all_buffers(&output_sec_buffer_desc);
                        free(buffer);
                        free(tmp);

                        if(error_flag == true)
                        {
                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        status = QueryContextAttributes(&ctxt_handle,
                                                        SECPKG_ATTR_STREAM_SIZES,
                                                        &stream_sizes);
                        if(status != SEC_E_OK)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] QueryContextAttributes error:%x\n",
                                        status);
#endif
                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        pipe->set_ctxt_handle(&ctxt_handle);
                        pipe->set_stream_sizes(stream_sizes);
                    }


                    // http connection
                    ret = pipe->do_http_connection_client();
                    if(ret < 0)
                    {
                        error_count++;
                    }else
                    {
                        error_count = 0;    // reset
                    }

                    if(tls_flag == true)
                    {
                        pipe->set_ctxt_handle(NULL);
                        pipe->set_stream_sizes({0});

                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);
                    }

                    pipe->set_sock(-1);
                    closesocket(pipe_sock);

                    if(error_count >= 10)
                    {
                        std::printf("[-] connect_pipe_http error\n");
                        std::printf("[-] close pipe client: %u\n",
                                    pipe_key);
                        pipe_manager->erase_pipe(pipe_key);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
                }
            }else if(family == AF_INET6)    // IPv6
            {
                while(1)
                {
                    pipe_sock = socket(AF_INET6,
                                       SOCK_STREAM,
                                       0);
                    if(pipe_sock == INVALID_SOCKET)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] socket error: %d\n",
                                    WSAGetLastError());
#endif
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

                    inet_ntop(AF_INET6,
                              &pipe_dest_addr6.sin6_addr,
                              pipe_dest_addr6_string_pointer,
                              INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
                    if(pipe_dest_addr6.sin6_scope_id > 0)
                    {
                        std::printf("[+] connecting to ip:%s%%%d port:%d\n",
                                    pipe_dest_addr6_string_pointer,
                                    pipe_dest_addr6.sin6_scope_id,
                                    ntohs(pipe_dest_addr6.sin6_port));
                    }else
                    {
                        std::printf("[+] connecting to ip:%s port:%d\n",
                                    pipe_dest_addr6_string_pointer,
                                    ntohs(pipe_dest_addr6.sin6_port));
                    }
#endif

                    // connect
                    ret = connect(pipe_sock,
                                  (struct sockaddr *)&pipe_dest_addr6,
                                  sizeof(pipe_dest_addr6));
                    if(ret != 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] connect failed: %d\n",
                                    WSAGetLastError());
#endif
                        closesocket(pipe_sock);
                        if(pipe != nullptr)
                        {
                            pipe_manager->erase_pipe(pipe_key);
                        }
                        break;
                    }

                    inet_ntop(AF_INET6,
                              &pipe_dest_addr6.sin6_addr,
                              pipe_dest_addr6_string_pointer,
                              INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
                    if(pipe_dest_addr6.sin6_scope_id > 0)
                    {
                        std::printf("[+] connected to ip:%s%%%d port:%d\n",
                                    pipe_dest_addr6_string_pointer,
                                    pipe_dest_addr6.sin6_scope_id,
                                    ntohs(pipe_dest_addr6.sin6_port));
                    }else
                    {
                        std::printf("[+] connected to ip:%s port:%d\n",
                                    pipe_dest_addr6_string_pointer,
                                    ntohs(pipe_dest_addr6.sin6_port));
                    }
#endif

                    pipe_destination_ip_scope_id = std::to_string(pipe_dest_addr6.sin6_scope_id);

                    if(pipe == nullptr)
                    {
                        pipe = std::make_shared<Pipe>(spider_ip,
                                                      0,
                                                      mode,
                                                      message_mode,
                                                      pipe_ip,
                                                      pipe_ip_scope_id,
                                                      pipe_destination_ip,
                                                      pipe_destination_ip_scope_id,
                                                      pipe_destination_port,
                                                      pipe_sock,
                                                      routing_manager,
                                                      message_manager);

                        do
                        {
                            pipe_key = generate_random_id();
                            ret = pipe_manager->insert_pipe(pipe_key,
                                                            pipe);
                        }while(ret != 0);
                    }else
                    {
                        pipe->set_sock(pipe_sock);
                    }


                    // TLS
                    SCHANNEL_CRED schannel_cred;
                    CredHandle cred_handle;
                    CtxtHandle ctxt_handle;
                    SecPkgContext_StreamSizes stream_sizes;
                    SECURITY_STATUS status;
                    unsigned long f_context_req;
                    unsigned long f_context_attr;
                    TimeStamp ts_expiry;
                    SecBufferDesc input_sec_buffer_desc;
                    SecBufferDesc output_sec_buffer_desc;
                    SecBuffer input_sec_buffer[4];
                    SecBuffer output_sec_buffer[3];
                    BOOL init_flag = true;
                    BOOL error_flag = false;
                    char *buffer = NULL;
                    char *tmp = NULL;
                    int32_t rec = 0;
                    int32_t tmprec = 0;
                    int32_t sen = 0;
                    unsigned long tv_sec = 30;
                    unsigned long tv_usec = 0;


                    if(tls_flag == true)
                    {
                        std::memset(&schannel_cred,
                                    0,
                                    sizeof(schannel_cred));
                        schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                        schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
                        schannel_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;

                        status = AcquireCredentialsHandleA(NULL,
                                                           const_cast<LPSTR>(UNISP_NAME_A),
                                                           SECPKG_CRED_OUTBOUND,
                                                           NULL,
                                                           &schannel_cred,
                                                           NULL,
                                                           NULL,
                                                           &cred_handle,
                                                           NULL);
                        if(status != SEC_E_OK)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                        status);
#endif
                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        std::memset(&ctxt_handle,
                                    0,
                                    sizeof(ctxt_handle));

                        f_context_req = ISC_REQ_ALLOCATE_MEMORY;

                        output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                        output_sec_buffer[0].pvBuffer = NULL;
                        output_sec_buffer[0].cbBuffer = 0;
                        output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                        output_sec_buffer[1].pvBuffer = NULL;
                        output_sec_buffer[1].cbBuffer = 0;
                        output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                        output_sec_buffer[2].pvBuffer = NULL;
                        output_sec_buffer[2].cbBuffer = 0;
                        output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                        output_sec_buffer_desc.cBuffers = 3;
                        output_sec_buffer_desc.pBuffers = output_sec_buffer;

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                        tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                             sizeof(char));

                        while(1)
                        {
                            if(init_flag == true)
                            {
                                init_flag = false;
                                status = InitializeSecurityContext(&cred_handle,
                                                                   NULL,
                                                                   target_server_name,
                                                                   f_context_req,
                                                                   0,
                                                                   0,
                                                                   NULL,
                                                                   0,
                                                                   &ctxt_handle,
                                                                   &output_sec_buffer_desc,
                                                                   &f_context_attr,
                                                                   &ts_expiry);
                            }else
                            {
                                input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                                input_sec_buffer[0].pvBuffer = buffer;
                                input_sec_buffer[0].cbBuffer = rec;
                                input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[1].pvBuffer = NULL;
                                input_sec_buffer[1].cbBuffer = 0;
                                input_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[2].pvBuffer = NULL;
                                input_sec_buffer[2].cbBuffer = 0;
                                input_sec_buffer[3].BufferType = SECBUFFER_EMPTY;
                                input_sec_buffer[3].pvBuffer = NULL;
                                input_sec_buffer[3].cbBuffer = 0;
                                input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                                input_sec_buffer_desc.cBuffers = 4;
                                input_sec_buffer_desc.pBuffers = input_sec_buffer;

                                status = InitializeSecurityContext(&cred_handle,
                                                                   &ctxt_handle,
                                                                   target_server_name,
                                                                   f_context_req,
                                                                   0,
                                                                   0,
                                                                   &input_sec_buffer_desc,
                                                                   0,
                                                                   &ctxt_handle,
                                                                   &output_sec_buffer_desc,
                                                                   &f_context_attr,
                                                                   &ts_expiry);
                            }

                            if(status == SEC_E_OK)
                            {
                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                break;
                            }else if(status == SEC_I_CONTINUE_NEEDED)
                            {
                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                free_all_buffers(&output_sec_buffer_desc);

                                rec = pipe->recv_data(buffer,
                                                      NODE_BUFFER_SIZE,
                                                      tv_sec,
                                                      tv_usec);
                                if(rec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }else if(status == SEC_I_COMPLETE_NEEDED)
                            {
                                status = CompleteAuthToken(&ctxt_handle,
                                                           &output_sec_buffer_desc);
                                if(status != SEC_E_OK)
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] CompleteAuthToken error: %x\n",
                                                status);
#endif
                                    error_flag = true;
                                    break;
                                }

                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                break;
                            }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                            {
                                status = CompleteAuthToken(&ctxt_handle,
                                                           &output_sec_buffer_desc);
                                if(status != SEC_E_OK)
                                {
#ifdef DEBUGPRINT
                                    printf("[-] CompleteAuthToken error: %x\n",
                                           status);
#endif
                                    error_flag = true;
                                    break;
                                }

                                if(output_sec_buffer[0].cbBuffer > 0)
                                {
                                    sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                          output_sec_buffer[0].cbBuffer,
                                                          tv_sec,
                                                          tv_usec);
                                    if(sen <= 0)
                                    {
                                        error_flag = true;
                                        break;
                                    }
                                }

                                free_all_buffers(&output_sec_buffer_desc);

                                rec = pipe->recv_data(buffer,
                                                      NODE_BUFFER_SIZE,
                                                      tv_sec,
                                                      tv_usec);
                                if(rec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                            {
                                tmprec = pipe->recv_data(tmp,
                                                         NODE_BUFFER_SIZE,
                                                         tv_sec,
                                                         tv_usec);
                                if(tmprec <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }

                                if(rec + tmprec <= NODE_BUFFER_SIZE)
                                {
                                    std::memcpy(buffer + rec,
                                                tmp,
                                                tmprec);
                                    rec += tmprec;
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                                rec + tmprec);
#endif
                                    error_flag = true;
                                    break;
                                }
                            }else
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] InitializeSecurityContext error:%x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                        }

                        free_all_buffers(&output_sec_buffer_desc);
                        free(buffer);
                        free(tmp);

                        if(error_flag == true)
                        {
                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        status = QueryContextAttributes(&ctxt_handle,
                                                        SECPKG_ATTR_STREAM_SIZES,
                                                        &stream_sizes);
                        if(status != SEC_E_OK)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] QueryContextAttributes error:%x\n",
                                        status);
#endif
                            DeleteSecurityContext(&ctxt_handle);
                            FreeCredentialsHandle(&cred_handle);

                            pipe->set_sock(-1);
                            closesocket(pipe_sock);
                            pipe_manager->erase_pipe(pipe_key);
                            break;
                        }

                        pipe->set_ctxt_handle(&ctxt_handle);
                        pipe->set_stream_sizes(stream_sizes);
                    }


                    // http connection
                    ret = pipe->do_http_connection_client();
                    if(ret < 0)
                    {
                        error_count++;
                    }else
                    {
                        error_count = 0;    // reset
                    }

                    if(tls_flag == true)
                    {
                        pipe->set_ctxt_handle(NULL);
                        pipe->set_stream_sizes({0});

                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);
                    }

                    pipe->set_sock(-1);
                    closesocket(pipe_sock);

                    if(error_count >= 10)
                    {
                        std::printf("[-] connect_pipe_http error\n");
                        std::printf("[-] close pipe client: %u\n",
                                    pipe_key);
                        pipe_manager->erase_pipe(pipe_key);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
                }
            }else
            {
                std::printf("[-] not implemented\n");
                return -1;
            }
        }

        return 0;
    }

    int Spidercommand::listen_pipe_http(char mode,
                                        BOOL tls_flag,
                                        std::string message_mode,
                                        std::string pipe_listen_ip,
                                        std::string pipe_listen_ip_scope_id,
                                        std::string pipe_listen_port)
    {
        int ret = 0;
        BOOL ret_b = false;
        uint32_t pipe_id = 0;
        struct sockaddr_in pipe_listen_addr, pipe_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 pipe_listen_addr6, pipe_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *pipe_listen_addr_info;
        SOCKET pipe_listen_sock = INVALID_SOCKET;
        SOCKET pipe_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int pipe_addr_length = sizeof(pipe_addr);
        int pipe_addr6_length = sizeof(pipe_addr6);
        char pipe_listen_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *pipe_listen_addr6_string_pointer = pipe_listen_addr6_string;
        char pipe_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *pipe_addr6_string_pointer = pipe_addr6_string;
        std::shared_ptr<Pipe> pipe_listen;
        uint32_t pipe_listen_key = 0;
        uint32_t pipe_key = 0;
        std::string pipe_destination_ip;
        std::string pipe_destination_ip_scope_id;
        std::string pipe_destination_port;
        std::shared_ptr<Pipe> pipe = nullptr;


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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            pipe_listen_ip.c_str());
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
                pipe_listen_addr6.sin6_scope_id = (uint32_t)std::stoi(pipe_listen_ip_scope_id);
            }

            freeaddrinfo(pipe_listen_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(pipe_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            pipe_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            if(pipe_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr,
                       sizeof(pipe_listen_addr));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            // listen
            ret = listen(pipe_listen_sock,
                         5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            std::printf("[+] listening port %d on %s\n",
                        ntohs(pipe_listen_addr.sin_port),
                        inet_ntoa(pipe_listen_addr.sin_addr));

            pipe_listen = std::make_shared<Pipe>(spider_ip,
                                                 0,
                                                 mode,
                                                 message_mode,
                                                 pipe_listen_ip,
                                                 "",
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 routing_manager,
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
                if(pipe != nullptr &&
                   pipe->get_sock() != -1)
                {
                    closesocket(pipe_sock);
                    continue;
                }

#ifdef DEBUGPRINT
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(pipe_addr.sin_addr),
                            ntohs(pipe_addr.sin_port));
#endif

                pipe_destination_ip = inet_ntoa(pipe_addr.sin_addr);
                pipe_destination_port = std::to_string(ntohs(pipe_addr.sin_port));

                if(pipe == nullptr)
                {
                    pipe = std::make_shared<Pipe>(spider_ip,
                                                  0,
                                                  '-',
                                                  message_mode,
                                                  pipe_listen_ip,
                                                  "",
                                                  pipe_destination_ip,
                                                  "",
                                                  pipe_destination_port,
                                                  pipe_sock,
                                                  routing_manager,
                                                  message_manager);

                    do
                    {
                        pipe_id = generate_random_id();
                        ret = pipe_manager->insert_pipe(pipe_id,
                                                        pipe);
                    }while(ret != 0);

                    pipe_key = pipe_id;
                }else
                {
                    pipe->set_pipe_destination_ip(pipe_destination_ip);
                    pipe->set_pipe_destination_port(pipe_destination_port);
                    pipe->set_sock(pipe_sock);
                }


                // TLS
                SCHANNEL_CRED schannel_cred;
                CredHandle cred_handle;
                CtxtHandle ctxt_handle;
                SecPkgContext_StreamSizes stream_sizes;
                SECURITY_STATUS status;
                unsigned long f_context_req;
                unsigned long f_context_attr;
                TimeStamp ts_expiry;
                SecBufferDesc input_sec_buffer_desc;
                SecBufferDesc output_sec_buffer_desc;
                SecBuffer input_sec_buffer[2];
                SecBuffer output_sec_buffer[3];
                HCERTSTORE h_cert_store;
                PCCERT_CONTEXT server_cert = NULL;
                HCRYPTPROV h_crypt_prov;
                DWORD key_spec;
                BOOL caller_free_prov_or_ncryptkey = false;
                BOOL init_flag = true;
                BOOL error_flag = false;
                char *buffer = NULL;
                char *tmp = NULL;
                int32_t rec = 0;
                int32_t tmprec = 0;
                int32_t sen = 0;
                unsigned long tv_sec = 30;
                unsigned long tv_usec = 0;

                if(tls_flag == true)
                {
                    h_cert_store = CertOpenStore(CERT_STORE_PROV_SYSTEM_A,
                                                 0,
                                                 NULL,
                                                 CERT_SYSTEM_STORE_CURRENT_USER,
                                                 store_name);
                    if(h_cert_store == NULL)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] CertOpenStore error\n");
#endif
                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    server_cert = find_certificate(h_cert_store,
                                                   cert_search_string);
                    if(server_cert == NULL)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] findCertificate error\n");
#endif
                        CertCloseStore(h_cert_store,
                                       0);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    BOOL ret_b = CryptAcquireCertificatePrivateKey(server_cert,
                                                                   CRYPT_ACQUIRE_ONLY_NCRYPT_KEY_FLAG | CRYPT_ACQUIRE_COMPARE_KEY_FLAG,
                                                                   NULL,
                                                                   &h_crypt_prov,
                                                                   &key_spec,
                                                                   &caller_free_prov_or_ncryptkey);
                    if(ret_b <= 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] CryptAcquireCertificatePrivateKey error\n");
#endif
                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }


                    std::memset(&schannel_cred,
                                0,
                                sizeof(schannel_cred));

                    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                    schannel_cred.cCreds = 1;
                    schannel_cred.paCred = &server_cert;
                    schannel_cred.hRootStore = h_cert_store;
                    schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_SERVER | SP_PROT_TLS1_3_SERVER;
                    schannel_cred.dwFlags = SCH_USE_STRONG_CRYPTO;

                    status = AcquireCredentialsHandleA(NULL,
                                                       const_cast<LPSTR>(UNISP_NAME_A),
                                                       SECPKG_CRED_INBOUND,
                                                       NULL,
                                                       &schannel_cred,
                                                       NULL,
                                                       NULL,
                                                       &cred_handle,
                                                       NULL);
                    if(status != SEC_E_OK)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                    status);
#endif
                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    std::memset(&ctxt_handle,
                                0,
                                sizeof(ctxt_handle));

                    f_context_req = ASC_REQ_ALLOCATE_MEMORY;

                    output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                    output_sec_buffer[0].pvBuffer = NULL;
                    output_sec_buffer[0].cbBuffer = 0;
                    output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                    output_sec_buffer[1].pvBuffer = NULL;
                    output_sec_buffer[1].cbBuffer = 0;
                    output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                    output_sec_buffer[2].pvBuffer = NULL;
                    output_sec_buffer[2].cbBuffer = 0;
                    output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                    output_sec_buffer_desc.cBuffers = 3;
                    output_sec_buffer_desc.pBuffers = output_sec_buffer;

                    buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
                    tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                         sizeof(char));

                    rec = pipe->recv_data(buffer,
                                          NODE_BUFFER_SIZE,
                                          tv_sec,
                                          tv_usec);
                    if(rec <= 0)
                    {
                        free_all_buffers(&output_sec_buffer_desc);
                        DeleteSecurityContext(&ctxt_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    while(1)
                    {
                        input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                        input_sec_buffer[0].pvBuffer = buffer;
                        input_sec_buffer[0].cbBuffer = rec;
                        input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                        input_sec_buffer[1].pvBuffer = NULL;
                        input_sec_buffer[1].cbBuffer = 0;
                        input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                        input_sec_buffer_desc.cBuffers = 2;
                        input_sec_buffer_desc.pBuffers = input_sec_buffer;

                        if(init_flag == true)
                        {
                            init_flag = false;

                            status = AcceptSecurityContext(&cred_handle,
                                                           NULL,
                                                           &input_sec_buffer_desc,
                                                           f_context_req,
                                                           SECURITY_NATIVE_DREP,
                                                           &ctxt_handle,
                                                           &output_sec_buffer_desc,
                                                           &f_context_attr,
                                                           &ts_expiry);
                        }else
                        {
                            status = AcceptSecurityContext(&cred_handle,
                                                           &ctxt_handle,
                                                           &input_sec_buffer_desc,
                                                           f_context_req,
                                                           SECURITY_NATIVE_DREP,
                                                           &ctxt_handle,
                                                           &output_sec_buffer_desc,
                                                           &f_context_attr,
                                                           &ts_expiry);
                        }

                        if(status == SEC_E_OK)
                        {
                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            break;
                        }else if(status == SEC_I_CONTINUE_NEEDED)
                        {
                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            free_all_buffers(&output_sec_buffer_desc);

                            rec = pipe->recv_data(buffer,
                                                  NODE_BUFFER_SIZE,
                                                  tv_sec,
                                                  tv_usec);
                            if(rec <= 0)
                            {
                                error_flag = true;
                                break;
                            }
                        }else if(status == SEC_I_COMPLETE_NEEDED)
                        {
                            status = CompleteAuthToken(&ctxt_handle,
                                                       &output_sec_buffer_desc);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] CompleteAuthToken error: %x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            break;
                        }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                        {
                            status = CompleteAuthToken(&ctxt_handle,
                                                       &output_sec_buffer_desc);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[E] CompleteAuthToken error: %x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            free_all_buffers(&output_sec_buffer_desc);

                            rec = pipe->recv_data(buffer,
                                                  NODE_BUFFER_SIZE,
                                                  tv_sec,
                                                  tv_usec);
                            if(rec <= 0)
                            {
                                error_flag = true;
                                break;
                            }
                        }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                        {
                            tmprec = pipe->recv_data(tmp,
                                                     NODE_BUFFER_SIZE,
                                                     tv_sec,
                                                     tv_usec);
                            if(tmprec <= 0)
                            {
                                error_flag = true;
                                break;
                            }

                            if(rec + tmprec <= NODE_BUFFER_SIZE)
                            {
                                std::memcpy(buffer + rec,
                                            tmp,
                                            tmprec);
                                rec += tmprec;
                            }else
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                            rec + tmprec);
#endif
                                error_flag = true;
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] AcceptSecurityContext error: %x\n",
                                        status);
#endif
                            error_flag = true;
                            break;
                        }
                    }

                    free_all_buffers(&output_sec_buffer_desc);
                    free(buffer);
                    free(tmp);

                    if(error_flag == true)
                    {
                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    status = QueryContextAttributes(&ctxt_handle,
                                                    SECPKG_ATTR_STREAM_SIZES,
                                                    &stream_sizes);
                    if(status != SEC_E_OK)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] QueryContextAttributes error: %x\n",
                                    status);
#endif
                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    pipe->set_ctxt_handle(&ctxt_handle);
                    pipe->set_stream_sizes(stream_sizes);
                }


                // http connection
                ret = pipe->do_http_connection_server();

                if(tls_flag == true)
                {
                    pipe->set_ctxt_handle(NULL);
                    pipe->set_stream_sizes({0});

                    DeleteSecurityContext(&ctxt_handle);
                    FreeCredentialsHandle(&cred_handle);

                    CertCloseStore(h_cert_store,
                                   0);
                    CertFreeCertificateContext(server_cert);
                    NCryptFreeObject(h_crypt_prov);
                }

                pipe->set_sock(-1);
                closesocket(pipe_sock);
            }

            pipe_manager->erase_pipe(pipe_key);
            pipe_manager->erase_pipe(pipe_listen_key);
        }else if(family == AF_INET6)
        {
            pipe_listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
            if(pipe_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(pipe_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(pipe_listen_sock,
                       (struct sockaddr *)&pipe_listen_addr6,
                       sizeof(pipe_listen_addr6));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            // listen
            ret = listen(pipe_listen_sock,
                         5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(pipe_listen_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &pipe_listen_addr6.sin6_addr,
                      pipe_listen_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

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

            pipe_listen = std::make_shared<Pipe>(spider_ip,
                                                 0,
                                                 mode,
                                                 message_mode,
                                                 pipe_listen_ip,
                                                 pipe_listen_ip_scope_id,
                                                 pipe_listen_port,
                                                 pipe_listen_sock,
                                                 routing_manager,
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

                if(pipe != nullptr &&
                   pipe->get_sock() != -1)
                {
                    closesocket(pipe_sock);
                    continue;
                }

                inet_ntop(AF_INET6,
                          &pipe_addr6.sin6_addr,
                          pipe_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
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

                pipe_destination_ip = pipe_addr6_string_pointer;
                pipe_destination_ip_scope_id = std::to_string(pipe_addr6.sin6_scope_id);
                pipe_destination_port = std::to_string(ntohs(pipe_addr6.sin6_port));

                if(pipe == nullptr)
                {
                    pipe = std::make_shared<Pipe>(spider_ip,
                                                  0,
                                                  '-',
                                                  message_mode,
                                                  pipe_listen_ip,
                                                  pipe_listen_ip_scope_id,
                                                  pipe_destination_ip,
                                                  pipe_destination_ip_scope_id,
                                                  pipe_destination_port,
                                                  pipe_sock,
                                                  routing_manager,
                                                  message_manager);

                    do
                    {
                        pipe_id = generate_random_id();
                        ret = pipe_manager->insert_pipe(pipe_id,
                                                        pipe);
                    }while(ret != 0);

                    pipe_key = pipe_id;
                }else
                {
                    pipe->set_pipe_destination_ip(pipe_destination_ip);
                    pipe->set_pipe_destination_ip_scope_id(pipe_destination_ip_scope_id);
                    pipe->set_pipe_destination_port(pipe_destination_port);
                    pipe->set_sock(pipe_sock);
                }


                // TLS
                SCHANNEL_CRED schannel_cred;
                CredHandle cred_handle;
                CtxtHandle ctxt_handle;
                SecPkgContext_StreamSizes stream_sizes;
                SECURITY_STATUS status;
                unsigned long f_context_req;
                unsigned long f_context_attr;
                TimeStamp ts_expiry;
                SecBufferDesc input_sec_buffer_desc;
                SecBufferDesc output_sec_buffer_desc;
                SecBuffer input_sec_buffer[2];
                SecBuffer output_sec_buffer[3];
                HCERTSTORE h_cert_store;
                PCCERT_CONTEXT server_cert = NULL;
                HCRYPTPROV h_crypt_prov;
                DWORD key_spec;
                BOOL caller_free_prov_or_ncryptkey = false;
                BOOL init_flag = true;
                BOOL error_flag = false;
                char *buffer = NULL;
                char *tmp = NULL;
                int32_t rec = 0;
                int32_t tmprec = 0;
                int32_t sen = 0;
                unsigned long tv_sec = 30;
                unsigned long tv_usec = 0;

                if(tls_flag == true)
                {
                    h_cert_store = CertOpenStore(CERT_STORE_PROV_SYSTEM_A,
                                                 0,
                                                 NULL,
                                                 CERT_SYSTEM_STORE_CURRENT_USER,
                                                 store_name);
                    if(h_cert_store == NULL)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] CertOpenStore error\n");
#endif
                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    server_cert = find_certificate(h_cert_store,
                                                   cert_search_string);
                    if(server_cert == NULL)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] findCertificate error\n");
#endif
                        CertCloseStore(h_cert_store,
                                       0);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    BOOL ret_b = CryptAcquireCertificatePrivateKey(server_cert,
                                                                   CRYPT_ACQUIRE_ONLY_NCRYPT_KEY_FLAG | CRYPT_ACQUIRE_COMPARE_KEY_FLAG,
                                                                   NULL,
                                                                   &h_crypt_prov,
                                                                   &key_spec,
                                                                   &caller_free_prov_or_ncryptkey);
                    if(ret_b <= 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] CryptAcquireCertificatePrivateKey error\n");
#endif
                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }


                    std::memset(&schannel_cred,
                                0,
                                sizeof(schannel_cred));

                    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
                    schannel_cred.cCreds = 1;
                    schannel_cred.paCred = &server_cert;
                    schannel_cred.hRootStore = h_cert_store;
                    schannel_cred.grbitEnabledProtocols = SP_PROT_TLS1_2_SERVER | SP_PROT_TLS1_3_SERVER;
                    schannel_cred.dwFlags = SCH_USE_STRONG_CRYPTO;

                    status = AcquireCredentialsHandleA(NULL,
                                                       const_cast<LPSTR>(UNISP_NAME_A),
                                                       SECPKG_CRED_INBOUND,
                                                       NULL,
                                                       &schannel_cred,
                                                       NULL,
                                                       NULL,
                                                       &cred_handle,
                                                       NULL);
                    if(status != SEC_E_OK)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] AcquireCredentialsHandleA error: %x\n",
                                    status);
#endif
                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    std::memset(&ctxt_handle,
                                0,
                                sizeof(ctxt_handle));

                    f_context_req = ASC_REQ_ALLOCATE_MEMORY;

                    output_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                    output_sec_buffer[0].pvBuffer = NULL;
                    output_sec_buffer[0].cbBuffer = 0;
                    output_sec_buffer[1].BufferType = SECBUFFER_ALERT;
                    output_sec_buffer[1].pvBuffer = NULL;
                    output_sec_buffer[1].cbBuffer = 0;
                    output_sec_buffer[2].BufferType = SECBUFFER_EMPTY;
                    output_sec_buffer[2].pvBuffer = NULL;
                    output_sec_buffer[2].cbBuffer = 0;
                    output_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                    output_sec_buffer_desc.cBuffers = 3;
                    output_sec_buffer_desc.pBuffers = output_sec_buffer;

                    buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
                    tmp = (char *)calloc(NODE_BUFFER_SIZE,
                                         sizeof(char));

                    rec = pipe->recv_data(buffer,
                                          NODE_BUFFER_SIZE,
                                          tv_sec,
                                          tv_usec);
                    if(rec <= 0)
                    {
                        free_all_buffers(&output_sec_buffer_desc);
                        DeleteSecurityContext(&ctxt_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    while(1)
                    {
                        input_sec_buffer[0].BufferType = SECBUFFER_TOKEN;
                        input_sec_buffer[0].pvBuffer = buffer;
                        input_sec_buffer[0].cbBuffer = rec;
                        input_sec_buffer[1].BufferType = SECBUFFER_EMPTY;
                        input_sec_buffer[1].pvBuffer = NULL;
                        input_sec_buffer[1].cbBuffer = 0;
                        input_sec_buffer_desc.ulVersion = SECBUFFER_VERSION;
                        input_sec_buffer_desc.cBuffers = 2;
                        input_sec_buffer_desc.pBuffers = input_sec_buffer;

                        if(init_flag == true)
                        {
                            init_flag = false;

                            status = AcceptSecurityContext(&cred_handle,
                                                           NULL,
                                                           &input_sec_buffer_desc,
                                                           f_context_req,
                                                           SECURITY_NATIVE_DREP,
                                                           &ctxt_handle,
                                                           &output_sec_buffer_desc,
                                                           &f_context_attr,
                                                           &ts_expiry);
                        }else
                        {
                            status = AcceptSecurityContext(&cred_handle,
                                                           &ctxt_handle,
                                                           &input_sec_buffer_desc,
                                                           f_context_req,
                                                           SECURITY_NATIVE_DREP,
                                                           &ctxt_handle,
                                                           &output_sec_buffer_desc,
                                                           &f_context_attr,
                                                           &ts_expiry);
                        }

                        if(status == SEC_E_OK)
                        {
                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            break;
                        }else if(status == SEC_I_CONTINUE_NEEDED)
                        {
                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            free_all_buffers(&output_sec_buffer_desc);

                            rec = pipe->recv_data(buffer,
                                                  NODE_BUFFER_SIZE,
                                                  tv_sec,
                                                  tv_usec);
                            if(rec <= 0)
                            {
                                error_flag = true;
                                break;
                            }
                        }else if(status == SEC_I_COMPLETE_NEEDED)
                        {
                            status = CompleteAuthToken(&ctxt_handle,
                                                       &output_sec_buffer_desc);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] CompleteAuthToken error: %x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            break;
                        }else if(status == SEC_I_COMPLETE_AND_CONTINUE)
                        {
                            status = CompleteAuthToken(&ctxt_handle,
                                                       &output_sec_buffer_desc);
                            if(status != SEC_E_OK)
                            {
#ifdef DEBUGPRINT
                                std::printf("[E] CompleteAuthToken error: %x\n",
                                            status);
#endif
                                error_flag = true;
                                break;
                            }

                            if(output_sec_buffer[0].cbBuffer > 0)
                            {
                                sen = pipe->send_data((char *)output_sec_buffer[0].pvBuffer,
                                                      output_sec_buffer[0].cbBuffer,
                                                      tv_sec,
                                                      tv_usec);
                                if(sen <= 0)
                                {
                                    error_flag = true;
                                    break;
                                }
                            }

                            free_all_buffers(&output_sec_buffer_desc);

                            rec = pipe->recv_data(buffer,
                                                  NODE_BUFFER_SIZE,
                                                  tv_sec,
                                                  tv_usec);
                            if(rec <= 0)
                            {
                                error_flag = true;
                                break;
                            }
                        }else if(status == SEC_E_INCOMPLETE_MESSAGE)
                        {
                            tmprec = pipe->recv_data(tmp,
                                                     NODE_BUFFER_SIZE,
                                                     tv_sec,
                                                     tv_usec);
                            if(tmprec <= 0)
                            {
                                error_flag = true;
                                break;
                            }

                            if(rec + tmprec <= NODE_BUFFER_SIZE)
                            {
                                std::memcpy(buffer + rec,
                                            tmp,
                                            tmprec);
                                rec += tmprec;
                            }else
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] received data size has exceeded the maximum value: %d\n",
                                            rec + tmprec);
#endif
                                error_flag = true;
                                break;
                            }
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] AcceptSecurityContext error: %x\n",
                                        status);
#endif
                            error_flag = true;
                            break;
                        }
                    }

                    free_all_buffers(&output_sec_buffer_desc);
                    free(buffer);
                    free(tmp);

                    if(error_flag == true)
                    {
                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    status = QueryContextAttributes(&ctxt_handle,
                                                    SECPKG_ATTR_STREAM_SIZES,
                                                    &stream_sizes);
                    if(status != SEC_E_OK)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] QueryContextAttributes error: %x\n",
                                    status);
#endif
                        DeleteSecurityContext(&ctxt_handle);
                        FreeCredentialsHandle(&cred_handle);

                        CertCloseStore(h_cert_store,
                                       0);
                        CertFreeCertificateContext(server_cert);
                        NCryptFreeObject(h_crypt_prov);

                        pipe->set_sock(-1);
                        closesocket(pipe_sock);
                        continue;
                    }

                    pipe->set_ctxt_handle(&ctxt_handle);
                    pipe->set_stream_sizes(stream_sizes);
                }


                // http connection
                ret = pipe->do_http_connection_server();

                if(tls_flag == true)
                {
                    pipe->set_ctxt_handle(NULL);
                    pipe->set_stream_sizes({0});

                    DeleteSecurityContext(&ctxt_handle);
                    FreeCredentialsHandle(&cred_handle);

                    CertCloseStore(h_cert_store,
                                   0);
                    CertFreeCertificateContext(server_cert);
                    NCryptFreeObject(h_crypt_prov);
                }

                pipe->set_sock(-1);
                closesocket(pipe_sock);
            }

            pipe_manager->erase_pipe(pipe_key);
            pipe_manager->erase_pipe(pipe_listen_key);
        }

        closesocket(pipe_listen_sock);

        return 0;
    }

    void Spidercommand::add_node_spider_pipe()
    {
        std::string config = "";
        char mode;  // self:s other:o
        char pipe_mode;  // client:c server:s
        std::string message_mode;  // default:d http:h https:s
        BOOL tls_flag = false;
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        char forward_proxy; // yes:y no:n
        BOOL forward_proxy_flag = false;
        std::string forward_proxy_type;    // http:h http-sslbump:hs https:s
        int32_t forward_proxy_authentication; // no:0 basic:1 digest:2 ntlmv2:3 spnego(kerberos):4
        std::string forward_proxy_ip;
        std::string forward_proxy_port;
        std::string forward_proxy_username;
        char password_type;
        std::string forward_proxy_password;
        std::string forward_proxy_nthash_hexstring;
        std::string forward_proxy_domainname;
        std::string forward_proxy_workstationname;
        std::string forward_proxy_service_principal_name;
        std::string pipe_ip;
        std::string pipe_ip_scope_id;
        std::string pipe_destination_ip;
        std::string pipe_destination_port;
        std::string pipe_listen_port;
        int32_t sleep_ms = 0;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("mode (self:s other:o)                               > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("pipe mode (client:c server:s)                       > ");
                std::cin >> pipe_mode;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(pipe_mode == 'c')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("forward proxy (yes:y no:n)                          > ");
                    std::cin >> forward_proxy;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");

                        forward_proxy = 'n';
                        forward_proxy_flag = false;
                    }else if(forward_proxy == 'y')   // forward proxy
                    {
                        forward_proxy_flag = true;

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy type (http:h http-sslbump:hs https:s) > ");
                        std::cin >> forward_proxy_type;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");

                            forward_proxy_type = "h";
                        }else if(forward_proxy_type != "h" &&
                                 forward_proxy_type != "hs" &&
                                 forward_proxy_type != "s")
                        {
                            forward_proxy_type = "h";
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy authentication\n");
                        std::printf("(no:0 basic:1 digest:2 ntlmv2:3 spnego(kerberos):4) > ");
                        std::cin >> forward_proxy_authentication;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");

                            forward_proxy_authentication = 0;
                        }else if(forward_proxy_authentication < 0 || forward_proxy_authentication > 4)
                        {
                            forward_proxy_authentication = 0;
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy ip                                    > ");
                        std::cin >> forward_proxy_ip;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy port                                  > ");
                        std::cin >> forward_proxy_port;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }

                        if(forward_proxy_authentication == 1 ||
                           forward_proxy_authentication == 2)  // 1:basic or 2:digest
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy username                              > ");
                            std::cin >> forward_proxy_username;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy password                              > ");
                            std::cin >> forward_proxy_password;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }else if(forward_proxy_authentication == 3) // 3:ntlmv2
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy username                              > ");
                            std::cin >> forward_proxy_username;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("passowrd type (password:p nthash:n)                 > ");
                            std::cin >> password_type;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");

                                password_type = 'p';
                            }else if(password_type != 'p' &&
                                     password_type != 'n')
                            {
                                password_type = 'p';
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            if(password_type == 'n') // nthash
                            {
                                std::printf("forward proxy nthash hexstring                      > ");
                                std::cin >> forward_proxy_nthash_hexstring;
                                if(std::cin.fail())
                                {
                                    std::printf("[-] input error\n");
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    continue;
                                }

                                forward_proxy_password = "";
                            }else   // password
                            {
                                std::printf("forward proxy password                              > ");
                                std::cin >> forward_proxy_password;
                                if(std::cin.fail())
                                {
                                    std::printf("[-] input error\n");
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    continue;
                                }

                                forward_proxy_nthash_hexstring = "";
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy domainname                            > ");
                            std::cin >> forward_proxy_domainname;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy workstationname                       > ");
                            std::cin >> forward_proxy_workstationname;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }else if(forward_proxy_authentication == 4) // 4:spnego(kerberos)
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy service principal name                > ");
                            std::cin >> forward_proxy_service_principal_name;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }
                    }else
                    {
                        forward_proxy = 'n';
                        forward_proxy_flag = false;
                    }

                    if(forward_proxy_type == "h")
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("message mode (http:h https:s)                       > ");
                        std::cin >> message_mode;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            message_mode = "h";
                            tls_flag = false;
                        }else if(message_mode == "h")
                        {
                            tls_flag = false;
                        }else if(message_mode == "s")
                        {
                            tls_flag = true;
                        }else if(message_mode != "h" &&
                                 message_mode != "s")
                        {
                            message_mode = 'h';
                            tls_flag = false;
                        }
                    }else if(forward_proxy_type == "hs" ||
                             forward_proxy_type == "s")
                    {
                        message_mode = 's';
                        tls_flag = true;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("message mode (default:d http:h https:s)             > ");
                        std::cin >> message_mode;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            message_mode = "d";
                            tls_flag = false;
                        }else if(message_mode == "d" ||
                                 message_mode == "h")
                        {
                            tls_flag = false;
                        }else if(message_mode == "s")
                        {
                            tls_flag = true;
                        }else if(message_mode != "d" &&
                                 message_mode != "h" &&
                                 message_mode != "s")
                        {
                            message_mode = "d";
                            tls_flag = false;
                        }
                    }

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("pipe ip                                             > ");
                    std::cin >> pipe_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(pipe_ip != spider_ip->get_spider_ipv4()
                       && pipe_ip != spider_ip->get_spider_ipv6_global()
                       && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
                       && pipe_ip != spider_ip->get_spider_ipv6_link_local())
                    {
                        std::printf("[-] please input spider ipv4 or ipv6\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
                    {
                        pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                    }

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("pipe destination ip                                 > ");
                    std::cin >> pipe_destination_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("pipe destination port                               > ");
                    std::cin >> pipe_destination_port;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(message_mode == "h" ||
                       message_mode == "s")   // http or https
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("sleep (0-1000 milliseconds)                         > ");
                        std::cin >> sleep_ms;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
                        }else if(sleep_ms < 0 || sleep_ms > 1000)
                        {
                            sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
                        }
                    }

                    std::printf("\n");
                    std::printf("pipe mode                            : %c\n", pipe_mode);
                    if(forward_proxy_flag == true)
                    {
                        std::printf("forward proxy type                   : %s\n", forward_proxy_type.c_str());
                        std::printf("forward proxy authentication         : %d\n", forward_proxy_authentication);
                        std::printf("forward proxy ip                     : %s\n", forward_proxy_ip.c_str());
                        std::printf("forward proxy port                   : %s\n", forward_proxy_port.c_str());
                        if(forward_proxy_authentication == 1 ||
                           forward_proxy_authentication == 2)   // 1:basic or 2:digest
                        {
                            std::printf("forward proxy username               : %s\n", forward_proxy_username.c_str());
                            std::printf("forward proxy password               : %s\n", forward_proxy_password.c_str());
                        }else if(forward_proxy_authentication == 3) // 3:ntlmv2
                        {
                            std::printf("forward proxy username               : %s\n", forward_proxy_username.c_str());
                            std::printf("forward proxy password type          : %c\n", password_type);
                            if(password_type == 'p')    // password
                            {
                                std::printf("forward proxy password               : %s\n", forward_proxy_password.c_str());
                            }else if(password_type == 'n')  // nthash
                            {
                                std::printf("forward proxy nthash hexstring       : %s\n", forward_proxy_nthash_hexstring.c_str());
                            }
                            std::printf("forward proxy domainname             : %s\n", forward_proxy_domainname.c_str());
                            std::printf("forward proxy workstationname        : %s\n", forward_proxy_workstationname.c_str());
                        }else if(forward_proxy_authentication == 4) // 4:spnego(kerberos)
                        {
                            std::printf("forward proxy service principal name : %s\n", forward_proxy_service_principal_name.c_str());
                        }
                    }
                    std::printf("message mode                         : %s\n", message_mode.c_str());
                    std::printf("pipe ip                              : %s\n", pipe_ip.c_str());
                    if(!pipe_ip_scope_id.empty() &&
                       forward_proxy_flag == false)
                    {
                        std::printf("pipe ip scope id                     : %s (%d)\n", pipe_ip_scope_id.c_str(), if_nametoindex(pipe_ip_scope_id.c_str()));
                    }
                    std::printf("pipe destination ip                  : %s\n", pipe_destination_ip.c_str());
                    std::printf("pipe destination port                : %s\n", pipe_destination_port.c_str());
                    if(message_mode == "h" ||
                       message_mode == "s")   // http or https
                    {
                        std::printf("sleep                                : %4d ms\n", sleep_ms);
                    }
                    std::printf("\n");

                    std::printf("ok? (yes:y no:n quit:q)                             > ");
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

                        if(message_mode == "d") // default
                        {
                            std::thread thread(&Spidercommand::connect_pipe,
                                               this,
                                               pipe_mode,
                                               pipe_ip,
                                               pipe_ip_scope_id,
                                               pipe_destination_ip,
                                               pipe_destination_port);
                            thread.detach();
                        }else if(message_mode == "h" ||
                                 message_mode == "s")   // http or https
                        {
                            std::thread thread(&Spidercommand::connect_pipe_http,
                                               this,
                                               pipe_mode,
                                               forward_proxy_flag,
                                               forward_proxy_type,
                                               forward_proxy_authentication,
                                               forward_proxy_ip,
                                               forward_proxy_port,
                                               forward_proxy_username,
                                               forward_proxy_password,
                                               forward_proxy_nthash_hexstring,
                                               forward_proxy_domainname,
                                               forward_proxy_workstationname,
                                               forward_proxy_service_principal_name,
                                               tls_flag,
                                               message_mode,
                                               pipe_ip,
                                               pipe_ip_scope_id,
                                               pipe_destination_ip,
                                               pipe_destination_port,
                                               sleep_ms);
                            thread.detach();
                        }

                        std::this_thread::sleep_for(std::chrono::seconds(5));  // 5s

                        break;
                    }else if(check == 'n')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }else if(check == 'q')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }

                    break;
                }else if(pipe_mode == 's')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("message mode (default:d http:h https:s)             > ");
                    std::cin >> message_mode;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        message_mode = "d";
                    }

                    if(message_mode != "d" &&
                       message_mode != "h" &&
                       message_mode != "s")
                    {
                        message_mode = "d";
                        tls_flag = false;
                    }else if(message_mode == "d" ||
                             message_mode == "h")
                    {
                        tls_flag = false;
                    }else if(message_mode == "s")
                    {
                        tls_flag = true;
                    }

                    std::printf("pipe listen ip                                      > ");
                    std::cin >> pipe_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(pipe_ip != spider_ip->get_spider_ipv4()
                       && pipe_ip != spider_ip->get_spider_ipv6_global()
                       && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
                       && pipe_ip != spider_ip->get_spider_ipv6_link_local())
                    {
                        std::printf("[-] please input spider ipv4 or ipv6\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
                    {
                        pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                    }

                    std::printf("pipe listen port                                    > ");
                    std::cin >> pipe_listen_port;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("\n");
                    std::printf("pipe mode                            : %c\n", pipe_mode);
                    std::printf("message mode                         : %s\n", message_mode.c_str());
                    std::printf("pipe listen ip                       : %s\n", pipe_ip.c_str());
                    if(!pipe_ip_scope_id.empty())
                    {
                        std::printf("pipe listen ip scope id              : %s (%d)\n", pipe_ip_scope_id.c_str(), if_nametoindex(pipe_ip_scope_id.c_str()));
                    }
                    std::printf("pipe listen port                     : %s\n", pipe_listen_port.c_str());
                    std::printf("\n");

                    std::printf("ok? (yes:y no:n quit:q)                             > ");
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

                        if(message_mode == "d") // default
                        {
                            std::thread thread(&Spidercommand::listen_pipe,
                                               this,
                                               pipe_mode,
                                               pipe_ip,
                                               pipe_ip_scope_id,
                                               pipe_listen_port);
                            thread.detach();
                        }else if(message_mode == "h" ||
                                 message_mode == "s")   // http or https
                        {
                            std::thread thread(&Spidercommand::listen_pipe_http,
                                               this,
                                               pipe_mode,
                                               tls_flag,
                                               message_mode,
                                               pipe_ip,
                                               pipe_ip_scope_id,
                                               pipe_listen_port);
                            thread.detach();
                        }

                        std::this_thread::sleep_for(std::chrono::seconds(2));  // 2s

                        break;
                    }else if(check == 'n')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }else if(check == 'q')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("pipe mode (client:c server:s)                       > ");
                std::cin >> pipe_mode;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(pipe_mode == 'c')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("source spider ip                                    > ");
                    std::cin >> source_spider_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(source_spider_ip != spider_ip->get_spider_ipv4()
                       && source_spider_ip != spider_ip->get_spider_ipv6_global()
                       && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                       && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                    {
                        std::printf("[-] please input spider ipv4 or ipv6\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                    {
                        source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                    }

                    std::printf("destination spider ip                               > ");
                    std::cin >> destination_spider_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("forward proxy (yes:y no:n)                          > ");
                    std::cin >> forward_proxy;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");

                        forward_proxy = 'n';
                        forward_proxy_flag = false;
                    }else if(forward_proxy == 'y')   // forward proxy
                    {
                        forward_proxy_flag = true;

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy type (http:h http-sslbump:hs https:s) > ");
                        std::cin >> forward_proxy_type;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");

                            forward_proxy_type = "h";
                        }else if(forward_proxy_type != "h" &&
                                 forward_proxy_type != "hs" &&
                                 forward_proxy_type != "s")
                        {
                            forward_proxy_type = "h";
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy authentication\n");
                        std::printf("(no:0 basic:1 digest:2 ntlmv2:3 spnego(kerberos):4) > ");
                        std::cin >> forward_proxy_authentication;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");

                            forward_proxy_authentication = 0;
                        }else if(forward_proxy_authentication < 0 || forward_proxy_authentication > 4)
                        {
                            forward_proxy_authentication = 0;
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy ip                                    > ");
                        std::cin >> forward_proxy_ip;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("forward proxy port                                  > ");
                        std::cin >> forward_proxy_port;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }

                        if(forward_proxy_authentication == 1 ||
                           forward_proxy_authentication == 2)  // 1:basic or 2:digest
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy username                              > ");
                            std::cin >> forward_proxy_username;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy password                              > ");
                            std::cin >> forward_proxy_password;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }else if(forward_proxy_authentication == 3) // 3:ntlmv2
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy username                              > ");
                            std::cin >> forward_proxy_username;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("passowrd type (password:p nthash:n)                 > ");
                            std::cin >> password_type;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");

                                password_type = 'p';
                            }else if(password_type != 'p' &&
                                     password_type != 'n')
                            {
                                password_type = 'p';
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            if(password_type == 'n') // nthash
                            {
                                std::printf("forward proxy nthash hexstring                      > ");
                                std::cin >> forward_proxy_nthash_hexstring;
                                if(std::cin.fail())
                                {
                                    std::printf("[-] input error\n");
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    continue;
                                }

                                forward_proxy_password = "";
                            }else   // password
                            {
                                std::printf("forward proxy password                              > ");
                                std::cin >> forward_proxy_password;
                                if(std::cin.fail())
                                {
                                    std::printf("[-] input error\n");
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    continue;
                                }

                                forward_proxy_nthash_hexstring = "";
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy domainname                            > ");
                            std::cin >> forward_proxy_domainname;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }

                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy workstationname                       > ");
                            std::cin >> forward_proxy_workstationname;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }else if(forward_proxy_authentication == 4) // 4:spnego(kerberos)
                        {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                            std::printf("forward proxy service principal name                > ");
                            std::cin >> forward_proxy_service_principal_name;
                            if(std::cin.fail())
                            {
                                std::printf("[-] input error\n");
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            }
                        }
                    }else
                    {
                        forward_proxy = 'n';
                        forward_proxy_flag = false;
                    }

                    if(forward_proxy_type == "h")
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("message mode (http:h https:s)                       > ");
                        std::cin >> message_mode;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            message_mode = "h";
                            tls_flag = false;
                        }else if(message_mode == "h")
                        {
                            tls_flag = false;
                        }else if(message_mode == "s")
                        {
                            tls_flag = true;
                        }else if(message_mode != "h" &&
                                 message_mode != "s")
                        {
                            message_mode = 'h';
                            tls_flag = false;
                        }
                    }else if(forward_proxy_type == "hs" ||
                             forward_proxy_type == "s")
                    {
                        message_mode = "s";
                        tls_flag = true;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        std::printf("message mode (default:d http:h https:s)             > ");
                        std::cin >> message_mode;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            message_mode = "d";
                            tls_flag = false;
                        }else if(message_mode == "d" ||
                                 message_mode == "h")
                        {
                            tls_flag = false;
                        }else if(message_mode == "s")
                        {
                            tls_flag = true;
                        }else if(message_mode != "d" &&
                                 message_mode != "h" &&
                                 message_mode != "s")
                        {
                            message_mode = "d";
                            tls_flag = false;
                        }
                    }

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("pipe ip                                             > ");
                    std::cin >> pipe_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("pipe destination ip                                 > ");
                    std::cin >> pipe_destination_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("pipe destination port                               > ");
                    std::cin >> pipe_destination_port;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(message_mode == "h" ||
                       message_mode == "s")   // http or https
                    {
                        std::printf("sleep (0-1000 milliseconds)                         > ");
                        std::cin >> sleep_ms;
                        if(std::cin.fail())
                        {
                            std::printf("[-] input error\n");
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
                        }else if(sleep_ms < 0 || sleep_ms > 1000)
                        {
                            sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
                        }
                    }

                    std::printf("\n");
                    std::printf("pipe mode                            : %c\n", pipe_mode);
                    std::printf("source spider ip                     : %s\n", source_spider_ip.c_str());
                    if(!source_spider_ip_scope_id.empty())
                    {
                        std::printf("source spider ip scope id            : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                    }
                    std::printf("destination spider ip                : %s\n", destination_spider_ip.c_str());
                    if(forward_proxy_flag == true)
                    {
                        std::printf("forward proxy type                   : %c\n", forward_proxy_type);
                        std::printf("forward proxy authentication         : %d\n", forward_proxy_authentication);
                        std::printf("forward proxy ip                     : %s\n", forward_proxy_ip.c_str());
                        std::printf("forward proxy port                   : %s\n", forward_proxy_port.c_str());
                        if(forward_proxy_authentication == 1 ||
                           forward_proxy_authentication == 2)   // 1:basic or 2:digest
                        {
                            std::printf("forward proxy username               : %s\n", forward_proxy_username.c_str());
                            std::printf("forward proxy password               : %s\n", forward_proxy_password.c_str());
                        }else if(forward_proxy_authentication == 3) // 3:ntlmv2
                        {
                            std::printf("forward proxy username               : %s\n", forward_proxy_username.c_str());
                            std::printf("forward proxy password type          : %c\n", password_type);
                            if(password_type == 'p')    // password
                            {
                                std::printf("forward proxy password               : %s\n", forward_proxy_password.c_str());
                            }else if(password_type == 'n')  // nthash
                            {
                                std::printf("forward proxy nthash hexstring       : %s\n", forward_proxy_nthash_hexstring.c_str());
                            }
                            std::printf("forward proxy domainname             : %s\n", forward_proxy_domainname.c_str());
                            std::printf("forward proxy workstationname        : %s\n", forward_proxy_workstationname.c_str());
                        }else if(forward_proxy_authentication == 4) // 4:spnego(kerberos)
                        {
                            std::printf("forward proxy service principal name : %s\n", forward_proxy_service_principal_name.c_str());
                        }
                    }
                    std::printf("message mode                         : %s\n", message_mode.c_str());
                    std::printf("pipe ip                              : %s\n", pipe_ip.c_str());
                    std::printf("pipe destination ip                  : %s\n", pipe_destination_ip.c_str());
                    std::printf("pipe destination port                : %s\n", pipe_destination_port.c_str());
                    if(message_mode == "h" ||
                       message_mode == "s")   // http or https
                    {
                        std::printf("sleep                                : %4d ms\n", sleep_ms);
                    }
                    std::printf("\n");

                    std::printf("ok? (yes:y no:n quit:q)                             > ");
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

                        if(message_mode == "d") // default
                        {
                            config = "";
                            config += "[pipe_client]\n";

                            config += "pipe_ip:";
                            config += pipe_ip;
                            config += "\n";

                            config += "pipe_destination_ip:";
                            config += pipe_destination_ip;
                            config += "\n";

                            config += "pipe_destination_port:";
                            config += pipe_destination_port;
                            config += "\n";

                            std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                               this,
                                               config,
                                               source_spider_ip,
                                               source_spider_ip_scope_id,
                                               destination_spider_ip);
                            thread.detach();
                        }else if(message_mode == "h" ||
                                 message_mode == "s")   // http or https
                        {
                            config = "";
                            config += "[pipe_client_http]\n";

                            config += "forward_proxy_flag:";
                            config += forward_proxy_flag ? "true" : "false";
                            config += "\n";

                            config += "forward_proxy_type:";
                            config += forward_proxy_type;
                            config += "\n";

                            config += "forward_proxy_authentication:";
                            config += std::to_string(forward_proxy_authentication);
                            config += "\n";

                            config += "forward_proxy_ip:";
                            config += forward_proxy_ip;
                            config += "\n";

                            config += "forward_proxy_port:";
                            config += forward_proxy_port;
                            config += "\n";

                            config += "forward_proxy_username:";
                            config += forward_proxy_username;
                            config += "\n";

                            config += "forward_proxy_password:";
                            config += forward_proxy_password;
                            config += "\n";

                            config += "forward_proxy_nthash_hexstring:";
                            config += forward_proxy_nthash_hexstring;
                            config += "\n";

                            config += "forward_proxy_domainname:";
                            config += forward_proxy_domainname;
                            config += "\n";

                            config += "forward_proxy_workstationname:";
                            config += forward_proxy_workstationname;
                            config += "\n";

                            config += "forward_proxy_service_principal_name:";
                            config += forward_proxy_service_principal_name;
                            config += "\n";

                            config += "tls_flag:";
                            config += tls_flag ? "true" : "false";
                            config += "\n";

                            config += "message_mode:";
                            config += message_mode;
                            config += "\n";

                            config += "pipe_ip:";
                            config += pipe_ip;
                            config += "\n";

                            config += "pipe_destination_ip:";
                            config += pipe_destination_ip;
                            config += "\n";

                            config += "pipe_destination_port:";
                            config += pipe_destination_port;
                            config += "\n";

                            config += "sleep_ms:";
                            config += std::to_string(sleep_ms);
                            config += "\n";

                            std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                               this,
                                               config,
                                               source_spider_ip,
                                               source_spider_ip_scope_id,
                                               destination_spider_ip);
                            thread.detach();
                        }

                        break;
                    }else if(check == 'n')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }else if(check == 'q')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }

                    break;
                }else if(pipe_mode == 's')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::printf("source spider ip                                    > ");
                    std::cin >> source_spider_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(source_spider_ip != spider_ip->get_spider_ipv4()
                       && source_spider_ip != spider_ip->get_spider_ipv6_global()
                       && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                       && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                    {
                        std::printf("[-] please input spider ipv4 or ipv6\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                    {
                        source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                    }

                    std::printf("destination spider ip                               > ");
                    std::cin >> destination_spider_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("message mode (default:d http:h https:s)             > ");
                    std::cin >> message_mode;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        message_mode = "d";
                    }

                    if(message_mode != "d" &&
                       message_mode != "h" &&
                       message_mode != "s")
                    {
                        message_mode = "d";
                        tls_flag = false;
                    }else if(message_mode == "d" ||
                             message_mode == "h")
                    {
                        tls_flag = false;
                    }else if(message_mode == "s")
                    {
                        tls_flag = true;
                    }

                    std::printf("pipe listen ip                                      > ");
                    std::cin >> pipe_ip;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("pipe listen port                                    > ");
                    std::cin >> pipe_listen_port;
                    if(std::cin.fail())
                    {
                        std::printf("[-] input error\n");
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }

                    std::printf("\n");
                    std::printf("pipe mode                            : %c\n", pipe_mode);
                    std::printf("source spider ip                     : %s\n", source_spider_ip.c_str());
                    if(!source_spider_ip_scope_id.empty())
                    {
                        std::printf("source spider ip scope id            : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                    }
                    std::printf("destination spider ip                : %s\n", destination_spider_ip.c_str());
                    std::printf("message mode                         : %s\n", message_mode.c_str());
                    std::printf("pipe listen ip                       : %s\n", pipe_ip.c_str());
                    std::printf("pipe listen port                     : %s\n", pipe_listen_port.c_str());
                    std::printf("\n");

                    std::printf("ok? (yes:y no:n quit:q)                             > ");
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

                        if(message_mode == "d") // default
                        {
                            config = "";
                            config += "[pipe_server]\n";

                            config += "pipe_listen_ip:";
                            config += pipe_ip;
                            config += "\n";

                            config += "pipe_listen_port:";
                            config += pipe_listen_port;
                            config += "\n";

                            std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                               this,
                                               config,
                                               source_spider_ip,
                                               source_spider_ip_scope_id,
                                               destination_spider_ip);
                            thread.detach();
                        }else if(message_mode == "h" ||
                                 message_mode == "s")   // http or https
                        {
                            config = "";
                            config += "[pipe_server_http]\n";

                            config += "tls_flag:";
                            config += tls_flag ? "true" : "false";
                            config += "\n";

                            config += "message_mode:";
                            config += message_mode;
                            config += "\n";

                            config += "pipe_listen_ip:";
                            config += pipe_ip;
                            config += "\n";

                            config += "pipe_listen_port:";
                            config += pipe_listen_port;
                            config += "\n";

                            std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                               this,
                                               config,
                                               source_spider_ip,
                                               source_spider_ip_scope_id,
                                               destination_spider_ip);
                            thread.detach();
                        }

                        break;
                    }else if(check == 'n')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                    }else if(check == 'q')
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }else
                    {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return;
                    }
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }

    void Spidercommand::show_node_information_worker(std::string source_spider_ip,
                                                     std::string source_spider_ip_scope_id,
                                                     std::string destination_spider_ip)
    {
        int32_t ret = 0;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        int32_t client_sock = INVALID_SOCKET;
        std::shared_ptr<Client> client;
        std::pair<uint32_t, uint32_t> client_key;


        client = std::make_shared<Client>("show n",
                                          0,
                                          0,
                                          0,
                                          source_spider_ip,
                                          source_spider_ip_scope_id,
                                          "",
                                          "",
                                          destination_spider_ip,
                                          client_sock,
                                          SHOW_NODE_INFORMATION_WORKER_TV_SEC,
                                          SHOW_NODE_INFORMATION_WORKER_TV_USEC,
                                          SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_SEC,
                                          SHOW_NODE_INFORMATION_WORKER_FORWARDER_TV_USEC,
                                          encryption,
                                          message_manager);

        do
        {
            connection_id = generate_random_id();
            client_id = generate_random_id();
            ret = client_manager->insert_client(connection_id,
                                                client_id,
                                                client);
        }while(ret != 0);

        ret = client->do_socks5_connection_show_node();
        if(ret == -1)
        {
            client_manager->erase_client(client->get_connection_id(),
                                         client->get_client_id());
        }

        return;
    }

    void Spidercommand::show_node_information()
    {
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        char check = 'n';


        while(1)
        {
            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                // client
                client_manager->show_clients_map();

                // server
                server_manager->show_servers_map();

                // pipe
                pipe_manager->show_pipes_map();

                break;
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                routing_manager->show_routing_table();
                std::printf("\n");

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                    && source_spider_ip != spider_ip->get_spider_ipv6_global()
                    && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                    && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("\n");
                std::printf("mode                      : %c\n", mode);
                std::printf("source spider ip          : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip     : %s\n", destination_spider_ip.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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


                    std::thread thread(&Spidercommand::show_node_information_worker,
                                       this,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    std::this_thread::sleep_for(std::chrono::seconds(20));  // 20s

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

    void Spidercommand::show_routing_table_worker(std::string source_spider_ip,
                                                  std::string source_spider_ip_scope_id,
                                                  std::string destination_spider_ip)
    {
        int32_t ret = 0;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        int32_t client_sock = INVALID_SOCKET;
        std::shared_ptr<Client> client;
        std::pair<uint32_t, uint32_t> client_key;


        client = std::make_shared<Client>("show r",
                                          0,
                                          0,
                                          0,
                                          source_spider_ip,
                                          source_spider_ip_scope_id,
                                          "",
                                          "",
                                          destination_spider_ip,
                                          client_sock,
                                          SHOW_ROUTING_TABLE_WORKER_TV_SEC,
                                          SHOW_ROUTING_TABLE_WORKER_TV_USEC,
                                          SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_SEC,
                                          SHOW_ROUTING_TABLE_WORKER_FORWARDER_TV_USEC,
                                          encryption,
                                          message_manager);

        do
        {
            connection_id = generate_random_id();
            client_id = generate_random_id();
            ret = client_manager->insert_client(connection_id,
                                                client_id,
                                                client);
        }while(ret != 0);

        ret = client->do_socks5_connection_show_route();
        if(ret == -1)
        {
            client_manager->erase_client(client->get_connection_id(),
                                         client->get_client_id());
        }

        return;
    }

    void Spidercommand::show_routing_table()
    {
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        char check = 'n';


        while(1)
        {
            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                routing_manager->show_routing_table();

                break;
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                routing_manager->show_routing_table();
                std::printf("\n");

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("\n");
                std::printf("mode                      : %c\n", mode);
                std::printf("source spider ip          : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip     : %s\n", destination_spider_ip.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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


                    std::thread thread(&Spidercommand::show_routing_table_worker,
                                       this,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    std::this_thread::sleep_for(std::chrono::seconds(20));  // 20s

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

    void Spidercommand::edit_routing_table()
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

            std::printf("edit mode (add:a delete:d quit:q)              > ");
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

                std::printf("ip address                                     > ");
                std::cin >> ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(ip == spider_ip->get_spider_ipv4()
                   || ip == spider_ip->get_spider_ipv6_global()
                   || ip == spider_ip->get_spider_ipv6_unique_local()
                   || ip == spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] cannot input spider ipv4 and ipv6\n");
                    continue;
                }

                std::printf("metric (0 < metric <= %3d)                     > ", METRIC_MAX);
                std::cin >> metric;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(metric == 0 || metric > METRIC_MAX)
                {
                    std::printf("[-] input error\n");
                    continue;
                }

                pipe_manager->show_pipes_map();

                std::printf("pipe id                                        > ");
                std::cin >> pipe_id;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("\n");
                std::printf("edit mode                 : %c\n", edit_mode);
                std::printf("ip address                : %s\n", ip.c_str());
                std::printf("metric                    : %u\n", metric);
                std::printf("pipe id                   : %10u\n", pipe_id);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                std::printf("ip address                                     > ");
                std::cin >> ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(ip == spider_ip->get_spider_ipv4()
                   || ip == spider_ip->get_spider_ipv6_global()
                   || ip == spider_ip->get_spider_ipv6_unique_local()
                   || ip == spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] cannot input spider ipv4 and ipv6\n");
                    continue;
                }

                std::printf("\n");
                std::printf("edit mode                 : %c\n", edit_mode);
                std::printf("ip address                : %s\n", ip.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

    void Spidercommand::client_tcp_worker(std::shared_ptr<Client> client)
    {
        int32_t ret = 0;

        ret = client->do_socks5_connection_tcp();
        if(ret == -1)
        {
            if(client_manager->erase_client(client->get_connection_id(),
                                            client->get_client_id()))
            {
                closesocket(client->get_sock());
            }
        }

        return;
    }

    int Spidercommand::listen_client_tcp(std::string client_listen_ip,
                                         std::string client_listen_ip_scope_id,
                                         std::string client_listen_port,
                                         std::string destination_spider_ip,
                                         std::string target_ip,
                                         std::string target_port,
                                         int32_t tv_sec,
                                         int32_t tv_usec,
                                         int32_t forwarder_tv_sec,
                                         int32_t forwarder_tv_usec)
    {
        int ret = 0;
        uint32_t connection_id = 0;
        struct sockaddr_in client_listen_addr, client_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 client_listen_addr6, client_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *client_listen_addr_info;
        SOCKET client_listen_sock = INVALID_SOCKET;
        SOCKET client_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int client_addr_length = sizeof(client_addr);
        int client_addr6_length = sizeof(client_addr6);
        char client_listen_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *client_listen_addr6_string_pointer = client_listen_addr6_string;
        char client_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                client_listen_addr6.sin6_scope_id = (uint32_t)std::stoi(client_listen_ip_scope_id);
            }

            freeaddrinfo(client_listen_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(client_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            client_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr,
                       sizeof(client_listen_addr));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            std::printf("[+] listening port %d on %s\n",
                        ntohs(client_listen_addr.sin_port),
                        inet_ntoa(client_listen_addr.sin_addr));

            client_listen = std::make_shared<Client>("tcp",
                                                     0,
                                                     0,
                                                     0,
                                                     client_listen_ip,
                                                     "",
                                                     client_listen_port,
                                                     "",
                                                     destination_spider_ip,
                                                     target_ip,
                                                     target_port,
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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_length)) != INVALID_SOCKET)
            {
                // accept
#ifdef DEBUGPRINT
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(client_addr.sin_addr),
                            ntohs(client_addr.sin_port));
#endif

                uint32_t client_id = 0;
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                std::string client_port = std::to_string(ntohs(client_addr.sin_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("tcp",
                                                                          connection_id,
                                                                          0,
                                                                          0,
                                                                          client_ip,
                                                                          "",
                                                                          "",
                                                                          client_port,
                                                                          destination_spider_ip,
                                                                          target_ip,
                                                                          target_port,
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

                std::thread thread(&Spidercommand::client_tcp_worker,
                                   this,
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
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr6,
                       sizeof(client_listen_addr6));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &client_listen_addr6.sin6_addr,
                      client_listen_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

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

            client_listen = std::make_shared<Client>("tcp",
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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr6, (socklen_t *)&client_addr6_length)) != INVALID_SOCKET)
            {
                // accept
                inet_ntop(AF_INET6,
                          &client_addr6.sin6_addr,
                          client_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
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

                uint32_t client_id = 0;
                std::string client_ip;
                std::string client_ip_scope_id;
                std::string client_port;
                client_ip = client_addr6_string_pointer;
                client_ip_scope_id = std::to_string(client_addr6.sin6_scope_id);
                client_port = std::to_string(ntohs(client_addr6.sin6_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("tcp",
                                                                          connection_id,
                                                                          0,
                                                                          0,
                                                                          client_ip,
                                                                          client_ip_scope_id,
                                                                          "",
                                                                          client_port,
                                                                          destination_spider_ip,
                                                                          target_ip,
                                                                          target_port,
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

                std::thread thread(&Spidercommand::client_tcp_worker,
                                   this,
                                   client);
                thread.detach();
            }

            client_manager->erase_client(connection_id,
                                         0);
        }

        closesocket(client_listen_sock);
        return 0;
    }

    void Spidercommand::add_node_spider_client_tcp()
    {
        std::string config = "";
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string client_destination_spider_ip;
        std::string target_ip;      // ipv4, domainname, ipv6
        std::string target_port;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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

                if(tv_sec == 0 && tv_usec == 0)
                {
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
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                if(!client_listen_ip_scope_id.empty())
                {
                    std::printf("client listen ip scope id    : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
                }
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("target ip                    : %s\n", target_ip.c_str());
                std::printf("target port                  : %s\n", target_port.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    std::thread thread(&Spidercommand::listen_client_tcp,
                                       this,
                                       client_listen_ip,
                                       client_listen_ip_scope_id,
                                       client_listen_port,
                                       client_destination_spider_ip,
                                       target_ip,
                                       target_port,
                                       tv_sec,
                                       tv_usec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    thread.detach();

                    std::this_thread::sleep_for(std::chrono::seconds(2));  // 2s

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client listen ip                               > ");
                std::cin >> client_listen_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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

                if(tv_sec == 0 && tv_usec == 0)
                {
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
                std::printf("source spider ip             : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id    : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip        : %s\n", destination_spider_ip.c_str());
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("target ip                    : %s\n", target_ip.c_str());
                std::printf("target port                  : %s\n", target_port.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    config = "";
                    config += "[client_tcp]\n";

                    config += "client_listen_ip:";
                    config += client_listen_ip;
                    config += "\n";

                    config += "client_listen_port:";
                    config += client_listen_port;
                    config += "\n";

                    config += "client_destination_spider_ip:";
                    config += client_destination_spider_ip;
                    config += "\n";

                    config += "target_ip:";
                    config += target_ip;
                    config += "\n";

                    config += "target_port:";
                    config += target_port;
                    config += "\n";

                    config += "tv_sec:";
                    config += std::to_string(tv_sec);
                    config += "\n";

                    config += "tv_usec:";
                    config += std::to_string(tv_usec);
                    config += "\n";

                    config += "forwarder_tv_sec:";
                    config += std::to_string(forwarder_tv_sec);
                    config += "\n";

                    config += "forwarder_tv_usec:";
                    config += std::to_string(forwarder_tv_usec);
                    config += "\n";

                    std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                       this,
                                       config,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }

    void Spidercommand::client_udp_worker(std::string client_listen_ip,
                                          std::string client_listen_ip_scope_id,
                                          std::string client_listen_port,
                                          std::string destination_spider_ip,
                                          std::string target_ip,
                                          std::string target_port,
                                          int32_t tv_sec,
                                          int32_t tv_usec,
                                          int32_t forwarder_tv_sec,
                                          int32_t forwarder_tv_usec)
    {
        int32_t ret = 0;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        SOCKET client_udp_sock = INVALID_SOCKET;
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

    void Spidercommand::add_node_spider_client_udp()
    {
        std::string config = "";
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string client_destination_spider_ip;
        std::string target_ip;      // ipv4, domainname, ipv6
        std::string target_port;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                if(!client_listen_ip_scope_id.empty())
                {
                    std::printf("client listen ip scope id    : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
                }
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("target ip                    : %s\n", target_ip.c_str());
                std::printf("target port                  : %s\n", target_port.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("FORWARDER_UDP_TIMEOUT        : %7d sec\n", FORWARDER_UDP_TIMEOUT);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    std::thread thread(&Spidercommand::client_udp_worker,
                                       this,
                                       client_listen_ip,
                                       client_listen_ip_scope_id,
                                       client_listen_port,
                                       client_destination_spider_ip,
                                       target_ip,
                                       target_port,
                                       tv_sec,
                                       tv_usec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client listen ip                               > ");
                std::cin >> client_listen_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                std::printf("source spider ip             : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id    : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip        : %s\n", destination_spider_ip.c_str());
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("target ip                    : %s\n", target_ip.c_str());
                std::printf("target port                  : %s\n", target_port.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("FORWARDER_UDP_TIMEOUT        : %7d sec\n", FORWARDER_UDP_TIMEOUT);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    config = "";
                    config += "[client_udp]\n";

                    config += "client_listen_ip:";
                    config += client_listen_ip;
                    config += "\n";

                    config += "client_listen_port:";
                    config += client_listen_port;
                    config += "\n";

                    config += "client_destination_spider_ip:";
                    config += client_destination_spider_ip;
                    config += "\n";

                    config += "target_ip:";
                    config += target_ip;
                    config += "\n";

                    config += "target_port:";
                    config += target_port;
                    config += "\n";

                    config += "tv_sec:";
                    config += std::to_string(tv_sec);
                    config += "\n";

                    config += "tv_usec:";
                    config += std::to_string(tv_usec);
                    config += "\n";

                    config += "forwarder_tv_sec:";
                    config += std::to_string(forwarder_tv_sec);
                    config += "\n";

                    config += "forwarder_tv_usec:";
                    config += std::to_string(forwarder_tv_usec);
                    config += "\n";

                    std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                       this,
                                       config,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }
        return;
    }

    void Spidercommand::client_shell_worker(std::shared_ptr<Client> client)
    {
        int32_t ret = 0;

        ret = client->do_socks5_connection_shell();
        if(ret == -1)
        {
            if(client_manager->erase_client(client->get_connection_id(),
                client->get_client_id()))
            {
                closesocket(client->get_sock());
            }
        }

        return;
    }

    int Spidercommand::listen_client_shell(std::string client_listen_ip,
                                           std::string client_listen_ip_scope_id,
                                           std::string client_listen_port,
                                           std::string destination_spider_ip,
                                           int32_t tv_sec,
                                           int32_t tv_usec,
                                           int32_t forwarder_tv_sec,
                                           int32_t forwarder_tv_usec)
    {
        int ret = 0;
        uint32_t connection_id = 0;
        struct sockaddr_in client_listen_addr, client_addr;
        struct sockaddr_in *tmp_ipv4;
        struct sockaddr_in6 client_listen_addr6, client_addr6;
        struct sockaddr_in6 *tmp_ipv6;
        struct addrinfo hints;
        struct addrinfo *client_listen_addr_info;
        SOCKET client_listen_sock = INVALID_SOCKET;
        SOCKET client_sock = INVALID_SOCKET;
        int family = 0;
        char reuse = 1;
        int flags = 0;
        int client_addr_length = sizeof(client_addr);
        int client_addr6_length = sizeof(client_addr6);
        char client_listen_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *client_listen_addr6_string_pointer = client_listen_addr6_string;
        char client_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                std::printf("[-] cannot resolv the address: %s\n",
                            client_listen_ip.c_str());
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
                client_listen_addr6.sin6_scope_id = (uint32_t)std::stoi(client_listen_ip_scope_id);
            }

            freeaddrinfo(client_listen_addr_info);
        }else
        {
            std::printf("[-] not implemented\n");
            freeaddrinfo(client_listen_addr_info);
            return -1;
        }

        if(family == AF_INET)   // IPv4
        {
            client_listen_sock = socket(AF_INET,
                                        SOCK_STREAM,
                                        0);
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr,
                       sizeof(client_listen_addr));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            std::printf("[+] listening port %d on %s\n",
                        ntohs(client_listen_addr.sin_port),
                        inet_ntoa(client_listen_addr.sin_addr));

            client_listen = std::make_shared<Client>("shell",
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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_length)) != INVALID_SOCKET)
            {
                // accept
#ifdef DEBUGPRINT
                std::printf("[+] connected from ip:%s port:%d\n",
                            inet_ntoa(client_addr.sin_addr),
                            ntohs(client_addr.sin_port));
#endif

                uint32_t client_id = 0;
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                std::string client_port = std::to_string(ntohs(client_addr.sin_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("shell",
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

                std::thread thread(&Spidercommand::client_shell_worker,
                                   this,
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
            if(client_listen_sock == INVALID_SOCKET)
            {
                std::printf("[-] socket error: %d\n",
                            WSAGetLastError());
                return -1;
            }

            reuse = 1;
            setsockopt(client_listen_sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (char *)&reuse,
                       sizeof(int));

            // bind
            ret = bind(client_listen_sock,
                       (struct sockaddr *)&client_listen_addr6,
                       sizeof(client_listen_addr6));
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] bind error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            // listen
            listen(client_listen_sock,
                   5);
            if(ret == SOCKET_ERROR)
            {
                std::printf("[-] listen error: %d\n",
                            WSAGetLastError());
                closesocket(client_listen_sock);
                return -1;
            }

            inet_ntop(AF_INET6,
                      &client_listen_addr6.sin6_addr,
                      client_listen_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

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

            client_listen = std::make_shared<Client>("shell",
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

            while((client_sock = accept(client_listen_sock, (struct sockaddr *)&client_addr6, (socklen_t *)&client_addr6_length)) != INVALID_SOCKET)
            {
                // accept
                inet_ntop(AF_INET6,
                          &client_addr6.sin6_addr,
                          client_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);
#ifdef DEBUGPRINT
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

                uint32_t client_id = 0;
                std::string client_ip;
                std::string client_ip_scope_id;
                std::string client_port;
                client_ip = client_addr6_string_pointer;
                client_ip_scope_id = std::to_string(client_addr6.sin6_scope_id);
                client_port = std::to_string(ntohs(client_addr6.sin6_port));
                std::shared_ptr<Client> client = std::make_shared<Client>("shell",
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

                std::thread thread(&Spidercommand::client_shell_worker,
                                   this,
                                   client);
                thread.detach();
            }

            client_manager->erase_client(connection_id,
                                         0);
        }

        closesocket(client_listen_sock);
        return 0;
    }

    void Spidercommand::add_node_spider_client_shell()
    {
        std::string config = "";
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        std::string client_listen_ip;
        std::string client_listen_ip_scope_id;
        std::string client_listen_port;
        std::string client_destination_spider_ip;
        int32_t tv_sec = 0;
        int32_t tv_usec = 0;
        int32_t forwarder_tv_sec = 0;
        int32_t forwarder_tv_usec = 0;
        char check = 'n';


        while(1)
        {
            routing_manager->show_routing_table();
            std::printf("\n");

            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                    forwarder_tv_sec = 300;
                }else if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 300;
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
                    forwarder_tv_sec = 300;
                    forwarder_tv_usec = 0;
                }

                std::printf("\n");
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                if(!client_listen_ip_scope_id.empty())
                {
                    std::printf("client listen ip scope id    : %s (%d)\n", client_listen_ip_scope_id.c_str(), if_nametoindex(client_listen_ip_scope_id.c_str()));
                }
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    std::thread thread(&Spidercommand::listen_client_shell,
                                       this,
                                       client_listen_ip,
                                       client_listen_ip_scope_id,
                                       client_listen_port,
                                       client_destination_spider_ip,
                                       tv_sec,
                                       tv_usec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    thread.detach();

                    std::this_thread::sleep_for(std::chrono::seconds(2));  // 2s

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("client listen ip                               > ");
                std::cin >> client_listen_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
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

                std::printf("client destination spider ip                   > ");
                std::cin >> client_destination_spider_ip;
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
                    forwarder_tv_sec = 300;
                }else if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 300;
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
                    forwarder_tv_sec = 300;
                    forwarder_tv_usec = 0;
                }

                std::printf("\n");
                std::printf("source spider ip             : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id    : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip        : %s\n", destination_spider_ip.c_str());
                std::printf("client listen ip             : %s\n", client_listen_ip.c_str());
                std::printf("client listen port           : %s\n", client_listen_port.c_str());
                std::printf("client destination spider ip : %s\n", client_destination_spider_ip.c_str());
                std::printf("recv/send tv_sec             : %7d sec\n", tv_sec);
                std::printf("recv/send tv_usec            : %7d microsec\n", tv_usec);
                std::printf("forwarder_tv_sec             : %7d sec\n", forwarder_tv_sec);
                std::printf("forwarder_tv_usec            : %7d microsec\n", forwarder_tv_usec);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    config = "";
                    config += "[client_shell]\n";

                    config += "client_listen_ip:";
                    config += client_listen_ip;
                    config += "\n";

                    config += "client_listen_port:";
                    config += client_listen_port;
                    config += "\n";

                    config += "client_destination_spider_ip:";
                    config += client_destination_spider_ip;
                    config += "\n";

                    config += "tv_sec:";
                    config += std::to_string(tv_sec);
                    config += "\n";

                    config += "tv_usec:";
                    config += std::to_string(tv_usec);
                    config += "\n";

                    config += "forwarder_tv_sec:";
                    config += std::to_string(forwarder_tv_sec);
                    config += "\n";

                    config += "forwarder_tv_usec:";
                    config += std::to_string(forwarder_tv_usec);
                    config += "\n";

                    std::thread thread(&Spidercommand::add_node_to_destination_spider_worker,
                                       this,
                                       config,
                                       source_spider_ip,
                                       source_spider_ip_scope_id,
                                       destination_spider_ip);
                    thread.detach();

                    break;
                }else if(check == 'n')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }else if(check == 'q')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }else
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return;
                }
            }else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }

    void Spidercommand::add_node_to_destination_spider_worker(std::string config,
                                                              std::string source_spider_ip,
                                                              std::string source_spider_ip_scope_id,
                                                              std::string destination_spider_ip)
    {
        int32_t ret = 0;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        int32_t client_sock = INVALID_SOCKET;
        std::shared_ptr<Client> client;
        std::pair<uint32_t, uint32_t> client_key;


        client = std::make_shared<Client>("add",
                                          0,
                                          0,
                                          0,
                                          source_spider_ip,
                                          source_spider_ip_scope_id,
                                          "",
                                          "",
                                          destination_spider_ip,
                                          client_sock,
                                          ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_SEC,
                                          ADD_NODE_TO_DESTINATION_SPIDER_WORKER_TV_USEC,
                                          ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_SEC,
                                          ADD_NODE_TO_DESTINATION_SPIDER_WORKER_FORWARDER_TV_USEC,
                                          encryption,
                                          message_manager);

        do
        {
            connection_id = generate_random_id();
            client_id = generate_random_id();
            ret = client_manager->insert_client(connection_id,
                                                client_id,
                                                client);
        }while(ret != 0);

        ret = client->do_socks5_connection_add_node(config);
        if(ret == -1)
        {
            client_manager->erase_client(client->get_connection_id(),
                                         client->get_client_id());
        }

        return;
    }

    std::string Spidercommand::get_line_value(std::string line,
                                              std::string name)
    {
        std::string value;
        size_t pos;


        pos = line.find(name.c_str()) + name.size();
        value = line.substr(pos);
        value.erase(0,
                    value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        return value;
    }

    std::string Spidercommand::get_line(char *data,
                                        std::size_t data_size,
                                        char **line_start,
                                        char **line_end)
    {
        char line_tmp[100] = {0};
        std::string line;


        while(1)
        {
            if(*line_start == *line_end)
            {
                return line;
            }

            *line_end = std::find(static_cast<char*>(*line_start), static_cast<char*>(data + data_size), '\n');
            if(*line_end - *line_start >= 100)
            {
                return line;
            }

            memset(line_tmp,
                   0,
                   100);
            memcpy(line_tmp,
                   *line_start,
                   *line_end - *line_start);
            line = line_tmp;

            if(*line_end == data + data_size)
            {
                *line_start = *line_end;
            }else
            {
                *line_start = *line_end + 1;
            }

            if(line.empty()
                || (line.rfind("//", 0) == 0))
            {
                continue;
            }else
            {
                break;
            }
        }

        return line;
    }

    int32_t Spidercommand::read_config(std::vector<char> config)
    {
        char *line_start = config.data();
        char *line_end = nullptr;
        std::string line;


        line = get_line(config.data(),
                        config.size(),
                        &line_start,
                        &line_end);
        if(line.empty())
        {
            return -1;
        }

        if(line == "[client]")
        {
            std::string client_listen_ip;
            std::string client_listen_ip_scope_id;
            std::string client_listen_port;
            std::string client_destination_spider_ip;
            std::string tv_sec_string;
            std::string tv_usec_string;
            std::string forwarder_tv_sec_string;
            std::string forwarder_tv_usec_string;
            int32_t tv_sec = 0;
            int32_t tv_usec = 0;
            int32_t forwarder_tv_sec = 0;
            int32_t forwarder_tv_usec = 0;


            // client_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_ip:") != std::string::npos)
            {
                client_listen_ip = get_line_value(line,
                                                  "client_listen_ip:");
            }

            if(client_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [client_listen_ip] error\n");
#endif
                return -1;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6_global()
               && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // client_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_port:") != std::string::npos)
            {
                client_listen_port = get_line_value(line,
                                                    "client_listen_port:");
            }

            if(client_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [client_listen_port] error\n");
#endif
                return -1;
            }


            // client_destination_spider_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("client_destination_spider_ip:") != std::string::npos)
            {
                client_destination_spider_ip = get_line_value(line,
                                                              "client_destination_spider_ip:");
            }

            if(client_destination_spider_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [client_destination_spider_ip] error\n");
#endif
                return -1;
            }


            // tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("tv_sec:") != std::string::npos)
            {
                tv_sec_string = get_line_value(line,
                                               "tv_sec:");
            }

            if(tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [tv_sec] error\n");
#endif
                return -1;
            }


            // tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("tv_usec:") != std::string::npos)
            {
                tv_usec_string = get_line_value(line,
                                                "tv_usec:");
            }

            if(tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [tv_usec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_sec:") != std::string::npos)
            {
                forwarder_tv_sec_string = get_line_value(line,
                    "forwarder_tv_sec:");
            }

            if(forwarder_tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [forwarder_tv_sec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_usec:") != std::string::npos)
            {
                forwarder_tv_usec_string = get_line_value(line,
                    "forwarder_tv_usec:");
            }

            if(forwarder_tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client] [forwarder_tv_usec] error\n");
#endif
                return -1;
            }


            tv_sec = std::stoi(tv_sec_string);
            tv_usec = std::stoi(tv_usec_string);
            forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
            forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

            if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }


            std::thread thread(&Spidercommand::listen_client,
                               this,
                               client_listen_ip,
                               client_listen_ip_scope_id,
                               client_listen_port,
                               client_destination_spider_ip,
                               tv_sec,
                               tv_usec,
                               forwarder_tv_sec,
                               forwarder_tv_usec);
            thread.detach();
        }else if(line == "[pipe_client]")
        {
            char mode = 'c';
            std::string pipe_ip;
            std::string pipe_ip_scope_id;
            std::string pipe_destination_ip;
            std::string pipe_destination_ip_scope_id;
            std::string pipe_destination_port;


            // pipe_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_ip:") != std::string::npos)
            {
                pipe_ip = get_line_value(line,
                                         "pipe_ip:");
            }

            if(pipe_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] [pipe_ip] error\n");
#endif
                return -1;
            }

            if(pipe_ip != spider_ip->get_spider_ipv4()
               && pipe_ip != spider_ip->get_spider_ipv6_global()
               && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
               && pipe_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] [pipe_ip] please input spider ipv4 or ipv6: %s\n",
                            pipe_ip.c_str());
#endif
                return -1;
            }

            if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
            {
                pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // pipe_destination_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] error\n");
#endif
                return -1;
            }


            if(line.find("pipe_destination_ip:") != std::string::npos)
            {
                pipe_destination_ip = get_line_value(line,
                                                     "pipe_destination_ip:");
            }

            if(pipe_destination_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] [pipe_destination_ip] error\n");
#endif
                return -1;
            }


            // pipe_destination_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_destination_port:") != std::string::npos)
            {
                pipe_destination_port = get_line_value(line,
                                                       "pipe_destination_port:");
            }

            if(pipe_destination_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client] [pipe_destination_port] error\n");
#endif
                return -1;
            }


            std::thread thread(&Spidercommand::connect_pipe,
                               this,
                               mode,
                               pipe_ip,
                               pipe_ip_scope_id,
                               pipe_destination_ip,
                               pipe_destination_port);
            thread.detach();
        }else if(line == "[pipe_client_http]")
        {
            char mode = 'c';
            BOOL proxy_flag = false;
            std::string proxy_flag_string;
            std::string forward_proxy_type;
            int32_t forward_proxy_authentication;
            std::string forward_proxy_authentication_string;
            std::string forward_proxy_ip;
            std::string forward_proxy_port;
            std::string forward_proxy_username;
            std::string forward_proxy_password;
            std::string forward_proxy_nthash_hexstring;
            std::string forward_proxy_domainname;
            std::string forward_proxy_workstationname;
            std::string forward_proxy_service_principal_name;
            BOOL tls_flag = false;
            std::string tls_flag_string;
            std::string message_mode;
            std::string pipe_ip;
            std::string pipe_ip_scope_id;
            std::string pipe_destination_ip;
            std::string pipe_destination_ip_scope_id;
            std::string pipe_destination_port;
            std::string sleep_ms_string;
            int32_t sleep_ms = 0;


            // proxy_flag
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("proxy_flag:") != std::string::npos)
            {
                proxy_flag_string = get_line_value(line,
                                                   "proxy_flag:");
            }

            if(proxy_flag_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [proxy_flag] error\n");
#endif
                return -1;
            }

            if(proxy_flag_string == "true")
            {
                proxy_flag = true;
            }else
            {
                proxy_flag = false;
            }


            // forward_proxy_type
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_type:") != std::string::npos)
            {
                forward_proxy_type = get_line_value(line,
                                                    "forward_proxy_type:");
            }


            // forward_proxy_authentication
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }

            if(line.find("forward_proxy_authentication:") != std::string::npos)
            {
                forward_proxy_authentication_string = get_line_value(line,
                                                                     "forward_proxy_authentication:");
            }

            if(forward_proxy_authentication_string.empty())
            {
                forward_proxy_authentication = 0;
            }else
            {
                forward_proxy_authentication = std::stoi(forward_proxy_authentication_string);

                if(forward_proxy_authentication < 0 || forward_proxy_authentication > 4)
                {
                    forward_proxy_authentication = 0;
                }
            }


            // forward_proxy_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_ip:") != std::string::npos)
            {
                forward_proxy_ip = get_line_value(line,
                                                  "forward_proxy_ip:");
            }


            // forward_proxy_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_port:") != std::string::npos)
            {
                forward_proxy_port = get_line_value(line,
                                                    "forward_proxy_port:");
            }


            // forward_proxy_username
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_username:") != std::string::npos)
            {
                forward_proxy_username = get_line_value(line,
                                                        "forward_proxy_username:");
            }


            // forward_proxy_password
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_password:") != std::string::npos)
            {
                forward_proxy_password = get_line_value(line,
                                                        "forward_proxy_password:");
            }


            // forward_proxy_nthash_hexstring
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_nthash_hexstring:") != std::string::npos)
            {
                forward_proxy_nthash_hexstring = get_line_value(line,
                                                                "forward_proxy_nthash_hexstring:");
            }


            // forward_proxy_domainname
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_domainname:") != std::string::npos)
            {
                forward_proxy_domainname = get_line_value(line,
                                                          "forward_proxy_domainname:");
            }


            // forward_proxy_workstationname
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_workstationname:") != std::string::npos)
            {
                forward_proxy_workstationname = get_line_value(line,
                                                               "forward_proxy_workstationname:");
            }


            // forward_proxy_service_principal_name
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("forward_proxy_service_principal_name:") != std::string::npos)
            {
                forward_proxy_service_principal_name = get_line_value(line,
                                                                      "forward_proxy_service_principal_name:");
            }


            // tls_flag
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("tls_flag:") != std::string::npos)
            {
                tls_flag_string = get_line_value(line,
                                                 "tls_flag:");
            }

            if(tls_flag_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [tls_flag] error\n");
#endif
                return -1;
            }

            if(tls_flag_string == "true")
            {
                tls_flag = true;
            }else
            {
                tls_flag = false;
            }


            // message_mode
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("message_mode:") != std::string::npos)
            {
                message_mode = get_line_value(line,
                                              "message_mode:");
            }

            if(message_mode.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [message_mode] error\n");
#endif
                return -1;
            }


            // pipe_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_ip:") != std::string::npos)
            {
                pipe_ip = get_line_value(line,
                                         "pipe_ip:");
            }

            if(pipe_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [pipe_ip] error\n");
#endif
                return -1;
            }

            if(pipe_ip != spider_ip->get_spider_ipv4()
               && pipe_ip != spider_ip->get_spider_ipv6_global()
               && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
               && pipe_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [pipe_ip] please input spider ipv4 or ipv6: %s\n",
                            pipe_ip.c_str());
#endif
                return -1;
            }

            if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
            {
                pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // pipe_destination_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }


            if(line.find("pipe_destination_ip:") != std::string::npos)
            {
                pipe_destination_ip = get_line_value(line,
                                                     "pipe_destination_ip:");
            }

            if(pipe_destination_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [pipe_destination_ip] error\n");
#endif
                return -1;
            }


            // pipe_destination_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_destination_port:") != std::string::npos)
            {
                pipe_destination_port = get_line_value(line,
                                                       "pipe_destination_port:");
            }

            if(pipe_destination_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [pipe_destination_port] error\n");
#endif
                return -1;
            }


            // sleep_ms
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] error\n");
#endif
                return -1;
            }

            if(line.find("sleep_ms:") != std::string::npos)
            {
                sleep_ms_string = get_line_value(line,
                                                 "sleep_ms:");
            }


            if(sleep_ms_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_client_http] [sleep_ms] error\n");
#endif
                return -1;
            }

            sleep_ms = std::stoi(sleep_ms_string);

            if(sleep_ms < 0 || sleep_ms > 1000)
            {
                sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
            }


            std::thread thread(&Spidercommand::connect_pipe_http,
                               this,
                               mode,
                               proxy_flag,
                               forward_proxy_type,
                               forward_proxy_authentication,
                               forward_proxy_ip,
                               forward_proxy_port,
                               forward_proxy_username,
                               forward_proxy_password,
                               forward_proxy_nthash_hexstring,
                               forward_proxy_domainname,
                               forward_proxy_workstationname,
                               forward_proxy_service_principal_name,
                               tls_flag,
                               message_mode,
                               pipe_ip,
                               pipe_ip_scope_id,
                               pipe_destination_ip,
                               pipe_destination_port,
                               sleep_ms);
            thread.detach();
        }else if(line == "[pipe_server]")
        {
            char mode = 's';
            std::string pipe_listen_ip;
            std::string pipe_listen_ip_scope_id;
            std::string pipe_listen_port;


            // pipe_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_listen_ip:") != std::string::npos)
            {
                pipe_listen_ip = get_line_value(line,
                                                "pipe_listen_ip:");
            }

            if(pipe_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server] [pipe_listen_ip] error\n");
#endif
                return -1;
            }

            if(pipe_listen_ip != spider_ip->get_spider_ipv4()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_global()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server] [pipe_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            pipe_listen_ip.c_str());
#endif
                return -1;
            }

            if(pipe_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                pipe_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // pipe_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_listen_port:") != std::string::npos)
            {
                pipe_listen_port = get_line_value(line,
                                                  "pipe_listen_port:");
            }

            if(pipe_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server] [pipe_listen_port] error\n");
#endif
                return -1;
            }


            std::thread thread(&Spidercommand::listen_pipe,
                               this,
                               mode,
                               pipe_listen_ip,
                               pipe_listen_ip_scope_id,
                               pipe_listen_port);
            thread.detach();
        }else if(line == "[pipe_server_http]")
        {
            char mode = 's';
            BOOL tls_flag = false;
            std::string tls_flag_string;
            std::string message_mode;
            std::string pipe_listen_ip;
            std::string pipe_listen_ip_scope_id;
            std::string pipe_listen_port;


            // tls_flag
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] error\n");
#endif
                return -1;
            }


            if(line.find("tls_flag:") != std::string::npos)
            {
                tls_flag_string = get_line_value(line,
                                                 "tls_flag:");
            }

            if(tls_flag_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] [tls_flag] error\n");
#endif
                return -1;
            }

            if(tls_flag_string == "true")
            {
                tls_flag = true;
            }else
            {
                tls_flag = false;
            }


            // message_mode
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] error\n");
#endif
                return -1;
            }

            if(line.find("message_mode:") != std::string::npos)
            {
                message_mode = get_line_value(line,
                                              "message_mode:");
            }

            if(message_mode.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] [message_mode] error\n");
#endif
                return -1;
            }


            // pipe_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_listen_ip:") != std::string::npos)
            {
                pipe_listen_ip = get_line_value(line,
                                                "pipe_listen_ip:");
            }

            if(pipe_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] [pipe_listen_ip] error\n");
#endif
                return -1;
            }

            if(pipe_listen_ip != spider_ip->get_spider_ipv4()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_global()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && pipe_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] [pipe_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            pipe_listen_ip.c_str());
#endif
                return -1;
            }

            if(pipe_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                pipe_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // pipe_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] error\n");
#endif
                return -1;
            }

            if(line.find("pipe_listen_port:") != std::string::npos)
            {
                pipe_listen_port = get_line_value(line,
                                                  "pipe_listen_port:");
            }

            if(pipe_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [pipe_server_http] [pipe_listen_port] error\n");
#endif
                return -1;
            }


            std::thread thread(&Spidercommand::listen_pipe_http,
                               this,
                               mode,
                               tls_flag,
                               message_mode,
                               pipe_listen_ip,
                               pipe_listen_ip_scope_id,
                               pipe_listen_port);
            thread.detach();
        }else if(line == "[client_tcp]")
        {
            std::string client_listen_ip;
            std::string client_listen_ip_scope_id;
            std::string client_listen_port;
            std::string client_destination_spider_ip;
            std::string target_ip;
            std::string target_port;
            std::string tv_sec_string;
            std::string tv_usec_string;
            std::string forwarder_tv_sec_string;
            std::string forwarder_tv_usec_string;
            int32_t tv_sec = 0;
            int32_t tv_usec = 0;
            int32_t forwarder_tv_sec = 0;
            int32_t forwarder_tv_usec = 0;


            // client_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_ip:") != std::string::npos)
            {
                client_listen_ip = get_line_value(line,
                                                  "client_listen_ip:");
            }

            if(client_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [client_listen_ip] error\n");
#endif
                return -1;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6_global()
               && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // client_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_port:") != std::string::npos)
            {
                client_listen_port = get_line_value(line,
                                                    "client_listen_port:");
            }

            if(client_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [client_listen_port] error\n");
#endif
                return -1;
            }


            // client_destination_spider_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("client_destination_spider_ip:") != std::string::npos)
            {
                client_destination_spider_ip = get_line_value(line,
                                                              "client_destination_spider_ip:");
            }

            if(client_destination_spider_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [client_destination_spider_ip] error\n");
#endif
                return -1;
            }


            // target_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("target_ip:") != std::string::npos)
            {
                target_ip = get_line_value(line,
                                           "target_ip:");
            }

            if(target_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [target_ip] error\n");
#endif
                return -1;
            }

            if(target_ip.size() >= 256)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [target_ip] size error\n");
#endif
                return -1;
            }


            // target_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("target_port:") != std::string::npos)
            {
                target_port = get_line_value(line,
                                             "target_port:");
            }

            if(target_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [target_port] error\n");
#endif
                return -1;
            }


            // tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("tv_sec:") != std::string::npos)
            {
                tv_sec_string = get_line_value(line,
                                               "tv_sec:");
            }

            if(tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [tv_sec] error\n");
#endif
                return -1;
            }


            // tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("tv_usec:") != std::string::npos)
            {
                tv_usec_string = get_line_value(line,
                                                "tv_usec:");
            }

            if(tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [tv_usec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_sec:") != std::string::npos)
            {
                forwarder_tv_sec_string = get_line_value(line,
                                                         "forwarder_tv_sec:");
            }

            if(forwarder_tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [forwarder_tv_sec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_usec:") != std::string::npos)
            {
                forwarder_tv_usec_string = get_line_value(line,
                                                          "forwarder_tv_usec:");
            }

            if(forwarder_tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_tcp] [forwarder_tv_usec] error\n");
#endif
                return -1;
            }


            tv_sec = std::stoi(tv_sec_string);
            tv_usec = std::stoi(tv_usec_string);
            forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
            forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

            if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }


            std::thread thread(&Spidercommand::listen_client_tcp,
                               this,
                               client_listen_ip,
                               client_listen_ip_scope_id,
                               client_listen_port,
                               client_destination_spider_ip,
                               target_ip,
                               target_port,
                               tv_sec,
                               tv_usec,
                               forwarder_tv_sec,
                               forwarder_tv_usec);
            thread.detach();
        }else if(line == "[client_udp]")
        {
            std::string client_listen_ip;
            std::string client_listen_ip_scope_id;
            std::string client_listen_port;
            std::string client_destination_spider_ip;
            std::string target_ip;
            std::string target_port;
            std::string tv_sec_string;
            std::string tv_usec_string;
            std::string forwarder_tv_sec_string;
            std::string forwarder_tv_usec_string;
            int32_t tv_sec = 0;
            int32_t tv_usec = 0;
            int32_t forwarder_tv_sec = 0;
            int32_t forwarder_tv_usec = 0;


            // client_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_ip:") != std::string::npos)
            {
                client_listen_ip = get_line_value(line,
                                                  "client_listen_ip:");
            }

            if(client_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [client_listen_ip] error\n");
#endif
                return -1;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6_global()
               && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // client_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_port:") != std::string::npos)
            {
                client_listen_port = get_line_value(line,
                                                    "client_listen_port:");
            }

            if(client_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [client_listen_port] error\n");
#endif
                return -1;
            }


            // client_destination_spider_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("client_destination_spider_ip:") != std::string::npos)
            {
                client_destination_spider_ip = get_line_value(line,
                                                              "client_destination_spider_ip:");
            }

            if(client_destination_spider_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [client_destination_spider_ip] error\n");
#endif
                return -1;
            }


            // target_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("target_ip:") != std::string::npos)
            {
                target_ip = get_line_value(line,
                                           "target_ip:");
            }

            if(target_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [target_ip] error\n");
#endif
                return -1;
            }

            if(target_ip.size() >= 256)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [target_ip] size error\n");
#endif
                return -1;
            }


            // target_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("target_port:") != std::string::npos)
            {
                target_port = get_line_value(line,
                                             "target_port:");
            }

            if(target_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [target_port] error\n");
#endif
                return -1;
            }


            // tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("tv_sec:") != std::string::npos)
            {
                tv_sec_string = get_line_value(line,
                                               "tv_sec:");
            }

            if(tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [tv_sec] error\n");
#endif
                return -1;
            }


            // tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("tv_usec:") != std::string::npos)
            {
                tv_usec_string = get_line_value(line,
                                                "tv_usec:");
            }

            if(tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [tv_usec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_sec:") != std::string::npos)
            {
                forwarder_tv_sec_string = get_line_value(line,
                                                         "forwarder_tv_sec:");
            }

            if(forwarder_tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [forwarder_tv_sec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_usec:") != std::string::npos)
            {
                forwarder_tv_usec_string = get_line_value(line,
                                                          "forwarder_tv_usec:");
            }

            if(forwarder_tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_udp] [forwarder_tv_usec] error\n");
#endif
                return -1;
            }


            tv_sec = std::stoi(tv_sec_string);
            tv_usec = std::stoi(tv_usec_string);
            forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
            forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

            if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }


            std::thread thread(&Spidercommand::client_udp_worker,
                               this,
                               client_listen_ip,
                               client_listen_ip_scope_id,
                               client_listen_port,
                               client_destination_spider_ip,
                               target_ip,
                               target_port,
                               tv_sec,
                               tv_usec,
                               forwarder_tv_sec,
                               forwarder_tv_usec);
            thread.detach();
        }else if(line == "[client_shell]")
        {
            std::string client_listen_ip;
            std::string client_listen_ip_scope_id;
            std::string client_listen_port;
            std::string client_destination_spider_ip;
            std::string tv_sec_string;
            std::string tv_usec_string;
            std::string forwarder_tv_sec_string;
            std::string forwarder_tv_usec_string;
            int32_t tv_sec = 0;
            int32_t tv_usec = 0;
            int32_t forwarder_tv_sec = 0;
            int32_t forwarder_tv_usec = 0;


            // client_listen_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_ip:") != std::string::npos)
            {
                client_listen_ip = get_line_value(line,
                                                  "client_listen_ip:");
            }

            if(client_listen_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [client_listen_ip] error\n");
#endif
                return -1;
            }

            if(client_listen_ip != spider_ip->get_spider_ipv4()
               && client_listen_ip != spider_ip->get_spider_ipv6_global()
               && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
               && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                            client_listen_ip.c_str());
#endif
                return -1;
            }

            if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
            {
                client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
            }


            // client_listen_port
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("client_listen_port:") != std::string::npos)
            {
                client_listen_port = get_line_value(line,
                                                    "client_listen_port:");
            }

            if(client_listen_port.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [client_listen_port] error\n");
#endif
                return -1;
            }


            // client_destination_spider_ip
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("client_destination_spider_ip:") != std::string::npos)
            {
                client_destination_spider_ip = get_line_value(line,
                                                              "client_destination_spider_ip:");
            }

            if(client_destination_spider_ip.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [client_destination_spider_ip] error\n");
#endif
                return -1;
            }


            // tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("tv_sec:") != std::string::npos)
            {
                tv_sec_string = get_line_value(line,
                                               "tv_sec:");
            }

            if(tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [tv_sec] error\n");
#endif
                return -1;
            }


            // tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("tv_usec:") != std::string::npos)
            {
                tv_usec_string = get_line_value(line,
                                                "tv_usec:");
            }

            if(tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [tv_usec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_sec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_sec:") != std::string::npos)
            {
                forwarder_tv_sec_string = get_line_value(line,
                                                         "forwarder_tv_sec:");
            }

            if(forwarder_tv_sec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [forwarder_tv_sec] error\n");
#endif
                return -1;
            }


            // forwarder_tv_usec
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] error\n");
#endif
                return -1;
            }

            if(line.find("forwarder_tv_usec:") != std::string::npos)
            {
                forwarder_tv_usec_string = get_line_value(line,
                                                          "forwarder_tv_usec:");
            }

            if(forwarder_tv_usec_string.empty())
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client_shell] [forwarder_tv_usec] error\n");
#endif
                return -1;
            }


            tv_sec = std::stoi(tv_sec_string);
            tv_usec = std::stoi(tv_usec_string);
            forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
            forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

            if(tv_sec < 0 || tv_sec > 60)
            {
                tv_sec = 3;
            }

            if(tv_usec < 0 || tv_usec > 1000000)
            {
                tv_usec = 0;
            }

            if(tv_sec == 0 && tv_usec == 0){
                tv_sec = 3;
                tv_usec = 0;
            }

            if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
            {
                forwarder_tv_sec = 30;
            }

            if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
            {
                forwarder_tv_usec = 0;
            }

            if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
            {
                forwarder_tv_sec = 30;
                forwarder_tv_usec = 0;
            }


            std::thread thread(&Spidercommand::listen_client_shell,
                               this,
                               client_listen_ip,
                               client_listen_ip_scope_id,
                               client_listen_port,
                               client_destination_spider_ip,
                               tv_sec,
                               tv_usec,
                               forwarder_tv_sec,
                               forwarder_tv_usec);
            thread.detach();
        }

        return 0;
    }

    int32_t Spidercommand::read_config_file(std::string config_file)
    {
        std::ifstream file(config_file);
        std::vector<char> config;
        std::streamsize file_size;
        char *line_start;
        char *line_end;
        std::string line;


        if(!file.is_open())
        {
            std::printf("[-] cannot open config file: %s\n",
                        config_file.c_str());
            return -1;
        }

        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        config.resize(file_size);

        if(!file.read(config.data(), file_size))
        {
            std::printf("[-] cannot read config file: %s\n",
                        config_file.c_str());
            return -1;
        }

        file.close();

        line_start = config.data();
        line_end = nullptr;

        while(1)
        {
            line = get_line(config.data(),
                            config.size(),
                            &line_start,
                            &line_end);
            if(line.empty())
            {
                break;
            }

            if(line == "[client]")
            {
                std::string client_listen_ip;
                std::string client_listen_ip_scope_id;
                std::string client_listen_port;
                std::string client_destination_spider_ip;
                std::string tv_sec_string;
                std::string tv_usec_string;
                std::string forwarder_tv_sec_string;
                std::string forwarder_tv_usec_string;
                int32_t tv_sec = 0;
                int32_t tv_usec = 0;
                int32_t forwarder_tv_sec = 0;
                int32_t forwarder_tv_usec = 0;


                // client_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("client_listen_ip:") != std::string::npos)
                {
                    client_listen_ip = get_line_value(line,
                                                      "client_listen_ip:");
                }

                if(client_listen_ip.empty())
                {
                    std::printf("[-] [client] [client_listen_ip] error\n");
                    break;
                }

                if(client_listen_ip != spider_ip->get_spider_ipv4()
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [client] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                client_listen_ip.c_str());
                    break;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // client_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("client_listen_port:") != std::string::npos)
                {
                    client_listen_port = get_line_value(line,
                                                        "client_listen_port:");
                }

                if(client_listen_port.empty())
                {
                    std::printf("[-] [client] [client_listen_port] error\n");
                    break;
                }


                // client_destination_spider_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("client_destination_spider_ip:") != std::string::npos)
                {
                    client_destination_spider_ip = get_line_value(line,
                                                                  "client_destination_spider_ip:");
                }

                if(client_destination_spider_ip.empty())
                {
                    std::printf("[-] [client] [client_destination_spider_ip] error\n");
                    break;
                }


                // tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("tv_sec:") != std::string::npos)
                {
                    tv_sec_string = get_line_value(line,
                                                   "tv_sec:");
                }

                if(tv_sec_string.empty())
                {
                    std::printf("[-] [client] [tv_sec] error\n");
                    break;
                }


                // tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("tv_usec:") != std::string::npos)
                {
                    tv_usec_string = get_line_value(line,
                                                    "tv_usec:");
                }

                if(tv_usec_string.empty())
                {
                    std::printf("[-] [client] [tv_usec] error\n");
                    break;
                }


                // forwarder_tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("forwarder_tv_sec:") != std::string::npos)
                {
                    forwarder_tv_sec_string = get_line_value(line,
                        "forwarder_tv_sec:");
                }

                if(forwarder_tv_sec_string.empty())
                {
                    std::printf("[-] [client] [forwarder_tv_sec] error\n");
                    break;
                }


                // forwarder_tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client] error\n");
                    break;
                }

                if(line.find("forwarder_tv_usec:") != std::string::npos)
                {
                    forwarder_tv_usec_string = get_line_value(line,
                        "forwarder_tv_usec:");
                }

                if(forwarder_tv_usec_string.empty())
                {
                    std::printf("[-] [client] [forwarder_tv_usec] error\n");
                    break;
                }


                tv_sec = std::stoi(tv_sec_string);
                tv_usec = std::stoi(tv_usec_string);
                forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
                forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

                if(tv_sec < 0 || tv_sec > 60)
                {
                    tv_sec = 3;
                }

                if(tv_usec < 0 || tv_usec > 1000000)
                {
                    tv_usec = 0;
                }

                if(tv_sec == 0 && tv_usec == 0){
                    tv_sec = 3;
                    tv_usec = 0;
                }

                if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 30;
                }

                if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
                {
                    forwarder_tv_usec = 0;
                }

                if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
                {
                    forwarder_tv_sec = 30;
                    forwarder_tv_usec = 0;
                }


                std::thread thread(&Spidercommand::listen_client,
                                   this,
                                   client_listen_ip,
                                   client_listen_ip_scope_id,
                                   client_listen_port,
                                   client_destination_spider_ip,
                                   tv_sec,
                                   tv_usec,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                thread.detach();
            }else if(line == "[pipe_client]")
            {
                char mode = 'c';
                std::string pipe_ip;
                std::string pipe_ip_scope_id;
                std::string pipe_destination_ip;
                std::string pipe_destination_ip_scope_id;
                std::string pipe_destination_port;


                // pipe_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client] error\n");
                    break;
                }

                if(line.find("pipe_ip:") != std::string::npos)
                {
                    pipe_ip = get_line_value(line,
                                             "pipe_ip:");
                }

                if(pipe_ip.empty())
                {
                    std::printf("[-] [pipe_client] [pipe_ip] error\n");
                    break;
                }

                if(pipe_ip != spider_ip->get_spider_ipv4()
                   && pipe_ip != spider_ip->get_spider_ipv6_global()
                   && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
                   && pipe_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [pipe_client] [pipe_ip] please input spider ipv4 or ipv6: %s\n",
                                pipe_ip.c_str());
                    break;
                }

                if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // pipe_destination_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client] error\n");
                    break;
                }


                if(line.find("pipe_destination_ip:") != std::string::npos)
                {
                    pipe_destination_ip = get_line_value(line,
                                                         "pipe_destination_ip:");
                }

                if(pipe_destination_ip.empty())
                {
                    std::printf("[-] [pipe_client] [pipe_destination_ip] error\n");
                    break;
                }


                // pipe_destination_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client] error\n");
                    break;
                }

                if(line.find("pipe_destination_port:") != std::string::npos)
                {
                    pipe_destination_port = get_line_value(line,
                                                           "pipe_destination_port:");
                }

                if(pipe_destination_port.empty())
                {
                    std::printf("[-] [pipe_client] [pipe_destination_port] error\n");
                    break;
                }


                std::thread thread(&Spidercommand::connect_pipe,
                                   this,
                                   mode,
                                   pipe_ip,
                                   pipe_ip_scope_id,
                                   pipe_destination_ip,
                                   pipe_destination_port);
                thread.detach();
            }else if(line == "[pipe_client_http]")
            {
                char mode = 'c';
                BOOL proxy_flag = false;
                std::string proxy_flag_string;
                std::string forward_proxy_type;
                int32_t forward_proxy_authentication;
                std::string forward_proxy_authentication_string;
                std::string forward_proxy_ip;
                std::string forward_proxy_port;
                std::string forward_proxy_username;
                std::string forward_proxy_password;
                std::string forward_proxy_nthash_hexstring;
                std::string forward_proxy_domainname;
                std::string forward_proxy_workstationname;
                std::string forward_proxy_service_principal_name;
                BOOL tls_flag = false;
                std::string tls_flag_string;
                std::string message_mode;
                std::string pipe_ip;
                std::string pipe_ip_scope_id;
                std::string pipe_destination_ip;
                std::string pipe_destination_ip_scope_id;
                std::string pipe_destination_port;
                std::string sleep_ms_string;
                int32_t sleep_ms = 0;


                // proxy_flag
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("proxy_flag:") != std::string::npos)
                {
                    proxy_flag_string = get_line_value(line,
                                                       "proxy_flag:");
                }

                if(proxy_flag_string.empty())
                {
                    std::printf("[-] [pipe_client_http] [proxy_flag] error\n");
                    break;
                }

                if(proxy_flag_string == "true")
                {
                    proxy_flag = true;
                }else
                {
                    proxy_flag = false;
                }


                // forward_proxy_type
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_type:") != std::string::npos)
                {
                    forward_proxy_type = get_line_value(line,
                                                        "forward_proxy_type:");
                }


                // forward_proxy_authentication
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_authentication:") != std::string::npos)
                {
                    forward_proxy_authentication_string = get_line_value(line,
                                                                         "forward_proxy_authentication:");
                }

                if(forward_proxy_authentication_string.empty())
                {
                    forward_proxy_authentication = 0;
                }else
                {
                    forward_proxy_authentication = std::stoi(forward_proxy_authentication_string);

                    if(forward_proxy_authentication < 0 || forward_proxy_authentication > 4)
                    {
                        forward_proxy_authentication = 0;
                    }
                }


                // forward_proxy_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_ip:") != std::string::npos)
                {
                    forward_proxy_ip = get_line_value(line,
                                                      "forward_proxy_ip:");
                }


                // forward_proxy_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_port:") != std::string::npos)
                {
                    forward_proxy_port = get_line_value(line,
                                                        "forward_proxy_port:");
                }


                // forward_proxy_username
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_username:") != std::string::npos)
                {
                    forward_proxy_username = get_line_value(line,
                                                            "forward_proxy_username:");
                }


                // forward_proxy_password
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_password:") != std::string::npos)
                {
                    forward_proxy_password = get_line_value(line,
                                                            "forward_proxy_password:");
                }


                // forward_proxy_nthash_hexstring
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_nthash_hexstring:") != std::string::npos)
                {
                    forward_proxy_nthash_hexstring = get_line_value(line,
                                                                    "forward_proxy_nthash_hexstring:");
                }


                // forward_proxy_domainname
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_domainname:") != std::string::npos)
                {
                    forward_proxy_domainname = get_line_value(line,
                                                              "forward_proxy_domainname:");
                }


                // forward_proxy_workstationname
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_workstationname:") != std::string::npos)
                {
                    forward_proxy_workstationname = get_line_value(line,
                                                                   "forward_proxy_workstationname:");
                }


                // forward_proxy_service_principal_name
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("forward_proxy_service_principal_name:") != std::string::npos)
                {
                    forward_proxy_service_principal_name = get_line_value(line,
                                                                          "forward_proxy_service_principal_name:");
                }


                // tls_flag
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("tls_flag:") != std::string::npos)
                {
                    tls_flag_string = get_line_value(line,
                                                     "tls_flag:");
                }

                if(tls_flag_string.empty())
                {
                    std::printf("[-] [pipe_client_http] [tls_flag] error\n");
                    break;
                }

                if(tls_flag_string == "true")
                {
                    tls_flag = true;
                }else
                {
                    tls_flag = false;
                }


                // message_mode
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("message_mode:") != std::string::npos)
                {
                    message_mode = get_line_value(line,
                                                  "message_mode:");
                }

                if(message_mode.empty())
                {
                    std::printf("[-] [pipe_client_http] [message_mode] error\n");
                    break;
                }


                // pipe_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }

                if(line.find("pipe_ip:") != std::string::npos)
                {
                    pipe_ip = get_line_value(line,
                                             "pipe_ip:");
                }

                if(pipe_ip.empty())
                {
                    std::printf("[-] [pipe_client_http] [pipe_ip] error\n");
                    break;
                }

                if(pipe_ip != spider_ip->get_spider_ipv4()
                   && pipe_ip != spider_ip->get_spider_ipv6_global()
                   && pipe_ip != spider_ip->get_spider_ipv6_unique_local()
                   && pipe_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [pipe_client_http] [pipe_ip] please input spider ipv4 or ipv6: %s\n",
                                pipe_ip.c_str());
                    break;
                }

                if(pipe_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // pipe_destination_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }


                if(line.find("pipe_destination_ip:") != std::string::npos)
                {
                    pipe_destination_ip = get_line_value(line,
                                                         "pipe_destination_ip:");
                }

                if(pipe_destination_ip.empty())
                {
                    std::printf("[-] [pipe_client_http] [pipe_destination_ip] error\n");
                    break;
                }


                // pipe_destination_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }

                if(line.find("pipe_destination_port:") != std::string::npos)
                {
                    pipe_destination_port = get_line_value(line,
                                                           "pipe_destination_port:");
                }

                if(pipe_destination_port.empty())
                {
                    std::printf("[-] [pipe_client_http] [pipe_destination_port] error\n");
                    break;
                }


                // sleep_ms
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_client_http] error\n");
                    break;
                }

                if(line.find("sleep_ms:") != std::string::npos)
                {
                    sleep_ms_string = get_line_value(line,
                                                     "sleep_ms:");
                }

                if(sleep_ms_string.empty())
                {
                    std::printf("[-] [pipe_client_http] [sleep_ms] error\n");
                    break;
                }

                sleep_ms = std::stoi(sleep_ms_string);

                if(sleep_ms < 0 || sleep_ms > 1000)
                {
                    sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
                }


                std::thread thread(&Spidercommand::connect_pipe_http,
                                   this,
                                   mode,
                                   proxy_flag,
                                   forward_proxy_type,
                                   forward_proxy_authentication,
                                   forward_proxy_ip,
                                   forward_proxy_port,
                                   forward_proxy_username,
                                   forward_proxy_password,
                                   forward_proxy_nthash_hexstring,
                                   forward_proxy_domainname,
                                   forward_proxy_workstationname,
                                   forward_proxy_service_principal_name,
                                   tls_flag,
                                   message_mode,
                                   pipe_ip,
                                   pipe_ip_scope_id,
                                   pipe_destination_ip,
                                   pipe_destination_port,
                                   sleep_ms);
                thread.detach();
            }else if(line == "[pipe_server]")
            {
                char mode = 's';
                std::string pipe_listen_ip;
                std::string pipe_listen_ip_scope_id;
                std::string pipe_listen_port;


                // pipe_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server] error\n");
                    break;
                }

                if(line.find("pipe_listen_ip:") != std::string::npos)
                {
                    pipe_listen_ip = get_line_value(line,
                                                    "pipe_listen_ip:");
                }

                if(pipe_listen_ip.empty())
                {
                    std::printf("[-] [pipe_server] [pipe_listen_ip] error\n");
                    break;
                }

                if(pipe_listen_ip != spider_ip->get_spider_ipv4()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_global()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [pipe_server] [pipe_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                pipe_listen_ip.c_str());
                    break;
                }

                if(pipe_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    pipe_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // pipe_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server] error\n");
                    break;
                }

                if(line.find("pipe_listen_port:") != std::string::npos)
                {
                    pipe_listen_port = get_line_value(line,
                                                      "pipe_listen_port:");
                }

                if(pipe_listen_port.empty())
                {
                    std::printf("[-] [pipe_server] [pipe_listen_port] error\n");
                    break;
                }


                std::thread thread(&Spidercommand::listen_pipe,
                                   this,
                                   mode,
                                   pipe_listen_ip,
                                   pipe_listen_ip_scope_id,
                                   pipe_listen_port);
                thread.detach();
            }else if(line == "[pipe_server_http]")
            {
                char mode = 's';
                BOOL tls_flag = false;
                std::string tls_flag_string;
                std::string message_mode;
                std::string pipe_listen_ip;
                std::string pipe_listen_ip_scope_id;
                std::string pipe_listen_port;


                // tls_flag
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server_http] error\n");
                    break;
                }


                if(line.find("tls_flag:") != std::string::npos)
                {
                    tls_flag_string = get_line_value(line,
                                                     "tls_flag:");
                }

                if(tls_flag_string.empty())
                {
                    std::printf("[-] [pipe_server_http] [tls_flag] error\n");
                    break;
                }

                if(tls_flag_string == "true")
                {
                    tls_flag = true;
                }else
                {
                    tls_flag = false;
                }


                // message_mode
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server_http] error\n");
                    break;
                }

                if(line.find("message_mode:") != std::string::npos)
                {
                    message_mode = get_line_value(line,
                                                  "message_mode:");
                }

                if(message_mode.empty())
                {
                    std::printf("[-] [pipe_server_http] [message_mode] error\n");
                    break;
                }


                // pipe_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server_http] error\n");
                    break;
                }

                if(line.find("pipe_listen_ip:") != std::string::npos)
                {
                    pipe_listen_ip = get_line_value(line,
                                                    "pipe_listen_ip:");
                }

                if(pipe_listen_ip.empty())
                {
                    std::printf("[-] [pipe_server_http] [pipe_listen_ip] error\n");
                    break;
                }

                if(pipe_listen_ip != spider_ip->get_spider_ipv4()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_global()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && pipe_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [pipe_server_http] [pipe_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                pipe_listen_ip.c_str());
                    break;
                }

                if(pipe_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    pipe_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // pipe_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [pipe_server_http] error\n");
                    break;
                }

                if(line.find("pipe_listen_port:") != std::string::npos)
                {
                    pipe_listen_port = get_line_value(line,
                                                      "pipe_listen_port:");
                }

                if(pipe_listen_port.empty())
                {
                    std::printf("[-] [pipe_server_http] [pipe_listen_port] error\n");
                    break;
                }


                std::thread thread(&Spidercommand::listen_pipe_http,
                                   this,
                                   mode,
                                   tls_flag,
                                   message_mode,
                                   pipe_listen_ip,
                                   pipe_listen_ip_scope_id,
                                   pipe_listen_port);
                thread.detach();
            }else if(line == "[client_tcp]")
            {
                std::string client_listen_ip;
                std::string client_listen_ip_scope_id;
                std::string client_listen_port;
                std::string client_destination_spider_ip;
                std::string target_ip;
                std::string target_port;
                std::string tv_sec_string;
                std::string tv_usec_string;
                std::string forwarder_tv_sec_string;
                std::string forwarder_tv_usec_string;
                int32_t tv_sec = 0;
                int32_t tv_usec = 0;
                int32_t forwarder_tv_sec = 0;
                int32_t forwarder_tv_usec = 0;


                // client_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("client_listen_ip:") != std::string::npos)
                {
                    client_listen_ip = get_line_value(line,
                                                      "client_listen_ip:");
                }

                if(client_listen_ip.empty())
                {
                    std::printf("[-] [client_tcp] [client_listen_ip] error\n");
                    break;
                }

                if(client_listen_ip != spider_ip->get_spider_ipv4()
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [client_tcp] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                client_listen_ip.c_str());
                    break;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // client_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("client_listen_port:") != std::string::npos)
                {
                    client_listen_port = get_line_value(line,
                                                        "client_listen_port:");
                }

                if(client_listen_port.empty())
                {
                    std::printf("[-] [client_tcp] [client_listen_port] error\n");
                    break;
                }


                // client_destination_spider_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("client_destination_spider_ip:") != std::string::npos)
                {
                    client_destination_spider_ip = get_line_value(line,
                                                                  "client_destination_spider_ip:");
                }

                if(client_destination_spider_ip.empty())
                {
                    std::printf("[-] [client_tcp] [client_destination_spider_ip] error\n");
                    break;
                }


                // target_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("target_ip:") != std::string::npos)
                {
                    target_ip = get_line_value(line,
                                               "target_ip:");
                }

                if(target_ip.empty())
                {
                    std::printf("[-] [client_tcp] [target_ip] error\n");
                    break;
                }

                if(target_ip.size() >= 256)
                {
                    std::printf("[-] [client_tcp] [target_ip] size error\n");
                    break;
                }


                // target_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("target_port:") != std::string::npos)
                {
                    target_port = get_line_value(line,
                                                 "target_port:");
                }

                if(target_port.empty())
                {
                    std::printf("[-] [client_tcp] [target_port] error\n");
                    break;
                }


                // tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("tv_sec:") != std::string::npos)
                {
                    tv_sec_string = get_line_value(line,
                                                   "tv_sec:");
                }

                if(tv_sec_string.empty())
                {
                    std::printf("[-] [client_tcp] [tv_sec] error\n");
                    break;
                }


                // tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("tv_usec:") != std::string::npos)
                {
                    tv_usec_string = get_line_value(line,
                                                    "tv_usec:");
                }

                if(tv_usec_string.empty())
                {
                    std::printf("[-] [client_tcp] [tv_usec] error\n");
                    break;
                }


                // forwarder_tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("forwarder_tv_sec:") != std::string::npos)
                {
                    forwarder_tv_sec_string = get_line_value(line,
                        "forwarder_tv_sec:");
                }

                if(forwarder_tv_sec_string.empty())
                {
                    std::printf("[-] [client_tcp] [forwarder_tv_sec] error\n");
                    break;
                }


                // forwarder_tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_tcp] error\n");
                    break;
                }

                if(line.find("forwarder_tv_usec:") != std::string::npos)
                {
                    forwarder_tv_usec_string = get_line_value(line,
                        "forwarder_tv_usec:");
                }

                if(forwarder_tv_usec_string.empty())
                {
                    std::printf("[-] [client_tcp] [forwarder_tv_usec] error\n");
                    break;
                }


                tv_sec = std::stoi(tv_sec_string);
                tv_usec = std::stoi(tv_usec_string);
                forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
                forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

                if(tv_sec < 0 || tv_sec > 60)
                {
                    tv_sec = 3;
                }

                if(tv_usec < 0 || tv_usec > 1000000)
                {
                    tv_usec = 0;
                }

                if(tv_sec == 0 && tv_usec == 0){
                    tv_sec = 3;
                    tv_usec = 0;
                }

                if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 30;
                }

                if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
                {
                    forwarder_tv_usec = 0;
                }

                if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
                {
                    forwarder_tv_sec = 30;
                    forwarder_tv_usec = 0;
                }


                std::thread thread(&Spidercommand::listen_client_tcp,
                                   this,
                                   client_listen_ip,
                                   client_listen_ip_scope_id,
                                   client_listen_port,
                                   client_destination_spider_ip,
                                   target_ip,
                                   target_port,
                                   tv_sec,
                                   tv_usec,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                thread.detach();
            }else if(line == "[client_udp]")
            {
                std::string client_listen_ip;
                std::string client_listen_ip_scope_id;
                std::string client_listen_port;
                std::string client_destination_spider_ip;
                std::string target_ip;
                std::string target_port;
                std::string tv_sec_string;
                std::string tv_usec_string;
                std::string forwarder_tv_sec_string;
                std::string forwarder_tv_usec_string;
                int32_t tv_sec = 0;
                int32_t tv_usec = 0;
                int32_t forwarder_tv_sec = 0;
                int32_t forwarder_tv_usec = 0;


                // client_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("client_listen_ip:") != std::string::npos)
                {
                    client_listen_ip = get_line_value(line,
                                                      "client_listen_ip:");
                }

                if(client_listen_ip.empty())
                {
                    std::printf("[-] [client_udp] [client_listen_ip] error\n");
                    break;
                }

                if(client_listen_ip != spider_ip->get_spider_ipv4()
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [client_udp] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                client_listen_ip.c_str());
                    break;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // client_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("client_listen_port:") != std::string::npos)
                {
                    client_listen_port = get_line_value(line,
                                                        "client_listen_port:");
                }

                if(client_listen_port.empty())
                {
                    std::printf("[-] [client_udp] [client_listen_port] error\n");
                    break;
                }


                // client_destination_spider_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("client_destination_spider_ip:") != std::string::npos)
                {
                    client_destination_spider_ip = get_line_value(line,
                                                                  "client_destination_spider_ip:");
                }

                if(client_destination_spider_ip.empty())
                {
                    std::printf("[-] [client_udp] [client_destination_spider_ip] error\n");
                    break;
                }


                // target_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("target_ip:") != std::string::npos)
                {
                    target_ip = get_line_value(line,
                                               "target_ip:");
                }

                if(target_ip.empty())
                {
                    std::printf("[-] [client_udp] [target_ip] error\n");
                    break;
                }

                if(target_ip.size() >= 256)
                {
                    std::printf("[-] [client_udp] [target_ip] size error\n");
                    break;
                }


                // target_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("target_port:") != std::string::npos)
                {
                    target_port = get_line_value(line,
                                                 "target_port:");
                }

                if(target_port.empty())
                {
                    std::printf("[-] [client_udp] [target_port] error\n");
                    break;
                }


                // tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("tv_sec:") != std::string::npos)
                {
                    tv_sec_string = get_line_value(line,
                                                   "tv_sec:");
                }

                if(tv_sec_string.empty())
                {
                    std::printf("[-] [client_udp] [tv_sec] error\n");
                    break;
                }


                // tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("tv_usec:") != std::string::npos)
                {
                    tv_usec_string = get_line_value(line,
                                                    "tv_usec:");
                }

                if(tv_usec_string.empty())
                {
                    std::printf("[-] [client_udp] [tv_usec] error\n");
                    break;
                }


                // forwarder_tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("forwarder_tv_sec:") != std::string::npos)
                {
                    forwarder_tv_sec_string = get_line_value(line,
                        "forwarder_tv_sec:");
                }

                if(forwarder_tv_sec_string.empty())
                {
                    std::printf("[-] [client_udp] [forwarder_tv_sec] error\n");
                    break;
                }


                // forwarder_tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_udp] error\n");
                    break;
                }

                if(line.find("forwarder_tv_usec:") != std::string::npos)
                {
                    forwarder_tv_usec_string = get_line_value(line,
                        "forwarder_tv_usec:");
                }

                if(forwarder_tv_usec_string.empty())
                {
                    std::printf("[-] [client_udp] [forwarder_tv_usec] error\n");
                    break;
                }


                tv_sec = std::stoi(tv_sec_string);
                tv_usec = std::stoi(tv_usec_string);
                forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
                forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

                if(tv_sec < 0 || tv_sec > 60)
                {
                    tv_sec = 3;
                }

                if(tv_usec < 0 || tv_usec > 1000000)
                {
                    tv_usec = 0;
                }

                if(tv_sec == 0 && tv_usec == 0){
                    tv_sec = 3;
                    tv_usec = 0;
                }

                if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 30;
                }

                if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
                {
                    forwarder_tv_usec = 0;
                }

                if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
                {
                    forwarder_tv_sec = 30;
                    forwarder_tv_usec = 0;
                }


                std::thread thread(&Spidercommand::client_udp_worker,
                                   this,
                                   client_listen_ip,
                                   client_listen_ip_scope_id,
                                   client_listen_port,
                                   client_destination_spider_ip,
                                   target_ip,
                                   target_port,
                                   tv_sec,
                                   tv_usec,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                thread.detach();
            }else if(line == "[client_shell]")
            {
                std::string client_listen_ip;
                std::string client_listen_ip_scope_id;
                std::string client_listen_port;
                std::string client_destination_spider_ip;
                std::string tv_sec_string;
                std::string tv_usec_string;
                std::string forwarder_tv_sec_string;
                std::string forwarder_tv_usec_string;
                int32_t tv_sec = 0;
                int32_t tv_usec = 0;
                int32_t forwarder_tv_sec = 0;
                int32_t forwarder_tv_usec = 0;


                // client_listen_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("client_listen_ip:") != std::string::npos)
                {
                    client_listen_ip = get_line_value(line,
                                                      "client_listen_ip:");
                }

                if(client_listen_ip.empty())
                {
                    std::printf("[-] [client_shell] [client_listen_ip] error\n");
                    break;
                }

                if(client_listen_ip != spider_ip->get_spider_ipv4()
                   && client_listen_ip != spider_ip->get_spider_ipv6_global()
                   && client_listen_ip != spider_ip->get_spider_ipv6_unique_local()
                   && client_listen_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] [client_shell] [client_listen_ip] please input spider ipv4 or ipv6: %s\n",
                                client_listen_ip.c_str());
                    break;
                }

                if(client_listen_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    client_listen_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
                }


                // client_listen_port
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("client_listen_port:") != std::string::npos)
                {
                    client_listen_port = get_line_value(line,
                                                        "client_listen_port:");
                }

                if(client_listen_port.empty())
                {
                    std::printf("[-] [client_shell] [client_listen_port] error\n");
                    break;
                }


                // client_destination_spider_ip
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("client_destination_spider_ip:") != std::string::npos)
                {
                    client_destination_spider_ip = get_line_value(line,
                                                                  "client_destination_spider_ip:");
                }

                if(client_destination_spider_ip.empty())
                {
                    std::printf("[-] [client_shell] [client_destination_spider_ip] error\n");
                    break;
                }


                // tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("tv_sec:") != std::string::npos)
                {
                    tv_sec_string = get_line_value(line,
                                                   "tv_sec:");
                }

                if(tv_sec_string.empty())
                {
                    std::printf("[-] [client_shell] [tv_sec] error\n");
                    break;
                }


                // tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("tv_usec:") != std::string::npos)
                {
                    tv_usec_string = get_line_value(line,
                                                    "tv_usec:");
                }

                if(tv_usec_string.empty())
                {
                    std::printf("[-] [client_shell] [tv_usec] error\n");
                    break;
                }


                // forwarder_tv_sec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("forwarder_tv_sec:") != std::string::npos)
                {
                    forwarder_tv_sec_string = get_line_value(line,
                        "forwarder_tv_sec:");
                }

                if(forwarder_tv_sec_string.empty())
                {
                    std::printf("[-] [client_shell] [forwarder_tv_sec] error\n");
                    break;
                }


                // forwarder_tv_usec
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [client_shell] error\n");
                    break;
                }

                if(line.find("forwarder_tv_usec:") != std::string::npos)
                {
                    forwarder_tv_usec_string = get_line_value(line,
                        "forwarder_tv_usec:");
                }

                if(forwarder_tv_usec_string.empty())
                {
                    std::printf("[-] [client_shell] [forwarder_tv_usec] error\n");
                    break;
                }


                tv_sec = std::stoi(tv_sec_string);
                tv_usec = std::stoi(tv_usec_string);
                forwarder_tv_sec = std::stoi(forwarder_tv_sec_string);
                forwarder_tv_usec = std::stoi(forwarder_tv_usec_string);

                if(tv_sec < 0 || tv_sec > 60)
                {
                    tv_sec = 3;
                }

                if(tv_usec < 0 || tv_usec > 1000000)
                {
                    tv_usec = 0;
                }

                if(tv_sec == 0 && tv_usec == 0){
                    tv_sec = 3;
                    tv_usec = 0;
                }

                if(forwarder_tv_sec < 0 || forwarder_tv_sec > 3600)
                {
                    forwarder_tv_sec = 30;
                }

                if(forwarder_tv_usec < 0 || forwarder_tv_usec > 1000000)
                {
                    forwarder_tv_usec = 0;
                }

                if(forwarder_tv_sec == 0 && forwarder_tv_usec == 0)
                {
                    forwarder_tv_sec = 30;
                    forwarder_tv_usec = 0;
                }


                std::thread thread(&Spidercommand::listen_client_shell,
                                   this,
                                   client_listen_ip,
                                   client_listen_ip_scope_id,
                                   client_listen_port,
                                   client_destination_spider_ip,
                                   tv_sec,
                                   tv_usec,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                thread.detach();
            }else if(line == "[sleep]")
            {
                std::string sleep_string;
                int32_t s;


                // sleep
                line = get_line(config.data(),
                                config.size(),
                                &line_start,
                                &line_end);
                if(line.empty())
                {
                    std::printf("[-] [sleep] error\n");
                    break;
                }

                if(line.find("sleep:") != std::string::npos)
                {
                    sleep_string = get_line_value(line,
                                                  "sleep:");
                }

                if(sleep_string.empty())
                {
                    std::printf("[-] [sleep] [sleep] error\n");
                    break;
                }


                s = std::stoi(sleep_string);
                std::this_thread::sleep_for(std::chrono::seconds(s));
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));

        return 0;
    }

    void Spidercommand::close_client_listener_tcp()
    {
        char mode;  // self:s other:o
        std::string source_spider_ip;
        std::string source_spider_ip_scope_id;
        std::string destination_spider_ip;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        std::shared_ptr<Client> client = nullptr;
        int ret = 0;
        char check = 'n';


        while(1)
        {
            std::printf("mode (self:s other:o)                          > ");
            std::cin >> mode;
            if(std::cin.fail())
            {
                std::printf("[-] input error\n");
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }else if(mode == 's')   // self
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                client_manager->show_client_listener_tcp();

                std::printf("connection id                                  > ");
                std::cin >> connection_id;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::printf("\n");
                std::printf("connection id             : %10u\n", connection_id);
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                    client_id = 0;
                    client = client_manager->get_client(connection_id,
                                                        client_id);
                    if(client != nullptr)
                    {
                        ret = closesocket(client->get_sock());
                        if(ret == SOCKET_ERROR)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] closesocket error: %d\n",
                                        WSAGetLastError());
#endif
                            std::printf("[-] close client listener error\n");
                            return;
                        }

                        std::printf("[+] close client listener connection_id: %10u\n",
                                    client->get_connection_id());
                    }else
                    {
                        std::printf("[-] close client listener error\n");
                        return;
                    }

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
            }else if(mode == 'o')   // other
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::printf("[-] not implemented\n");

/*
                routing_manager->show_routing_table();
                std::printf("\n");

                std::printf("source spider ip                               > ");
                std::cin >> source_spider_ip;
                if(std::cin.fail())
                {
                    std::printf("[-] input error\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip != spider_ip->get_spider_ipv4()
                   && source_spider_ip != spider_ip->get_spider_ipv6_global()
                   && source_spider_ip != spider_ip->get_spider_ipv6_unique_local()
                   && source_spider_ip != spider_ip->get_spider_ipv6_link_local())
                {
                    std::printf("[-] please input spider ipv4 or ipv6\n");
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if(source_spider_ip == spider_ip->get_spider_ipv6_link_local())
                {
                    source_spider_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
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

                std::printf("\n");
                std::printf("mode                      : %c\n", mode);
                std::printf("source spider ip          : %s\n", source_spider_ip.c_str());
                if(!source_spider_ip_scope_id.empty())
                {
                    std::printf("source spider ip scope id : %s (%d)\n", source_spider_ip_scope_id.c_str(), if_nametoindex(source_spider_ip_scope_id.c_str()));
                }
                std::printf("destination spider ip     : %s\n", destination_spider_ip.c_str());
                std::printf("\n");

                std::printf("ok? (yes:y no:n quit:q)                        > ");
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

                break;*/
            }else{
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        return;
    }
}

