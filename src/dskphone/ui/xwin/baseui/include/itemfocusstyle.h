#ifndef __ITEM_FOCUS_STYLE_H__
#define __ITEM_FOCUS_STYLE_H__

#include "xwindow/xViewHandler.h"
#include <xwindow/xlistviewdefine.h>
#include "xwindow/xlistviewstyle.h"

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER

enum
{
    ITEM_BG_PAINTER_TYPE_IMAGE = 2,
    ITEM_BG_PAINTER_TYPE_IMAGE_OPTION_MENU = 3,
    ITEM_BG_PAINTER_TYPE_IMAGE_DIAL_SEARCH = 3,
};

class CItemImageBGPainter : public xItemBGPainter
{
public:
    CItemImageBGPainter();
    virtual ~CItemImageBGPainter();
    static CItemImageBGPainter * GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter);

    static void Init();
    static void UnInit();

public:
    virtual yl::string GetPainterName()
    {
        return "CItemImageBGPainter";
    }
    virtual void DrawItemBG(xPainter & painter, xListViewStyle * pStyle, xListViewItem * pFocusItem,
                            const chRect & rcItem, int nItemIndex);

protected:
    virtual void LoadRes();
    virtual void ReleaseRes();

protected:
    xPixmap m_pmBG;
};

class COptionMenuItemBG : public CItemImageBGPainter
{
public:
    COptionMenuItemBG();
    virtual ~COptionMenuItemBG();
    static COptionMenuItemBG * GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter);

    static void Init();
    static void UnInit();

public:
    virtual yl::string GetPainterName()
    {
        return "COptionMenuItemBG";
    }

protected:
    virtual void LoadRes();

};

class CDialSearchItemBG : public CItemImageBGPainter
{
public:
    CDialSearchItemBG();
    virtual ~CDialSearchItemBG();
    static CDialSearchItemBG * GetItemImageBGPainter(xItemBGPainterPtr pItemBGPainter);

    static void Init();
    static void UnInit();

public:
    virtual yl::string GetPainterName()
    {
        return "CDialSearchItemBG";
    }

protected:
    virtual void LoadRes();

};

#endif

#endif
