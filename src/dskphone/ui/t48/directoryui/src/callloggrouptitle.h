
#ifndef __CALL_LOG_GROUP_TITLE_H__
#define __CALL_LOG_GROUP_TITLE_H__

#include "baseui/framelisttitle.h"
#include "baseui/titlebutton.h"

#include "directorycommon.h"

enum
{
    FRAME_LIST_TITLE_TYPE_CALL_LOG_GROUP            = FRAME_LIST_TITLE_TYPE_NONE + 9,
};

class CCallLogGroupTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
    QString m_strBtn;
#endif

public:
    explicit CCallLogGroupTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CCallLogGroupTitle(void);

    static bool IsCallLogGroupTitle(CFrameListTitlePtr pTitle);

    void SetBtnVisible(bool bVisible);

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    void SetDeleteState(bool bDeleteState);
#endif

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

private:
    CTitleButton m_btnSetting;

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    bool        m_bDeleteState;
#endif
};
typedef CCallLogGroupTitle * CCallLogGroupTitlePtr;

#endif
