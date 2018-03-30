#ifndef _TITLE_BAR_TITLE_H
#define _TITLE_BAR_TITLE_H

#include <ylstring.h>
#include "yllist.h"

#include "statusbutton.h"
#include "statusbtnbar.h"

#include "idlescreen/include/idlescreen_def.h"

struct ICON_BUTTON
{
    int             m_nId;
    vecIconString   m_vecstrIcon;
    yl::string      m_strTip;
    bool            m_bFlash;
};

typedef YLList<ICON_BUTTON> ICON_BUTTON_LIST;


class CStatusIconsBar : public QWidget
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();

#endif
public:
    CStatusIconsBar(QWidget * parent);
    ~CStatusIconsBar();

    inline void SetIsAllowTitleStatusClick(bool bIsAllowTitleStatusClick)
    {
        m_bIsAllowTitleStatusClick = bIsAllowTitleStatusClick;
    }

    void SetGeometry(QRect rect);

    // 刷新状态栏的图标
    // 状态、标题
    void RefreshStatusIcon();
    void RefreshStatusIcon(YLList<IdleStatusItem> listStatusItem);
    void RefreshStatusIcon(ICON_BUTTON_LIST & StatusIconHintList);

    int GetMousePressBtnId(QMouseEvent * pEvent);
    int GetMouseReleaseBtnId(QMouseEvent * pEvent);

    void SetIconAlign(int nAlign);

protected:
    virtual void paintEvent(QPaintEvent * e);

private:
    void Init();

    //获取状态与图标的映射
    bool GetStatusShowInfo(const IdleStatusItem & stateData, vecIconString & vecstrIcon,
                           yl::string & strTip);

    bool JudgeIsFlash(int nIconID, yl::string & strTip);

private Q_SLOTS:
    void RepainStatusBar();

private:
    CStatusBtnBar   m_barStatus;

    bool            m_bIsAllowTitleStatusClick;
};

#endif // _TITLE_BAR_TITLE_H
