#ifndef __AOC_MANAGER_H__
#define __AOC_MANAGER_H__

// 记录通话时长信息
struct DataTalkInfo
{
    bool        bShowAoc;       // 是否显示Aoc
    int         iCallId;        // 通话ID
    int         iTimeOut;       // 超时保护
    DataAoc     tAocData;       // Aoc信息

    DataTalkInfo()
    {
        bShowAoc = false;
        iCallId = -1;
        iTimeOut = -1;
    }
};
typedef YLList<DataTalkInfo> ListDataTalkInfo;
typedef ListDataTalkInfo::iterator IterDataTalkInfo;

class CAocManager
{
    DECLARE_SINGLETON_CLASS(CAocManager)

public:
    // 初始化
    void Init();

    // 是否显示AOC
    bool IsAocShow(int iCallId);

    // 获取AOC信息
    bool GetAocInfo(DataAoc & tAocData, int iCallId);

    // 通话开启的处理
    bool ProcessTalkStart(int iCallId);

    // 通话结束的处理
    bool ProcessTalkFinish(int iCallId);

    // 清除AOC数据
    void ClearAOCData(int iCallId);

public:
    // 处理AocNotify
    bool ProcessAocNotify(int iAccId, int iCallId, const char * lpInfo);

    // 处理定时器消息
    bool OnTimer(UINT uTimerID);

private:
    // 是否可用
    bool IsAocEnable();

    // 解析Aoc
    bool ParserAocInfo(const yl::string & strInfo, DataAoc & tAocData);

    // 进入AocFinishRoutine
    bool EnterAocFinishRoutine(int iAccId, int iCallId, const DataAoc & tAocData);

    // 通过CallId找通话信息
    DataTalkInfo * GetTalkDurationInfoByCallID(int iCallId);

private:
    // 通话时长信息
    ListDataTalkInfo m_lstTalkInfo;

};

#define _AOCManager GET_SINGLETON_INSTANCE(CAocManager)

#endif //__AOC_MANAGER_H__
