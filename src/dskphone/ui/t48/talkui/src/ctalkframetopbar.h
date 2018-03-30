#ifndef _TALK_FRAME_TOP_BAR_
#define _TALK_FRAME_TOP_BAR_

#include <QFrame>
#include <QLabel>
#include <ETLLib.hpp>
#include <ylstring.h>

#include "mainwnd/accountmenuselector.h"
#include "uicommon/messagebox/modmessagebox.h"


// 前向声明
class qWidgetList;
class qDateTimeLabel;
class qUIPushButton;

class CTalkFrameTopbar : public QFrame, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    CTalkFrameTopbar(QWidget * parent = 0);
    ~CTalkFrameTopbar();

    // 设置通道图标
    void SetChannelIcon(const QString & strPath);

    // 设置Title
    void SetTitle(const yl::string & strTitle);

    // 设置账号按钮的文字
    void SetLineBtnText(const QString & strText);

    // 设置账号按钮是否激活
    void SetLineBtnActive(bool bActive);

    void SetAccountID(int iAccountID);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private:
    // 初始化数据
    void InitData();

signals:
    void SelectUserLine(int iAccountID);

public slots:
    void OnLineBtnClick();

private:
    QString         m_strChannelIconPath;
    QLabel          m_labChannelIcon;
    QLabel          m_labTitle;
    qDateTimeLabel * m_pDateTime;
    qUIPushButton * m_pLineBtn;
    int             m_iAccountID;

    CAccountMenuSelector m_accountSelector;
};

#endif
