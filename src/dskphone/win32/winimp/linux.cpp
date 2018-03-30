#include "common.h"

int gettimeofday(struct timeval * tp, void * tzp)
{
    time_t      clock;
    struct tm   tm;
    SYSTEMTIME  wtm;
    GetLocalTime(&wtm);
    tm.tm_year  = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday  = wtm.wDay;
    tm.tm_hour  = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm.tm_isdst = -1;
    clock       = mktime(&tm);
    tp->tv_sec  = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}

size_t getline(char ** lineptr, size_t * n, FILE * stream)
{
    if (!lineptr || !n || !stream)
    {
        return -1;
    }

    free(*lineptr);
    *lineptr = NULL;

    const int buf_size = BLOCK_BUFFER_SIZE;
    char * buffer = (char *)malloc(buf_size);
    if (!buffer)
    {
        return -1;
    }
    memset(buffer, 0, buf_size);

    if (fgets(buffer, buf_size, stream))
    {
        *lineptr = buffer;
        return *n = strlen(buffer);
    }

    free(buffer);
    return -1;
}

/*
 * ??IPv4????????????????????????????
 * ?????????
 * int inet_aton(const char *cp, struct in_addr *inp);
 */
unsigned int __inet_aton(const char * c_ipaddr, struct in_addr * ul_ipaddr)
{
    if (!c_ipaddr || !ul_ipaddr)
    {
        return -1;
    }

    unsigned long & u_ipaddr = ul_ipaddr->S_un.S_addr;
    unsigned int u_tmp = 0;
    char c;
    int i_base = 10;
    int i_shift = 0;
    int i_recycle = 0;

    c = *c_ipaddr;
    while (1)
    {
        u_tmp = 0;
        while (1)
        {
            //?????????
            if (isdigit(c))
            {
                u_tmp = (u_tmp * i_base) + (c - 0x30);
                c = *++c_ipaddr;
            }
            else
            {
                //????????????.??,???.???????????????????
                break;
            }
        }

        //??????λ,??????????????????
        i_shift = 8 * i_recycle++;
        u_tmp <<= i_shift;
        u_ipaddr += u_tmp;

        //??(.)??????
        if (c == '.')
        {
            c = *++c_ipaddr;
        }
        else
        {
            //????????????????????\0??????????????
            break;
        }
    }

    //????????????????,??????0
    if (c != '\0' && (!isspace(c)))
    {
        goto ret_0;
    }

    return u_ipaddr;
ret_0:
    return (0);
}

/**
 * ???????????????????IPv4???????????,???????????????????????,???????????????????????λbyte[0]
 * ????????
 * int inet_aton(const char *cp, struct in_addr *inp);
 */

char * __inet_ntoa(struct in_addr in)
{
    static char buffer[16];
    unsigned char * bytes = (unsigned char *) &in;
    snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return buffer;
}

char * strsep(char ** stringp, const char * delim)
{
    char * s;
    const char * spanp;
    int c, sc;
    char * tok;
    if ((s = *stringp) == NULL)
    {
        return (NULL);
    }
    for (tok = s;;)
    {
        c = *s++;
        spanp = delim;
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                {
                    s = NULL;
                }
                else
                {
                    s[-1] = 0;
                }
                *stringp = s;
                return (tok);
            }
        }
        while (sc != 0);
    }
}

typedef std::map<FILE *, std::string> MAP_TEMP_FILE;
static MAP_TEMP_FILE mapFile;

FILE * popen(const char * filename, const char * flag)
{
    if (!filename || !flag)
    {
        return NULL;
    }

    std::string strFile = formatString(TEMP_PATH "%s", WAPIGetUUID().c_str());
    std::string strOut;

    WAPIExecDosCmd(filename, strOut);
    write_file_imp(strFile, strOut);

    FILE * pfile = fopen(strFile.c_str(), flag);
    if (pfile)
    {
        mapFile[pfile] = strFile;
    }
    return pfile;
}

int pclose(FILE * file)
{
    if (file)
    {
        int iRet = fclose(file);
        MAP_TEMP_FILE::iterator it = mapFile.find(file);
        if (it != mapFile.end())
        {
            unlink(it->second.c_str());
        }

        return iRet;
    }
    return -1;
}

int syscall(int, ...)
{
    return GetCurrentThreadId();
}

char * crypt(const char * key, const char * salt)
{
    if (!key || !salt)
    {
        return NULL;
    }

    typedef char * (*PFN_CYGEIN_CRYPT)(const char *, const char *);

    LibraryLoadGuard lib("crypt.dll");
    PFN_CYGEIN_CRYPT pcrypt = (PFN_CYGEIN_CRYPT)lib.GetInstance("crypt");
    if (pcrypt == NULL)
    {
        fprintf(stderr, "get instance error [%d] \n", GetLastError());
        return NULL;
    }

    static std::string str;
    str = pcrypt(key, salt);

    return (char *)str.c_str();
}

int ioctl(int fd, int iOperator, unsigned long uPoint) TRACE_FUCTION;