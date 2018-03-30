#ifndef __DLG_DIRECTORY_BASE_H__
#define __DLG_DIRECTORY_BASE_H__

#include "xwindow/xListView.h"
#include "uimanager/basedialog.h"
#include <diruilogic/include/diruilogicdefine.h>
#include "normaltitle.h"

class CDlgDirectoryBase : public xLinearLayout, public CBaseDialog
{
public:
    CDlgDirectoryBase();
    ~CDlgDirectoryBase();

public:
    virtual void Init();
    virtual void Uninit();

    virtual void UpdateWnd(bool bUIReturn = false);

    virtual void OnReturnToIdle();

public:
    xListView * GetListView() const;

    bool SetTitle(const yl::string & strTitle);
    virtual bool SetTitlePageInfo(const yl::string & strPageInfo);

    virtual void loadChildrenElements(xml_node & node);

    virtual void RefreshTitle();

    virtual void GetSoftkey(CArraySoftKeyBarData & vecSoftKey);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual bool onKeyPressedEvent(int nKeyCode);

#if IS_COLOR
    virtual xBrush GetBackGroundBrush();
#endif

protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    bool IsShowTitle();
    void UpdateTitleInfo();

protected:
    xListView * m_pListView;
    CNormalTitle m_titleNormal;
};

#endif
