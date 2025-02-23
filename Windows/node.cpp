/*
 * Title:  spider node.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "node.hpp"
#include "messagemanager.hpp"
#include "socks5message.hpp"
#include "socks5messagequeue.hpp"


namespace spider
{
    Node::Node(SOCKET sock,
               std::shared_ptr<Messagemanager> message_manager)
    {
        this->sock = sock;
        this->message_manager = message_manager;
        this->socks5_messages_queue = std::make_unique<Socks5messagequeue>();
    }

    Node::~Node()
    {

    }

    void Node::set_sock(SOCKET sock)
    {
        this->sock = sock;
    }

    SOCKET Node::get_sock()
    {
        return sock;
    }

    void Node::set_message_manager(std::shared_ptr<Messagemanager> messagemanager)
    {
        this->message_manager = message_manager;
    }

    std::shared_ptr<Messagemanager> Node::get_message_manager()
    {
        return message_manager;
    }

    void Node::push_socks5_message(std::shared_ptr<Socks5message> socks5_message)
    {

        socks5_messages_queue->push(socks5_message);

        return;
    }

    int32_t Node::push_timeout_socks5_message(std::shared_ptr<Socks5message> socks5_message,
                                              int32_t tv_sec,
                                              int32_t tv_usec)
    {
        int ret = 0;

        ret = socks5_messages_queue->push_timeout(socks5_message,
                                                  tv_sec,
                                                  tv_usec);

        return ret;
    }

    std::shared_ptr<Socks5message> Node::pop_socks5_message()
    {
        std::shared_ptr<Socks5message> socks5_message = socks5_messages_queue->pop();

        return socks5_message;
    }

    std::shared_ptr<Socks5message> Node::pop_timeout_socks5_message(int32_t tv_sec,
                                                                    int32_t tv_usec)
    {
        std::shared_ptr<Socks5message> socks5_message = socks5_messages_queue->pop_timeout(tv_sec,
                                                                                           tv_usec);

        return socks5_message;
    }

    int32_t Node::recv_data(char *buffer,
                            int32_t buffer_size,
                            long tv_sec,
                            long tv_usec)
    {
        int ret = 0;
        int32_t rec = 0;
        fd_set readfds;
        struct timeval tv;
        struct timeval start;
        struct timeval end;
        long t = 0;


        if(buffer_size > NODE_BUFFER_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] buffer size error\n");
#endif
            return -1;
        }

        std::memset((char *)buffer,
                    0,
                    buffer_size);

        ret = gettimeofday(&start, NULL);
        if(ret == -1)
        {
#ifdef DEBUGPRINT
            std::printf("[-] gettimeofday error\n");
#endif
            return -1;
        }

        while(1)
        {
            ret = gettimeofday(&end, NULL);
            if(ret == -1)
            {
#ifdef DEBUGPRINT
                std::printf("[-] gettimeofday error\n");
#endif
                return -1;
            }

            t = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);	// microsecond
            if(t >= (tv_sec * 1000000 + tv_usec))
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_data timeout\n");
#endif
                return -1;
            }

            FD_ZERO(&readfds);
            FD_SET(sock,
                   &readfds);
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(NULL,
                         &readfds,
                         NULL,
                         NULL,
                         &tv);
            if(ret == 0)
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_data select timeout\n");
#endif
                return -1;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[-] recv_data select error: 0x%x\n", ret);
#endif
                return -1;
            }

            ret = FD_ISSET(sock,
                           &readfds);
            if(ret)
            {
                rec = recv(sock,
                           buffer,
                           NODE_BUFFER_SIZE,
                           0);
                if(rec == SOCKET_ERROR)
                {
                    ret = WSAGetLastError();
                    if(ret == WSAEWOULDBLOCK)
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        continue;
                    }
#ifdef DEBUGPRINT
                    std::printf("[-] recv error: %d\n",
                                ret);
#endif
                    return -1;
                }else
                {
                    break;
                }
            }
        }

        return rec;
    }

    int32_t Node::send_data(char *buffer,
                            int32_t data_size,
                            long tv_sec,
                            long tv_usec)
    {
        int ret = 0;
        int32_t sen = 0;
        int32_t send_length = 0;
        int32_t len = 0;
        fd_set writefds;
        struct timeval tv;
        struct timeval start;
        struct timeval end;
        long t = 0;


        if(data_size > NODE_BUFFER_SIZE)
        {
#ifdef DEBUGPRINT
            std::printf("[-] data size error\n");
#endif
            return -1;
        }

        len = data_size;
        ret = gettimeofday(&start,
                           NULL);
        if(ret == -1)
        {
#ifdef DEBUGPRINT
            std::printf("[-] gettimeofday error\n");
#endif
            return -1;
        }

        while(len > 0)
        {
            ret = gettimeofday(&end,
                               NULL);
            if(ret == -1)
            {
#ifdef DEBUGPRINT
                std::printf("[-] gettimeofday error\n");
#endif
                return -1;
            }

            t = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);	// microsecond
            if(t >= (tv_sec * 1000000 + tv_usec))
            {
#ifdef DEBUGPRINT
                std::printf("[-] send_data timeout\n");
#endif
                return -1;
            }

            FD_ZERO(&writefds);
            FD_SET(sock,
                   &writefds);
            tv.tv_sec = tv_sec;
            tv.tv_usec = tv_usec;

            ret = select(NULL,
                         NULL,
                         &writefds,
                         NULL,
                         &tv);
            if(ret == 0){
#ifdef DEBUGPRINT
                std::printf("[-] send_data select timeout\n");
#endif
                return -1;
            }else if(ret == SOCKET_ERROR)
            {
#ifdef DEBUGPRINT
                std::printf("[-] send_data select error: 0x%0x\n", ret);
#endif
                return -1;
            }

            ret = FD_ISSET(sock,
                           &writefds);
            if(ret)
            {
                sen = send(sock,
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
                    }
#ifdef DEBUGPRINT
                    std::printf("[-] send error: %d\n",
                                ret);
#endif
                    return -1;
                }
                send_length += sen;
                len -= sen;
            }
        }

        return data_size;
    }

    void Node::print_bytes(char *buffer, int buffer_length)
    {
        for(int i=0; i<buffer_length; i++){
            if(i != 0 && i%16 == 0){
                std::printf("\n");
            }else if(i%16 == 8){
                std::printf(" ");
            }
            std::printf("%02x ", buffer[i] & 0xff);
        }
        std::printf("\n");

        return;
    }

    /*
     * Reference:
     * https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows
     */
    int Node::gettimeofday(timeval *tv, timezone *tz)
    {
        if(tv){
            FILETIME filetime;
            ULARGE_INTEGER x;
            ULONGLONG usec;
            static const ULONGLONG epoch_offset_us = 11644473600000000ULL;

#if _WIN32_WINNT >= WIN32_WINNT_WIN8
            GetSystemTimePreciseAsFileTime(&filetime);
#else
            GetSystemTimeAsFileTime(&filetime);
#endif

            x.LowPart = filetime.dwLowDateTime;
            x.HighPart = filetime.dwHighDateTime;
            usec = x.QuadPart / 10 - epoch_offset_us;
            tv->tv_sec = (long)(usec / 1000000ULL);
            tv->tv_usec = (long)(usec % 1000000ULL);
        }else{
            return -1;
        }

        if(tz){
            TIME_ZONE_INFORMATION timezone;
            GetTimeZoneInformation(&timezone);
            tz->tz_minuteswest = timezone.Bias;
            tz->tz_dsttime = 0;
        }

        return 0;
    }
}

