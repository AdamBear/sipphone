#ifndef CDLGRESETTOFACTORY_H
#define CDLGRESETTOFACTORY_H

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "uikernel/quipushbutton.h"
#include "baseui/t4xpicpath.h"
#include <QDialog>

class CDlgResetToFactory : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgResetToFactory(QWidget * parent = 0);
    ~CDlgResetToFactory();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgResetToFactory();
    }

public:
    enum
    {
        RESET_CONFIG_ACTION_BASE = 0,
        RESET_CONFIG_ACTION_RESET_LOCAL_SETTINGS,
        RESET_CONFIG_ACTION_RESET_WEB_SETTINGS,
        RESET_CONFIG_ACTION_RESET_NON_STATIC_SETTINGS,
        RESET_CONFIG_ACTION_RESET_STATIC_SETTINGS,
        RESET_CONFIG_ACITON_RESET_USERDATA_AND_CONFIG,
        RESET_CONFIG_ACTION_RESET_FACTORY,
        RESET_CONFIG_ACTION_RESET_END,
    };
    enum RESET_PAGE_TYPE
    {
        RPT_ADV = 0,
        RPT_BASIC = 1,
    };

public:
    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool OnAction(const QString & strAction);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置小窗口数据
    virtual void SetData(void* pData = NULL);

private:
    // 获得分层恢复的类型
    int GetRestoreType(int nAction);

    // 执行提示框提示操作
    bool OnShowPromptAction(const QString & strAction);

    void ShowWaitMessageBox(int nAction);
private:
    RESET_PAGE_TYPE m_eType;
};

#endif // CDLGRESETTOFACTORY_H
