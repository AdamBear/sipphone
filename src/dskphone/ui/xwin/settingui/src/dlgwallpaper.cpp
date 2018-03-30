#include "dlgwallpaper.h"

#if IF_SUPPORT_WALLPAPER
#include "xwindow/xWindow.h"
#include "wallpaperdelegate.h"
#include "imagecache/include/modimagecache.h"
#include "baseui/include/baseuicommon.h"

namespace
{
#define  DLG_WALLPAPER_XML          "setting/dlgwallpaper.xml"
#define  PIC_WALLPAPER              "picWallpaper"
}

CBaseDialog* CDlgWallpaper::CreateInstance()
{
    CDlgWallpaper* pDlg = new CDlgWallpaper;
    if (NULL != pDlg)
    {
        pDlg->loadContent(DLG_WALLPAPER_XML);
    }
    return pDlg;
}

CDlgWallpaper::CDlgWallpaper()
    : m_pImgWallpaper(NULL)
{
}

CDlgWallpaper::~CDlgWallpaper()
{
    Image_ClearImage(IMG_CLASSIFY_WALLPAPER);
}

void CDlgWallpaper::loadChildrenElements(xml_node& node)
{
    CDlgSettingUI::loadChildrenElements(node);
    m_pImgWallpaper = static_cast<xImageView*>(getViewById(PIC_WALLPAPER));
}

void CDlgWallpaper::Init()
{

}

void CDlgWallpaper::RefreshUI(bool bReLoadData /*= false*/)
{
    CDlgSettingUI::RefreshUI(bReLoadData);

    CWallpaperDelegate* pDelegate = static_cast<CWallpaperDelegate*>(m_pSettingUIDelegate);
    if (NULL == pDelegate || NULL == m_pImgWallpaper || NULL == m_pListView)
    {
        return;
    }

    ST_DisPlayWallPaperData stWallData;
    xPixmap pix;

    if (pDelegate->GetWallpaperData(stWallData))
    {
        Image_GetImage(pix, stWallData.strCurPaper, IMG_CLASSIFY_WALLPAPER);
    }

#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
    m_pImgWallpaper->setPixmap(pix, true);
#else
    m_pImgWallpaper->setPixmap(pix);
#endif

}
#endif

