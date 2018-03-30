#ifndef _PING_AND_TRACEOUT_DELEGATE_H_
#define _PING_AND_TRACEOUT_DELEGATE_H_

#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"

enum NETWORK_CHECK_SHOWINFO_TYPE
{
    NETWORK_CHECK_SHOWINFO_TYPE_NONE = 0,       //未知类型
    NETWORK_CHECK_SHOWINFO_TYPE_IP,             //显示输入ip
    NETWORK_CHECK_SHOWINFO_TYPE_WAIT,           //显示等待界面
    NETWORK_CHECK_SHOWINFO_TYPE_RESULT,         //显示检测结果
};

class CPingAndTraceoutDelegate : public CSettinguiDataDelegateBase, public CMsgBoxCallBackAction
{
public:
    CPingAndTraceoutDelegate();
    virtual ~CPingAndTraceoutDelegate();

    static CSettingUIDelegateBase * CreatePingAndTraceDelegate();
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    static LRESULT OnMessage(msgObject & msg);
    virtual bool OnAction(int eActionType, int iDataIndex);
public:
    void OnDiagnosticsResultMessage();
    //开始网络检测
    void StartNetworkCheck();
private:
    void ShowWaitingMsgBox();
    void ShowErrorMsgBox(const yl::string & strNote, int nID, int nTime);
protected:
    NETWORK_CHECK_SHOWINFO_TYPE m_eShowInfoType;        //当前显示的信息 类型
};
#endif //_PING_AND_TRACEOUT_DELEGATE_H_
