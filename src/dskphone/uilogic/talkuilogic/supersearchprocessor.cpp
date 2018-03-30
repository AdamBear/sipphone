#include "supersearchprocessor.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "supersearchreceiverbase.h"

//mod-logic
#include "contacts/directory/include/moddirinterface.h"

//超级搜索的定时时间  单位：MS
const int iSuperSearchElapse = 200;
//第一次搜索延迟时间
const int iFirstSuperSearchElapse = 300;

CSuperSearchProcessor * g_pSuperSearchProcessor = NULL;

LRESULT OnTalkUILogicSuperSearchMessage(msgObject & refMessage);

LRESULT OnTalkUILogicSuperSearchMessage(msgObject & refMessage)
{
    if (NULL != g_pSuperSearchProcessor)
    {
        return g_pSuperSearchProcessor->DealSuperSearchMessage(refMessage);
    }

    return FALSE;
}

CSuperSearchProcessor::CSuperSearchProcessor()
{
    //改成在初始化的时候设置 反初始化的时候置空
    //g_pSuperSearchProcessor = this;

    //搜索关键字设置为空
    m_strSearchData = "";

    m_bFirstSuperSearch = true;     //是否第一次超级搜索

    m_bIsFocusChangeSearch = false; //是否是焦点变化引发的搜索

    m_bSuperSearchResult = false;       //超级搜索结果 是否成功

    m_pReceiver = NULL;

}

CSuperSearchProcessor::~CSuperSearchProcessor()
{
    //改成在初始化的时候设置 反初始化的时候置空
    //析构后要把该值置空
    //这边设置 只是为了容错
    if (this == g_pSuperSearchProcessor)
    {
        g_pSuperSearchProcessor = NULL;
    }
    //g_pSuperSearchProcessor = NULL;

    //析构停止定时器， 只是作为容错  不一定有使用
    StopTime();
}

//初始化函数
void CSuperSearchProcessor::Init()
{
    //设置成是第一次超级搜索
    m_bFirstSuperSearch = true;

    //搜索关键字设置为空
    m_strSearchData = "";

    m_bIsFocusChangeSearch = false; //是否是焦点变化引发的搜索

    m_bSuperSearchResult = false;       //超级搜索结果 是否成功

    //注册超级搜索结果消息
    etl_RegisterMsgHandle(SUPER_SEARCH_RESULT,
                          SUPER_SEARCH_RESULT,
                          OnTalkUILogicSuperSearchMessage);

    //在初始化的时候 设置  在反初始化的时候置空
    g_pSuperSearchProcessor = this;

}

//反初始化函数
void CSuperSearchProcessor::Uninit()
{
    //反初始化要停止定时器
    StopTime();

    //判断一下 若果设置的是自己 那么要置空
    if (this == g_pSuperSearchProcessor)
    {
        g_pSuperSearchProcessor = NULL;
    }

    //注销超级搜索回调
    etl_UnregisterMsgHandle(SUPER_SEARCH_RESULT, SUPER_SEARCH_RESULT, OnTalkUILogicSuperSearchMessage);
}

//失去、获得焦点 调用
void CSuperSearchProcessor::OnFocusChange(bool bFoucus)
{
    //需要比较一下 是否和当前搜索的数据相同, 需要和 逻辑层 当前搜索的key进行比较
    if (0)
    {
        if (true == bFoucus)
        {
            StopTime();

            //设置成不是因为focuschange 引发的搜索
            m_bIsFocusChangeSearch = true;

            StartTime();

        }
    }

}

void CSuperSearchProcessor::DealSearchData(yl::string strSearchData)
{
    StopTime();

    //设置成不是因为focuschange 引发的搜索
    m_bIsFocusChangeSearch = false;

    //保存搜索数据
    m_strSearchData = strSearchData;

    StartTime();
}

//搜索联系人结果返回
LRESULT CSuperSearchProcessor::DealSuperSearchMessage(msgObject & refMessage)
{
    //focuschange引发的搜索 是不需要通知的， 因为只是防止有多个搜索的时候， 当前的搜索下次获取数据的时候 获取不到
    //如果不是 因为focuschange 引发的搜索  那么是需要调用回调函数的
    if (false == m_bIsFocusChangeSearch)
    {
        //调用回调函数
        if (NULL != m_pReceiver)
        {
            m_pReceiver->DealSuperSearchResult(true, refMessage);
        }
    }

    return 0;
}

//获取超级搜索数据
bool CSuperSearchProcessor::GetSuperSearchData(SuperSearchItemListData & objSuperSearchListData)
{
    //超级搜索成功 才需要取数据
    if (true == m_bSuperSearchResult)
    {
        //这边赋值搜索的key
        objSuperSearchListData.m_strSearch = m_strSearchData;

        //这边为空的时候也是要取 因为为空可以取历史记录
        bool bResult = SuperSearch_GetContactListData(objSuperSearchListData, m_strSearchData);

        return bResult;

    }

    return false;

}

//设置 超级搜索回调函数
void CSuperSearchProcessor::SetSuperSearchReceiver(CSuperSearchReceiverBase * pReceiver)
{
    m_pReceiver = pReceiver;
}

//定时函数
BOOL CSuperSearchProcessor::OnSuperSearchTimer(msgObject & msg)
{
    TALKUI_INFO("CSuperSearchProcessor::OnSuperSearchTimer begin");

    //不管如何 先停止定时器
    StopTime();

    //如果查找不成功 要重新刷一遍数据  不成功的情况有 搜索内容为空 从历史记录中查找没确认
    //使用新的方式搜索
    if (!SuperSearch_ReqSearch(m_strSearchData))
    {
        m_bSuperSearchResult = false;

        //调用回调函数
        if (NULL != m_pReceiver)
        {
            m_pReceiver->DealSuperSearchResult(false, msg);
            //  m_pSuperSearchHandleFun(m_pSuperSearchHandleData, false, msg);
        }
    }
    else
    {
        m_bSuperSearchResult = true;
    }

    return TRUE;
}

//启动定时器
void CSuperSearchProcessor::StartTime()
{
    //无论如何 先调用一次 停止定时函数
    StopTime();

    //搜索延时
    int iSpan = iSuperSearchElapse;

    //设置延时时间  如果是第一次搜索 要设置为第一次搜索的延时时间  并且把标识位设置成非第一次搜索
    if (true == m_bFirstSuperSearch)
    {
        m_bFirstSuperSearch = false;
        iSpan = iFirstSuperSearchElapse;
    }
    else
    {
        iSpan = iSuperSearchElapse;
    }
    m_objSearchTimer.SetTimer(iSpan, NULL, MK_MSG_ENTRY(this,
                              CSuperSearchProcessor::OnSuperSearchTimer));
}

//停止定时器
void CSuperSearchProcessor::StopTime()
{
    //如果定时器在跑 先关闭定时器
    if (m_objSearchTimer.IsTimerRuning())
    {
        m_objSearchTimer.KillTimer();
    }
}

void CSuperSearchProcessor::SetSuperSearchResultEnable(bool bEnable)
{
    m_bSuperSearchResult = bEnable;
}
