#ifndef _MTSW_VM_LIST_DELEGATE_H_
#define _MTSW_VM_LIST_DELEGATE_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"

class CMtswVoiceMailListDelegate : public CSettinguiDataDelegateBase, public CMsgBoxCallBackAction
{
public:
    CMtswVoiceMailListDelegate();
    virtual ~CMtswVoiceMailListDelegate();
    static CSettingUIDelegateBase* CreateMtswVoiceMailListDelegate();
    static LRESULT OnMTSWVoiceMailListMsg(msgObject& msg);

public:
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

private:
    void AddVoiceMailItem(CSettingUIPageDataBase* pPageData,
                          const yl::string& strItemID, const yl::string& strDisp, const yl::string& strValue, bool bRead, bool bUrgent);
    //获取MLHG列表
    void LoadVoicemailList();
    // 发送处理
    void ProcessCallBack();
    // Play键处理
    void ProcessPlay();
    // 进入Detail界面
    void EnterDetail();
    // 进入播放界面
    void EnterPlay();
    // 添加到联系人
    void AddToContacts(bool bIsLocal);
    // 标记读取状态
    void MarkReadStatus(ReadStatus readStatus);
    // 显示选项
    void ShowOption();

};
#endif

#endif //_MTSW_VM_LIST_DELEGATE_H_
