#ifndef __WIN32_ADAPTER_H__
#define __WIN32_ADAPTER_H__

#if WIN32

#include <windows.h>

// Windows下使用stl容器
#include <string>
#define yl          std
#include <map>
#define YLHashMap   std::map

// 日志
#define COMMONUNITS_LOG(fmt, ...) do\
{\
    FILE *f = fopen("pstn-box.log", "a");\
    fprintf(f, "[%d@%d:%d]" fmt "\n", GetTickCount(), GetCurrentThreadId(), GetCurrentProcessId(), __VA_ARGS__);\
    fclose(f);\
} while(false)
#define COMMONUNITS_DEBUG(fmt, ...) COMMONUNITS_LOG("(D)" fmt, __VA_ARGS__)
#define COMMONUNITS_INFO(fmt, ...) COMMONUNITS_LOG("(I)" fmt, __VA_ARGS__)
#define COMMONUNITS_WARN(fmt, ...) COMMONUNITS_LOG("(W)" fmt, __VA_ARGS__)
#define COMMONUNITS_ERR(fmt, ...) COMMONUNITS_LOG("(E)" fmt, __VA_ARGS__)

void COMMONUNITS_WIN32_LOG(const char * szMessage);

// etl
#define SingleMsgReg
#define etl_UnregisterMsgHandle
#define etl_NotifyAppEx

// 其他模块
#define DECLARE_ATTR
#define DECLARE_SINGLETON_CLASS(className) \
        public:\
            static className& GetInstance() DECLARE_ATTR;\
        private:\
            className();\
            ~className();
#define GET_SINGLETON_INSTANCE(className)  className::GetInstance()
#define IMPLEMENT_GETINSTANCE(className) \
        className& className::GetInstance() \
        {\
            static className s_Instance;\
            return s_Instance;\
        }
typedef unsigned long HPDEV_HANDLE;
#define HOTPLUG_USB_PSTN_BOX_STATE_CHANGE 1

// IO操作
#define close(fd) CloseHandle((HANDLE)fd)
size_t read(int fd, char * buf, size_t count);
int write(int fd, const char * line, long size);

#endif // WIN32

#endif // !__WIN32_ADAPTER_H__