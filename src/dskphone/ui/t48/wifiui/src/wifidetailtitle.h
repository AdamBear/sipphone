#ifndef WIFIDETAILTITLE_H
#define WIFIDETAILTITLE_H

#include "uikernel/btnbarwrapper.h"
#include "baseui/framelisttitle.h"
#include "baseui/titlebutton.h"

enum
{
    FRAME_LIST_TITLE_TYPE_WIFI_DETAIL_TITLE     = 131,
};

class WifiDetailTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
#endif
public:
    WifiDetailTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~WifiDetailTitle(void);

    static bool IsWifiDetailTitle(CFrameListTitlePtr pTitle);

public:
    // 设置按钮数量
    void SetBtnCount(int nCount);
    // 添加按钮(将自动按顺序添加)
    void AddButton(int nAction, const QString & strTitle);
    // 设置连接与否
    void SetConnectOrNot(bool bConnectOrNot);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 重新布局
    virtual void Relayout();

protected:
    enum
    {
        MAX_BTN_COUNT       = 2,
    };

    // 连接/断开按钮
    CTitleButton m_btnCon;
    // 添加按钮
    CTitleButton m_btnAdd;

    // 按钮工具条
    CBtnBarWrapper m_btnBar;

    // 是否连接
    bool            m_bConnectOrNot;

    // 按钮数量
    int m_nBtnCount;
    // 按钮标题
    QString m_arrTitle[MAX_BTN_COUNT];
};
typedef WifiDetailTitle * WifiDetailTitlePtr;

#endif
