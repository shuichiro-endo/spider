/*
 * Title:  spider server.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "spiderip.hpp"
#include "spidercommand.hpp"
#include "socks5.hpp"
#include "server.hpp"
#include "clientmanager.hpp"
#include "servermanager.hpp"
#include "pipemanager.hpp"
#include "routingmanager.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "encryption.hpp"


namespace spider
{
    Server::Server(std::shared_ptr<Spiderip> spider_ip,
                   uint32_t connection_id,
                   uint32_t client_id,
                   uint32_t server_id,
                   std::string server_ip,
                   std::string server_port,
                   std::string client_destination_ip,
                   SOCKET server_sock,
                   int32_t tv_sec,
                   int32_t tv_usec,
                   int32_t forwarder_tv_sec,
                   int32_t forwarder_tv_usec,
                   std::shared_ptr<Encryption> encryption,
                   std::shared_ptr<Clientmanager> client_manager,
                   std::shared_ptr<Servermanager> server_manager,
                   std::shared_ptr<Pipemanager> pipe_manager,
                   std::shared_ptr<Routingmanager> routing_manager,
                   std::shared_ptr<Messagemanager> message_manager,
                   Spidercommand *spider_command)
    : Node(server_sock, message_manager)
    {
        this->spider_ip = spider_ip;
        this->connection_id = connection_id;
        this->client_id = client_id;
        this->server_id = server_id;
        this->server_ip = server_ip;
        this->server_port = server_port;
        this->client_destination_ip = client_destination_ip;
        this->tv_sec = tv_sec;
        this->tv_usec = tv_usec;
        this->forwarder_tv_sec = forwarder_tv_sec;
        this->forwarder_tv_usec = forwarder_tv_usec;
        this->encryption = encryption;

        this->sock = server_sock;
        this->target_sock = INVALID_SOCKET;
        this->client_manager = client_manager;
        this->server_manager = server_manager;
        this->pipe_manager = pipe_manager;
        this->routing_manager = routing_manager;
        this->message_manager = message_manager;
        this->spider_command = spider_command;
    }

    Server::~Server()
    {

    }

    void Server::set_spider_ip(std::shared_ptr<Spiderip> spider_ip)
    {
        this->spider_ip = spider_ip;
    }

    std::shared_ptr<Spiderip> Server::get_spider_ip()
    {
        return spider_ip;
    }

    void Server::set_connection_id(uint32_t connection_id)
    {
        this->connection_id = connection_id;
    }

    uint32_t Server::get_connection_id()
    {
        return connection_id;
    }

    void Server::set_client_id(uint32_t client_id)
    {
        this->client_id = client_id;
    }

    uint32_t Server::get_client_id()
    {
        return client_id;
    }

    void Server::set_server_id(uint32_t server_id)
    {
        this->server_id = server_id;
    }

    uint32_t Server::get_server_id()
    {
        return server_id;
    }

    void Server::set_server_ip(std::string server_ip)
    {
        this->server_ip = server_ip;
    }

    std::string Server::get_server_ip()
    {
        return server_ip;
    }

    void Server::set_server_port(std::string server_port)
    {
        this->server_port = server_port;
    }

    std::string Server::get_server_port()
    {
        return server_port;
    }

    void Server::set_client_destination_ip(std::string client_destination_ip)
    {
        this->client_destination_ip = client_destination_ip;
    }

    std::string Server::get_client_destination_ip()
    {
        return client_destination_ip;
    }

    void Server::set_target_ip(std::string target_ip)
    {
        this->target_ip = target_ip;
    }

    std::string Server::get_target_ip()
    {
        return target_ip;
    }

    void Server::set_target_port(std::string target_port)
    {
        this->target_port = target_port;
    }

    std::string Server::get_target_port()
    {
        return target_port;
    }

    void Server::set_target_sock(SOCKET target_sock)
    {
        this->target_sock = target_sock;
    }

    SOCKET Server::get_target_sock()
    {
        return target_sock;
    }

    void Server::set_tv_sec(int32_t tv_sec)
    {
        this->tv_sec = tv_sec;
    }

    int32_t Server::get_tv_sec()
    {
        return tv_sec;
    }

    void Server::set_tv_usec(int32_t tv_usec)
    {
        this->tv_usec = tv_usec;
    }

    int32_t Server::get_tv_usec()
    {
        return tv_usec;
    }

    void Server::set_forwarder_tv_sec(int32_t forwarder_tv_sec)
    {
        this->forwarder_tv_sec = forwarder_tv_sec;
    }

    int32_t Server::get_forwarder_tv_sec()
    {
        return forwarder_tv_sec;
    }

    void Server::set_forwarder_tv_usec(int32_t forwarder_tv_usec)
    {
        this->forwarder_tv_usec = forwarder_tv_usec;
    }

    int32_t Server::get_forwarder_tv_usec()
    {
        return forwarder_tv_usec;
    }

    void Server::set_encryption(std::shared_ptr<Encryption> encryption)
    {
        this->encryption = encryption;
    }

    std::shared_ptr<Encryption> Server::get_encryption()
    {
        return encryption;
    }

    int32_t Server::recv_message(char *buffer,
                                 int32_t buffer_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec)
    {
        int32_t ret = 0;
        char message_type;
        int32_t rec = 0;
        std::shared_ptr<Socks5message> socks5_message;


        std::memset(buffer,
                    0,
                    buffer_size);

        socks5_message = pop_timeout_socks5_message(tv_sec,
                                                    tv_usec);
        if(socks5_message !=  nullptr)
        {
            message_type = socks5_message->get_message_type();
            if(message_type == 's')     // socks5 message
            {
                rec = socks5_message->get_data_size();
                if(rec >= 0)
                {
                    recv_message_id = socks5_message->get_message_id();

                    std::memcpy(buffer,
                                socks5_message->get_data(),
                                rec);

#ifdef DEBUGPRINT
                    print_bytes(buffer, rec);
#endif

                    if(encryption != nullptr
                       && encryption->get_flag())
                    {
                        ret = encryption->decrypt(buffer,
                                                  rec,
                                                  buffer_size);
                        if(ret <= 0)
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] recv_message decrypt error: %d\n",
                                        ret);
#endif
                            return -1;
                        }

                        rec = ret;
                    }
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[-] recv_message recv error: %d\n",
                                rec);
#endif
                    return -1;
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_message unknown message type: %c\n",
                            message_type);
#endif
                return -1;
            }
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] recv_message error\n");
#endif
            return -1;
        }

        return rec;
    }

    int32_t Server::send_message(char *buffer,
                                 int32_t data_size,
                                 int32_t tv_sec,
                                 int32_t tv_usec)
    {
        int32_t ret = 0;
        int32_t sen = 0;
        std::shared_ptr<Socks5message> socks5_message;


        if(data_size > SOCKS5_MESSAGE_DATA_SIZE){
#ifdef DEBUGPRINT
            std::printf("[-] send_message data size error: %d\n",
                        data_size);
#endif
            return -1;
        }

        if(encryption != nullptr
           && encryption->get_flag())
        {
            ret = encryption->encrypt(buffer,
                                      data_size,
                                      SOCKS5_MESSAGE_DATA_SIZE);
            if(ret <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] send_message encrypt error: %d\n",
                            ret);
#endif
                return -1;
            }

            data_size = ret;
        }

#ifdef DEBUGPRINT
        print_bytes(buffer, data_size);
#endif

        socks5_message = std::make_shared<Socks5message>('s',
                                                         this->send_message_id,
                                                         this->connection_id,
                                                         this->client_id,
                                                         this->server_id,
                                                         's',
                                                         this->server_ip,
                                                         'c',
                                                         this->client_destination_ip,
                                                         this->tv_sec,
                                                         this->tv_usec,
                                                         this->forwarder_tv_sec,
                                                         this->forwarder_tv_usec,
                                                         data_size,
                                                         buffer);

        ret = message_manager->push_timeout_socks5_message(socks5_message,
                                                           tv_sec,
                                                           tv_usec);
        if(ret < 0)
        {
            sen = -1;
        }else
        {
            sen = data_size;
        }

        return sen;
    }



    int32_t Server::forwarder_recv_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set writefds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(target_sock, &writefds);
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_recv_data timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_recv_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &writefds);
            if(ret){
#ifdef DEBUGPRINT
                std::printf("[+] [client -> server] recv_message\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv_message(buffer,
                                   buffer_max_length,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                if(rec > 0)
                {
                    if(recv_message_id == next_recv_message_id)
                    {
                        len = rec;
                        send_length = 0;
#ifdef DEBUGPRINT
                        std::printf("[+] [server -> target] send message_id:%u\n",
                                    next_recv_message_id);
#endif
                        while(len > 0)
                        {
                            sen = send(target_sock,
                                       buffer+send_length,
                                       len,
                                       0);
                            if(sen == SOCKET_ERROR)
                            {
                                ret = WSAGetLastError();
                                if(ret == WSAEWOULDBLOCK)
                                {
                                    std::this_thread::sleep_for(std::chrono::microseconds(5000));
                                    continue;
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] forwarder_recv_data send error: %d\n",
                                                ret);
#endif
                                    break;
                                }
                            }
                            send_length += sen;
                            len -= sen;
                        }

                        next_recv_message_id++;
                    }else
                    {
                        msg = std::make_pair(rec,
                                             buffer);
                        msgs_map.insert({recv_message_id, msg});

                        check_msg_count = msgs_map.count(next_recv_message_id);
                        while(check_msg_count > 0)
                        {
                            msg = msgs_map[next_recv_message_id];
                            msgs_map.erase(next_recv_message_id);

                            len = msg.first;
                            buffer = msg.second;
                            send_length = 0;
#ifdef DEBUGPRINT
                            std::printf("[+] [server -> target] send message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
                            {
                                sen = send(target_sock,
                                           buffer+send_length,
                                           len,
                                           0);
                                if(sen == SOCKET_ERROR)
                                {
                                    ret = WSAGetLastError();
                                    if(ret == WSAEWOULDBLOCK)
                                    {
                                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                                        continue;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] forwarder_recv_data send error: %d\n",
                                                    ret);
#endif
                                        break;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            free(buffer);
                            next_recv_message_id++;
                            check_msg_count = msgs_map.count(next_recv_message_id);
                        }

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                    }
                }else
                {
                    break;
                }
            }
        }

        for(auto it = msgs_map.begin(); it != msgs_map.end();)
        {
            msg = it->second;
            free(msg.second);
            it = msgs_map.erase(it);
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_send_data()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        fd_set readfds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;


        while(1)
        {
            FD_ZERO(&readfds);
            FD_SET(target_sock, &readfds);
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(NULL,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_send_data timeout\n");
#endif
                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_send_data select error: 0x%x\n", ret);
#endif
                break;
            }

            ret = FD_ISSET(target_sock,
                           &readfds);
            if(ret)
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server <- target] recv\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv(target_sock,
                           buffer,
                           socks5_message_data_max_size,
                           0);
                if(rec == SOCKET_ERROR)
                {
                    ret =  WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }else
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] forwarder_send_data recv error: %d\n",
                                    ret);
#endif
                        break;
                    }
                }else if(rec > 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] send_message message_id:%u\n",
                                send_message_id);
#endif
                    sen = send_message(buffer,
                                       rec,
                                       forwarder_tv_sec,
                                       forwarder_tv_usec);
                    if(sen <= 0)
                    {
                        break;
                    }

                    send_message_id++;
                }else
                {
                    break;
                }
            }
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder()
    {
        std::thread thread1(&Server::forwarder_recv_data,
                            this);
        std::thread thread2(&Server::forwarder_send_data,
                            this);

        thread1.join();
        thread2.join();

        return 0;
    }

    int32_t Server::send_socks_response_ipv4(char *buffer,
                                             int32_t buffer_size,
                                             char ver,
                                             char rep,
                                             char rsv,
                                             char atyp)
    {
        int32_t sen = 0;
        std::memset(buffer,
                    0,
                    buffer_size);

        struct socks_response_ipv4 *socks_response_ipv4 = (struct socks_response_ipv4 *)buffer;

        socks_response_ipv4->ver = ver;              // protocol version
        socks_response_ipv4->rep = rep;              // Connection refused
        socks_response_ipv4->rsv = rsv;              // RESERVED
        socks_response_ipv4->atyp = atyp;            // IPv4
        std::memset(socks_response_ipv4->bnd_addr,   // BND.ADDR
                    0,
                    4);
        std::memset(socks_response_ipv4->bnd_port,   // BND.PORT
                    0,
                    2);

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct socks_response_ipv4),
                           tv_sec,
                           tv_usec);

        return sen;
    }

    int32_t Server::send_socks_response_ipv6(char *buffer,
                                             int32_t buffer_size,
                                             char ver,
                                             char rep,
                                             char rsv,
                                             char atyp)
    {
        int32_t sen = 0;
        std::memset(buffer,
                    0,
                    buffer_size);

        struct socks_response_ipv6 *socks_response_ipv6 = (struct socks_response_ipv6 *)buffer;

        socks_response_ipv6->ver = ver;                // protocol version
        socks_response_ipv6->rep = rep;                // Connection refused
        socks_response_ipv6->rsv = rsv;                // RESERVED
        socks_response_ipv6->atyp = atyp;              // IPv6
        std::memset(socks_response_ipv6->bnd_addr,     // BND.ADDR
                    0,
                    16);
        std::memset(socks_response_ipv6->bnd_port,     // BND.PORT
                    0,
                    2);

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct socks_response_ipv6),
                           tv_sec,
                           tv_usec);

        return sen;
    }

    std::string Server::execute_command(const std::string& command)
    {
        HANDLE handle_read_pipe;
        HANDLE handle_write_pipe;
        SECURITY_ATTRIBUTES sa;
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        std::vector<char> buffer(4096);
        DWORD read_length = 0;
        std::string cmd = "cmd.exe /C " + command;
        std::string result;


        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        if(!CreatePipe(&handle_read_pipe, &handle_write_pipe, &sa, 0))
        {
#ifdef DEBUGPRINT
            std::printf("[-] CreatePipe error: %d\n",
                        GetLastError());
#endif
            return result;
        }

        SetHandleInformation(handle_read_pipe,
                             HANDLE_FLAG_INHERIT,
                             0);

        ZeroMemory(&pi,
                   sizeof(pi));

        ZeroMemory(&si,
                   sizeof(si));

        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdOutput = handle_write_pipe;
        si.hStdError = handle_write_pipe;

        if(!CreateProcess(NULL, const_cast<char *>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
#ifdef DEBUGPRINT
            std::printf("[-] CreateProcess error: %d\n",
                        GetLastError());
#endif
            result = "[-] CreateProcess error\n";
            CloseHandle(handle_read_pipe);
            CloseHandle(handle_write_pipe);
            return result;
        }

        CloseHandle(handle_write_pipe);

        while(ReadFile(handle_read_pipe, buffer.data(), buffer.size() - 1, &read_length, NULL)
              && read_length > 0)
        {
            buffer[read_length] = '\0';
            result += buffer.data();
        }

        CloseHandle(handle_read_pipe);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return result;
    }

    std::vector<std::string> Server::split_input(const std::string& input)
    {
        std::vector<std::string> tokens;
        std::istringstream stream(input);
        std::string token;

        while(stream >> token)
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    int32_t Server::forwarder_shell()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;

        std::string prompt = "\ncommand >";
        int32_t prompt_size = prompt.size();
        std::string input;
        std::vector <std::string> tokens;
        std::string result;
        int32_t result_size = 0;
        BOOL exit_flag = false;

        std::string upload_file_name = "";
        std::string upload_file_path = "";
        uint64_t upload_file_size;
        uint64_t recv_upload_file_data_size;
        uint64_t upload_file_remaining_size;
        char *upload_file_data = NULL;
        struct upload_download_data *upload_download_data;
        BOOL upload_file_flag = false;

        std::string download_file_name = "";
        std::string download_file_path = "";
        uint64_t download_file_size;
        uint64_t download_file_remaining_size;
        uint64_t read_bytes;


        while(1)
        {
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] recv_message\n");
#endif
            std::memset(buffer,
                        0,
                        buffer_max_length);

            rec = recv_message(buffer,
                               buffer_max_length,
                               forwarder_tv_sec,
                               forwarder_tv_usec);
            if(rec > 0)
            {
                if(recv_message_id == next_recv_message_id)
                {
                    next_recv_message_id++;
                    result = "";

                    upload_download_data = (struct upload_download_data *)buffer;
                    ret = strncmp(upload_download_data->command,
                                  "upload",
                                  sizeof("upload"));
                    if(ret == 0)
                    {
                        if(upload_file_flag == false)
                        {
                            upload_file_flag = true;
                            upload_file_path = upload_download_data->file_path;
                            upload_file_path += "\\";
                            upload_file_name = upload_download_data->file_name;
                            upload_file_name = upload_file_path + upload_file_name;
                            upload_file_size = ntohll(upload_download_data->file_size);
                            upload_file_data = (char *)calloc(upload_file_size,
                                                              sizeof(char));

                            std::memcpy(upload_file_data,
                                        upload_download_data->data,
                                        ntohll(upload_download_data->data_size));

                            recv_upload_file_data_size = ntohll(upload_download_data->data_size);
                            upload_file_remaining_size = upload_file_size - recv_upload_file_data_size;
                            if(upload_file_remaining_size > 0)
                            {
                                continue;
                            }
                        }else
                        {
                            std::memcpy(upload_file_data + recv_upload_file_data_size,
                                        upload_download_data->data,
                                        ntohll(upload_download_data->data_size));

                            recv_upload_file_data_size += ntohll(upload_download_data->data_size);
                            upload_file_remaining_size -= ntohll(upload_download_data->data_size);
                            if(upload_file_remaining_size > 0)
                            {
                                continue;
                            }
                        }

                        std::ofstream output_file(upload_file_name.c_str(),
                                                  std::ios::binary);

                        output_file.write(upload_file_data,
                                          upload_file_size);

                        output_file.close();

                        std::memset(buffer,
                                    0,
                                    buffer_max_length);

                        result = "[+] upload file: ";
                        result += upload_file_name;
                        result += prompt;

                        std::memcpy(buffer,
                                    result.c_str(),
                                    result.size());

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] send_message message_id:%u\n",
                                    send_message_id);
#endif
                        sen = send_message(buffer,
                                           result.size(),
                                           forwarder_tv_sec,
                                           forwarder_tv_usec);
                        if(sen > 0)
                        {
                            send_message_id++;
                        }

                        upload_file_flag = false;
                        upload_file_name = "";
                        upload_file_path = "";
                        upload_file_size = 0;
                        recv_upload_file_data_size = 0;
                        upload_file_remaining_size = 0;
                        free(upload_file_data);
                        upload_file_data = NULL;

                        continue;
                    }else
                    {

                        input = buffer;
                        tokens = split_input(input);
                        if(tokens.empty())
                        {
                            std::memset(buffer,
                                        0,
                                        buffer_max_length);

                            std::memcpy(buffer,
                                        prompt.c_str(),
                                        prompt_size);

#ifdef DEBUGPRINT
                            std::printf("[+] [client <- server] send_message message_id:%u\n",
                                        send_message_id);
#endif
                            sen = send_message(buffer,
                                               prompt_size,
                                               forwarder_tv_sec,
                                               forwarder_tv_usec);
                            if(sen > 0)
                            {
                                send_message_id++;
                            }

                            continue;
                        }

                        if(tokens[0] == "exit")
                        {
                            exit_flag = true;
                            break;
                        }else if(tokens[0] == "cd")
                        {
                            if(tokens.size() > 1)
                            {
                                ret = SetCurrentDirectory(tokens[1].c_str());
                                if(ret == 0)
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] change directory error: %s, %d\n",
                                                tokens[1].c_str(),
                                                GetLastError());
#endif
                                    result = "[-] change directory error\n";
                                }else{
                                    std::memset(buffer,
                                                0,
                                                buffer_max_length);

                                    std::memcpy(buffer,
                                                prompt.c_str(),
                                                prompt_size);

#ifdef DEBUGPRINT
                                    std::printf("[+] [client <- server] send_message message_id:%u\n",
                                                send_message_id);
#endif
                                    sen = send_message(buffer,
                                                       prompt_size,
                                                       forwarder_tv_sec,
                                                       forwarder_tv_usec);
                                    if(sen > 0)
                                    {
                                        send_message_id++;
                                    }

                                    continue;
                                }

                            }else
                            {
                                result = execute_command(input);
                            }
                        }else if(tokens.size() == 3
                                 &&tokens[0] == "download")
                        {
                            std::ifstream download_file(tokens[1].c_str(),
                                                        std::ios::binary);
                            if(!download_file.is_open())
                            {
#ifdef DEBUGPRINT
                                std::printf("[-] file open error\n");
#endif

                                std::memset(buffer,
                                            0,
                                            buffer_max_length);

                                result = "[-] file open error";
                                result += prompt;

                                std::memcpy(buffer,
                                            result.c_str(),
                                            result.size());

#ifdef DEBUGPRINT
                                std::printf("[+] [client <- server] send_message message_id:%u\n",
                                            send_message_id);
#endif
                                sen = send_message(buffer,
                                                   result.size(),
                                                   forwarder_tv_sec,
                                                   forwarder_tv_usec);
                                if(sen > 0)
                                {
                                    send_message_id++;
                                }
                            }else
                            {
                                download_file.seekg(0, std::ios::end);
                                download_file_size = download_file.tellg();
                                download_file_remaining_size = download_file_size;
                                download_file.seekg(0, std::ios::beg);

                                std::filesystem::path path(tokens[1].c_str());
                                download_file_name = path.filename().string();
                                download_file_path = tokens[2];

                                while(download_file_remaining_size > 0)
                                {
                                    std::memset(buffer,
                                                0,
                                                buffer_max_length);

                                    upload_download_data = (struct upload_download_data *)buffer;

                                    download_file.read(upload_download_data->data,
                                                       SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);

                                    read_bytes = download_file.gcount();

                                    std::memcpy(upload_download_data->command,
                                                "download",
                                                sizeof("download"));

                                    if(download_file_name.size() > 256)
                                    {
                                        std::memcpy(upload_download_data->file_name,
                                                    download_file_name.c_str(),
                                                    255);
                                    }else
                                    {
                                        std::memcpy(upload_download_data->file_name,
                                                    download_file_name.c_str(),
                                                    download_file_name.size());
                                    }

                                    if(download_file_path.size() > 256)
                                    {
                                        std::memcpy(upload_download_data->file_path,
                                                    download_file_path.c_str(),
                                                    255);
                                    }else
                                    {
                                        std::memcpy(upload_download_data->file_path,
                                                    download_file_path.c_str(),
                                                    download_file_path.size());
                                    }

                                    upload_download_data->file_size = htonll(download_file_size);
                                    upload_download_data->data_size = htonll(read_bytes);

                                    len = sizeof(struct upload_download_data_header) + read_bytes;

#ifdef DEBUGPRINT
                                    std::printf("[+] [client <- server] send_message message_id:%u\n",
                                                send_message_id);
#endif
                                    sen = send_message(buffer,
                                                       len,
                                                       forwarder_tv_sec,
                                                       forwarder_tv_usec);
                                    if(sen > 0)
                                    {
                                        send_message_id++;
                                    }

                                    download_file_remaining_size -= read_bytes;
                                }
                            }

                            continue;
                        }else
                        {
                            result = execute_command(input);
                        }

                        std::memset(buffer,
                                    0,
                                    buffer_max_length);

                        result_size = result.size();
                        if(result_size > SOCKS5_MESSAGE_DATA_SIZE - 0x10 - prompt_size)
                        {
                            result_size = SOCKS5_MESSAGE_DATA_SIZE - 0x10 - prompt_size;
                        }

                        std::memcpy(buffer,
                                    result.c_str(),
                                    result_size);

                        std::memcpy(buffer+result_size,
                                    prompt.c_str(),
                                    prompt_size);

                        result_size += prompt_size;

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] send_message message_id:%u\n",
                                    send_message_id);
#endif
                        sen = send_message(buffer,
                                           result_size,
                                           forwarder_tv_sec,
                                           forwarder_tv_usec);
                        if(sen > 0)
                        {
                            send_message_id++;
                        }
                    }
                }else
                {
                    msg = std::make_pair(rec,
                                         buffer);
                    msgs_map.insert({recv_message_id, msg});

                    check_msg_count = msgs_map.count(next_recv_message_id);
                    while(check_msg_count > 0)
                    {
                        msg = msgs_map[next_recv_message_id];
                        msgs_map.erase(next_recv_message_id);
                        next_recv_message_id++;
                        check_msg_count = msgs_map.count(next_recv_message_id);

                        result = "";
                        buffer = msg.second;

                        upload_download_data = (struct upload_download_data *)buffer;
                        ret = strncmp(upload_download_data->command,
                                      "upload",
                                      sizeof("upload"));
                        if(ret == 0)
                        {
                            if(upload_file_flag == false)
                            {
                                upload_file_flag = true;
                                upload_file_path = upload_download_data->file_path;
                                upload_file_path += "\\";
                                upload_file_name = upload_download_data->file_name;
                                upload_file_name = upload_file_path + upload_file_name;
                                upload_file_size = ntohll(upload_download_data->file_size);
                                upload_file_data = (char *)calloc(upload_file_size,
                                                                  sizeof(char));
                                std::memcpy(upload_file_data,
                                            upload_download_data->data,
                                            ntohll(upload_download_data->data_size));

                                recv_upload_file_data_size = ntohll(upload_download_data->data_size);
                                upload_file_remaining_size = upload_file_size - recv_upload_file_data_size;
                                if(upload_file_remaining_size > 0)
                                {
                                    free(buffer);
                                    continue;
                                }
                            }else
                            {
                                std::memcpy(upload_file_data + recv_upload_file_data_size,
                                            upload_download_data->data,
                                            ntohll(upload_download_data->data_size));

                                recv_upload_file_data_size += ntohll(upload_download_data->data_size);
                                upload_file_remaining_size -= ntohll(upload_download_data->data_size);
                                if(upload_file_remaining_size > 0)
                                {
                                    free(buffer);
                                    continue;
                                }
                            }

                            std::ofstream output_file(upload_file_name.c_str(),
                                                      std::ios::binary);

                            output_file.write(upload_file_data,
                                              upload_file_size);

                            output_file.close();

                            std::memset(buffer,
                                        0,
                                        buffer_max_length);

                            result = "[+] upload file: ";
                            result += upload_file_name;
                            result += prompt;

                            std::memcpy(buffer,
                                        result.c_str(),
                                        result.size());

#ifdef DEBUGPRINT
                            std::printf("[+] [client <- server] send_message message_id:%u\n",
                                        send_message_id);
#endif
                            sen = send_message(buffer,
                                               result.size(),
                                               forwarder_tv_sec,
                                               forwarder_tv_usec);
                            if(sen > 0)
                            {
                                send_message_id++;
                            }

                            upload_file_flag = false;
                            upload_file_name = "";
                            upload_file_path = "";
                            upload_file_size = 0;
                            recv_upload_file_data_size = 0;
                            upload_file_remaining_size = 0;
                            free(upload_file_data);
                            upload_file_data = NULL;

                            free(buffer);
                            continue;
                        }else
                        {
                            buffer = msg.second;
                            input = buffer;

                            tokens = split_input(input);
                            if(tokens.empty())
                            {
                                std::memset(buffer,
                                            0,
                                            buffer_max_length);

                                std::memcpy(buffer,
                                            prompt.c_str(),
                                            prompt_size);

#ifdef DEBUGPRINT
                                std::printf("[+] [client <- server] send_message message_id:%u\n",
                                            send_message_id);
#endif
                                sen = send_message(buffer,
                                                   prompt_size,
                                                   forwarder_tv_sec,
                                                   forwarder_tv_usec);
                                if(sen > 0)
                                {
                                    send_message_id++;
                                }

                                free(buffer);
                                continue;
                            }
                            if(tokens[0] == "exit")
                            {
                                exit_flag = true;
                                break;
                            }else if(tokens[0] == "cd")
                            {
                                if(tokens.size() > 1)
                                {
                                    ret = SetCurrentDirectory(tokens[1].c_str());
                                    if(ret == 0)
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] change directory error: %s, %d\n",
                                                    tokens[1].c_str(),
                                                    GetLastError());
#endif
                                        result = "[-] change directory error\n";
                                    }else
                                    {
                                        std::memset(buffer,
                                                    0,
                                                    buffer_max_length);

                                        std::memcpy(buffer,
                                                    prompt.c_str(),
                                                    prompt_size);

#ifdef DEBUGPRINT
                                        std::printf("[+] [client <- server] send_message message_id:%u\n",
                                                    send_message_id);
#endif
                                        sen = send_message(buffer,
                                                           prompt_size,
                                                           forwarder_tv_sec,
                                                           forwarder_tv_usec);
                                        if(sen > 0)
                                        {
                                            send_message_id++;
                                        }

                                        free(buffer);
                                        continue;
                                    }
                                }else
                                {
                                    result = execute_command(input);
                                }
                            }else if(tokens.size() == 3
                                     && tokens[0] == "download")
                            {
                                std::ifstream download_file(tokens[1].c_str(),
                                                            std::ios::binary);
                                if(!download_file.is_open())
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] file open error\n");
#endif

                                    std::memset(buffer,
                                                0,
                                                buffer_max_length);

                                    result = "[-] file open error";
                                    result += prompt;

                                    std::memcpy(buffer,
                                                result.c_str(),
                                                result.size());

#ifdef DEBUGPRINT
                                    std::printf("[+] [client <- server] send_message message_id:%u\n",
                                                send_message_id);
#endif
                                    sen = send_message(buffer,
                                                       result.size(),
                                                       forwarder_tv_sec,
                                                       forwarder_tv_usec);
                                    if(sen > 0)
                                    {
                                        send_message_id++;
                                    }
                                }else
                                {
                                    download_file.seekg(0, std::ios::end);
                                    download_file_size = download_file.tellg();
                                    download_file_remaining_size = download_file_size;
                                    download_file.seekg(0, std::ios::beg);

                                    std::filesystem::path path(tokens[1].c_str());
                                    download_file_name = path.filename().string();
                                    download_file_path = tokens[2];

                                    while(download_file_remaining_size > 0)
                                    {
                                        std::memset(buffer,
                                                    0,
                                                    buffer_max_length);

                                        upload_download_data = (struct upload_download_data *)buffer;

                                        download_file.read(upload_download_data->data,
                                                           SHELL_UPLOAD_DOWNLOAD_DATA_SIZE);

                                        read_bytes = download_file.gcount();

                                        std::memcpy(upload_download_data->command,
                                                    "download",
                                                    sizeof("download"));

                                        if(download_file_name.size() > 256)
                                        {
                                            std::memcpy(upload_download_data->file_name,
                                                        download_file_name.c_str(),
                                                        255);
                                        }else
                                        {
                                            std::memcpy(upload_download_data->file_name,
                                                        download_file_name.c_str(),
                                                        download_file_name.size());
                                        }

                                        if(download_file_path.size() > 256)
                                        {
                                            std::memcpy(upload_download_data->file_path,
                                                        download_file_path.c_str(),
                                                        255);
                                        }else
                                        {
                                            std::memcpy(upload_download_data->file_path,
                                                        download_file_path.c_str(),
                                                        download_file_path.size());
                                        }

                                        upload_download_data->file_size = htonll(download_file_size);
                                        upload_download_data->data_size = htonll(read_bytes);

                                        len = sizeof(struct upload_download_data_header) + read_bytes;

#ifdef DEBUGPRINT
                                        std::printf("[+] [client <- server] send_message message_id:%u\n",
                                                    send_message_id);
#endif
                                        sen = send_message(buffer,
                                                           len,
                                                           forwarder_tv_sec,
                                                           forwarder_tv_usec);
                                        if(sen > 0)
                                        {
                                            send_message_id++;
                                        }

                                        download_file_remaining_size -= read_bytes;
                                    }
                                }

                                free(buffer);
                                continue;
                            }else
                            {
                                result = execute_command(input);
                            }

                            std::memset(buffer,
                                        0,
                                        buffer_max_length);

                            result_size = result.size();
                            if(result_size > SOCKS5_MESSAGE_DATA_SIZE - 0x10 - prompt_size)
                            {
                                result_size = SOCKS5_MESSAGE_DATA_SIZE - 0x10 - prompt_size;
                            }

                            std::memcpy(buffer,
                                        result.c_str(),
                                        result_size);

                            std::memcpy(buffer+result_size,
                                        prompt.c_str(),
                                        prompt_size);

                            result_size += prompt_size;

#ifdef DEBUGPRINT
                            std::printf("[+] [client <- server] send_message message_id:%u\n",
                                        send_message_id);
#endif
                            sen = send_message(buffer,
                                               result_size,
                                               forwarder_tv_sec,
                                               forwarder_tv_usec);
                            if(sen > 0)
                            {
                                send_message_id++;
                            }
                        }

                        if(exit_flag == true)
                        {
                            break;
                        }

                        free(buffer);
                    }

                    buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                            sizeof(char));
                }
            }else
            {
                break;
            }
        }

        for(auto it = msgs_map.begin(); it != msgs_map.end();)
        {
            msg = it->second;
            free(msg.second);
            it = msgs_map.erase(it);
        }

        if(upload_file_data != NULL)
        {
            free(upload_file_data);
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_add_node()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;

        std::vector<char> config;
        int32_t config_size = 0;
        std::string result = "";
        int32_t result_size = 0;


#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv_message\n");
#endif
        std::memset(buffer,
                    0,
                    buffer_max_length);

        rec = recv_message(buffer,
                           buffer_max_length,
                           forwarder_tv_sec,
                           forwarder_tv_usec);
        if(rec > 0)
        {
            if(recv_message_id == next_recv_message_id)
            {
                config.resize(rec);
                std::memcpy(config.data(),
                            buffer,
                            rec);

                std::memset(buffer,
                            0,
                            buffer_max_length);

                ret = spider_command->read_config(config);
                if(ret != 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] read_config error\n");
#endif
                    result = "read_config ng\n";
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[+] read_config\n");
#endif
                    result = "read_config ok\n";
                }

                result_size = result.size();
                std::memcpy(buffer,
                            result.c_str(),
                            result_size);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] send_message message_id:%u\n",
                            send_message_id);
#endif
                sen = send_message(buffer,
                                   result_size,
                                   forwarder_tv_sec,
                                   forwarder_tv_usec);
                if(sen > 0)
                {
                    send_message_id++;
                }
            }
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> server] recv_message error\n");
#endif
            free(buffer);
            return -1;
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_show_node()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;

        std::string result = "";
        int32_t result_size = 0;


        // client
        result += client_manager->show_clients_map_string();

        // server
        result += server_manager->show_servers_map_string();

        // pipe
        result += pipe_manager->show_pipes_map_string();

        result_size = result.size();

        if(result_size <= socks5_message_data_max_size)
        {
            std::memcpy(buffer,
                        result.c_str(),
                        result_size);
        }else
        {
            std::memcpy(buffer,
                        result.c_str(),
                        socks5_message_data_max_size);

            result_size = socks5_message_data_max_size;
        }

#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] send_message message_id:%u\n",
                    send_message_id);
#endif
        sen = send_message(buffer,
                           result_size,
                           forwarder_tv_sec,
                           forwarder_tv_usec);
        if(sen > 0)
        {
            send_message_id++;
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_show_route()
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        recv_message_id = 0;

        std::string result = "";
        int32_t result_size = 0;


        result += routing_manager->show_routing_table_string();

        result_size = result.size();

        if(result_size <= socks5_message_data_max_size)
        {
            std::memcpy(buffer,
                        result.c_str(),
                        result_size);
        }else
        {
            std::memcpy(buffer,
                        result.c_str(),
                        socks5_message_data_max_size);

            result_size = socks5_message_data_max_size;
        }

#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] send_message message_id:%u\n",
                    send_message_id);
#endif
        sen = send_message(buffer,
                           result_size,
                           forwarder_tv_sec,
                           forwarder_tv_usec);
        if(sen > 0)
        {
            send_message_id++;
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_udp_recv_send_data(struct sockaddr *target_addr,
                                                 int target_addr_length)
    {
        int32_t ret = 0;
        int32_t rec = 0;
        int32_t sen = 0;
        int32_t len = 0;
        int32_t send_length = 0;
        fd_set readfds;
        fd_set writefds;
        struct timeval tv;

        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        int32_t socks5_message_data_max_size = (int32_t)SOCKS5_MESSAGE_DATA_SIZE;
        std::map<uint32_t, std::pair<int32_t, char *>> msgs_map;
        std::size_t check_msg_count = 0;
        std::pair<int32_t, char *> msg;
        recv_message_id = 0;


        while(1)
        {
            FD_ZERO(&writefds);
            FD_SET(target_sock, &writefds);
            tv.tv_sec = forwarder_tv_sec;
            tv.tv_usec = forwarder_tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
//                break;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[+] forwarder_udp_recv_send_data select error: 0x%x\n",
                            ret);
#endif
//                break;
            }

            ret = FD_ISSET(target_sock,
                           &writefds);
            if(ret){
#ifdef DEBUGPRINT
                std::printf("[+] [client -> server] recv_message\n");
#endif
                std::memset(buffer,
                            0,
                            buffer_max_length);

                rec = recv_message(buffer,
                                   buffer_max_length,
                                   FORWARDER_UDP_TIMEOUT,
                                   0);
                if(rec > 0)
                {
                    if(recv_message_id == next_recv_message_id)
                    {
                        len = rec;
                        send_length = 0;
#ifdef DEBUGPRINT
                        std::printf("[+] [server -> target] sendto message_id:%u\n",
                                    next_recv_message_id);
#endif
                        while(len > 0)
                        {
                            sen = sendto(target_sock,
                                         buffer+send_length,
                                         len,
                                         0,
                                         target_addr,
                                         target_addr_length);
                            if(sen == SOCKET_ERROR)
                            {
                                ret =  WSAGetLastError();
                                if(ret == WSAEWOULDBLOCK)
                                {
                                    std::this_thread::sleep_for(std::chrono::microseconds(5000));
                                    continue;
                                }else
                                {
#ifdef DEBUGPRINT
                                    std::printf("[-] forwarder_udp_recv_send_data sendto error: %d\n",
                                                ret);
#endif
//                                  free(buffer);
//                                  return 0;
                                }
                            }
                            send_length += sen;
                            len -= sen;
                        }

                        next_recv_message_id++;
                    }else
                    {
                        msg = std::make_pair(rec,
                                             buffer);
                        msgs_map.insert({recv_message_id, msg});

                        check_msg_count = msgs_map.count(next_recv_message_id);
                        while(check_msg_count > 0)
                        {
                            msg = msgs_map[next_recv_message_id];
                            msgs_map.erase(next_recv_message_id);

                            len = msg.first;
                            buffer = msg.second;
                            send_length = 0;
#ifdef DEBUGPRINT
                            std::printf("[+] [server -> target] sendto message_id:%u\n",
                                        next_recv_message_id);
#endif
                            while(len > 0)
                            {
                                sen = sendto(target_sock,
                                             buffer+send_length,
                                             len,
                                             0,
                                             target_addr,
                                             target_addr_length);
                                if(sen == SOCKET_ERROR)
                                {
                                    ret =  WSAGetLastError();
                                    if(ret == WSAEWOULDBLOCK)
                                    {
                                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                                        continue;
                                    }else
                                    {
#ifdef DEBUGPRINT
                                        std::printf("[-] forwarder_udp_recv_send_data sendto error: %d\n",
                                                    ret);
#endif
//                                      free(buffer);
//                                      return 0;
                                    }
                                }
                                send_length += sen;
                                len -= sen;
                            }

                            free(buffer);
                            next_recv_message_id++;
                            check_msg_count = msgs_map.count(next_recv_message_id);
                        }

                        buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                                sizeof(char));
                    }
                }else
                {
                    break;
                }


                FD_ZERO(&readfds);
                FD_SET(target_sock, &readfds);
                tv.tv_sec = forwarder_tv_sec;
                tv.tv_usec = forwarder_tv_usec;

                ret = select(NULL,
                             &readfds,
                             NULL,
                             NULL,
                             &tv);
                if(ret == 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] forwarder_udp_recv_send_data select timeout\n");
#endif
//                    break;
                }else if(ret == SOCKET_ERROR)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] forwarder_udp_recv_send_data select error: 0x%x\n",
                                ret);
#endif
//                    break;
                }

                ret = FD_ISSET(target_sock,
                               &readfds);
                if(ret)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [server <- target] recvfrom\n");
#endif
                    std::memset(buffer,
                                0,
                                buffer_max_length);

                    rec = recvfrom(target_sock,
                                   buffer,
                                   socks5_message_data_max_size,
                                   0,
                                   target_addr,
                                   (socklen_t *)&target_addr_length);
                    if(rec == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
                        if(ret == WSAEWOULDBLOCK)
                        {
                            std::this_thread::sleep_for(std::chrono::microseconds(5000));
                            continue;
                        }else
                        {
#ifdef DEBUGPRINT
                            std::printf("[-] forwarder_udp_recv_send_data recvfrom error: %d\n",
                                        ret);
#endif
//                            break;
                        }
                    }else if(rec > 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] send_message message_id:%u\n",
                                    send_message_id);
#endif
                        sen = send_message(buffer,
                                           rec,
                                           forwarder_tv_sec,
                                           forwarder_tv_usec);
                        if(sen <= 0){
//                          break;
                        }else
                        {
                            send_message_id++;
                        }
                    }else
                    {
//                        break;
                    }
                }
            }
        }

        for(auto it = msgs_map.begin(); it != msgs_map.end();)
        {
            msg = it->second;
            free(msg.second);
            it = msgs_map.erase(it);
        }

        free(buffer);
        return 0;
    }

    int32_t Server::forwarder_udp(struct sockaddr *target_addr,
                                  int target_addr_lengthh)
    {
        std::thread thread(&Server::forwarder_udp_recv_send_data,
                           this,
                           target_addr,
                           target_addr_lengthh);

        thread.join();

        return 0;
    }

    int32_t Server::do_socks5_connection(std::shared_ptr<Socks5message> socks5_message)
    {
        BOOL socks5_connect_flag = true;
        BOOL socks5_connect_shell_flag = false;
        BOOL socks5_connect_add_node_flag = false;
        BOOL socks5_connect_show_node_flag = false;
        BOOL socks5_connect_show_route_flag = false;
        BOOL socks5_connect_udp_flag = false;

        static char authentication_method = SOCKS5_AUTHENTICATION_METHOD;   // 0x0:No Authentication Required  0x2:Username/Password Authentication
        char username[256] = SOCKS5_USERNAME;
        char password[256] = SOCKS5_PASSWORD;

        int32_t rec;
        int32_t sen;
        int32_t ret;
        char *buffer = (char *)calloc(NODE_BUFFER_SIZE,
                                      sizeof(char));
        int32_t buffer_max_length = (int32_t)NODE_BUFFER_SIZE;
        recv_message_id = 0;
        next_recv_message_id = 0;
        send_message_id = generate_random_id();

        unsigned char method;
        char ver;

        unsigned char ulen = 0;
        unsigned char plen = 0;
        char uname[256] = {0};
        char passwd[256] = {0};

        struct socks_request *socks_request = (struct socks_request *)buffer;
        struct socks_request_ipv4 *socks_request_ipv4;
        struct socks_request_domainname *socks_request_domainname;
        struct socks_request_ipv6 *socks_request_ipv6;

        char atyp;
        char cmd;

        struct sockaddr_in target_addr, *tmp_ipv4;      // IPv4
        struct sockaddr_in6 target_addr6, *tmp_ipv6;    // IPv6
        struct addrinfo hints, *target_host_addr_info;
        int target_addr_length = sizeof(target_addr);
        int target_addr6_length = sizeof(target_addr6);

        std::memset(&target_addr,
                    0,
                    sizeof(struct sockaddr_in));

        std::memset(&target_addr6,
                    0,
                    sizeof(struct sockaddr_in6));

        std::memset(&hints,
                    0,
                    sizeof(struct addrinfo));

        int family = 0;
        char domainname[256] = {0};
        std::string domainname_string;
        unsigned short domainname_length = 0;

        char target_addr6_string[INET6_ADDR_STRING_LENGTH + 1] = {0};
        char *target_addr6_string_pointer = target_addr6_string;

        std::string ipv6_link_local_prefix = "fe80:";


        // socks SELECTION_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv selection request\n");
#endif
        rec = socks5_message->get_data_size();
        if(rec <= 0 || rec > NODE_BUFFER_SIZE){
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] recv selection request error\n");
#endif
            free(buffer);
            return -1;
        }
        std::memcpy(buffer,
                    socks5_message->get_data(),
                    rec);
        recv_message_id = socks5_message->get_message_id();
        next_recv_message_id = recv_message_id + 1;

        if(encryption != nullptr
           && encryption->get_flag())
        {
            ret = encryption->decrypt(buffer,
                                      rec,
                                      buffer_max_length);
            if(ret <= 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client -> server] selection request decrypt error: %d\n",
                            ret);
#endif
                free(buffer);
                return -1;
            }

            rec = ret;
        }

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv selection request: %d bytes\n",
                    rec);
#endif
        struct selection_request *selection_request = (struct selection_request *)buffer;
        method = 0xFF;
        for(int i=0; i<selection_request->nmethods; i++)
        {
            if(selection_request->methods[i] == authentication_method)  // NO AUTHENTICATION REQUIRED or USERNAME/PASSWORD
            {
#ifdef DEBUGPRINT
                std::printf("[+] selection_request->methods[%d]: %02x\n", i, selection_request->methods[i]);
#endif
                method = selection_request->methods[i];
                break;
            }
        }
        if(method == 0xFF)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> server] selection request method error\n");
#endif
        }
        ver = selection_request->ver;


        // socks SELECTION_RESPONSE [client <- server]
        std::memset(buffer,
                    0,
                    buffer_max_length);
        struct selection_response *selection_response = (struct selection_response *)buffer;
        selection_response->ver = 0x5;          // socks version 5
        selection_response->method = method;    // no authentication required or username/password
        if(ver != 0x5 || authentication_method != method)
        {
            selection_response->method = 0xFF;
        }

        send_message_id++;
        sen = send_message(buffer,
                           sizeof(struct selection_response),
                           tv_sec,
                           tv_usec);

#ifdef DEBUGPRINT
        std::printf("[+] [client <- server] send selection response: %d bytes\n",
                    sen);
#endif

        if(authentication_method != method)
        {
#ifdef DEBUGPRINT
            std::printf("[-] authentication method error: server:0x%x client:0x%x\n",
                        authentication_method,
                        method);
#endif
            free(buffer);
            return -1;
        }


        // socks USERNAME_PASSWORD_AUTHENTICATION [client -> server]
        if(method == 0x2)
        {
            // socks USERNAME_PASSWORD_AUTHENTICATION_REQUEST [client -> server]
#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] recv username password authentication request\n");
#endif

            rec = recv_message(buffer,
                               buffer_max_length,
                               tv_sec,
                               tv_usec);

            if(rec <= 0
               || next_recv_message_id != recv_message_id)
            {
#ifdef DEBUGPRINT
                std::printf("[-] [client -> server] recv username password authentication request error\n");
#endif
                free(buffer);
                return -1;
            }

            next_recv_message_id++;

#ifdef DEBUGPRINT
            std::printf("[+] [client -> server] Receive username password authentication request: %d bytes\n",
                        rec);
#endif
            struct username_password_authentication_request_tmp *username_password_authentication_request_tmp = (struct username_password_authentication_request_tmp *)buffer;

            ulen = username_password_authentication_request_tmp->ulen;
            memcpy(uname, &username_password_authentication_request_tmp->uname, ulen);
            memcpy(&plen, &username_password_authentication_request_tmp->uname + ulen, 1);
            memcpy(passwd, &username_password_authentication_request_tmp->uname + ulen + 1, plen);
#ifdef DEBUGPRINT
            std::printf("[+] uname:%s, ulen:%d, passwd:%s, plen:%d\n",
                        uname,
                        ulen,
                        passwd,
                        plen);
#endif
            ver = username_password_authentication_request_tmp->ver;


            // socks USERNAME_PASSWORD_AUTHENTICATION_RESPONSE [client <- server]
            std::memset(buffer,
                        0,
                        buffer_max_length);
            struct username_password_authentication_response *username_password_authentication_response = (struct username_password_authentication_response *)buffer;
            username_password_authentication_response->ver = 0x1;

            if(ver == 0x1
               && !strncmp(uname, username, sizeof(username))
               && !strncmp(passwd, password, sizeof(password)))
            {
#ifdef DEBUGPRINT
                std::printf("[+] succeed username password authentication\n");
#endif
                username_password_authentication_response->status = 0x0;

                send_message_id++;
                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] send username password authentication response: %d bytes\n",
                            sen);
#endif
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] fail username password authentication\n");
#endif
                username_password_authentication_response->status = 0xFF;

                send_message_id++;
                sen = send_message(buffer,
                                   sizeof(struct username_password_authentication_response),
                                   tv_sec,
                                   tv_usec);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] send selection response: %d bytes\n",
                            sen);
#endif
                free(buffer);
                return -1;
            }
        }


        // socks SOCKS_REQUEST [client -> server]
#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv socks request\n");
#endif
        std::memset(buffer,
                    0,
                    buffer_max_length);

        rec = recv_message(buffer,
                           buffer_max_length,
                           tv_sec,
                           tv_usec);

        if(rec <= 0
           || next_recv_message_id != recv_message_id)
        {
#ifdef DEBUGPRINT
            std::printf("[-] [client -> server] recv socks request error\n");
#endif
            free(buffer);
            return -1;
        }

        next_recv_message_id++;

#ifdef DEBUGPRINT
        std::printf("[+] [client -> server] recv socks request: %d bytes\n",
                    rec);
#endif

        atyp = socks_request->atyp;
        if(atyp != 0x1
           && atyp != 0x3
           && atyp != 0x4
           && atyp != 0x0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks request atyp(%d) error\n",
                        atyp);
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x8,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }

        cmd = socks_request->cmd;
        if(cmd != 0x1       // CONNECT (0x1)
           && cmd != 0x8    // CONNECT UDP (0x8, UDP over TCP, original command)
           && cmd != 0x9    // SHELL (0x9, shell, original command)
           && cmd != 0xa    // ADD NODE (0xa, add node, original command)
           && cmd != 0xb    // SHOW NODE (0xb, show node information, original command)
           && cmd != 0xc)   // SHOW ROUTE (0xc, show routing table, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[-] socks request cmd(%d) error\n",
                        cmd);
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            if(atyp == 0x1
               || atyp == 0x3)  // IPv4
            {
                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);
            }else   // IPv6
            {
                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);
            }

            free(buffer);
            return -1;
        }

        if(socks_request->atyp == 0x1)  // IPv4
        {
            family = AF_INET;
            target_addr.sin_family = AF_INET;
            socks_request_ipv4 = (struct socks_request_ipv4 *)buffer;
            std::memcpy(&target_addr.sin_addr.s_addr,
                        &socks_request_ipv4->dst_addr,
                        4);
            std::memcpy(&target_addr.sin_port,
                        &socks_request_ipv4->dst_port,
                        2);
        }else if(socks_request->atyp == 0x3)    // domain name
        {
            socks_request_domainname = (struct socks_request_domainname*)buffer;
            domainname_length = socks_request_domainname->dst_addr_len;
            memcpy(&domainname,
                   &socks_request_domainname->dst_addr,
                   domainname_length);
            domainname_string = domainname;
#ifdef DEBUGPRINT
            std::printf("[+] domainname:%s, length:%d\n", domainname_string.c_str(), domainname_length);
#endif

            if(domainname_string.find(":") == std::string::npos)   // ipv4 address or domainname
            {
                hints.ai_family = AF_INET;  // IPv4
                ret = getaddrinfo(domainname_string.c_str(),
                                  NULL,
                                  &hints,
                                  &target_host_addr_info);
                if(ret != 0)
                {
                    hints.ai_family = AF_INET6; // IPv6
                    ret = getaddrinfo(domainname_string.c_str(),
                                      NULL,
                                      &hints,
                                      &target_host_addr_info);
                    if(ret != 0)
                    {
#ifdef DEBUGPRINT
                        std::printf("[-] cannot resolv the domain name: %s\n",
                                    domainname_string.c_str());
#endif

                        // socks SOCKS_RESPONSE send error [client <- server]
                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

                        free(buffer);
                        return -1;
                    }
                }
            }else   // ipv6 address
            {
                hints.ai_family = AF_INET6; // IPv6
                ret = getaddrinfo(domainname_string.c_str(),
                                  NULL,
                                  &hints,
                                  &target_host_addr_info);
                if(ret != 0)
                {
#ifdef DEBUGPRINT
                    std::printf("[-] cannot resolv the domain name: %s\n",
                                domainname_string.c_str());
#endif

                    // socks SOCKS_RESPONSE send error [client <- server]
                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);
                    free(buffer);
                    return -1;
                }
            }

            if(target_host_addr_info->ai_family == AF_INET)
            {
                tmp_ipv4 = (struct sockaddr_in *)target_host_addr_info->ai_addr;

                family = AF_INET;
                target_addr.sin_family = AF_INET;
                std::memcpy(&target_addr.sin_addr,
                            &tmp_ipv4->sin_addr,
                            sizeof(struct in_addr));
                std::memcpy(&target_addr.sin_port,
                            &socks_request_domainname->dst_addr[domainname_length],
                            2);

                freeaddrinfo(target_host_addr_info);
            }else if(target_host_addr_info->ai_family == AF_INET6)
            {
                tmp_ipv6 = (struct sockaddr_in6 *)target_host_addr_info->ai_addr;

                family = AF_INET6;
                target_addr6.sin6_family = AF_INET6;
                memcpy(&target_addr6.sin6_addr,
                       &tmp_ipv6->sin6_addr,
                       sizeof(struct in6_addr));
                memcpy(&target_addr6.sin6_port,
                       &socks_request_domainname->dst_addr[domainname_length],
                       2);

                freeaddrinfo(target_host_addr_info);
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                // socks SOCKS_RESPONSE send error [client <- server]
                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);


                freeaddrinfo(target_host_addr_info);
                free(buffer);
                return -1;
            }
        }else if(socks_request->atyp == 0x4)    // IPv6
        {
            family = AF_INET6;
            target_addr6.sin6_family = AF_INET6;
            socks_request_ipv6 = (struct socks_request_ipv6*)buffer;
            memcpy(&target_addr6.sin6_addr,
                   &socks_request_ipv6->dst_addr,
                   16);
            memcpy(&target_addr6.sin6_port,
                   &socks_request_ipv6->dst_port,
                   2);
        }else if((cmd != 0x9
                  && cmd != 0xa
                  && cmd != 0xb
                  && cmd != 0xc)
                 && socks_request->atyp != 0x0)
        {
#ifdef DEBUGPRINT
            std::printf("[-] not implemented\n");
#endif

            // socks SOCKS_RESPONSE send error [client <- server]
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x1,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }


        // socks SOCKS_RESPONSE [client <- server]
        if(atyp == 0x1) // IPv4
        {
            target_ip = inet_ntoa(target_addr.sin_addr);
            target_port = std::to_string(ntohs(target_addr.sin_port));

            if(cmd == 0x1)  // CONNECT
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET,
                                     SOCK_STREAM,
                                     0);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                ret = connect(target_sock,
                              (struct sockaddr *)&target_addr,
                              sizeof(target_addr));
                if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] cannot connect error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    closesocket(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef DEBUGPRINT
                std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x1);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

            }else if(cmd == 0x2)    // BIND
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: BIND\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }else if(cmd == 0x3)    // UDP ASSOCIATE
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                std::printf("[+] ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                target_sock = socket(AF_INET,
                                     SOCK_DGRAM,
                                     IPPROTO_UDP);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif
                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x1);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

                socks5_connect_flag = false;
                socks5_connect_udp_flag = true;
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }
        }else if(atyp == 0x3)   // domainname
        {
            if(family == AF_INET)   // IPv4
            {
                target_ip = inet_ntoa(target_addr.sin_addr);
                target_port = std::to_string(ntohs(target_addr.sin_port));

                if(cmd == 0x1)  // CONNECT
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET,
                                         SOCK_STREAM,
                                         0);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr,
                                  sizeof(target_addr));
                    if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] cannot connect error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        closesocket(target_sock);
                        free(buffer);
                        return -1;
                    }
#ifdef DEBUGPRINT
                    std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: BIND\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    target_sock = socket(AF_INET,
                                         SOCK_DGRAM,
                                         IPPROTO_UDP);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv4(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x1);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x1);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    socks5_connect_flag = false;
                    socks5_connect_udp_flag = true;
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv4(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x1,
                                                   0x0,
                                                   0x1);

                    free(buffer);
                    return -1;
                }
            }else if(family == AF_INET6)    // IPv6
            {
                inet_ntop(AF_INET6,
                          &target_addr6.sin6_addr,
                          target_addr6_string_pointer,
                          INET6_ADDR_STRING_LENGTH);

                if(target_addr6.sin6_scope_id > 0)
                {
                    target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                    target_port = std::to_string(ntohs(target_addr6.sin6_port));
                }else
                {
                    target_ip = target_addr6_string_pointer;
                    target_port = std::to_string(ntohs(target_addr6.sin6_port));

                    ret = target_ip.rfind(ipv6_link_local_prefix, 0);
                    if(ret == 0)     // ipv6 link local address
                    {
                        target_addr6.sin6_scope_id = (uint32_t)std::stoi(spider_ip->get_spider_ipv6_link_local_scope_id());
                        target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                    }
                }

                if(cmd == 0x1)  // CONNECT
                {
#ifdef DEBUGPRINT
                    std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                    target_sock = socket(AF_INET6,
                                         SOCK_STREAM,
                                         0);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    ret = connect(target_sock,
                                  (struct sockaddr *)&target_addr6,
                                  sizeof(target_addr6));
                    if(ret == SOCKET_ERROR)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] cannot connect error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        closesocket(target_sock);
                        free(buffer);
                        return -1;
                    }

#ifdef DEBUGPRINT
                    std::printf("[+] [server <- target] connected ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif
                }else if(cmd == 0x2)    // BIND
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: BIND\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x3)    // UDP ASSOCIATE
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x7,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
                {
#ifdef DEBUGPRINT
                    std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                    std::printf("[+] ip: %s port: %s\n",
                                target_ip.c_str(),
                                target_port.c_str());
#endif

                    target_sock = socket(AF_INET6,
                                         SOCK_DGRAM,
                                         IPPROTO_UDP);
                    if(target_sock == INVALID_SOCKET)
                    {
                        ret = WSAGetLastError();
#ifdef DEBUGPRINT
                        std::printf("[-] [server <- target] socket error: %d\n",
                                    ret);
#endif

                        sen = send_socks_response_ipv6(buffer,
                                                       buffer_max_length,
                                                       0x5,
                                                       0x5,
                                                       0x0,
                                                       0x4);

#ifdef DEBUGPRINT
                        std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                    rec,
                                    sen);
#endif

                        free(buffer);
                        return -1;
                    }

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x0,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    socks5_connect_flag = false;
                    socks5_connect_udp_flag = true;
                }else
                {
#ifdef DEBUGPRINT
                    std::printf("[-] not implemented\n");
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x1,
                                                   0x0,
                                                   0x4);

                    free(buffer);
                    return -1;
                }
            }else
            {
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv4(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x1);

                free(buffer);
                return -1;
            }
        }else if(atyp == 0x4)   // IPv6
        {
            inet_ntop(AF_INET6,
                      &target_addr6.sin6_addr,
                      target_addr6_string_pointer,
                      INET6_ADDR_STRING_LENGTH);

            if(target_addr6.sin6_scope_id > 0)
            {
                target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                target_port = std::to_string(ntohs(target_addr6.sin6_port));
            }else
            {
                target_ip = target_addr6_string_pointer;
                target_port = std::to_string(ntohs(target_addr6.sin6_port));

                ret = target_ip.rfind(ipv6_link_local_prefix, 0);
                if(ret == 0)     // ipv6 link local address
                {
                    target_addr6.sin6_scope_id = (uint32_t)std::stoi(spider_ip->get_spider_ipv6_link_local_scope_id());
                    target_ip = target_addr6_string_pointer + std::string("%") + std::to_string(target_addr6.sin6_scope_id);
                }
            }

            if(cmd == 0x1)  // CONNECT
            {
#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connecting ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT\n");
#endif
                target_sock = socket(AF_INET6,
                                     SOCK_STREAM,
                                     0);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                ret = connect(target_sock,
                              (struct sockaddr *)&target_addr6,
                              sizeof(target_addr6));
                if(ret == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    closesocket(target_sock);
                    free(buffer);
                    return -1;
                }

#ifdef DEBUGPRINT
                std::printf("[+] [server -> target] connected ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x4);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif
            }else if(cmd == 0x2)    // BIND
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: BIND\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }else if(cmd == 0x3)    // UDP ASSOCIATE
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: UDP ASSOCIATE\n");
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x7,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }else if(cmd == 0x8)    // CONNECT UDP (0x8, UDP over TCP, original command)
            {
#ifdef DEBUGPRINT
                std::printf("[+] socks5 response cmd: CONNECT UDP (0x8, UDP over TCP, original command)\n");
#endif

#ifdef DEBUGPRINT
                std::printf("[+] ip: %s port: %s\n",
                            target_ip.c_str(),
                            target_port.c_str());
#endif

                target_sock = socket(AF_INET6,
                                     SOCK_DGRAM,
                                     IPPROTO_UDP);
                if(target_sock == INVALID_SOCKET)
                {
                    ret = WSAGetLastError();
#ifdef DEBUGPRINT
                    std::printf("[-] [server <- target] socket error: %d\n",
                                ret);
#endif

                    sen = send_socks_response_ipv6(buffer,
                                                   buffer_max_length,
                                                   0x5,
                                                   0x5,
                                                   0x0,
                                                   0x4);

#ifdef DEBUGPRINT
                    std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                                rec,
                                sen);
#endif

                    free(buffer);
                    return -1;
                }

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x0,
                                               0x0,
                                               0x4);

#ifdef DEBUGPRINT
                std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                            rec,
                            sen);
#endif

                socks5_connect_flag = false;
                socks5_connect_udp_flag = true;
            }else{
#ifdef DEBUGPRINT
                std::printf("[-] not implemented\n");
#endif

                sen = send_socks_response_ipv6(buffer,
                                               buffer_max_length,
                                               0x5,
                                               0x1,
                                               0x0,
                                               0x4);

                free(buffer);
                return -1;
            }
        }else if(cmd == 0x9
                 && atyp == 0x0)    // SHELL (0x9, shell, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[+] socks5 response cmd: CONNECT SHELL (0x9, original command)\n");
#endif
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x0,
                                           0x0,
                                           0x0);

#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                        rec,
                        sen);
#endif

            socks5_connect_flag = false;
            socks5_connect_shell_flag = true;
        }else if(cmd == 0xa
                 && atyp == 0x0)    // ADD NODE (0xa, add node, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[+] socks5 response cmd: ADD NODE (0xa, original command)\n");
#endif
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x0,
                                           0x0,
                                           0x0);

#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                        rec,
                        sen);
#endif

            socks5_connect_flag = false;
            socks5_connect_add_node_flag = true;
        }else if(cmd == 0xb
                 && atyp == 0x0)    // SHOW NODE (0xb, show node information, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[+] socks5 response cmd: SHOW NODE (0xb, original command)\n");
#endif
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x0,
                                           0x0,
                                           0x0);

#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                        rec,
                        sen);
#endif

            socks5_connect_flag = false;
            socks5_connect_show_node_flag = true;
        }else if(cmd == 0xc
                 && atyp == 0x0)     // SHOW ROUTE (0xc, show routing table, original command)
        {
#ifdef DEBUGPRINT
            std::printf("[+] socks5 response cmd: SHOW ROUTE (0xc, original command)\n");
#endif
            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x0,
                                           0x0,
                                           0x0);

#ifdef DEBUGPRINT
            std::printf("[+] [client <- server] socks request: %d bytes, socks response: %d bytes\n",
                        rec,
                        sen);
#endif

            socks5_connect_flag = false;
            socks5_connect_show_route_flag = true;
        }else
        {
#ifdef DEBUGPRINT
            std::printf("[-] not implemented\n");
#endif

            sen = send_socks_response_ipv4(buffer,
                                           buffer_max_length,
                                           0x5,
                                           0x1,
                                           0x0,
                                           0x1);

            free(buffer);
            return -1;
        }


        // forwarder [client <> client <> server <> target]
#ifdef DEBUGPRINT
        std::printf("[+] [client <> client <> server <> target] forwarder\n");
#endif

        send_message_id++;
        if(socks5_connect_flag == true)
        {
            ret = forwarder();
        }else if(socks5_connect_shell_flag == true)
        {
            ret = forwarder_shell();
        }else if(socks5_connect_add_node_flag == true)
        {
            ret = forwarder_add_node();
        }else if(socks5_connect_show_node_flag == true)
        {
            ret = forwarder_show_node();
        }else if(socks5_connect_show_route_flag == true)
        {
            ret = forwarder_show_route();
        }else if(socks5_connect_udp_flag == true)
        {
            if(family == AF_INET)
            {
                ret = forwarder_udp((sockaddr *)&target_addr,
                                    target_addr_length);
            }else if(family == AF_INET6)
            {
                ret = forwarder_udp((sockaddr *)&target_addr6,
                                    target_addr6_length);
            }
        }


#ifdef DEBUGPRINT
        std::printf("[+] worker exit\n");
#endif

        closesocket(target_sock);
        free(buffer);
        return -1;
    }
}

