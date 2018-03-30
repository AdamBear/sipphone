#ifndef __MESSAGE_BOX_FACTORY_H_
#define __MESSAGE_BOX_FACTORY_H_

#include "messageboxbase.h"
#include <ylvector.h>

struct MsgBoxCacheInfo
{
    //弹框指针
    CMessageBoxBase * m_pMessageBox;
    //是否已被使用
    bool                m_bUsed;
};

typedef YLVector<MsgBoxCacheInfo>       vecMsgBoxCacheInfo;
typedef vecMsgBoxCacheInfo::iterator        iterCacheInfo;

class CMessageboxFactory
{
public:
    explicit CMessageboxFactory(MESSAGEBOX_TYPE eMsgType);
    ~CMessageboxFactory();

private:
    static CMessageBoxBase * GreateMsgBox(MESSAGEBOX_TYPE eMsgType);

public:
    CMessageBoxBase * GetMsgBox();
    void PutMsgBox(CMessageBoxBase * pMessageBox);

protected:
    CMessageBoxBase * CreateMsgBoxAndAddToCache();

protected:
    MESSAGEBOX_TYPE     m_eMsgType;
    vecMsgBoxCacheInfo  m_vecMsgBoxCacheInfo;

};

#endif //__MESSAGE_BOX_FACTORY_H_
