#ifdef __GNUC__
#include "profdata.h"


#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
//#include "stringsplit.h"

#include <ylstring.h>

/************Switch**********/
#define PROF_GOLABEL           0
#define PROF_DUMP_THREAD_NAME  1
#define PROF_USE_CPU_TIME      1
/****************************/

#if PROF_DUMP_THREAD_NAME
#include <ETLLib.hpp>
static __thread bool thread_name_dumped = 0;
#endif

#define PROF_FILE_NAME_ENV     "PROF_FILE_NAME"
#define PROF_DEFINE_FILE_NAME  "/mnt/profile"
#define PROF_API   __attribute__((__no_instrument_function__))

#ifdef __cplusplus
extern "C"
{
#endif

void start_profile(const char * filename)                        PROF_API;
void stop_profile()                                             PROF_API;
bool dump_thread_name()                                         PROF_API;
void dump_maps()                                                PROF_API;

void process_signal(int sig)                                    PROF_API;

void __cyg_profile_func_enter(void * this_func, void * call_site) PROF_API;
void __cyg_profile_func_exit(void * this_func, void * call_site)  PROF_API;
int dump_data(PROF_NODE_TYPE type, const char * str, size_t size, FILE * file)   PROF_API;

static __thread pthread_t threadid = 0;
static FILE * profile = 0;
static pthread_t prof_threadid = 0;
static bool maps_delay_dump = false;
static bool prof_failed = false;
#if PROF_USE_CPU_TIME
static __thread timespec nstime;
#else
static __thread timeval tv;
#endif

static __thread function_node node;

void process_signal(int sig)
{
    printf("PROF#On signal(%d)\n", sig);
    if (profile == 0)
    {
        if (!prof_failed)
        {
            char filename[255] = {0};
#if PROF_USE_CPU_TIME
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nstime);
            sprintf(filename, "/mnt/profile-%d", nstime.tv_sec);
#else
            gettimeofday(&tv, NULL);
            sprintf(filename, "/mnt/profile-%d", tv.tv_sec);
#endif
            start_profile(filename);
        }
    }
    else
    {
        stop_profile();
    }
}

void __cyg_profile_func_enter(void * this_func, void * call_site)
{
    static bool signal_registered = false;
    if (!signal_registered)
    {
        void * ret = (void *)signal(40, process_signal);
        printf("PROF#Register signal: %p func[%p]\n", ret, process_signal);
        signal_registered = true;
    }

    if (threadid == 0)
    {
        threadid = pthread_self() % 60000 + pthread_self() % 55666;
        printf("PROF#New Thread[%hd]\n", threadid);
    }

#if PROF_DUMP_THREAD_NAME
    if (!thread_name_dumped)
    {
        thread_name_dumped = dump_thread_name();
    }
#endif

#if PROF_GOLABEL
    if (profile == 0 && !prof_failed)
    {
        start_profile(NULL);
    }
#endif

    if (profile != 0)
    {
#if PROF_USE_CPU_TIME
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nstime);
        node.data.sec        = nstime.tv_sec;
        node.data.nsec       = nstime.tv_nsec;
#else
        gettimeofday(&tv, NULL);
        node.data.sec        = tv.tv_sec;
        node.data.nsec       = tv.tv_usec * 1000;
#endif

        node.type            = PROF_TYPE_FUNCTION_BEGIN;
        node.data.pid        = threadid;
        node.data.func_addr  = (PROF_ADDR_TYPE)this_func;
        fwrite(&node, sizeof(node), 1, profile);
    }
}

void __cyg_profile_func_exit(void * this_func, void * call_site)
{
    if (profile != 0)
    {
#if PROF_USE_CPU_TIME
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &nstime);
        node.data.sec        = nstime.tv_sec;
        node.data.nsec       = nstime.tv_nsec;
#else
        gettimeofday(&tv, NULL);
        node.data.sec        = tv.tv_sec;
        node.data.nsec       = tv.tv_usec * 1000;
#endif
        node.type            = PROF_TYPE_FUNCTION_EXIT;
        node.data.pid        = threadid;
        node.data.func_addr  = (PROF_ADDR_TYPE)this_func;

        fwrite(&node, sizeof(node), 1, profile);
    }
}

void start_profile(const char * filename)
{
    printf("start_profile(%s)\n", filename);
    if (profile == 0)
    {
        if (filename == NULL)
        {
            //get file name from env
            filename = getenv(PROF_FILE_NAME_ENV);
            if (filename == NULL)
            {
                filename = PROF_DEFINE_FILE_NAME;
            }
        }
        printf("PROF#Start prof version %d.%d.%d.\n", sizeof(function_node), sizeof(uint16_t),
               sizeof(uint32_t));
        profile = fopen(filename, "w");
        if (profile == 0)
        {
            printf("PROF#Can not open file %s\n", filename);
            prof_failed = true;
            return;
        }

        if (maps_delay_dump)
        {
            maps_delay_dump = false;
            dump_maps();
        }
    }
}

void stop_profile()
{
    printf("stop_profile()\n");
    if (profile != 0)
    {
        fclose(profile);
        profile = 0;
    }
}


int dump_data(PROF_NODE_TYPE type, const char * str, size_t size, FILE * file)
{
    yl::string str_write(sizeof(PROF_NODE_TYPE) + sizeof(PROF_SIZE_TYPE) + size, 0);
    char * p = &str_write[0];

    //type
    *((PROF_NODE_TYPE *)p) = (PROF_NODE_TYPE)type;
    p += sizeof(PROF_NODE_TYPE);

    //size
    *((PROF_SIZE_TYPE *)p) = (PROF_SIZE_TYPE)size;
    p += sizeof(PROF_SIZE_TYPE);

    //data
    memcpy(p, str, size);

    fwrite(&str_write[0], sizeof(char), str_write.size(), file);
}

yl::string get_stdout(const yl::string commad)
{
    //打开命令
    FILE * ps = popen(commad.c_str(), "r");
    if (ps == 0)
    {
        printf("popen file [%d] failed\n", ps);
    }

    const size_t buffer_size = 60;
    char buffer[buffer_size];
    size_t size = 0;
    yl::string ret;
    while (size = fread((void *)buffer, 1, buffer_size, ps))
    {
        ret.append(buffer, size);
    }

    pclose(ps);
    return ret;
}

void dump_maps()
{
    if (profile != 0)
    {
        char maps_filename[100] = "";

        sprintf(maps_filename, "cat /proc/%d/maps", getpid());
        yl::string maps = get_stdout(maps_filename);
        YOUT("dump %s.\n %s\n", maps_filename, maps.c_str());
        dump_data(PROF_TYPE_MAPSFILE, maps.c_str(), maps.size(), profile);
    }
    else
    {
        maps_delay_dump = true;
    }
}

#ifdef PROF_DUMP_THREAD_NAME
extern bool g_bAfterInit;
bool dump_thread_name()
{
    if (profile == 0)
    {
        return false;
    }

    if (g_bAfterInit)
    {
#ifndef _MSGKIT
        chThreadLocal * pEtlThread = g_uCSystemMessage.GetCurrentThreadObject();
        if (pEtlThread)
        {
            char buffer[128];
            int size = sprintf(buffer, "%x:%s", threadid, (LPCSTR)pEtlThread->m_strThreadName);
            dump_data(PROF_TYPE_THREAD_NAME, buffer, size, profile);
            return true;
        }
#else
        char szSrcName[128] = {0};
        mkit_target_t self = 0;
        mkit_get_target(mkGetCurrent(), &self);
        if (self != 0)
        {
            mkit_name(mkGetCurrent(), self, szSrcName);
            char buffer[128];
            int size = sprintf(buffer, "%x:%s", threadid, szSrcName);
            dump_data(PROF_TYPE_THREAD_NAME, buffer, size, profile);
            return true;
        }
#endif
    }

    return false;
}
#else
void dump_thread_name()
{
}
#endif

#ifdef __cplusplus
}
#endif


#endif //#ifdef __GNUC__
