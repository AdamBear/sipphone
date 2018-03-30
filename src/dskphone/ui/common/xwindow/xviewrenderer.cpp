#include "xviewrenderer.h"
#include "xWindow.h"
#include "xWindowManager.h"
#if IF_DEBUG_PAINTER
#include "dsklog/log.h"
#endif


#if IF_VIEW_RENDER
xRenderElement::xRenderElement(int nType)
    : m_nType(nType)
    , m_bVisible(true)
{

}

xRenderElement::~xRenderElement()
{

}

void xRenderElement::SetRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcElement.SetRect(nLeft, nTop, nWidth, nHeight);
}

void xRenderElement::SetRect(const chRect& rcElement)
{
    m_rcElement = rcElement;
}

void xRenderElement::SetVisible(bool bVisible)
{
    m_bVisible = bVisible;
}

int xRenderElement::GetType() const
{
    return m_nType;
}

const chRect& xRenderElement::GetRect() const
{
    return m_rcElement;
}

bool xRenderElement::IsVisible() const
{
    return m_bVisible;
}

const yl::string& xRenderElement::GetId() const
{
    return m_strId;
}

void xRenderElement::LoadElement(xml_node& node)
{
    PopupAndroidString(node, XWINDOW_NAME_ID, m_strId);
    PopupAndroidRect(node, XWINDOW_NAME_LOGIC_LTWH, m_rcElement);
    PopupAndroidBool(node, XWINDOW_NAME_VISIBLE, m_bVisible);
}

xPixmapElement::xPixmapElement()
    : xRenderElement(RENDER_ELEMENT_TYPE_PIXMAP)
    , m_nAlign(PIXMAP_ALIGN_SCALE_FILL)
{

}

xPixmapElement::~xPixmapElement()
{

}

xPixmapElement* xPixmapElement::GetPixmapElement(xRenderElementPtr pElement)
{
    if (NULL == pElement || RENDER_ELEMENT_TYPE_PIXMAP != pElement->GetType())
    {
        return NULL;
    }

    return (xPixmapElementPtr)pElement;
}

void xPixmapElement::SetAlign(int nAlign)
{
    m_nAlign = nAlign;
}

void xPixmapElement::SetPixmap(const yl::string& strFile)
{
    if (strFile == m_strFile)
    {
        return;
    }

    m_strFile = strFile;

    RefreshPixmap();
}

void xPixmapElement::RefreshPixmap()
{
    m_pmPic.setNull();

    if (!m_strFile.empty())
    {
        g_WorkStation.GetImageByPath(m_strFile, m_pmPic);
    }
}

int xPixmapElement::GetAlign() const
{
    return m_nAlign;
}

bool xPixmapElement::GetPixmapSize(int& nWidth, int& nHeight)
{
    if (m_pmPic.isNull())
    {
        return false;
    }

    nWidth = m_pmPic.width();
    nHeight = m_pmPic.height();

    return true;
}

const yl::string& xPixmapElement::GetPixmapFile() const
{
    return m_strFile;
}

void xPixmapElement::LoadElement(xml_node& node)
{
    xRenderElement::LoadElement(node);

    PopupAndroidInt(node, XWINDOW_NAME_ALIGN, m_nAlign);
    PopupAndroidString(node, XWINDOW_NAME_FILE, m_strFile);

    RefreshPixmap();
}

void xPixmapElement::Render(xPainter& painter, xView* pView)
{
    if (!m_bVisible || NULL == pView || m_strFile.empty())
    {
        return;
    }

    if (m_pmPic.isNull())
    {
        return;
    }

#if IF_DEBUG_PAINTER
    XWINDOW_INFO("Render xPixmapElement Geometry:%d %d %d %d Align:%d",
                 m_rcElement.left, m_rcElement.top, m_rcElement.right,
                 m_rcElement.bottom, m_nAlign);
#endif

    if (PIXMAP_ALIGN_FIX_COPY == m_nAlign)
    {
        chRect rcPic(0, 0, m_pmPic.width(), m_pmPic.height());
        chRect rcDst = m_rcElement;

        rcDst.OffsetRect(-rcDst.left, -rcDst.top);
        rcPic = rcPic & rcDst;
        rcDst = rcPic;
        rcDst.OffsetRect(m_rcElement.left, m_rcElement.top);

        painter.drawItemPixmapEx(rcDst, m_pmPic, rcPic);
    }
    else if (PIXMAP_ALIGN_FIX_CENTER == m_nAlign)
    {
        chRect rcPic(0, 0, m_pmPic.width(), m_pmPic.height());
        chRect rcDst = m_rcElement;
        int nLeft = (rcDst.Width() - rcPic.Width()) >> 1;
        int nTop = (rcDst.Height() - rcPic.Height()) >> 1;

        if (nLeft < 0)
        {
            rcPic.InflateRect(nLeft, 0, nLeft, 0);
        }
        else if (nLeft > 0)
        {
            rcDst.DeflateRect(nLeft, 0, nLeft, 0);
        }

        if (nTop < 0)
        {
            rcPic.InflateRect(0, nTop, 0, nTop);
        }
        else if (nTop > 0)
        {
            rcDst.DeflateRect(0, nTop, 0, nTop);
        }

        painter.drawItemPixmapEx(rcDst, m_pmPic, rcPic);
    }
    else
    {
        painter.drawItemPixmap(m_rcElement, m_pmPic);
    }
}

xTextElement::xTextElement()
    : xRenderElement(RENDER_ELEMENT_TYPE_TEXT)
    , m_nPointSize(20)
    , m_nAlign(gravity_left | gravity_top)
    , m_bBold(false)
    , m_bContainRTLChar(false)
{

}

xTextElement::~xTextElement()
{

}

xTextElement* xTextElement::GetTextElement(xRenderElementPtr pElement)
{
    if (NULL == pElement || RENDER_ELEMENT_TYPE_TEXT != pElement->GetType())
    {
        return NULL;
    }

    return (xTextElementPtr)pElement;
}

void xTextElement::SetAlign(int nAlgin)
{
    m_nAlign = nAlgin;
}

void xTextElement::SetPointSize(int nPointSize)
{
    m_nPointSize = nPointSize;
}

void xTextElement::SetBold(bool bBold)
{
    m_bBold = bBold;
}

void xTextElement::SetColor(const xColor& clrText)
{
    m_clrText = clrText;
}

void xTextElement::SetFamily(const yl::string& strFamily)
{
    m_strFamily = strFamily;
}

void xTextElement::SetText(const yl::string& strText)
{
    if (!g_WorkStation.SetupTextTran(this, strText))
    {
        m_strText = strText;
        m_bContainRTLChar = g_WorkStation.IsRTLText(m_strText);
    }
}

int xTextElement::GetAlign() const
{
    return m_nAlign;
}

int xTextElement::GetPointSize() const
{
    return m_nPointSize;
}

const xColor& xTextElement::GetColor() const
{
    return m_clrText;
}

const yl::string& xTextElement::GetFamily() const
{
    return m_strFamily;
}

const yl::string& xTextElement::GetText() const
{
    return m_strText;
}

void xTextElement::LoadElement(xml_node& node)
{
    xRenderElement::LoadElement(node);

    PopupAndroidGravity(node, XWINDOW_NAME_ALIGN, m_nAlign);
    PopupAndroidInt(node, XWINDOW_NAME_SIZE, m_nPointSize);
    PopupAndroidBool(node, XWINDOW_NAME_TEXT_BOLD, m_bBold);
    PopupAndroidColor(node, XWINDOW_NAME_COLOR, m_clrText);
    PopupAndroidString(node, XWINDOW_NAME_FAMILY, m_strFamily);
    PopupAndroidString(node, XWINDOW_NAME_TEXT, m_strText);

    if (!m_strText.empty())
    {
        xWorkStation& currentStation = g_WorkStation;
        yl::string strText = currentStation.Tran(m_strText);

        if (strText != m_strText)
        {
            currentStation.SetupTextTran(this, m_strText);
        }
        else
        {
            m_bContainRTLChar = currentStation.IsRTLText(m_strText);
        }
    }
}

void xTextElement::Render(xPainter& painter, xView* pView)
{
    if (!m_bVisible || NULL == pView || m_strText.empty())
    {
        return;
    }

    xFont font;

    font.setPointSize(m_nPointSize);
    font.setBold(m_bBold);

    if (!m_strFamily.empty())
    {
        font.setFamily(m_strFamily);
    }

#if IF_DEBUG_PAINTER
    XWINDOW_INFO("Render xTextElement Geometry:%d %d %d %d Text:%s Align:%d",
                 m_rcElement.left, m_rcElement.top, m_rcElement.right,
                 m_rcElement.bottom, m_strText.c_str(), m_nAlign);
#endif

    if (m_bContainRTLChar)
    {
        yl::string strShowText;

        if (g_WorkStation.GetRTLShowText(m_strText, strShowText))
        {
            painter.drawText(m_rcElement, strShowText.c_str(), m_nAlign, font, m_clrText);

            return;
        }
    }

    painter.drawText(m_rcElement, m_strText.c_str(), m_nAlign, font, m_clrText);
}

void xTextElement::SetTranText(const yl::string& strText)
{
    m_strText = strText;
    m_bContainRTLChar = g_WorkStation.IsRTLText(m_strText);
}

xRectangleElement::xRectangleElement()
    : xRenderElement(RENDER_ELEMENT_TYPE_BLOCK)
{

}

xRectangleElement::~xRectangleElement()
{

}

xRectangleElement* xRectangleElement::GetRectangleElement(xRenderElementPtr pElement)
{
    if (NULL == pElement || RENDER_ELEMENT_TYPE_BLOCK != pElement->GetType())
    {
        return NULL;
    }

    return (xRectangleElementPtr)pElement;
}

void xRectangleElement::SetColor(xColor& clrRect)
{
    m_clrRect = clrRect;
}

const xColor& xRectangleElement::GetColor() const
{
    return m_clrRect;
}

void xRectangleElement::LoadElement(xml_node& node)
{
    xRenderElement::LoadElement(node);

    PopupAndroidColor(node, XWINDOW_NAME_COLOR, m_clrRect);
}

void xRectangleElement::Render(xPainter& painter, xView* pView)
{
    if (!m_bVisible || NULL == pView)
    {
        return;
    }

    painter.fillRect(m_rcElement, m_clrRect);
}

xFrameElement::xFrameElement()
    : xRenderElement(RENDER_ELEMENT_TYPE_FRAME)
    , m_nLineStyle(LINE_TYPE_SOLID)
{

}

xFrameElement::~xFrameElement()
{

}

xFrameElement* xFrameElement::GetFrameElement(xRenderElementPtr pElement)
{
    if (NULL == pElement || RENDER_ELEMENT_TYPE_FRAME != pElement->GetType())
    {
        return NULL;
    }

    return (xFrameElementPtr)pElement;
}

void xFrameElement::SetLineStyle(int nStyle)
{
    m_nLineStyle = nStyle;
}

void xFrameElement::SetColor(xColor& clrFrame)
{
    m_clrFrame = clrFrame;
}

int xFrameElement::GetLineStyle() const
{
    return m_nLineStyle;
}

const xColor& xFrameElement::GetColor() const
{
    return m_clrFrame;
}

void xFrameElement::LoadElement(xml_node& node)
{
    xRenderElement::LoadElement(node);

    PopupAndroidInt(node, XWINDOW_NAME_STYLE, m_nLineStyle);
    PopupAndroidColor(node, XWINDOW_NAME_COLOR, m_clrFrame);
}

void xFrameElement::Render(xPainter& painter, xView* pView)
{
    if (!m_bVisible || NULL == pView)
    {
        return;
    }

    painter.setLineType(m_nLineStyle);
    painter.drawRect(m_rcElement, m_clrFrame);
}

xLineElement::xLineElement()
    : xRenderElement(RENDER_ELEMENT_TYPE_LINE)
    , m_nStyle(LINE_TYPE_SOLID)
{

}

xLineElement::~xLineElement()
{

}

xLineElement* xLineElement::GetLineElement(xRenderElementPtr pElement)
{
    if (NULL == pElement || RENDER_ELEMENT_TYPE_LINE != pElement->GetType())
    {
        return NULL;
    }

    return (xLineElementPtr)pElement;
}

void xLineElement::SetStyle(int nStyle)
{
    m_nStyle = nStyle;
}

void xLineElement::SetColor(xColor& clrLine)
{
    m_clrLine = clrLine;
}

int xLineElement::GetStyle() const
{
    return m_nStyle;
}

const xColor& xLineElement::GetColor() const
{
    return m_clrLine;
}

void xLineElement::LoadElement(xml_node& node)
{
    xRenderElement::LoadElement(node);

    PopupAndroidInt(node, XWINDOW_NAME_STYLE, m_nStyle);
    PopupAndroidColor(node, XWINDOW_NAME_COLOR, m_clrLine);
}

void xLineElement::Render(xPainter& painter, xView* pView)
{
    if (!m_bVisible || NULL == pView)
    {
        return;
    }

    if (m_rcElement.left == m_rcElement.right)
    {
        painter.drawLineY(m_rcElement.left, m_rcElement.top, m_rcElement.bottom, m_clrLine);
    }
    else if (m_rcElement.top == m_rcElement.bottom)
    {
        painter.drawLineX(m_rcElement.top, m_rcElement.left, m_rcElement.right, m_clrLine);
    }
}

xViewRenderer::xViewRenderer()
{
}

xViewRenderer::~xViewRenderer()
{
    ClearElement();
}

void xViewRenderer::LoadContent(const yl::string& strFileName)
{
    xWorkStation& currentStation = g_WorkStation;

    xml_document doc;

    if (!currentStation.LoadLayout(strFileName, doc))
    {
        return;
    }

    xml_node nodeRoot = doc.document_element();

    LoadElement(nodeRoot);
}

void xViewRenderer::LoadElement(xml_node& node)
{
    if (node.empty())
    {
        return;
    }

    ClearElement();

    xml_node nodeChild = node.first_child();

    while (!nodeChild.empty())
    {
        ParseElement(nodeChild);

        nodeChild = nodeChild.next_sibling();
    }

    PopupAndroidString(node, XWINDOW_NAME_ID, m_strId);
}

void xViewRenderer::ParseElement(xml_node& node)
{
    if (node.empty())
    {
        return;
    }

    xRenderElementPtr pElement = NULL;
    yl::string strName = node.name();

    if (strName == "Pixmap")
    {
        pElement = new xPixmapElement;
    }
    else if (strName == "Text")
    {
        pElement = new xTextElement;
    }
    else if (strName == "Rect")
    {
        pElement = new xRectangleElement;
    }
    else if (strName == "Frame")
    {
        pElement = new xFrameElement;
    }
    else if (strName == "Line")
    {
        pElement = new xLineElement;
    }
    else
    {
        return;
    }

    if (NULL == pElement)
    {
        return;
    }

    pElement->LoadElement(node);
    m_vecElement.push_back(pElement);
}

void xViewRenderer::Render(xPainter& painter, xView* pView)
{
    VEC_RENDER_ELEMENT::iterator iter = m_vecElement.begin();

    for (; iter != m_vecElement.end(); ++iter)
    {
        xRenderElementPtr pElement = *iter;

        if (NULL != pElement)
        {
            pElement->Render(painter, pView);
        }
    }
}

xPixmapElementPtr xViewRenderer::GetPixmapById(const char* pszId)
{
    xRenderElementPtr pRender = GetElementById(pszId);

    return xPixmapElement::GetPixmapElement(pRender);
}

xTextElementPtr xViewRenderer::GetTextById(const char* pszId)
{
    xRenderElementPtr pRender = GetElementById(pszId);

    return xTextElement::GetTextElement(pRender);
}

xRectangleElementPtr xViewRenderer::GetRectangleById(const char* pszId)
{
    xRenderElementPtr pRender = GetElementById(pszId);

    return xRectangleElement::GetRectangleElement(pRender);
}

xFrameElementPtr xViewRenderer::GetFrameById(const char* pszId)
{
    xRenderElementPtr pRender = GetElementById(pszId);

    return xFrameElement::GetFrameElement(pRender);
}

xLineElementPtr xViewRenderer::GetLineById(const char* pszId)
{
    xRenderElementPtr pRender = GetElementById(pszId);

    return xLineElement::GetLineElement(pRender);
}

xRenderElementPtr xViewRenderer::GetElementById(const char* pszId)
{
    if (NULL == pszId)
    {
        return NULL;
    }

    VEC_RENDER_ELEMENT::iterator iter = m_vecElement.begin();

    for (; iter != m_vecElement.end(); ++iter)
    {
        xRenderElementPtr pElement = *iter;

        if (NULL != pElement && pElement->GetId() == pszId)
        {
            return pElement;
        }
    }

    return NULL;
}

xRenderElementPtr xViewRenderer::GetElementByIndex(int nIndex)
{
    int nSize = m_vecElement.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return NULL;
    }

    return m_vecElement[nIndex];
}

void xViewRenderer::ClearElement()
{
    VEC_RENDER_ELEMENT::iterator iter = m_vecElement.begin();

    for (; iter != m_vecElement.end(); ++iter)
    {
        xRenderElementPtr pElement = *iter;

        if (NULL != pElement)
        {
            delete pElement;
        }
    }

    m_vecElement.clear();
}
#endif
