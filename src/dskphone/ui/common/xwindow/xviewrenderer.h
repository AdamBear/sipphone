#ifndef __X_VIEW_RENDERER_H__
#define __X_VIEW_RENDERER_H__

#include "ETLLib/ETLLib.hpp"
#include <ylvector.h>
#include "xGraphic.h"
#include "xwindowcommon.h"
#include <xmlparser/xmlparser.hpp>

#if IF_VIEW_RENDER
enum
{
    RENDER_ELEMENT_TYPE_NONE        = 0,
    RENDER_ELEMENT_TYPE_PIXMAP      = 1,
    RENDER_ELEMENT_TYPE_TEXT        = 2,
    RENDER_ELEMENT_TYPE_BLOCK       = 3,
    RENDER_ELEMENT_TYPE_FRAME       = 4,
    RENDER_ELEMENT_TYPE_LINE        = 5,
};

class xView;
class xRenderElement
{
public:
    explicit xRenderElement(int nType);
    virtual ~xRenderElement();

public:
    void SetRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetRect(const chRect& rcElement);

    void SetVisible(bool bVisible);

    int GetType() const;
    const chRect& GetRect() const;
    bool IsVisible() const;
    const yl::string& GetId() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView)        = 0;

protected:
    int m_nType;
    chRect m_rcElement;
    bool m_bVisible;
    yl::string m_strId;
};
typedef xRenderElement* xRenderElementPtr;
typedef YLVector<xRenderElementPtr> VEC_RENDER_ELEMENT;

class xPixmapElement : public xRenderElement
{
public:
    xPixmapElement();
    virtual ~xPixmapElement();

    static xPixmapElement* GetPixmapElement(xRenderElementPtr pElement);

public:
    void SetAlign(int nAlign);
    void SetPixmap(const yl::string& strFile);

    int GetAlign() const;
    bool GetPixmapSize(int& nWidth, int& nHeight);
    const yl::string& GetPixmapFile() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView);

    enum
    {
        PIXMAP_ALIGN_SCALE_FILL         = 1,            // 拉伸
        PIXMAP_ALIGN_FIX_COPY           = 2,            // 拷贝
        PIXMAP_ALIGN_FIX_CENTER         = 3,            // 居中
    };

protected:
    void RefreshPixmap();

protected:
    int m_nAlign;
    xPixmap m_pmPic;
    yl::string m_strFile;
};
typedef xPixmapElement* xPixmapElementPtr;

class xTextElement : public xRenderElement, public xTextTranslate
{
public:
    xTextElement();
    virtual ~xTextElement();

    static xTextElement* GetTextElement(xRenderElementPtr pElement);

public:
    void SetAlign(int nAlgin);
    void SetPointSize(int nPointSize);
    void SetBold(bool bBold);
    void SetColor(const xColor& clrText);
    void SetFamily(const yl::string& strFamily);
    void SetText(const yl::string& strText);

    int GetAlign() const;
    int GetPointSize() const;
    const xColor& GetColor() const;
    const yl::string& GetFamily() const;
    const yl::string& GetText() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView);
    virtual void SetTranText(const yl::string& strText);

protected:
    int m_nAlign;
    int m_nPointSize;
    bool m_bBold;
    bool m_bContainRTLChar;
    xColor m_clrText;
    yl::string m_strFamily;
    yl::string m_strText;
};
typedef xTextElement* xTextElementPtr;

class xRectangleElement : public xRenderElement
{
public:
    xRectangleElement();
    virtual ~xRectangleElement();

    static xRectangleElement* GetRectangleElement(xRenderElementPtr pElement);

public:
    void SetColor(xColor& clrRect);

    const xColor& GetColor() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView);

protected:
    xColor m_clrRect;
};
typedef xRectangleElement* xRectangleElementPtr;

class xFrameElement : public xRenderElement
{
public:
    xFrameElement();
    virtual ~xFrameElement();

    static xFrameElement* GetFrameElement(xRenderElementPtr pElement);

public:
    void SetLineStyle(int nStyle);
    void SetColor(xColor& clrFrame);

    int GetLineStyle() const;
    const xColor& GetColor() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView);

protected:
    int m_nLineStyle;
    xColor m_clrFrame;
};
typedef xFrameElement* xFrameElementPtr;

class xLineElement : public xRenderElement
{
public:
    xLineElement();
    virtual ~xLineElement();

    static xLineElement* GetLineElement(xRenderElementPtr pElement);

public:
    void SetStyle(int nStyle);
    void SetColor(xColor& clrLine);

    int GetStyle() const;
    const xColor& GetColor() const;

    virtual void LoadElement(xml_node& node);
    virtual void Render(xPainter& painter, xView* pView);

protected:
    int m_nStyle;
    xColor m_clrLine;
};
typedef xLineElement* xLineElementPtr;

class xViewRenderer
{
public:
    xViewRenderer();
    virtual ~xViewRenderer();

public:
    void LoadContent(const yl::string& strFileName);
    void LoadElement(xml_node& node);

    void Render(xPainter& painter, xView* pView);

    xPixmapElementPtr GetPixmapById(const char* pszId);
    xTextElementPtr GetTextById(const char* pszId);
    xRectangleElementPtr GetRectangleById(const char* pszId);
    xFrameElementPtr GetFrameById(const char* pszId);
    xLineElementPtr GetLineById(const char* pszId);
    xRenderElementPtr GetElementById(const char* pszId);
    xRenderElementPtr GetElementByIndex(int nIndex);

protected:
    void ParseElement(xml_node& node);
    void ClearElement();

protected:
    VEC_RENDER_ELEMENT m_vecElement;
    yl::string m_strId;
};
typedef xViewRenderer* xViewRendererPtr;
#endif

#endif
