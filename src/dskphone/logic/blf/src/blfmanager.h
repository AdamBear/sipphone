#ifndef __BLF_MANAGER_H__
#define __BLF_MANAGER_H__

#include "blfbasemanager.h"
#include "ETLLib.hpp"

class CBLFManager : public CBLFBaseManager
{
public:
    static CBLFManager & GetInstance();

    void Init();

    //消息入口
    static LRESULT OnBlfMsgCallBack(msgObject & msg);

    // 向SIP的缓存发起查询
    bool QueryBlfStatus(int iAccount);
    bool QueryBlfStatus();

    //处理通话焦点改变
    void ProcessTalkFoucsChange();

protected:
    // 解析回调处理函数
    virtual void ProcessTaskActionCallBack(CBLFPraseAction * pAction) {}

    // BLF Dsskey配置更改
    bool ProcessConfigChange(int nDsskeyId);
    void ProcessServerChange(int nAccountId);

    // 通过Dsskey id生成数据
    bool CreatDataByDsskeyId(YLList<int> & lstDsskeyId);
    bool CreatDataByDsskeyId(int nDsskeyId);
    bool ProcessExpStatusChange(int iExpIndex);

    // 删除某个扩展台的数据
    void RemoveExtIter(int iExtIndex);

    void ProcessTimer(int iTimerID);

    void TalkStart();

    void ReqBlfStatus(int iDsskeyID);

    void CancelReqBlfStatus(int iDsskeyID);

private:
    CBLFManager();
    virtual ~CBLFManager();

    // 获取亮灯的模式
    int GetMode(int flag);

    // 分解SIP下发的URI, 输出Number+Server
    yl::string SpliSipUri(const yl::string & strInfo);

    bool IsDsskeySupport(int iDsskeyId);

private:

    YLHashMap<int, int> m_listReqDsskeyID;

    struct ItemData
    {
        int iLineId;
        int iExtIndex;
        yl::string strKey;

        ItemData()
        {
            iLineId = -1;
            iExtIndex = -1;
        }
    };

    YLHashMap<int, ItemData> m_mapKey;
};

#define g_objBLFManager (CBLFManager::GetInstance())

#endif
