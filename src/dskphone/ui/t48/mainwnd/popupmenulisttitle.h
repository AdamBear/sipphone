#ifndef __POPUP_MENU_LIST_TITLE_H__
#define __POPUP_MENU_LIST_TITLE_H__

#include "baseui/framelisttitle.h"

enum
{
    FRAME_LIST_TITLE_TYPE_POPUP_MENU            = FRAME_LIST_TITLE_TYPE_NONE + 10,
};

class CPopupMenuListTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcText;
    QRect m_rcIcon;
#endif
public:
    explicit CPopupMenuListTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CPopupMenuListTitle(void);

    static bool IsPopupMenuTitle(CFrameListTitlePtr pTitle);

    void SetTitleText(const QString & strText);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

private:
    QString m_strText;
};
typedef CPopupMenuListTitle * CPopupMenuListTitlePtr;

#endif // __POPUP_MENU_LIST_TITLE_H__
