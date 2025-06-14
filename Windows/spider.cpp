/*
 * Title:  spider spider.cpp (Windows)
 * Author: Shuichiro Endo
 * Ver:    2.6
 */

#define NOMINMAX

#include "spider.hpp"
#include "spiderip.hpp"
#include "spidercommand.hpp"
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


static int optstringIndex = 0;
static char *optarg = NULL;


namespace spider
{
    static void print_title();
    static void usage(char *filename);
    static int getopt(int argc,
                      char **argv,
                      char *optstring);

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
        std::printf("        =-    -:               Windows Ver: 2.6   Author: Shuichiro Endo  \n");
        std::printf("\n");
    }

    static void usage(char *filename)
    {
        std::printf("\n");
        std::printf("usage   : %s\n", filename);
        std::printf("        : [-4 spider_ipv4] [-6 spider_ipv6_global] [-u spider_ipv6_unique_local] [-l spider_ipv6_link_local]\n");
        std::printf("        : [-f config_file]\n");
        std::printf("        : [-d (hide)] [-i pipe_destination_ip] [-p pipe_destination_port] [-m message_mode(default:d http:h https:s)] [-t sleep_ms(0-1000 ms)]\n");
        std::printf("        : [-r routing_mode(auto:a self:s)]\n");
        std::printf("        : [-e x(xor encryption)] [-k key(hexstring)]\n");
        std::printf("        : [-e a(aes-256-cbc encryption)] [-k key(hexstring)] [-v iv(hexstring)]\n");
        std::printf("        : [-s (prevent spider server startup)]\n");
        std::printf("example : %s\n", filename);
        std::printf("        : %s -4 192.168.0.10\n", filename);
        std::printf("        : %s -6 2001::xxxx:xxxx:xxxx:xxxx\n", filename);
        std::printf("        : %s -u fd00::xxxx:xxxx:xxxx:xxxx\n", filename);
        std::printf("        : %s -l fe80::xxxx:xxxx:xxxx:xxxx%%14\n", filename);
        std::printf("        : %s -4 192.168.0.10 -6 2001::xxxx:xxxx:xxxx:xxxx -u fd00::xxxx:xxxx:xxxx:xxxx -l fe80::xxxx:xxxx:xxxx:xxxx%%14\n", filename);
        std::printf("        : %s -f config_sample.txt\n", filename);
        std::printf("        : %s -d -i 192.168.0.25 -p 1025 -m d\n", filename);
        std::printf("        : %s -4 192.168.0.10 -r s\n", filename);
        std::printf("        : %s -4 192.168.0.10 -e x -k deadbeef\n", filename);
        std::printf("        : %s -4 192.168.0.10 -e a -k 47a2baa1e39fa16752a2ea8e8e3e24256b3c360f382b9782e2e57d4affb19f8c -v c87114c8b36088074c7ec1398f5c168a\n", filename);
        std::printf("        : %s -s\n", filename);
        std::printf("\n");
    }

    static int getopt(int argc,
                      char **argv,
                      const char *optstring)
    {

        unsigned char opt = '\0';
        unsigned char next = '\0';
        char *argtmp = NULL;

        while(1){
            opt = *(optstring + optstringIndex);
            optstringIndex++;
            if(opt == '\0'){
                break;
            }

            next = *(optstring + optstringIndex);
            if(next == ':'){
                optstringIndex++;
            }

            for(int i=1; i<argc; i++){
                argtmp = argv[i];
                if(argtmp[0] == '-'){
                    if(argtmp[1] == opt){
                        if(next == ':'){
                            optarg = argv[i+1];
                            return (int)opt;
                        }else{
                            return (int)opt;
                        }
                    }
                }
            }
        }

        return 0;
    }

}

int main(int argc,
         char **argv)
{
    int opt;
    const char *optstring = "h:4:6:u:l:f:di:p:m:t:r:e:k:v:s";
    int opterr = 0;
    std::string spider_ipv4;
    std::string spider_ipv6_global;
    std::string spider_ipv6_unique_local;
    std::string spider_ipv6_link_local;
    unsigned long out_buffer_length = 0;
    IP_ADAPTER_ADDRESSES *addresses = NULL;
    sockaddr_in *sa_in = NULL;
    sockaddr_in6 *sa_in6 = NULL;
    char ip_tmp[INET6_ADDR_STRING_LENGTH + 1] = {0};
    std::string ip;
    std::string percent = "%";
    std::string scope_id;
    std::string config_file;
    bool hide_flag = false;
    HWND handle_window;
    char mode;
    std::string pipe_ip;
    std::string pipe_ip_scope_id;
    std::string pipe_destination_ip;
    std::string pipe_destination_port;
    std::string message_mode;
    BOOL tls_flag = false;
    int32_t sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
    std::string ipv6_global_prefix = "2001:";
    std::string ipv6_unique_local_prefix = "fd00:";
    std::string ipv6_link_local_prefix = "fe80:";
    std::string routing_mode = "a";
    std::string encryption_type;
    std::string key;
    std::string iv;
    BOOL xor_flag = false;
    BOOL aes_flag = false;
    std::string xor_key_hex_string;
    std::string aes_key_hex_string;
    std::string aes_iv_hex_string;
    BOOL prevent_spider_server_startup_flag = false;


    while((opt = spider::getopt(argc, argv, optstring)) > 0)
    {
        switch(opt)
        {
            case 'h':
                spider::print_title();
                spider::usage(argv[0]);
                exit(-1);

            case '4':
                spider_ipv4 = optarg;
                break;

            case '6':
                spider_ipv6_global = optarg;
                break;

            case 'u':
                spider_ipv6_unique_local = optarg;
                break;

            case 'l':
                spider_ipv6_link_local = optarg;
                break;

            case 'f':
                config_file = optarg;
                break;

            case 'd':
                hide_flag = true;
                break;

            case 'i':
                pipe_destination_ip = optarg;
                break;

            case 'p':
                pipe_destination_port = optarg;
                break;

            case 'm':
                message_mode = optarg;
                break;

            case 't':
                sleep_ms = atoi(optarg);
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

            case 's':
                prevent_spider_server_startup_flag = true;
                break;

            default:
                spider::print_title();
                spider::usage(argv[0]);
                exit(-1);
        }
    }

    // hide
    if(hide_flag == true)
    {
        handle_window = GetConsoleWindow();
        if(handle_window != NULL)
        {
            ShowWindow(handle_window,
                       SW_HIDE);
        }
    }

    spider::print_title();

    if(spider_ipv4.empty()
       && spider_ipv6_global.empty()
       && spider_ipv6_unique_local.empty()
       && spider_ipv6_link_local.empty())
    {
        GetAdaptersAddresses(AF_UNSPEC,
                             0,
                             NULL,
                             NULL,
                             &out_buffer_length);

        addresses = (IP_ADAPTER_ADDRESSES *)calloc(out_buffer_length,
                                                   sizeof(char));

        if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, addresses, &out_buffer_length) == NO_ERROR)
        {
            for(IP_ADAPTER_ADDRESSES *current_addresses = addresses; current_addresses != NULL; current_addresses = current_addresses->Next)
            {
                for (IP_ADAPTER_UNICAST_ADDRESS* unicast = current_addresses->FirstUnicastAddress; unicast != NULL; unicast = unicast->Next)
                {
                    if(unicast->Address.lpSockaddr->sa_family == AF_INET)
                    {
                        sa_in = (sockaddr_in *)unicast->Address.lpSockaddr;
                        memset(ip_tmp,
                               0,
                               INET6_ADDR_STRING_LENGTH + 1);

                        if(inet_ntop(AF_INET, &sa_in->sin_addr, ip_tmp, INET_ADDRSTRLEN) == NULL)
                        {
                            std::printf("[-] inet_ntop error\n");
                            free(addresses);
                            exit(-1);
                        }

                        ip = ip_tmp;

                        if(spider_ipv4.empty())
                        {
                            spider_ipv4 = ip;
                        }
                    }else if(unicast->Address.lpSockaddr->sa_family == AF_INET6)
                    {
                        sa_in6 = (sockaddr_in6 *)unicast->Address.lpSockaddr;
                        memset(ip_tmp,
                               0,
                               INET6_ADDR_STRING_LENGTH + 1);

                        if(inet_ntop(AF_INET6, &sa_in6->sin6_addr, ip_tmp, INET6_ADDR_STRING_LENGTH) == NULL)
                        {
                            std::printf("[-] inet_ntop error\n");
                            free(addresses);
                            exit(-1);
                        }

                        ip = ip_tmp;

                        if(ip.rfind("2001:", 0) == 0)
                        {
                            if(spider_ipv6_global.empty())
                            {
                                spider_ipv6_global = ip;
                            }
                        }else if(ip.rfind("fd00:", 0) == 0)
                        {
                            if(spider_ipv6_unique_local.empty())
                            {
                                spider_ipv6_unique_local = ip;
                            }
                        }else if(ip.rfind("fe80:", 0) == 0)
                        {
                            if(spider_ipv6_link_local.empty())
                            {
                                scope_id = std::to_string(sa_in6->sin6_scope_id);
                                ip = ip + percent + scope_id;
                                spider_ipv6_link_local = ip;
                            }
                        }
                    }
                }
            }
        }

        free(addresses);
    }

    std::shared_ptr<spider::Spiderip> spider_ip = std::make_shared<spider::Spiderip>(spider_ipv4,
                                                                                     spider_ipv6_global,
                                                                                     spider_ipv6_unique_local,
                                                                                     spider_ipv6_link_local);
    if(spider_ip->get_init_flag() == false)
    {
        spider::usage(argv[0]);
        exit(-1);
    }

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

    int ret = 0;
    WSADATA wsaData;

    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(ret != 0)
    {
#ifdef DEBUGPRINT
        std::printf("[-] WSAStartup error: %d\n", ret);
#endif
        return -1;
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

    std::shared_ptr<spider::Spidercommand> spider_command = std::make_shared<spider::Spidercommand>(spider_ip,
                                                                                                    encryption,
                                                                                                    client_manager,
                                                                                                    server_manager,
                                                                                                    pipe_manager,
                                                                                                    routing_manager,
                                                                                                    message_manager);

    if(routing_mode != "s") // auto
    {
        std::thread routing_manager_thread(&spider::Spidercommand::routing_manager_worker,
                                           spider_command);
        routing_manager_thread.detach();
    }

    std::thread message_manager_thread(&spider::Spidercommand::message_manager_worker,
                                       spider_command,
                                       prevent_spider_server_startup_flag);
    message_manager_thread.detach();

    if(!config_file.empty())
    {
        ret = spider_command->read_config_file(config_file);
        if(ret < 0)
        {
            std::printf("[-] read config file error\n");
            exit(-1);
        }
    }

    if(!pipe_destination_ip.empty()
       && !pipe_destination_port.empty())
    {
        mode = 'c';

        if(pipe_destination_ip.find(":") == std::string::npos)  // ipv4
        {
            if(spider_ip->get_spider_ipv4().empty())
            {
                std::printf("[-] spider_ipv4 empty\n");
                exit(-1);
            }

            pipe_ip = spider_ip->get_spider_ipv4();
        }else if(pipe_destination_ip.rfind(ipv6_global_prefix, 0) == 0) // ipv6 global
        {
            if(spider_ip->get_spider_ipv6_global().empty())
            {
                std::printf("[-] spider_ipv6_global empty\n");
                exit(-1);
            }

            pipe_ip = spider_ip->get_spider_ipv6_global();
        }else if(pipe_destination_ip.rfind(ipv6_unique_local_prefix, 0) == 0)   // ipv6 unique local
        {
            if(spider_ip->get_spider_ipv6_unique_local().empty())
            {
                std::printf("[-] spider_ipv6_unique_local empty\n");
                exit(-1);
            }

            pipe_ip = spider_ip->get_spider_ipv6_unique_local();
        }else if(pipe_destination_ip.rfind(ipv6_link_local_prefix, 0) == 0) // ipv6 link local
        {
            if(spider_ip->get_spider_ipv6_link_local().empty())
            {
                std::printf("[-] spider_ipv6_link_local empty\n");
                exit(-1);
            }

            if(pipe_destination_ip.find("%") != std::string::npos)
            {
                std::printf("[-] pipe_destination_ip includes a scope id: %s\n",
                            pipe_destination_ip.c_str());
                exit(-1);
            }

            pipe_ip = spider_ip->get_spider_ipv6_link_local();
            pipe_ip_scope_id = spider_ip->get_spider_ipv6_link_local_scope_id();
        }else
        {
            std::printf("[-] pipe_destination_ip error: %s\n",
                        pipe_destination_ip.c_str());
            exit(-1);
        }

        if(message_mode == "h" ||
           message_mode == "s") // http or https
        {
            if(message_mode == "s")
            {
                tls_flag = true;
            }else
            {
                tls_flag = false;
            }

            if(sleep_ms < 0 || sleep_ms > 1000)
            {
                sleep_ms = PIPE_MESSAGE_MODE_HTTP_SLEEP;
            }

            std::thread thread(&spider::Spidercommand::connect_pipe_http,
                               spider_command,
                               mode,
                               tls_flag,
                               pipe_ip,
                               pipe_ip_scope_id,
                               pipe_destination_ip,
                               pipe_destination_port,
                               sleep_ms);
            thread.detach();
        }else   // default
        {
            std::thread thread(&spider::Spidercommand::connect_pipe,
                               spider_command,
                               mode,
                               pipe_ip,
                               pipe_ip_scope_id,
                               pipe_destination_ip,
                               pipe_destination_port);
            thread.detach();
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));  // 5s
    }


    int spider_command_input = 0;

    while(1)
    {
        std::printf("\n");
        std::printf("--------------------------------- spider ---------------------------------\n");
        if(!spider_ip->get_spider_ipv4().empty())
        {
            std::printf(" spider ipv4                     : %s\n", spider_ip->get_spider_ipv4().c_str());
        }
        if(!spider_ip->get_spider_ipv6_global().empty())
        {
            std::printf(" spider ipv6 global              : %s\n", spider_ip->get_spider_ipv6_global().c_str());
        }
        if(!spider_ip->get_spider_ipv6_unique_local().empty())
        {
            std::printf(" spider ipv6 unique local        : %s\n", spider_ip->get_spider_ipv6_unique_local().c_str());
        }
        if(!spider_ip->get_spider_ipv6_link_local().empty())
        {
            std::printf(" spider ipv6 link local          : %s\n", spider_ip->get_spider_ipv6_link_local().c_str());
            if(!spider_ip->get_spider_ipv6_link_local_scope_id().empty())
            {
                std::printf(" spider ipv6 link local scope id : %s\n", spider_ip->get_spider_ipv6_link_local_scope_id().c_str());
            }
        }
        std::printf(" config file                     : %s\n", config_file.c_str());
        std::printf(" routing mode                    : %s\n", (routing_mode == "s" ? "self" : "auto"));
        std::printf(" xor encryption                  : %s\n", (xor_flag ? "on" : "off"));
        std::printf(" xor key hex string              : %s\n", xor_key_hex_string.c_str());
        std::printf(" aes encryption                  : %s\n", (aes_flag ? "on" : "off"));
        std::printf(" aes key hex string              : %s\n", aes_key_hex_string.c_str());
        std::printf(" aes iv hex string               : %s\n", aes_iv_hex_string.c_str());
        std::printf(" prevent spider server startup   : %s\n", (prevent_spider_server_startup_flag ? "on" : "off"));
        std::printf("----------------------------- spider command -----------------------------\n");
        std::printf(" %d: add node (spider pipe)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE);
        std::printf(" %d: add node (spider client)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT);
        std::printf(" %d: show node information\n", SPIDER_COMMAND_SHOW_NODE_INFORMATION);
        std::printf(" %d: show routing table\n", SPIDER_COMMAND_SHOW_ROUTING_TABLE);
        std::printf(" %d: edit routing table\n", SPIDER_COMMAND_EDIT_ROUTING_TABLE);
        std::printf(" %d: add node (spider client tcp)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP);
        std::printf(" %d: add node (spider client udp)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP);
        std::printf(" %d: add node (spider client shell)\n", SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL);
        std::printf(" %d: close client listener (tcp)\n", SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP);
        std::printf(" %d: exit\n", SPIDER_COMMAND_EXIT);
        std::printf("--------------------------------------------------------------------------\n");
        std::printf("\n");
        std::printf("command > ");

        std::cin >> spider_command_input;

        if(std::cin.fail())
        {
#ifdef DEBUGPRINT
            std::printf("[-] spider command error\n");
#endif
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch(spider_command_input)
        {
            case SPIDER_COMMAND_ADD_NODE_SPIDER_PIPE:
                std::printf("[+] add node (spider pipe)\n");
                spider_command->add_node_spider_pipe();
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT:
                std::printf("[+] add node (spider client)\n");
                spider_command->add_node_spider_client();
                break;

            case SPIDER_COMMAND_SHOW_NODE_INFORMATION:
                std::printf("[+] show node information\n");
                spider_command->show_node_information();
                break;

            case SPIDER_COMMAND_SHOW_ROUTING_TABLE:
                std::printf("[+] show routing table\n");
                spider_command->show_routing_table();
                break;

            case SPIDER_COMMAND_EDIT_ROUTING_TABLE:
                std::printf("[+] edit routing table\n");
                spider_command->edit_routing_table();
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_TCP:
                std::printf("[+] add node (spider client tcp)\n");
                std::printf("[!] This is not SOCKS5 connection. (Something like TCP over TCP)\n");
                spider_command->add_node_spider_client_tcp();
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_UDP:
                std::printf("[+] add node (spider client udp)\n");
                std::printf("[!] This is not SOCKS5 connection. (Something like UDP over TCP)\n");
                spider_command->add_node_spider_client_udp();
                break;

            case SPIDER_COMMAND_ADD_NODE_SPIDER_CLIENT_SHELL:
                std::printf("[+] add node (spider client shell)\n");
                std::printf("[!] This is not SOCKS5 connection.\n");
                spider_command->add_node_spider_client_shell();
                break;

            case SPIDER_COMMAND_CLOSE_CLIENT_LISTENER_TCP:
                std::printf("[+] close client listener (tcp)\n");
                spider_command->close_client_listener_tcp();
                break;

            case SPIDER_COMMAND_EXIT:
                std::printf("[+] exit\n");
                WSACleanup();
                exit(0);
                break;

            default:
                break;
        }
    }

    WSACleanup();
    return 0;
}

