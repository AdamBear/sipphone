#ifndef __FRAME_REMOTE_TITLE_H__
#define __FRAME_REMOTE_TITLE_H__

#include "baseui/frametiptitle.h"
#include "baseui/titlebutton.h"
#include "uikernel/btnbarwrapper.h"

enum
{
    FRAME_LIST_TITLE_TYPE_REMOTE_TITLE          = FRAME_LIST_TITLE_TYPE_NONE + 6,
};

class CRemoteListTitle : public CFrameTipTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif
public:
    explicit CRemoteListTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CRemoteListTitle(void);

    static bool IsRemoteListTitle(CFrameListTitlePtr pTitle);
    static CRemoteListTitle * GetRemoteListTitle(CFrameListTitlePtr pTitle);

public:
    // 设置按钮数量
    void SetBtnCount(int nCount);
    // 添加按钮(将自动按顺序添加)
    void AddButton(int nAction, const QString & strTitle);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 重新布局
    virtual void Relayout();

public:
    void SetRemoteTip(yl::string strTitleTip, int nAmount);

protected:
    enum
    {
        MAX_BTN_COUNT       = 2,
    };

    // page按钮
    CTitleButton m_btnPrePage;
    // next按钮
    CTitleButton m_btnNextPage;

    // 按钮工具条
    CBtnBarWrapper m_btnBar;
    // 按钮数量
    int m_nBtnCount;
    // 按钮标题
    QString m_arrTitle[MAX_BTN_COUNT];
};
typedef CRemoteListTitle * CRemoteListTitlePtr;

#endif
