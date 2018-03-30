#ifndef __SETTINGUI_DIALOG_MANAGER_H__
#define __SETTINGUI_DIALOG_MANAGER_H__

#include "ylstl/ylstring.h"
#include "ylhashmap.h"
#include "settinguicommondata.h"

class CTileSubDialog;
class CTileDialog;
class CSettingDialogBase;

typedef YLHashMap<CSettingDialogBase *, int> MAP_DELEGATE_FALLBACKING;

class CSettingUIDialogManager
{
private:
    CSettingUIDialogManager();
    virtual ~CSettingUIDialogManager();

public:
    static CSettingUIDialogManager * GetInstance();
    static void ReleaseInstance();

public:
    // 打开界面
    bool OpenPage(const yl::string & strPageID, bool bRefresh = true);
    bool OpenPage(const PageOpenData & openPageData, bool bRefresh = true);
    // 打开一连串界面
    bool OpenPageOrderByPageID(const yl::string & strPageIDList);
    // 回退到上一层界面
    void FallBackToThePreviousInterface(CSettingDialogBase * pCurDlg);
    // 把当前界面所有关联Delegate释放掉
    void DropOutCurrentPage(CSettingDialogBase * pCurDlg, bool bDelDlg = true);
    // 释放界面资源
    void ReleasePageResource(CSettingDialogBase * pDlg);
    // 获取界面
    CSettingDialogBase * GetDialog(const yl::string & strPageID);
    // 获取顶层活动窗口
    CSettingDialogBase * GetTopActiveDialog(const yl::string & strPageId = "");

    void Init();
    void UnInit();
    // 收回界面
    void DropSettingUI(const yl::string & strPageId = "");
    ////////////////////////////////////// 内部调用函数 ///////////////////////////
private:
    // 收回界面
    void ReleasePrivateDialog(CSettingDialogBase * pDlg);
    // 移除标记
    void RemoveFallBackingMark(CSettingDialogBase * pDlg);
    // 从字符串中取第nIndex个字符
    yl::string GetValueFromListByIndex(LIST_STRING& listValue, int nIndex);

private:
    static CSettingUIDialogManager   *  s_SettingUIDialogMgr;
    MAP_DELEGATE_FALLBACKING
    m_mapDlgFallBacking;        // 界面回退标记，防止同个界面回退多次
};

#define g_pSettingUIDialogManager (CSettingUIDialogManager::GetInstance())

#endif // __SETTINGUI_DIALOG_MANAGER_H__

