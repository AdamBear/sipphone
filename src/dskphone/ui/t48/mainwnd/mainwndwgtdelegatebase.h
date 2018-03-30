#ifndef _MAINWND_WGT_PROXY_BASE_H_
#define _MAINWND_WGT_PROXY_BASE_H_

#include <QWidget>
#include "uimanager/basedialog.h"

// 相关控件的action
#define DSSKEY_FRAME_CLICK_DSSKEY           "ClickDsskey"           // 点击Dsskey

#define SOFTKEY_BAR_DISPLAY_IME             "DisplayIME"            // 显示输入法
#define SOFTKEY_BAR_SWITCH_IME              "SwitchIME"             // 切换输入法
#define SOFTKEY_BAR_RESET_DOWN              "ResetDown"             // 重置按下状态

#define TITLE_BAR_SET_TITLE                 "SetTitle"              // 设置Title
#define TITLE_BAR_SET_LEFT_AREA_TEXT        "SetLeftAreaText"       // 设置左边区域的文字
#define TITLE_BAR_SET_LEFT_AREA_ICON        "SetLeftAreaIcon"       // 设置左边区域的图标
#define TITLE_BAR_SET_SELECT_ACCOUNT_ID     "SetSelectAccountID"    // 设置左边区域选择账号的ID
#define TITLE_BAR_SET_TITLE_INFO            "SetTitleInfo"          // 设置Title信息

class CBaseDialog;
class CMainWndWgtDelegateBase : public QObject
{
public:
    CMainWndWgtDelegateBase() {}
    virtual ~CMainWndWgtDelegateBase() {}

    virtual void DrawWidget(CBaseDialog *) = 0;
    virtual QWidget * GetWidget() = 0;
    virtual void SetParent(QWidget * pWgt) = 0;
    virtual void SetVisible(bool bVisible) = 0;
    virtual void Raise() = 0;

    virtual bool Operator(const QString & strAction, const QVariant & data)
    {
        return false;
    }

    virtual bool ProcessKeyEvent(QObject * pObject, QKeyEvent * pEvent)
    {
        return false;
    }
    virtual bool ProcessMouseEvent(QObject * pObject, QMouseEvent * pEvent)
    {
        return false;
    }
};

#endif // _MAINWND_WGT_PROXY_BASE_H_
