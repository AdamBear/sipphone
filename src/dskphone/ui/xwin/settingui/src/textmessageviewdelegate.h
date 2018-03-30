#ifndef _TEXT_MESSAGE_VIEW_DELEGATE_H_
#define _TEXT_MESSAGE_VIEW_DELEGATE_H_

#include "pingandtraceoutresultdelegate.h"
#include "ylvector.h"
#include "settinguilogic/include/common.h"

typedef YLVector<yl::string> VEC_TEXT;

class CTextMessageViewDelegate : public CPingAndTraceOutResultDelegate
{
public:
    CTextMessageViewDelegate();
    virtual ~CTextMessageViewDelegate();
    static CSettingUIDelegateBase * CreatTextMessageViewDelegate();
    void SavePreValue(TextMsgItemDetailData & stDetailData, int nTextMember);
    void EnterPageNewMessage();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
protected:
    yl::string m_strFromUserName;
    yl::string m_strText;
    int m_nTextMember;
    bool m_bReply;
};

#endif //_TEXT_MESSAGE_VIEW_DELEGATE_H_
