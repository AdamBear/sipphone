#ifndef __PSTN_ADAPTER_DATADEFINE_H__
#define __PSTN_ADAPTER_DATADEFINE_H__

#ifdef IF_FEATURE_PSTN

#include <pthread.h>
#include <sys/time.h>


#define MAX_PSTN_LINE_COUNT 2 // 最多支持PSTN线路数量

/// PSTN线路连接状态
enum PSTN_LINE_CONN_STATUS
{
    PSTN_LINE_DISCONNECTED = 0, ///< PSTN线路断开
    PSTN_LINE_CONNECTED, ///< PSTN线路连接
    PSTN_LINE_STATUS_CHANGED ///< PSTN线路状态变化
};

class SerialCmdEvent
{
public:
    enum { TIMEOUT_NEVER = -1 };
    SerialCmdEvent()
        : state(false)
    {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }
    ~SerialCmdEvent()
    {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
    void Pulse()
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond);
        state = true;
        pthread_mutex_unlock(&mutex);
    }
    void Reset()
    {
        pthread_mutex_lock(&mutex);
        state = false;
        pthread_mutex_unlock(&mutex);
    }
    bool Wait(int nTimeout = TIMEOUT_NEVER) // ms
    {
        bool bResult = true;
        pthread_mutex_lock(&mutex);
        if (state)
        {
            bResult = state;
            state = false;
        }
        else
        {
            if (TIMEOUT_NEVER == nTimeout)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            else
            {
                struct timeval now;
                gettimeofday(&now, NULL);
                struct timespec outtime;
                outtime.tv_sec = now.tv_sec + nTimeout / 1000;
                outtime.tv_nsec = now.tv_usec * 1000 + (nTimeout % 1000) * 1000000;
                bResult = (pthread_cond_timedwait(&cond, &mutex, &outtime) != ETIMEDOUT);
            }
        }
        pthread_mutex_unlock(&mutex);
        return bResult;
    }
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool state; // 是否触发
};

struct SerialCmdResult
{
    SerialCmdEvent m_event;
    char m_szVaule[32];
};

#define NUM_LEN_MAX 50
struct CidMsgInfo
{
    char m_szNumber[NUM_LEN_MAX + 1];
    bool m_bCidAfterRing;
    CidMsgInfo()
    {
        memset(m_szNumber, 0, NUM_LEN_MAX + 1);
        m_bCidAfterRing = false;
    }
};

#endif // IF_FEATURE_PSTN

#endif // __PSTN_ADAPTER_DATADEFINE_H__