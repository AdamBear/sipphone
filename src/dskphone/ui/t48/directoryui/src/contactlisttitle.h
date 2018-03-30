
#ifndef __CONTACT_LIST_TITLE_H__
#define __CONTACT_LIST_TITLE_H__

#include "baseui/frameselecttitle.h"
#include "baseui/titlebutton.h"
#include "uikernel/btnbarwrapper.h"

enum
{
    FRAME_LIST_TITLE_TYPE_CONTACT_LIST_TITLE            = FRAME_LIST_TITLE_TYPE_NONE + 4,
};

class CContactListTitle : public CFrameSelectTitle
{
    Q_OBJECT

public:
    explicit CContactListTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CContactListTitle(void);

    static bool IsContactListTitle(CFrameListTitlePtr pTitle);

public:
    // 设置按钮数量
    void SetBtnCount(int nCount);
    // 添加按钮(将自动按顺序添加)
    void AddButton(int nAction, const QString & strTitle);
#ifdef IF_ENABLE_TESTABILITY
    virtual QString GetTestInfo();
#endif

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 重新布局
    virtual void Relayout();

protected:
    enum
    {
        MAX_BTN_COUNT       = 3,
    };

    // 移动按钮
    CTitleButton m_btnMove;
    // 黑名单按钮
    CTitleButton m_btnBlackList;
    // 删除按钮
    CTitleButton m_btnDel;
    // 按钮工具条
    CBtnBarWrapper m_btnBar;

    // 按钮数量
    int m_nBtnCount;
    // 按钮标题
    QString m_arrTitle[MAX_BTN_COUNT];
};
typedef CContactListTitle * CContactListTitlePtr;

#endif
