#ifndef __SETTINGUI_MESSAGE_DATA_H__
#define __SETTINGUI_MESSAGE_DATA_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"

// 消息类型
enum MESSAGE_METHOD
{
    MESSAGE_METHOD_INVALID          = -1,
    MESSAGE_METHOD_SEND             = 1,
    MESSAGE_METHOD_POST_THREAD      = 2,
    MESSAGE_METHOD_BROADCAST        = 3,
    MESSAGE_METHOD_NOTIFYAPP        = 4,
    MESSAGE_METHOD_NOTIFYAPPEX      = 5,
};

class CSettingUIMessageData
{
public:
    CSettingUIMessageData();
    CSettingUIMessageData(const CSettingUIMessageData & objMsgData);
    virtual ~CSettingUIMessageData();

public:
    void Reset();

    //CSettingUIMessageData& operator = (const CSettingUIMessageData& objMsgData);

    bool GetMessageData(CSettingUIMessageData & objMsgData);
    bool SetMessageData(const CSettingUIMessageData & objMsgData);

public:
    int GetMsgID() const;
    void SetMsgID(int iMsgId);
    int GetlParam() const;
    void SetlParam(int ilParam);
    int GetwParam() const;
    void SetwParam(int iwParam);
    int GetPriority() const;
    void SetPriority(int iPriority);
    int GetSendMethod() const;
    void SetSendMethod(int iSendMethod);

    const yl::string & GetDstThread();
    void SetDstThread(const yl::string & strDstThread);
    const yl::string & GetVarDataID();
    void SetVarDataID(const yl::string & strVarDataID);
    const yl::string & GetLparamID() const;
    void SetLparamID(const yl::string & strLparamID);
    const yl::string & GetWparamID() const;
    void SetWparamID(const yl::string & strWparamID);

private:
    int                         m_iMsgId;
    int                         m_lParam;
    int                         m_wParam;
    int                         m_iPriority;    //发送优先级
    MESSAGE_METHOD              m_eMethod;      //发送方式
    yl::string                  m_strDstThread; //目标线程
    yl::string
    m_strVarDataID; //作为消息附件数据的控件值对应的控件的id
    yl::string                  m_strLparamID;
    yl::string                  m_strWparamID;
};

typedef     CSettingUIMessageData   *   CSettingUIMessageDataPtr;
typedef YLList<CSettingUIMessageData*>  LIST_SETTING_MSG_DATA;

#endif
