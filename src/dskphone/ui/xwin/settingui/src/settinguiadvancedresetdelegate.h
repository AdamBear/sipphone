#ifndef __SETTING_UI_ADVANCED_RESET_DELEGATE_H__
#define __SETTING_UI_ADVANCED_RESET_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "settingmsgboxmgr.h"

class CSettingUIAdvancedResetDelegate : public CSettingUIDelegateBase, public CAsyncOperateAction
{
public:
    CSettingUIAdvancedResetDelegate();
    virtual ~CSettingUIAdvancedResetDelegate();

    static CSettingUIDelegateBase * CreateAdvancedResetDelegate();

public:
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    //界面Item统一操作
    virtual bool ProcessAction(const yl::string strAction, int iDataIndex);

    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);

    virtual void AsyncOperateCallBack();

protected:
    bool GetResetInfoByItemID(const yl::string& strItemID, int& nMsgBoxID, yl::string& strTitle, yl::string& strNote);
    yl::string GetResttingStringByID(int nID);
private:
    int         m_nID;
};

#endif
