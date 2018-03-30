/************************************************************************
 * FileName  : CTagTitlebar.h (header file)
 * Purpose   :
 * Date      : 2012/10/22 14:18:31
 ************************************************************************/

#ifndef __TITLE_BAR_H__
#define __TITLE_BAR_H__

#include "config.h"
#include <ylstring.h>
#include <QWidget>
#include "ETLLib/ETLLib.hpp"
#include "baseui/titlebutton.h"
#include "setting/include/common.h"
#include "statusbutton.h"
#include "statusbtnbar.h"
#include "uimanager/uimanager_common.h"
#include "idlescreen/include/idlescreen_def.h"
#include "mainwnd/accountmenuselector.h"
#include "mainwndhelp/statusiconsbar.h"
#include "uicommon/messagebox/modmessagebox.h"
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "uicommon/uikernel/qtimepiece.h"
#endif
#include "uikernel/qscrolltextlabel.h"

class QStylePainter;
class qDateTimeLabel;

class CTitleBar : public QWidget, public CMsgBoxCallBackAction
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();

#endif
public:
    CTitleBar(QWidget * parent = NULL);

    virtual ~CTitleBar();

    // 设置Title
    void SetTitle(const QString & strTitle);

    // 获取Title
    QString GetTitle() const;

    // 设置Title信息
    void SetLayoutInfo(const TitleLayoutInfo & objTitleInfo);

    // 设置左边区域信息
    void SetMidAreaType(TITLE_AREA_MODE eType);

    // 设置左边区域文字
    // 显示默认账号类型的时候不生效
    void SetAccountDisplayName(const QString & strText);

    // 设置左边区域图标
    // 显示默认账号类型的时候不生效
    void SetAccountIcon(const QString & strIcon);

    // 设置选择账号的ID,-1为选中默认账号
    void SetSelectAccountID(int nAccountID);

    // 设置背景图片
    void SetBackgroundImage(const yl::string & strImg);

    // 刷新帐号
    void RefreshAccount();

    // 刷新状态栏的图标
    void RefreshStatusIcon(const YLList<IdleStatusItem> & vecStatusItem);

    // 获取当前设置的日期和时间格式，进行刷新显示
    void RefreshDateTimeFormat();

    LRESULT OnMessage(msgObject & refMessage);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

#if IF_FEATURE_STATION_NAME
    // 刷新station name
    void RefreshStationName();
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    void StartUSBRecord();
    void PauseUSBRecord();
    void StopUSBRecord();
#endif

signals:
    // 选择账号弹出框发出的信号
    void SelectAccount(int iAccount);

private:
    void Init();

    void CheckShowStatusIcon(int nModeType);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * e);

    void SetLeftText(const QString & strText);

    void OnLineBtnClick();
    void OnStatusBtnClick(int nStatusBtnId);

private:
    yl::string      m_strBGImg;
    QString         m_strLeftAreaText;
    QString         m_strIcon;
    int             m_iSelectAccountID;
    QString         m_strTitle;
    CTitleButton    m_btnLine;
    CStatusIconsBar m_TitleBarStatusBar;
    qDateTimeLabel * m_pDateTime;
#if IF_FEATURE_STATION_NAME
    qScrollTextLabel*    m_pLabScrollStationName;
    bool                 m_bShowStation;
#endif
    TitleLayoutInfo     m_titleInfo;
    bool                m_bAccountUsable; // 账号是否有效

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    //录音时间提示控件
    QTimePiece      m_RecordTime;
    QLabel          m_RecordIcon;
#endif

    CAccountMenuSelector m_accountSelector;
};

#endif // __TITLE_BAR_H__
