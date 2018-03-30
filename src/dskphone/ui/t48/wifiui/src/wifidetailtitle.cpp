#include "wifi_inc.h"
#include "qtcommon/xmlhelper.h"

#include <QStylePainter>

namespace
{
#ifdef _T49
#define WIFI_DETAIL_TITLE_HEIGHT 117

#define WIFI_DETAIL_TITLE_BTN_COMMON_TOP 0
#define WIFI_DETAIL_TITLE_BTN_COMMON_WIDTH 122
#define WIFI_DETAIL_TITLE_BTN_COMMON_HEIGHT 96

#define WIFI_DETAIL_TITLE_ICON_COMMON_TOP 19
#define WIFI_DETAIL_TITLE_TEXT_COMMON_TOP 55
#else
#define WIFI_DETAIL_TITLE_HEIGHT 60

#define WIFI_DETAIL_TITLE_BTN_COMMON_TOP 0
#define WIFI_DETAIL_TITLE_BTN_COMMON_WIDTH 82
#define WIFI_DETAIL_TITLE_BTN_COMMON_HEIGHT 58

#define WIFI_DETAIL_TITLE_ICON_COMMON_TOP 8
#define WIFI_DETAIL_TITLE_TEXT_COMMON_TOP 36
#endif

#define WIFI_DETAIL_TITLE_BTN_SPACE 1
#define WIFI_DETAIL_TITLE_BTN_SEPARATE 1

#ifdef _T49
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_TOP 25
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_WIDTH 1
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_HEIGHT 46

#define WIFI_DETAIL_TITLE_FONT_SIZE_BTN_BAR 18
#else
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_TOP 15
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_WIDTH 1
#define WIFI_DETAIL_TREE_TITLE_SEPARATE_HEIGHT 30

#define WIFI_DETAIL_TITLE_FONT_SIZE_BTN_BAR 14
#endif
}

WifiDetailTitle::WifiDetailTitle(CFrameList * pFrameList/* = NULL*/,
                                 IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_WIFI_DETAIL_TITLE, pFrameList, pAction)
    , m_btnCon(this)
    , m_btnAdd(this)
    , m_btnBar(this, false)
    , m_nBtnCount(0)
    , m_bConnectOrNot(false)
{
    setGeometry(0, 0, 0, WIFI_DETAIL_TITLE_HEIGHT);

    qWidgetUtility::setFontStyle(this, WIFI_DETAIL_TITLE_FONT_SIZE_BTN_BAR);

    setObjectName("WifiDetailTitle");
}

WifiDetailTitle::~WifiDetailTitle(void)
{
}

bool WifiDetailTitle::IsWifiDetailTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_WIFI_DETAIL_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void WifiDetailTitle::SetBtnCount(int nCount)
{
    if (nCount > MAX_BTN_COUNT)
    {
        return;
    }

    m_btnBar.SetBtnCount(nCount);

    m_nBtnCount = 0;

    m_btnCon.SetRect(0, 0, 0, 0);
    m_btnAdd.SetRect(0, 0, 0, 0);

    m_btnCon.ResetDown();
    m_btnAdd.ResetDown();

    for (int i = 0; i < MAX_BTN_COUNT; ++i)
    {
        m_arrTitle[i] = "";
    }
}

void WifiDetailTitle::AddButton(int nAction, const QString & strTitle)
{
    if (m_nBtnCount >= MAX_BTN_COUNT)
    {
        return;
    }

    CTitleButton * pBtn = NULL;

    switch (nAction)
    {
    case WIFI_TITLE_ITEM_ACTION_CON:
        {
            pBtn = &m_btnCon;
        }
        break;
    case WIFI_TITLE_ITEM_ACTION_ADD:
        {
            pBtn = &m_btnAdd;
        }
        break;
    default:
        break;
    }

    if (NULL == pBtn)
    {
        return;
    }

    m_btnBar.SetBtn(m_nBtnCount, pBtn);
    m_arrTitle[m_nBtnCount] = strTitle;

    ++m_nBtnCount;
}

void WifiDetailTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void WifiDetailTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        return;
    }

    if (pBtn == &m_btnCon)
    {
        DoAction(WIFI_TITLE_ITEM_ACTION_CON);
    }
    else if (pBtn == &m_btnAdd)
    {
        DoAction(WIFI_TITLE_ITEM_ACTION_ADD);
    }
}

void WifiDetailTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    int nBtnCount = m_nBtnCount;

    if (nBtnCount > MAX_BTN_COUNT)
    {
        nBtnCount = MAX_BTN_COUNT;
    }

    for (int i = 0; i < nBtnCount; ++i)
    {
        CTitleButton * pBtn = (CTitleButton *)m_btnBar.GetBtn(i);

        if (NULL == pBtn)
        {
            continue;
        }

        // 画按钮
        const QString & strTitle = m_arrTitle[i];

        if (pBtn == &m_btnCon)
        {
#ifdef _T49
            if (pBtn->IsDown())
            {
                pBtn->PaintVButton(stylePainter, NULL,
                                   m_bConnectOrNot ? PIC_WIFI_NETWORK_DETAILS_DISCONNECT_CLICK : PIC_WIFI_NETWORK_DETAILS_CONNECT_CLICK
                                   , m_bConnectOrNot ? LANG_TRANSLATE(TRID_DISCONNECT) : LANG_TRANSLATE(TRID_CONNECT));
            }
            else
            {
                pBtn->PaintVButton(stylePainter, NULL,
                                   m_bConnectOrNot ? PIC_WIFI_NETWORK_DETAILS_DISCONNECT : PIC_WIFI_NETWORK_DETAILS_CONNECT
                                   , m_bConnectOrNot ? LANG_TRANSLATE(TRID_DISCONNECT) : LANG_TRANSLATE(TRID_CONNECT));
            }
#else
            pBtn->PaintVButton(stylePainter, NULL,
                               m_bConnectOrNot ? PIC_WIFI_NETWORK_DETAILS_DISCONNECT_CLICK : PIC_WIFI_NETWORK_DETAILS_CONNECT_CLICK
                               , m_bConnectOrNot ? LANG_TRANSLATE(TRID_DISCONNECT) : LANG_TRANSLATE(TRID_CONNECT));
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
#endif
        }
        else if (pBtn == &m_btnAdd)
        {
#ifdef _T49
            if (pBtn->IsDown())
            {
                pBtn->PaintVButton(stylePainter, NULL, PIC_WIFI_NETWORK_DETAILS_ADD_CLICK, strTitle);
            }
            else
            {
                pBtn->PaintVButton(stylePainter, NULL, PIC_WIFI_NETWORK_DETAILS_ADD, strTitle);
            }
#else
            pBtn->PaintVButton(stylePainter, NULL, PIC_WIFI_NETWORK_DETAILS_ADD, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
#endif
        }

        // 画分隔线
        if ((i + 1) != nBtnCount)
        {
            QRect rcBtn = pBtn->GetRect();
            QRect rcSeparate(rcBtn.right() + WIFI_DETAIL_TITLE_BTN_SEPARATE,
                             WIFI_DETAIL_TREE_TITLE_SEPARATE_TOP,
                             WIFI_DETAIL_TREE_TITLE_SEPARATE_WIDTH, WIFI_DETAIL_TREE_TITLE_SEPARATE_HEIGHT);

            QPixmap pmSeparate = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_SEPARATE);

            if (!pmSeparate.isNull())
            {
                stylePainter.drawPixmap(rcSeparate, pmSeparate);
            }
        }
    }
}

void WifiDetailTitle::Relayout()
{
    QRect rcTitle = rect();
    int nTotalWidth = WIFI_DETAIL_TITLE_BTN_COMMON_WIDTH * m_nBtnCount + WIFI_DETAIL_TITLE_BTN_SPACE *
                      (m_nBtnCount - 1);
    int nLeft = (rcTitle.right() - nTotalWidth) >> 1;

    int nBtnCount = m_nBtnCount;

    if (nBtnCount > MAX_BTN_COUNT)
    {
        nBtnCount = MAX_BTN_COUNT;
    }

    for (int i = 0; i < nBtnCount; ++i)
    {
        CTitleButton * pBtn = (CTitleButton *)m_btnBar.GetBtn(i);

        if (NULL == pBtn)
        {
            continue;
        }

        pBtn->SetRect(nLeft, WIFI_DETAIL_TITLE_BTN_COMMON_TOP,
                      WIFI_DETAIL_TITLE_BTN_COMMON_WIDTH, WIFI_DETAIL_TITLE_BTN_COMMON_HEIGHT);

        pBtn->SetIconTop(WIFI_DETAIL_TITLE_ICON_COMMON_TOP);
        pBtn->SetTextTop(WIFI_DETAIL_TITLE_TEXT_COMMON_TOP);

        nLeft += (WIFI_DETAIL_TITLE_BTN_COMMON_WIDTH + WIFI_DETAIL_TITLE_BTN_SPACE);
    }
}
// 设置连接与否
void WifiDetailTitle::SetConnectOrNot(bool bConnectOrNot)
{
    if (m_bConnectOrNot != bConnectOrNot)
    {
        m_bConnectOrNot = bConnectOrNot;
        update();
    }
}

#ifdef IF_ENABLE_TESTABILITY
QString WifiDetailTitle::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    AddBtnNode(nodeRoot, m_btnCon, BTN_CONNECT);
    AddBtnNode(nodeRoot, m_btnAdd, BTN_ADD);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif
