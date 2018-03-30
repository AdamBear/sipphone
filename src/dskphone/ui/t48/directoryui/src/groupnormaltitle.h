
#ifndef __GROUP_NORMAL_TITLE_H__
#define __GROUP_NORMAL_TITLE_H__

#include "baseui/framelisttitle.h"
#include "baseui/titlebutton.h"
#include "uikernel/btnbarwrapper.h"

enum
{
    FRAME_LIST_TITLE_TYPE_GROUP_NOTMAL          = FRAME_LIST_TITLE_TYPE_NONE + 6,
};

class CGroupNormalTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
#endif

public:
    explicit CGroupNormalTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CGroupNormalTitle(void);

    static bool IsGroupNormalTitle(CFrameListTitlePtr pTitle);

    void SetReadOnly(bool bReadOnly);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void Relayout();

private:
    // 搜索按钮
    CTitleButton m_btnSearch;
    // 添加联系人按钮
    CTitleButton m_btnAddContact;
    // 设置按钮
    CTitleButton m_btnSetting;
    // 按钮工具条
    CBtnBarWrapper m_btnBar;

    bool m_bReadOnly;

    QString testinfo;
};
typedef CGroupNormalTitle * CGroupNormalTitlePtr;

#endif
