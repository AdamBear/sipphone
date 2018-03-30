#ifndef __CTAG_TITLE_H__
#define __CTAG_TITLE_H__

#include "xwindow/xViewHandler.h"
#include "xwindow/xWindow.h"
#include "xwindow/xwindowcommon.h"
#include "ylvector.h"
#include "ylstring.h"
#include "ETLLib/ETLLib.hpp"
#include "xmlparser/pugixml.hpp"

class xColor;

typedef YLVector<xTextView*> VEC_TEXTVIEW;

struct ST_CUTLINE_INFO
{
    int m_nCutLineLeft;
    int m_nCutLineTop;
    int m_nCutWidth;
    int m_nCutLineHeight;
    int m_nCutLineSpace;
    xBrush m_bruCutlineBG;
};

///////////////////////////////////////////////////////////
enum
{
    VIEW_HANDLE_TYPE_TAG_TITLE = 0x00110000,
};

class CTagTitleHandle : public xViewHandler
{
public:
    CTagTitleHandle();
    virtual ~CTagTitleHandle();
public:
    static CTagTitleHandle* GetTagTitleHandle(xViewHandler* pHandle);
    virtual void loadAttributes(xml_node & node);
    const xColor& GetFontColor(bool bNormal) const;
    const xBrush& GetTextFontBG(bool bNormal) const;
    const ST_CUTLINE_INFO& GetCutLineInfo() const;

protected:
    void ResetCutLineInfo();

protected:
    xColor m_clrNormalText;
    xColor m_clrSelectText;
    xBrush m_bruNormalBG;
    xBrush m_bruSelectBG;
    ST_CUTLINE_INFO m_stCutLineInfo;
};

class CTagTitle : public xFrameLayout
{
public:
    CTagTitle();
    virtual ~CTagTitle();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual bool onPaintContent(xPainter& p);

public:
    void SetTagList(VEC_STRING& vecTitleList);
    void SetCurrenTagIndex(int nIndex);
    void SetCurrenTagIndex(const yl::string& strTag);
    yl::string GetCurrentTag();
    int GetCurrentTagIndex();
    int GetTagCount();

#if !IS_COLOR
    void SetPageIndexTip(const yl::string& strTip);
#endif

protected:
    void UpdateTitleAttribute();
    void UpdateTagText();
    void UpdateArrow();
#if IS_COLOR
    void UpdatePageInfo();
#endif

    int GetIndex(const yl::string& strTitle);
    yl::string GetString(int nIndex);
#if IS_COLOR
    void PaintCutLine(xPainter& p);
#endif

protected:
    xButton* m_pBtnLeftArrow;
    xButton* m_pBtnRightArrow;
    int m_nCurrentTagIndex;
    VEC_STRING m_vecTitle;

protected:
    VEC_TEXTVIEW m_vecTagTextView;
    xTextView* m_pTagTextPageInfo;
};

#endif // !__CTAG_TITLE_H__
