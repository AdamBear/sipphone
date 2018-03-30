#ifndef __SETTINGUI_DIRECTORY_DELEGATE_H__
#define __SETTINGUI_DIRECTORY_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "directoryui/include/moddirectoryui.h"


class CSettingUIDirectoryDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIDirectoryDelegate();
    virtual ~CSettingUIDirectoryDelegate();

    static CSettingUIDelegateBase * CreateDirectoryDelegate();
public:
    // 加载配置数据
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);

    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
#if IF_SUPPORT_SELECT_CONTACT_INFO
    // 打开界面附加操作
    static void UpdateContactInfoCallBack(yl::string strContactName, yl::string strContactNumber);
    void UpdateContactInfo(yl::string strContactName, yl::string strContactNumber);
    virtual void InitExtraData(void* pData);
#endif
public:
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool FocusEvent(bool bFocusIn);
protected:
    void OnEnterPage(int nDataIndex);
    void OnEnterSearch(int nDataIndex);
private:
#if IF_SUPPORT_SELECT_CONTACT_INFO
    DIR_SELECT_CONTACT_FUN m_pFunSelectContact;
#endif
    bool m_bNeedBack;
};

#endif
