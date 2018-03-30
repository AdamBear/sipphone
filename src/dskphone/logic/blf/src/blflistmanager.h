#ifndef __BLFLIST_MANAGER_H__
#define __BLFLIST_MANAGER_H__

#include "blfbasemanager.h"
#include "ETLLib.hpp"

class CBLFlistManager: public CBLFBaseManager
{
public:

    static CBLFlistManager & GetInstance();

    void Init();

    //消息入口
    static LRESULT OnBlflistMsgCallBack(msgObject & msg);

    // 获取blflist的数据
    bool GetBlfListData(YLVector<AccountMoitorData> & refVector);

    // 记住监控该号码是用哪个dsskey来绑定的
    void SetDsskeyId(int iDsskeyId, int iAccountId, const yl::string & strKey);

    // 解绑dsskeyid，由网页把blflist设置成别的类型，需要把该dsskeyid解绑掉
    void UnBindDsskeyId(int iDsskeyId, int iAccountId);

    // 向SIP的缓存发起查询
    bool QueryBlfList(int iAccount);

#if IF_BUG_32812
    // 获取对应账号监控列表
    bool GetBLFListMonitorList(const int iLineID, YLList<yl::string> & listMonitor);
    // 账号是否有blflist数据
    bool IsAccountHaveBLFListData(const int iLineID);
#endif

    void SetFlag(int iAccountID, bool bEnable);

    bool IsEnable(int iAccountID);

protected:
    // 解析回调处理函数
    virtual void ProcessTaskActionCallBack(CBLFPraseAction * pAction);

    // 处理账号状态改变的消息
    bool OnLineStateChange(msgObject & objMsg);

    // Çå¿ÕËùÓÐµÄÊý¾Ý
    void ResetBlfListByLineId(int iLineID);

    void UnBindDsskeyByAccountID(int iDsskeyId, int iAccountId);

private:
    CBLFlistManager();
    virtual ~CBLFlistManager();

private:
    char*   m_pEnabled;
};

#define g_objBLFListManager (CBLFlistManager::GetInstance())


#endif

