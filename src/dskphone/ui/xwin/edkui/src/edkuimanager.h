#ifndef _EDKUI_MANAGER_H_
#define _EDKUI_MANAGER_H_

#include <singletonclassdefine.h>
#include <ETLLib.hpp>
#include "common/common_data_define.h"
#include "../include/edkui_def.h"


class CEdkUIManager
{
    DECLARE_SINGLETON_CLASS(CEdkUIManager)
public:
    // 初始化
    bool Init();

    // 初始化EDK自定义softkey接口
    void InitGetEdkSoftkeyFun();

    // 通过softkey type获取提示名称
    bool GetTipName(SoftKey_TYPE eType, yl::string & strTipName);

    // 响应edk调用的softkey功能
    bool OnExcuteSoftkey(const yl::string & strSoftkeyID);

    // 响应edk调用的进入菜单功能
    bool EnterMenu(const yl::string & strMenuID);

    // 获取softkey对应的dsskey
    DssKey_Type GetDsskeyTypeBySoftkey(SoftKey_TYPE eType);

    // 判断是否允许进入menu的条件
    bool IsAllowEnterUI(const yl::string & strMenuID);

    yl::string GetMapNameByPageData(const yl::string & strPageName, const yl::string & strNextPageName,
                                    const int & nID);

private:
    static LRESULT OnMessage(msgObject & objMessage);

    bool EnterSettingPage(const yl::string & strPageName);
};
// 实例
#define _EDKUIManager GET_SINGLETON_INSTANCE(CEdkUIManager)
#endif // _EDKUI_MANAGER_H_
