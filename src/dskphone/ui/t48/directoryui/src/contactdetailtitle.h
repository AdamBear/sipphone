
#ifndef __CONTACT_DETAIL_TITLE_H__
#define __CONTACT_DETAIL_TITLE_H__

#include "baseui/framelisttitle.h"
#include "baseui/titlebutton.h"
#include "uikernel/btnbarwrapper.h"

enum
{
    FRAME_LIST_TITLE_TYPE_CONTACT_DETAIL_TITLE      = FRAME_LIST_TITLE_TYPE_NONE + 5,
};

class CContactDetailTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
#endif

public:
    CContactDetailTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CContactDetailTitle(void);

    static bool IsContactDetailTitle(CFrameListTitlePtr pTitle);

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

protected:
    enum
    {
        MAX_BTN_COUNT       = 5,
    };

    // 拔号按钮
    CTitleButton m_btnDial;
    // 移动按钮
    CTitleButton m_btnMove;
    // 编辑按钮
    CTitleButton m_btnEdit;
    // 黑名单按钮
    CTitleButton m_btnBlackList;
#if IF_FEATURE_METASWITCH_DIRECTORY
    // Metaswitch按钮
    CTitleButton m_btnAddMtswContact;
#endif
#if IF_FEATURE_FAVORITE_DIR
    CTitleButton m_btnFavorite;
    CTitleButton m_btnRemoveFavorite;
#endif
    // 删除按钮
    CTitleButton m_btnDel;
    // 按钮工具条
    CBtnBarWrapper m_btnBar;

    // 按钮数量
    int m_nBtnCount;
    // 按钮标题
    QString m_arrTitle[MAX_BTN_COUNT];
};
typedef CContactDetailTitle * CContactDetailTitlePtr;

#endif
