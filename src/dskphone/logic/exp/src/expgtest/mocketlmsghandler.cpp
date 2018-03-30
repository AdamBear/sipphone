#include "mocketlmsghandler.h"
#include "mockmsgobject.h"
#include <map>
#include <vector>


// 使用时不判断MockEtlMsgHandler是否为空，let it crash
MockEtlMsgHandler * g_pMockEtlMsgHandler;
std::map<UINT, std::vector<FunctionETLMsg> > g_mapEtlMsgCallbacks;

//std::vector<UINT> g_mapETlTimer;

BOOL etl_RegisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun)
{
    for (UINT i = uMsgFrom; i <= uMsgTo; i++)
    {
        std::vector<FunctionETLMsg>::iterator it = std::find(
                    g_mapEtlMsgCallbacks[i].begin(),
                    g_mapEtlMsgCallbacks[i].end(),
                    pFun);
        if (it == g_mapEtlMsgCallbacks[i].end())
        {
            g_mapEtlMsgCallbacks[i].push_back(pFun);
        }
    }
    return g_pMockEtlMsgHandler->RegisterMsgHandle(uMsgFrom, uMsgTo, pFun);
}

BOOL etl_UnregisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun)
{
    for (UINT i = uMsgFrom; i <= uMsgTo; i++)
    {
        std::vector<FunctionETLMsg>::iterator it = std::find(
                    g_mapEtlMsgCallbacks[i].begin(),
                    g_mapEtlMsgCallbacks[i].end(),
                    pFun);
        if (it != g_mapEtlMsgCallbacks[i].end())
        {
            g_mapEtlMsgCallbacks[i].erase(it);
        }
    }
    // 为了兼容单例释放时调用注销消息回调（程序退出前释放），此处需要判断是否为空
    return g_pMockEtlMsgHandler && g_pMockEtlMsgHandler->UnregisterMsgHandle(uMsgFrom, uMsgTo, pFun);
}

BOOL etl_NotifyAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                     int nExtraSize, LPCVOID lpExtraData)
{
    return g_pMockEtlMsgHandler->NotifyAppEx(bSync, uMsg, wParam, lParam, nExtraSize, lpExtraData);
}

BOOL etl_NotifyApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_pMockEtlMsgHandler->NotifyAppEx(bSync, uMsg, wParam, lParam, 0, NULL);
}

bool TriggerEtlMsgCallback(msgObject & msg)
{
    std::map<UINT, std::vector<FunctionETLMsg> >::iterator itCallbacks =
        g_mapEtlMsgCallbacks.find(msg.message);
    if (itCallbacks == g_mapEtlMsgCallbacks.end())
    {
        return false;
    }
    for (std::vector<FunctionETLMsg>::iterator it = itCallbacks->second.begin();
            it != itCallbacks->second.end(); ++it)
    {
        (*it)(msg);
    }
    return true;
}




BOOL msgPostMsgToThreadEx(HTHREAD hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          int nExtraSize, LPCVOID lpExtraData)
{
    return g_pMockEtlMsgHandler->msgPostMsgToThreadEx(hTargetThread, uMsg, wParam, lParam, nExtraSize,
            lpExtraData);
}


void msgBroadpostThreadMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    g_pMockEtlMsgHandler->msgBroadpostThreadMsg(uMsg, wParam, lParam);
    return;
}


BOOL msgPostMsgToThread1(HTHREAD hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         const char * file, int line)
{
    return g_pMockEtlMsgHandler->msgPostMsgToThreadEx(hTargetThread, uMsg, wParam, lParam, 0, NULL);
}


HTHREAD msgGetThreadByName(LPCSTR lpszThreadName)
{
    return g_pMockEtlMsgHandler->msgGetThreadByName(lpszThreadName);
}

void timerKillThreadTimer(UINT nTimerID)
{

    g_pMockEtlMsgHandler->timerKillThreadTimer(nTimerID);
}

UINT timerSetThreadTimer1(UINT nTimerID, UINT uTimerOut, const char * file, int line)
{
    msgObject msg;
    msg.message = TM_TIMER;
    msg.wParam = nTimerID;
    bool bret = TriggerEtlMsgCallback(msg);

    return g_pMockEtlMsgHandler->timerSetThreadTimer1(nTimerID, uTimerOut, file, line);
}

bool commonUnits_GetIntValue(LPCSTR lpCfgContent, LPCSTR lpKey, int & iOutValue)
{
    if (NULL == lpCfgContent
            || NULL == lpKey)
    {
        return false;
    }

    int iLength = strlen(lpKey);
    if (iLength <= 0)
    {
        return false;
    }

    LPSTR lpStartPos = (char *)strstr(lpCfgContent, lpKey);
    if (NULL == lpStartPos)
    {
        return false;
    }

    // "=%d"共3个字符,加上串尾符共需4个
    char * lpBuffer = new char[iLength + 4];
    sprintf(lpBuffer, "%s=%s", lpKey, "%d");

    int nRet = sscanf(lpStartPos, lpBuffer, &iOutValue);

    delete[] lpBuffer;
    return (1 == nRet);
}
bool commonUnits_GetStringValue(LPCSTR lpCfgContent, LPCSTR lpKey, LPSTR lpOut)
{
    if (NULL == lpCfgContent
            || NULL == lpKey
            || NULL == lpOut)
    {
        return false;
    }

    int iLength = strlen(lpKey);
    if (iLength <= 0)
    {
        return false;
    }


    LPSTR lpStartPos = (char *)strstr(lpCfgContent, lpKey);
    if (lpStartPos != NULL)
    {
        // "=%[^&]&"共7个字符,加上串尾符共需8个
        char * lpBuffer = new char[iLength + 8];
        // 生成格式化串,用于sscanf
        sprintf(lpBuffer, "%s=%s", lpKey, "%[^&]&");
        lpOut[0] = '\0';
        sscanf(lpStartPos, lpBuffer, lpOut);
        delete[] lpBuffer;
        return true;
    }

    return false;
}
