#include "inputbar.h"

namespace
{
const char * INPUTBAR_UI_LAYOUT_FILE = "imebar.xml";
}

const char * INPUTBAR_CHAR_ITEM_WIDTH = "charitemwidth";
const char * INPUTBAR_IME_ITEM_WIDTH = "imeitemwidth";
const char * INPUTBAR_SOFTBAR_HEIGHT = "softbarheight";
const char * INPUTBAR_BACKGROUND_COLOR_FOCUS = "focusbgcolor";
const char * INPUTBAR_BACKGROUND_COLOR_NORMAL = "normalbgcolor";
const char * INPUTBAR_TEXT_COLOR_FOCUS = "focustxtcolor";
const char * INPUTBAR_TEXT_COLOR_NORMAL = "normaltxtcolor";
const char * INPUTBAR_FRAME_COLOR = "framecolor";
const char * INPUTBAR_FRAME_VISIBLE = "showframe";

CInputBar::CInputBar()
    : xLinearLayout()
    , m_bShowFrame(true)
    , m_nCharItemWidth(0)
    , m_nImeItemWidth(0)
    , m_nSoftbarHeight(0)
{

}


CInputBar::~CInputBar()
{
}

void CInputBar::Init()
{
    loadContent(INPUTBAR_UI_LAYOUT_FILE);
    m_pWindow = m_pHostStation->createWindow(this, TML_USER + 2);
    hide();
}

void CInputBar::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);
    PopupAndroidInt(node, INPUTBAR_CHAR_ITEM_WIDTH, m_nCharItemWidth);
    PopupAndroidInt(node, INPUTBAR_IME_ITEM_WIDTH, m_nImeItemWidth);
    PopupAndroidInt(node, INPUTBAR_SOFTBAR_HEIGHT, m_nSoftbarHeight);
    PopupAndroidBool(node, INPUTBAR_FRAME_VISIBLE, m_bShowFrame);
    PopupAndroidColor(node, INPUTBAR_BACKGROUND_COLOR_FOCUS, m_colorFocusBG);
    PopupAndroidColor(node, INPUTBAR_BACKGROUND_COLOR_NORMAL, m_colorNormalBG);
    PopupAndroidColor(node, INPUTBAR_TEXT_COLOR_FOCUS, m_colorFocusText);
    PopupAndroidColor(node, INPUTBAR_TEXT_COLOR_NORMAL, m_colorNormalText);
    PopupAndroidColor(node, INPUTBAR_FRAME_COLOR, m_colorFrame);
}

void CInputBar::loadChildrenElements(xml_node& node)
{
    xLinearLayout::loadChildrenElements(node);
    for (xViewIterator it = child_head(); it.hasData(); ++it)
    {
#if IF_XWINDOW_RTTI_CHECK
        xTextView* item = dynamic_cast<xTextView*>(it.operator ->());
#else
        xTextView* item = static_cast<xTextView*>(it.operator ->());
#endif
        if (item != NULL)
        {
            m_vecTextItem.push_back(item);
        }
    }
}

void CInputBar::SetBarType(INPUTBAR_TYPE eType)
{
    m_eBarType = eType;
}

void CInputBar::SetBarInfo(int nIndex, int nCount, yl::wstring & strSequence)
{
    chRect rcBar = geometry();
    rcBar.ResizeWidth(m_nCharItemWidth * nCount);
    this->setGeometry(rcBar);

    TextItemVector::iterator iter;
    xTextView * pTextItem = NULL;
    int index = 0;

    for (iter = m_vecTextItem.begin();
            iter != m_vecTextItem.end() && index != BAR_ITEM_COUNT; ++index, ++iter)
    {

        pTextItem = *iter;
        if (pTextItem != NULL)
        {
            if (index >= nCount)
            {
                if (pTextItem->isVisible())
                {
                    pTextItem->hide(true);
                }
                continue;
            }
            else
            {
                pTextItem->show();
            }

            yl::wstring strLetter = strSequence.substr(index, 1);

            if (index == nIndex)
            {
                pTextItem->setBackground(m_colorFocusBG);
                pTextItem->SetTextColor(m_colorFocusText);
            }
            else
            {
                pTextItem->setBackground(m_colorNormalBG);
                pTextItem->SetTextColor(m_colorNormalText);
            }
            pTextItem->SetText(yl::to_utf8(strLetter));
        }
    }
}

void CInputBar::SetInputInfo(int nIndex, const VEC_STRING& vecImeShow)
{
    TextItemVector::iterator iter;
    VEC_STRING::const_iterator iterIme;
    xTextView * pTextItem = NULL;
    int index = 0;
    int nCount = vecImeShow.size();

    chRect rcBar = geometry();
    rcBar.ResizeWidth(m_nImeItemWidth * nCount);
    this->setGeometry(rcBar);

    for (iter = m_vecTextItem.begin(),
            iterIme = vecImeShow.begin();
            iter != m_vecTextItem.end()
            && index != BAR_ITEM_COUNT;
            ++index, ++iter, ++iterIme)
    {

        pTextItem = *iter;
        if (pTextItem != NULL)
        {
            if (index >= nCount || iterIme == vecImeShow.end())
            {
                if (pTextItem->isVisible())
                {
                    pTextItem->hide(true);
                }
                continue;
            }
            else
            {
                pTextItem->show();
            }

            yl::string strLetter = *iterIme;

            if (index == nIndex)
            {
                pTextItem->setBackground(m_colorFocusBG);
                pTextItem->SetTextColor(m_colorFocusText);
            }
            else
            {
                pTextItem->setBackground(m_colorNormalBG);
                pTextItem->SetTextColor(m_colorNormalText);
            }
            pTextItem->SetText(strLetter);
        }
    }
}

