#include "itemfocusstyle.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewitem.h"
#include "baseui/include/editlistitem.h"
#include "xwindow/xWindowManager.h"
#include "t2xpicpath.h"
#include "imeedit/ximeedit.h"

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
///////////////////////////////////////////////////////////////////////////////
EXPORT_ITEM_BG_PAINTER_CLASS(CItemImageBGPainter);
CItemImageBGPainter::CItemImageBGPainter()
{
    SetType(ITEM_BG_PAINTER_TYPE_IMAGE);
}

CItemImageBGPainter::~CItemImageBGPainter()
{

}

CItemImageBGPainter * CItemImageBGPainter::GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter)
{
    if (NULL == pItemBGPainter || ITEM_BG_PAINTER_TYPE_IMAGE != pItemBGPainter->GetType())
    {
        return NULL;
    }

    return (CItemImageBGPainter *)pItemBGPainter;
}

void CItemImageBGPainter::Init()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("CItemImageBGPainter");
    CItemImageBGPainter * pImageBGPainter = CItemImageBGPainter::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->LoadRes();
    }
}

void CItemImageBGPainter::LoadRes()
{
    g_WorkStation.GetImageByPath(PIC_DEFAULT_FOCUS_ITEM_BG, m_pmBG);
}

void CItemImageBGPainter::UnInit()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("xItemImageBGPainter");
    CItemImageBGPainter * pImageBGPainter = CItemImageBGPainter::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->ReleaseRes();
    }
}

void CItemImageBGPainter::ReleaseRes()
{
    m_pmBG.setNull();
}

void CItemImageBGPainter::DrawItemBG(xPainter & painter, xListViewStyle * pStyle,
                                     xListViewItem * pFocusItem,
                                     const chRect & rcItem, int nItemIndex)
{
    if (NULL == pStyle || NULL == pFocusItem)
    {
        return;
    }

    painter.fillRect(rcItem, m_pmBG);
}

///////////////////////////////////////////////////////////////////////////////
EXPORT_ITEM_BG_PAINTER_CLASS(COptionMenuItemBG);
COptionMenuItemBG::COptionMenuItemBG()
{
    SetType(ITEM_BG_PAINTER_TYPE_IMAGE_OPTION_MENU);
}

COptionMenuItemBG::~COptionMenuItemBG()
{

}

COptionMenuItemBG * COptionMenuItemBG::GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter)
{
    if (NULL == pItemBGPainter || ITEM_BG_PAINTER_TYPE_IMAGE_OPTION_MENU != pItemBGPainter->GetType())
    {
        return NULL;
    }

    return (COptionMenuItemBG *)pItemBGPainter;
}

void COptionMenuItemBG::Init()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("COptionMenuItemBG");
    COptionMenuItemBG * pImageBGPainter = COptionMenuItemBG::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->LoadRes();
    }
}

void COptionMenuItemBG::LoadRes()
{
    g_WorkStation.GetImageByPath(PIC_DEFAULT_FOCUS_OPTION_ITEM_BG, m_pmBG);
}

void COptionMenuItemBG::UnInit()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("xItemImageBGPainter");
    COptionMenuItemBG * pImageBGPainter = COptionMenuItemBG::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->ReleaseRes();
    }
}

///////////////////////////////////////////////////////////////////////////////
EXPORT_ITEM_BG_PAINTER_CLASS(CDialSearchItemBG);
CDialSearchItemBG::CDialSearchItemBG()
{
    SetType(ITEM_BG_PAINTER_TYPE_IMAGE_DIAL_SEARCH);
}

CDialSearchItemBG::~CDialSearchItemBG()
{

}

CDialSearchItemBG * CDialSearchItemBG::GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter)
{
    if (NULL == pItemBGPainter || ITEM_BG_PAINTER_TYPE_IMAGE_DIAL_SEARCH != pItemBGPainter->GetType())
    {
        return NULL;
    }

    return (CDialSearchItemBG *)pItemBGPainter;
}

void CDialSearchItemBG::Init()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("CDialSearchItemBG");
    CDialSearchItemBG * pImageBGPainter = CDialSearchItemBG::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->LoadRes();
    }
}

void CDialSearchItemBG::LoadRes()
{
    g_WorkStation.GetImageByPath(PIC_TALK_DIAL_SEARCH_FOCUS_ITEM_BG, m_pmBG);
}

void CDialSearchItemBG::UnInit()
{
    xItemBGPainter * pBGPainter = xItemBGPainter::getObjectByName("xItemImageBGPainter");
    CDialSearchItemBG * pImageBGPainter = CDialSearchItemBG::GetItemImageBGPainter(pBGPainter);

    if (NULL != pImageBGPainter)
    {
        pImageBGPainter->ReleaseRes();
    }
}

#endif
