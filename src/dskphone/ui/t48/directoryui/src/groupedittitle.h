
#ifndef __GROUP_EDIT_TITLE_H__
#define __GROUP_EDIT_TITLE_H__

#include "baseui/framelisttitle.h"
#include "baseui/titlebutton.h"

enum
{
    FRAME_LIST_TITLE_TYPE_GROUP_EDIT            = FRAME_LIST_TITLE_TYPE_NONE + 7,
};

class CGroupEditTitle : public CFrameListTitle
{
    Q_OBJECT

public:
    explicit CGroupEditTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CGroupEditTitle(void);

    static CGroupEditTitle * GetGroupEditTitle(CFrameListTitlePtr pTitle);

    void SetBtnVisible(bool bVisible);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

private:
    CTitleButton m_btnAddGroup;
};
typedef CGroupEditTitle * CGroupEditTitlePtr;

#endif
