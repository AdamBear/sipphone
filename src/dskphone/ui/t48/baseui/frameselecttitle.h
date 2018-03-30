#ifndef __FRAME_SELECT_TITLE_H__
#define __FRAME_SELECT_TITLE_H__

#include "baseui/framelisttitle.h"
#include "uikernel/buttonwrapper.h"

enum
{
    FRAME_LIST_TITLE_TYPE_SELECT_TITLE          = FRAME_LIST_TITLE_TYPE_NONE + 1,
};

class QMouseEvent;
class QPaintEvent;
class QStylePainter;

class CFrameSelectTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString m_strText;
    QRect m_rcText;
    virtual QString GetTestInfo();
#endif
public:
    explicit CFrameSelectTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CFrameSelectTitle(void);

    static bool IsSelectTitle(CFrameListTitlePtr pTitle);

public:
    void SetTotalAmount(int nTotalAmount, bool bRefresh = false);
    void SetSelectAmount(int nSelectAmount, bool bRefresh = false);

    void SetCheck(bool bCheck);

    bool IsCheck() const;

#if IF_FEATURE_GBBOOK
    void SetCheckable(bool bCheckable);
#endif

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintCheckBox(QStylePainter & stylePainter);

#if IF_FEATURE_GBBOOK
    virtual void Relayout();
#endif

protected:
    CButtonWrapper m_btnCheckBox;
    // 总列数
    int m_nTotalAmount;
    // 选中列数
    int m_nSelectAmount;
    // 复选框是否选择中
    bool m_bChecked;

#if IF_FEATURE_GBBOOK
    bool m_bShowChecked;
#endif
};
typedef CFrameSelectTitle * CFrameSelectTitlePtr;

#endif
