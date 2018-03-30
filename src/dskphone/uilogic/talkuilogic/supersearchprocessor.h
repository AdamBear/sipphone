#ifndef _TALKUILOGIC_SUPERSEARCH_PROCESSOR_H
#define _TALKUILOGIC_SUPERSEARCH_PROCESSOR_H

#include <ylstl/ylstring.h>
#include "commonunits/basedeletelater.h"

#include "talkuilogiccommon.h"

//超级搜索相关
#include "contacts/supersearch/include/modsupersearch.h"
#include "contacts/supersearch/src/searchaction.h"

////搜索结果回调函数
////pData 为回调参数， bSuperSearchResult 为搜索结果, refMessage为回调数据
//typedef LRESULT (*PSuperSearchHandleFun)(void* pData, bool bSuperSearchResult, msgObject& refMessage);

//声明一下超级搜索接收者
class CSuperSearchReceiverBase;


class CSuperSearchProcessor : public CBaseDeleteLater, public CMKProcessor
{
public:
    CSuperSearchProcessor();
    virtual ~CSuperSearchProcessor();

public:
    //初始化函数
    void Init();

    //反初始化函数
    void Uninit();

    //失去、获得焦点 调用
    void OnFocusChange(bool bFoucus);

public:
    //处理搜索
    void DealSearchData(yl::string strSearchData);

    //搜索联系人结果返回
    LRESULT DealSuperSearchMessage(msgObject & refMessage);

    //获取超级搜索数据
    bool GetSuperSearchData(SuperSearchItemListData & objSuperSearchListData);

    //设置 超级搜索回调函数
    void SetSuperSearchReceiver(CSuperSearchReceiverBase * pReceiver);

    void SetSuperSearchResultEnable(bool bEnable);

public:
    //定时函数
    BOOL OnSuperSearchTimer(msgObject & msg);

protected:
    //启动定时器
    void StartTime();
    //停止定时器
    void StopTime();

protected:
    //搜索的数据
    yl::string m_strSearchData;

    mkThreadTimer m_objSearchTimer;     //搜索定时器

    bool m_bFirstSuperSearch;       //是否第一次超级搜索

    bool m_bIsFocusChangeSearch;    //是否是焦点变化引发的搜索

    bool m_bSuperSearchResult;      //超级搜索结果 是否成功

    CSuperSearchReceiverBase * m_pReceiver;     //超级搜索结果接收

};


#endif
