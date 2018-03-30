#ifndef _DLG_WALLPAPER_H_
#define _DLG_WALLPAPER_H_

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_WALLPAPER
#include "dlgsettingui.h"
#include "settinguiwizardtitle.h"

class xImageView;
class CDlgWallpaper : public CDlgSettingUI
{
public:
    static CBaseDialog* CreateInstance();

public:
    CDlgWallpaper();
    virtual ~CDlgWallpaper();

public:
    virtual void Init();
    virtual void loadChildrenElements(xml_node& node);
    virtual void RefreshUI(bool bReLoadData = false);

protected:
    xImageView*                 m_pImgWallpaper;
};
#endif

#endif //_DLG_WALLPAPER_H_
