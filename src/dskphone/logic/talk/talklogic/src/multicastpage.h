#if IF_FEATURE_PAGING
#ifndef __MULTICAST_PAGE_H__
#define __MULTICAST_PAGE_H__

#include "talk/ippaging/include/ippaging.h"
#include "singletonclassdefine.h"
#include "service_ipvp.h"

class CCallInfo;

struct RECPage_Info
{
    int iPriority;
    yl::string strLabel;
    yl::string strValue;
    int iChannel;
};

enum RECV_CHECK_RET
{
    RCR_OK = 0,
    RCR_WAIT,
    RCR_REFUSE,
};

typedef YLList<RECPage_Info*>  list_page;
typedef YLList<PAGING_PARAM*>  list_wait_page;

#define size_MIME               16

//////////////////////////////////////////////////////////////////////////
class CMulticastPage
{
    DECLARE_SINGLETON_CLASS(CMulticastPage)
public:
    // 初始化库信息
    void InitInfo();
    // 加载配置信息
    void InitListenInfo();

    // 是否为组播IP
    bool BeMulticastIP(const yl::string& strIPPort);

    //////////////////////呼出paging//////////////////////////
    // 呼出paging的请求
    bool CallPageOut(const char* cszDisplay, const char* cszNum, int iDsskey = -1,
                     const char* cszChannel = "");

    //////////////////// 接收paging的请求 //////////////////////
    // 接收外部发回的Paging请求
    // 返回：1-通话创建成功，0-通话创建失败，-1-条件不满足，失败
    bool ReceiveCallInPage(PAGING_PARAM* param);
    // 中止已经加入列表的的Paging请求
    bool BreakOffCallInPage(PAGING_PARAM* param);
    // 处理等待队列里的paging接收请求
    bool RestartWaitPage();

    ///////////////////// 声音参数设置   ///////////////////////
    // 由VPM回调的关于Paging的控制
    // 打开或关闭已经处于通话中的Paging。从传入的CallID可知，此时的session已经建立，只是未调用Paging库的接口.
    // 呼出时的作用：通知paging线程可以将来自VPM的rtp包以组播的形式发出去了。
    // 接收时的作用：通知paging线程可以接收组播包并转给VPM
    bool PagingTalkSwitch(int iCallID, bool bStart);
    // 开启或关闭paging的通话
    bool StartPagingTalk(PAGING_PARAM* param);
    bool StopPagingTalk(PAGING_PARAM* param, STOPPAGE eType);

    // 网络状态改变消息处理
    void OnNetworkChange(bool bConnectOk = true);

    void CheckChannel(int& iChannel);

    int GetPriorityByIP(char* pStrIP, int iPort, int iChannel);

private:
    // 为呼出的数据设置声音及组播参数
    void SetPageVoiceParam(CCallInfo* pCallInfo, PAGING_PARAM* pPage = NULL);

    // 设置通话的Voice参数
    // | pCallInfo    | [out] 通话的信息属性
    // | cszCodecName | [in/out]  指定Paging参数
    // 返回：true-成功，false-失败
    bool SetVoiceParam(ipvp_speech_t* pVmParam, PAGING_PARAM* pPageParam);
    // 设置通话的Paging参数
    // | pCallInfo | 通话的信息属性
    // | pParam    | Paging的参数
    // 返回：true-成功，false-失败
    bool SetMultiPageParam(CCallInfo* pCallInfo, PAGING_PARAM* pPageParam);

    // 检测Paging的接收条件
    // 返回：-1-拒绝，0-等待，1-可以建立paging通话
    int CheckRecvCondition(int iPriority);

    // 获取rtp端口
    int GetRtpPort();

    void ClearPageList();
    void ClearWaitList();
    RECPage_Info* ExistInList(const char* pStrPage, int iChannel);
    RECPage_Info* GetPageById(int iID);
    void ChangeListen(int iID);
    void RemoveListenByID(RECPage_Info* pInfo);
    PAGING_PARAM GetPageInfo(RECPage_Info* pInfo);
    int GetCodecIDByName(char* pCodec);
    const char* GetCodecNameByID(int iID);
    void PrintPage(PAGING_PARAM* pInfo);
    void AddWaitPage(PAGING_PARAM* param);
    void RemoveWaitPage(PAGING_PARAM* param);

    // 去掉所有的监听Paging
    void RemoveAllListenPaging();
    // 退出所有监听的组播账号
    void HangUpAllRecPaging();

private:
    int             m_iCurrentPort;
    int             m_iMinRTPPort;
    int             m_iMaxRTPPort;
    int             m_iDTMFPayload;
    int             m_iReceivePriority;
    bool            m_bPriorityActive;
    char            m_strCodecName[size_MIME];
    list_page       m_pListPage;
    list_wait_page  m_pListWait;
};

// 获取单键静态实例
#define _MulticastPage GET_SINGLETON_INSTANCE(CMulticastPage)

#endif
#endif //IF_FEATURE_PAGING
