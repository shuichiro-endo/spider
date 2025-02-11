/*
 * Title:  spider route.cpp (Windows)
 * Author: Shuichiro Endo
 */

#include "spider.hpp"
#include "route.hpp"
#include "pipe.hpp"


namespace spider
{
    Route::Route(char type,
                 std::string ip,
                 uint8_t metric,
                 uint32_t pipe_id)
    {
        this->type = type;
        this->ip = ip;
        this->metric = metric;
        this->pipe_id = pipe_id;

        if(gettimeofday(&this->time, NULL) == -1)
        {
#ifdef _DEBUG
            std::printf("[-] gettimeofday error\n");
#endif
            this->time.tv_sec = 0;
            this->time.tv_usec = 0;
        }
    }

    Route::~Route()
    {

    }

    void Route::set_type(char type)
    {
        this->type = type;
    }

    char Route::get_type()
    {
        return type;
    }

    void Route::set_ip(std::string ip)
    {
        this->ip = ip;
    }

    std::string Route::get_ip()
    {
        return ip;
    }

    void Route::set_metric(uint8_t metric)
    {
        this->metric = metric;
    }

    uint8_t Route::get_metric()
    {
        return metric;
    }

    void Route::set_pipe_id(uint32_t pipe_id)
    {
        this->pipe_id = pipe_id;
    }

    uint32_t Route::get_pipe_id()
    {
        return pipe_id;
    }

    void Route::set_time(struct timeval time)
    {
        this->time = time;
    }

    struct timeval Route::get_time()
    {
        return time;
    }

    /*
     * Reference:
     * https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows
     */
    int Route::gettimeofday(timeval *tv, timezone *tz)
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

