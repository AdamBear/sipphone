#ifndef _TALKUILOGIC_SUPERSEARCH_RECEIVER_BASE_H
#define _TALKUILOGIC_SUPERSEARCH_RECEIVER_BASE_H

#include <ETLLib.hpp>

class CSuperSearchReceiverBase
{
public:
    CSuperSearchReceiverBase();
    virtual ~CSuperSearchReceiverBase();

public:
    //bResult搜索是否成功， refMessage搜索返回的数据
    virtual void DealSuperSearchResult(bool bResult, msgObject & refMessage);

};


#endif
