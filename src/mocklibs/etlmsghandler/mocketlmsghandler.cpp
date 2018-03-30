#include "mocketlmsghandler.h"

#include <map>
#include <vector>


// 使用时不判断MockEtlMsgHandler是否为空，let it crash
MockEtlMsgHandler * g_pMockEtlMsgHandler = NULL;
std::map<UINT, std::vector<CMSGSubscriber> > g_mapEtlMsgCallbacks;

BOOL etl_RegisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun)
{
    return etl_RegisterMsgHandle(uMsgFrom, uMsgTo, CMSGSubscriber(pFun));
}

BOOL etl_UnregisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun)
{
    return etl_UnregisterMsgHandle(uMsgFrom, uMsgTo, CMSGSubscriber(pFun));
}

BOOL etl_RegisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, const CMSGSubscriber & subscriber)
{
    for (UINT i = uMsgFrom; i <= uMsgTo; i++)
    {
        std::vector<CMSGSubscriber>::iterator it = std::find(
                    g_mapEtlMsgCallbacks[i].begin(),
                    g_mapEtlMsgCallbacks[i].end(),
                    subscriber);
        if (it == g_mapEtlMsgCallbacks[i].end())
        {
            g_mapEtlMsgCallbacks[i].push_back(subscriber);
        }
    }
    return g_pMockEtlMsgHandler->RegisterMsgHandle(uMsgFrom, uMsgTo, subscriber);
}

BOOL etl_UnregisterMsgHandle(UINT uMsgFrom, UINT uMsgTo, const CMSGSubscriber & subscriber)
{
    for (UINT i = uMsgFrom; i <= uMsgTo; i++)
    {
        std::vector<CMSGSubscriber>::iterator it = std::find(
                    g_mapEtlMsgCallbacks[i].begin(),
                    g_mapEtlMsgCallbacks[i].end(),
                    subscriber);
        if (it != g_mapEtlMsgCallbacks[i].end())
        {
            g_mapEtlMsgCallbacks[i].erase(it);
        }
    }
    // 为了兼容单例释放时调用注销消息回调（程序退出前释放），此处需要判断是否为空
    return g_pMockEtlMsgHandler
           && g_pMockEtlMsgHandler->UnregisterMsgHandle(uMsgFrom, uMsgTo, subscriber);
}

BOOL etl_NotifyApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_pMockEtlMsgHandler->NotifyApp(bSync, uMsg, wParam, lParam);
}

BOOL etl_NotifyAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                     int nExtraSize, LPCVOID lpExtraData)
{
    return g_pMockEtlMsgHandler->NotifyAppEx(bSync, uMsg, wParam, lParam, nExtraSize, lpExtraData);
}

namespace ETL_MsgQueueHelper
{
msgObject::msgObject(UINT uMsgID /* = 0 */, WPARAM wPar /* = 0 */, LPARAM lPar /* = 0 */) {}
msgObject::~msgObject() {}
int msgObject::GetExtraSize()
{
    return 0;
}
LPVOID msgObject::GetExtraData()
{
    return NULL;
}
}

MockEtlMsgHandler::MockEtlMsgHandler()
{
    // 严格模式：如果用例中没有EXPECT_CALL，则不允许被测程序调用接口
    using ::testing::_;
    EXPECT_CALL(*this, NotifyApp(_, _, _, _)).Times(0);
    EXPECT_CALL(*this, NotifyAppEx(_, _, _, _, _, _)).Times(0);
}

bool TriggerEtlMsgCallback(msgObject & msg)
{
    std::map<UINT, std::vector<CMSGSubscriber> >::iterator itCallbacks =
        g_mapEtlMsgCallbacks.find(msg.message);
    if (itCallbacks == g_mapEtlMsgCallbacks.end())
    {
        return false;
    }
    for (std::vector<CMSGSubscriber>::iterator it = itCallbacks->second.begin();
            it != itCallbacks->second.end(); ++it)
    {
        (*it)(msg);
    }
    return true;
}
