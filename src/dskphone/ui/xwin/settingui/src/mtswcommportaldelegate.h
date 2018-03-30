#ifndef _MTSW_COMMPORTAL_DELEGATE_H_
#define _MTSW_COMMPORTAL_DELEGATE_H_

#if IF_FEATURE_METASWITCH
#include "ylvector.h"
#include "settinguilogic/include/common.h"
#include "settinguidatadelegatebase.h"
#include "setuidatadefine.h"

class CMtswCommportalDelegate : public CSettinguiDataDelegateBase
{
public:
    CMtswCommportalDelegate();
    virtual ~CMtswCommportalDelegate();
    static CSettingUIDelegateBase* CreatMtswCommportalDelegate();
    static LRESULT OnMTSWCommportalMsg(msgObject& msg);

public:
    virtual bool SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
    virtual bool OnBackAction();

    void OnReturnCallback(bool bLoginSuccess);
private:
    FunMTSWLoginReturnCallBack      m_pReturnCallBack;
};
#endif

#endif //_MTSW_COMMPORTAL_DELEGATE_H_
