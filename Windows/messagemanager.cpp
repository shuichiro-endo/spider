/*
 * Title:  spider messagemanager.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "messagemanager.hpp"
#include "clientmanager.hpp"
#include "servermanager.hpp"
#include "pipemanager.hpp"
#include "routingmanager.hpp"
#include "socks5message.hpp"
#include "message.hpp"
#include "routingmessagequeue.hpp"
#include "socks5messagequeue.hpp"
#include "routingmessage.hpp"
#include "client.hpp"
#include "server.hpp"
#include "pipe.hpp"


namespace spider
{
    Messagemanager::Messagemanager(std::string spider_ip,
                                   std::shared_ptr<Clientmanager> client_manager,
                                   std::shared_ptr<Servermanager> server_manager,
                                   std::shared_ptr<Pipemanager> pipe_manager,
                                   std::shared_ptr<Routingmanager> routing_manager)
    {
        this->spider_ip = spider_ip;
        this->client_manager = client_manager;
        this->server_manager = server_manager;
        this->pipe_manager = pipe_manager;
        this->routing_manager = routing_manager;
        this->routing_messages_queue = std::make_unique<Routingmessagequeue>();
        this->socks5_messages_queue = std::make_unique<Socks5messagequeue>();
    }

    Messagemanager::~Messagemanager()
    {

    }

    void Messagemanager::push_routing_message(std::shared_ptr<Routingmessage> routing_message)
    {
        routing_messages_queue->push(routing_message);

        return;
    }

    int32_t Messagemanager::push_timeout_routing_message(std::shared_ptr<Routingmessage> routing_message,
                                                         int32_t tv_sec,
                                                         int32_t tv_usec)
    {
        int32_t ret = 0;

        ret = routing_messages_queue->push_timeout(routing_message,
                                                   tv_sec,
                                                   tv_usec);

        return ret;
    }

    void Messagemanager::push_socks5_message(std::shared_ptr<Socks5message> socks5_message)
    {
        socks5_messages_queue->push(socks5_message);

        return;
    }

    int32_t Messagemanager::push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                                        int32_t tv_sec,
                                                        int32_t tv_usec)
    {
        int32_t ret = 0;

        ret = socks5_messages_queue->push_timeout(socks5_message,
                                                  tv_sec,
                                                  tv_usec);

        return ret;
    }

    std::shared_ptr<Routingmessage> Messagemanager::pop_routing_message()
    {
        std::shared_ptr<Routingmessage> routing_message = routing_messages_queue->pop();

        return routing_message;
    }

    std::shared_ptr<Socks5message> Messagemanager::pop_socks5_message()
    {
        std::shared_ptr<Socks5message> socks5_message = socks5_messages_queue->pop();

        return socks5_message;
    }

    void Messagemanager::transfer_routing_message()
    {
        char message_type;
        std::shared_ptr<Routingmessage> routing_message;


        routing_message = pop_routing_message();
        if(routing_message != nullptr )
        {
            message_type = routing_message->get_message_type();
            if(message_type == 'r')  // routing message
            {
                routing_manager->push_routing_message(routing_message);
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

    std::shared_ptr<Socks5message> Messagemanager::transfer_socks5_message()
    {
        int ret = 0;
        char message_type;
        std::shared_ptr<Socks5message> socks5_message;
        char destination_node_type;
        std::shared_ptr<Client> client;
        std::shared_ptr<Server> server;
        std::shared_ptr<Pipe> pipe;
        uint32_t connection_id = 0;
        uint32_t client_id = 0;
        uint32_t server_id = 0;
        uint32_t pipe_id = 0;


        socks5_message = pop_socks5_message();
        if(socks5_message != nullptr)
        {
            message_type = socks5_message->get_message_type();
            if(message_type == 's')  // socks5 message
            {
                if(spider_ip == socks5_message->get_destination_ip())
                {
                    connection_id = socks5_message->get_connection_id();
                    client_id = socks5_message->get_client_id();
                    server_id = socks5_message->get_server_id();
                    destination_node_type = socks5_message->get_destination_node_type();
                    if(destination_node_type == 'c')    // client
                    {
                        client = client_manager->get_client(connection_id,
                                                            client_id);
                        if(client != nullptr)
                        {
//                            client->push_socks5_message(socks5_message);
                            std::thread thread_client(&Client::push_socks5_message,
                                                      client,
                                                      socks5_message);
                            thread_client.detach();

                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] cannot transfer client message\n");
#endif
                        }
                    }else if(destination_node_type == 's')  // server
                    {
                       server = server_manager->get_server(connection_id,
                                                            server_id);
                        if(server != nullptr)
                        {
//                            server->push_socks5_message(socks5_message);
                            std::thread thread_server(&Server::push_socks5_message,
                                                      server,
                                                      socks5_message);
                            thread_server.detach();
                        }else
                        {
                            // generate server
                            return socks5_message;
                        }
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] cannot transfer socks5 message\n");
#endif
                    }
                }else
                {
                    pipe = routing_manager->get_destination_pipe(socks5_message->get_destination_node_type(),
                                                                 socks5_message->get_destination_ip());
                    if(pipe != nullptr)
                    {
//                        pipe->push_socks5_message(socks5_message);
                        std::thread thread_pipe(&Pipe::push_socks5_message,
                                                pipe,
                                                socks5_message);
                        thread_pipe.detach();
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] cannot transfer pipe message\n");
#endif
                    }
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] unknown message type: %c\n",
                            message_type);
#endif
            }
        }

        return nullptr;
    }
}

