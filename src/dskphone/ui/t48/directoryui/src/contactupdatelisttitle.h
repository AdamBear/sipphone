
#ifndef __CONTACT_UPDATE_LIST_TITLE_H__
#define __CONTACT_UPDATE_LIST_TITLE_H__

#include "baseui/frametiptitle.h"
#include "baseui/titlebutton.h"

enum
{
    FRAME_LIST_TITLE_TYPE_CONTACT_UPDATE_TITLE  = FRAME_LIST_TITLE_TYPE_NONE + 8,
};

enum
{
    LIST_TITLE_ACTION_CONTACT_UPDATE        = LIST_TITLE_ACTION_BASE + 1,   // 联系人更新操作
    LIST_TITLE_ACTION_CONTACT_SORT          = LIST_TITLE_ACTION_BASE + 2,   // 联系人SORT操作
};

class CContactUpdateListTitle : public CFrameTipTitle
{
#ifdef IF_ENABLE_TESTABILITY
    Q_OBJECT
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif
public:
    explicit CContactUpdateListTitle(CFrameList * pFrameList = NULL,
                                     IFrameTitleAction * pAction = NULL);
    virtual ~CContactUpdateListTitle(void);

    static bool IsContactUpdateListTitle(CFrameListTitlePtr pTitle);

public:
    // 设置群组名称
    void SetGroupName(const QString & strGroupName);
    // 设置联系人总人数
    void SetContactCount(int nCount);

#if IF_FEATURE_METASWITCH_DIRECTORY
    void SetUpdatable(bool bUpdatable, bool bSortable = false);
#else
    void SetUpdatable(bool bUpdatable);
#endif
    void SetSortable(bool bSortable);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 重新布局
    virtual void Relayout();

protected:
    // 更新按钮
    QString m_strGroupName;
    CTitleButton m_btnUpdate;
    bool m_bUpdatable;
    //MTSW SORT按钮
    CTitleButton m_btnSort;
    bool m_bSortable;
};
typedef CContactUpdateListTitle * CContactUpdateListTitlePtr;

#endif
