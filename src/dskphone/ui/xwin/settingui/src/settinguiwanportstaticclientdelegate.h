#ifndef _SETTINGUI_WANPORT_STATIC_CLIENT_DELEGATE_H_
#define _SETTINGUI_WANPORT_STATIC_CLIENT_DELEGATE_H_

#include "settingui/include/settinguidefine.h"
#if !IF_SUPPORT_IP_SET_SINGLE_MENU
#include "settinguidatadelegatebase.h"

class CSettingUIWanPortStaticClientDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIWanPortStaticClientDelegate();
    virtual ~CSettingUIWanPortStaticClientDelegate();

    static CSettingUIDelegateBase * CreatWanPortStaticClientDelegate();

public:
    // 保存配置数据
    bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);
    virtual void SaveExtraData();
protected:
    bool SetInvalidNote(CSettingUIDataValidInfo & validInfo, const yl::string & strNote, int nIndex);
};
#endif

#endif //_SETTINGUI_WANPORT_STATIC_CLIENT_DELEGATE_H_
