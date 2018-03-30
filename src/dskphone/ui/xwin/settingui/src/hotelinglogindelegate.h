#ifndef HOTELING_LOGIN_DELEGATE_H_
#define HOTELING_LOGIN_DELEGATE_H_

#include "settinguidatadelegatebase.h"
#include "messagebox/messageboxcallback.h"

class CHotelingLoginDelegate : public CSettinguiDataDelegateBase, public CMsgBoxCallBackAction
{
public:
    CHotelingLoginDelegate();
    ~CHotelingLoginDelegate();

    static CSettingUIDelegateBase * CreateHotelingLoginDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);

private:
    int m_nHotelingType;
};

#endif //HOTELING_LOGIN_DELEGATE_H_
