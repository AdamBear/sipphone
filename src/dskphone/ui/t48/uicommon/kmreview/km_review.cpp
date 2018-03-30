#ifdef KM_REVIEW_SUPPORT
#include "km_review.h"
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "dsklog/log.h"
#include <winimp/winimp.h>
// 记录的最大长度
#define MAX_RECORD_LEN      512

CKMManager & GetKMManagerInstance()
{
    static CKMManager s_KM_Manager;
    return s_KM_Manager;
}

void RecordString(const char * lpszFmt, ...)
{
    char szLog[MAX_RECORD_LEN] = {0};
    va_list argList;
    va_start(argList, lpszFmt);
    vsprintf(szLog, lpszFmt, argList);
    if (szLog[MAX_RECORD_LEN] != '\0')
    {
        // 容错，防溢出处理
        szLog[MAX_RECORD_LEN] == '\0';
    }
    _KM_Manager.RecordString2Log(szLog);
    va_end(argList);
}

int tv_Cal(const struct timeval * tCur, const struct timeval * tLast)
{
    struct timeval ret;
    int iret = -1;
    ret.tv_sec = tCur->tv_sec - tLast->tv_sec;
    if ((ret.tv_usec = (tCur->tv_usec - tLast->tv_usec)) < 0)
    {
        -- ret.tv_sec;
        ret.tv_usec += 1000000;
    }
    iret = ret.tv_sec * 1000 + ret.tv_usec / 1000;
    return iret;
}



CKMManager::CKMManager()
{
    InitData();
}

CKMManager::~CKMManager()
{
    if (m_fpFile != NULL)
    {
        fflush(m_fpFile);
        fclose(m_fpFile);
    }
}

// 记录按键信息
void CKMManager::Record_Key(int nKeyCode, bool bDown)
{
    struct timeval tData;
    gettimeofday(&tData, NULL);
    char szKey[MAX_RECORD_LEN] = {0};
    int iTimeInterval = tv_Cal(&tData, &m_pLastTime);
    m_pLastTime = tData;
    if (!bFirst)
    {
        // 第一次操作只延迟1ms
        iTimeInterval = 1;
        bFirst = true;
    }
    if (iTimeInterval < 0)
    {
        return;
    }
//  GLOBALINPUTHOOK_INFO("Record_Key :: nKeyCode is %d, iTv is %d, the key is %s", nKeyCode, iTimeInterval, bDown ? "down" : "up");
    RecordString("K[%d],T[%d],D[%d].\n", nKeyCode, iTimeInterval, bDown);
}

// 记录鼠标信息
void CKMManager::Record_Mouse(int x, int y, bool bDown)
{
    struct timeval tData;
    gettimeofday(&tData, NULL);
    char szKey[MAX_RECORD_LEN] = {0};
    int iTimeInterval = tv_Cal(&tData, &m_pLastTime);
    m_pLastTime = tData;
    if (!bFirst)
    {
        // 第一次操作只延迟1ms
        iTimeInterval = 1;
        bFirst = true;
    }
    if (iTimeInterval < 0)
    {
        return;
    }
//  GLOBALINPUTHOOK_INFO("Record_Mouse :: x is %d, y is %d, iTv is %d, the key is %s", x, y, iTimeInterval, bDown ? "down" : "up");
    RecordString("MP[%d,%d],T[%d],D[%d].\n", x, y, iTimeInterval, bDown);
}

// 记录到log文件中
void CKMManager::RecordString2Log(const char * lpszFmt)
{
    if (m_fpFile != NULL)
    {
        fwrite(lpszFmt, strlen(lpszFmt), 1, m_fpFile);
        fflush(m_fpFile);
    }
    else
    {
        GLOBALINPUTHOOK_WARN("KM_REVIEW : The KM Record File is not exit!");
    }
}

// 重新记录log文件
void CKMManager::RestartRecord()
{
    if (m_fpFile != NULL)
    {
        fclose(m_fpFile);
        m_fpFile = NULL;
    }
    // 重新设置初始化参数
    InitData();
}

// 重新记录log文件
void CKMManager::InitData()
{
    gettimeofday(&m_pLastTime, NULL);
    m_fpFile = fopen(TEMP_PATH "/log/kmr.log", "wb+");
    bFirst = false;
}

#endif


