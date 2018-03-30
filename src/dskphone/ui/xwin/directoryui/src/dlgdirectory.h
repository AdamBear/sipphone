#ifndef __DLG_DIRECTORY_H__
#define __DLG_DIRECTORY_H__

#include "dlgdirectorybase.h"
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
#include "baseui/include/ctagtitle.h"
#endif

class CDlgDirectory : public CDlgDirectoryBase
{
public:
    CDlgDirectory();
    virtual ~CDlgDirectory();

public:
    static CBaseDialog * CreateInstance();

#if !IS_COLOR && IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    virtual bool SetTitlePageInfo(const yl::string & strPageInfo);
#endif

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    virtual void loadChildrenElements(xml_node & node);
    virtual void RefreshTitle();
    void ChangeTitleStatus(bool bShowTag);
protected:
    CTagTitle* m_pTagTitle;
#endif
};

#endif
