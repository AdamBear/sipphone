#include <QtGui>
#include "remotelisttitle.h"
#include "uikernel/languagehelper.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"

#include "directoryui/src/directorylistaction.h"
#include "directoryui/include/directorycommon.h"

#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{

#define CONTACT_DETAIL_TITLE_BTN_COMMON_TOP 0
#define CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH 82
#define CONTACT_DETAIL_TITLE_BTN_COMMON_HEIGHT 54

#define CONTACT_DETAIL_TITLE_ICON_COMMON_TOP 4
#define CONTACT_DETAIL_TITLE_TEXT_COMMON_TOP 28

#define CONTACT_DETAIL_TITLE_BTN_RIGHT 0
#define CONTACT_DETAIL_TITLE_BTN_SPACE 1
#define CONTACT_DETAIL_TITLE_BTN_SEPARATE 1

#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_TOP 21
#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_WIDTH 1
#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_HEIGHT 30

}

CRemoteListTitle::CRemoteListTitle(CFrameList * pFrameList/* = NULL*/,
                                   IFrameTitleAction * pAction/* = NULL*/)
    : CFrameTipTitle(pFrameList, pAction)
    , m_btnPrePage(this)
    , m_btnNextPage(this)
    , m_btnBar(this, false)
    , m_nBtnCount(0)
{
    setObjectName("CRemoteListTitle");
    m_nType = FRAME_LIST_TITLE_TYPE_REMOTE_TITLE;
}

CRemoteListTitle::~CRemoteListTitle(void)
{
}

CRemoteListTitle * CRemoteListTitle::GetRemoteListTitle(CFrameListTitlePtr pTitle)
{
    if (NULL == pTitle || FRAME_LIST_TITLE_TYPE_REMOTE_TITLE != pTitle->GetType())
    {
        return NULL;
    }

    return (CRemoteListTitle *)pTitle;
}

bool CRemoteListTitle::IsRemoteListTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_REMOTE_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CRemoteListTitle::SetRemoteTip(yl::string strTitleTip, int nAmount)
{
    if (strTitleTip.empty() && nAmount == 0)
    {
        return;
    }

    QString strTip = LANG_TRANSLATE(strTitleTip.c_str());

    strTip += QString(": %1").arg(nAmount);

    SetTip(strTip);
}


void CRemoteListTitle::SetBtnCount(int nCount)
{
    m_btnBar.SetBtnCount(nCount);

    m_nBtnCount = 0;

    m_btnPrePage.SetRect(0, 0, 0, 0);
    m_btnNextPage.SetRect(0, 0, 0, 0);

    m_btnPrePage.ResetDown();
    m_btnNextPage.ResetDown();

    for (int i = 0; i < MAX_BTN_COUNT; ++i)
    {
        m_arrTitle[i] = "";
    }
}

void CRemoteListTitle::AddButton(int nAction, const QString & strTitle)
{
    if (m_nBtnCount >= MAX_BTN_COUNT)
    {
        return;
    }

    CTitleButton * pBtn = NULL;

    switch (nAction)
    {
    case LIST_TITLE_ACTION_PRE_PAGE:
        {
            pBtn = &m_btnPrePage;
        }
        break;
    case LIST_TITLE_ACTION_NEXT_PAGE:
        {
            pBtn = &m_btnNextPage;
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

void CRemoteListTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void CRemoteListTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        return;
    }

    if (pBtn == &m_btnPrePage)
    {
        DoAction(LIST_TITLE_ACTION_PRE_PAGE);
    }
    else if (pBtn == &m_btnNextPage)
    {
        DoAction(LIST_TITLE_ACTION_NEXT_PAGE);
    }
}

void CRemoteListTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintTip(stylePainter);

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

        if (pBtn == &m_btnPrePage)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_REMOTE_BTN_PRE_PAGE, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
        else if (pBtn == &m_btnNextPage)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_REMOTE_BTN_NEXT_PAGE, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }

        // 画分隔线
        if ((i + 1) != nBtnCount)
        {
            QRect rcBtn = pBtn->GetRect();
            QRect rcSeparate(rcBtn.right() + CONTACT_DETAIL_TITLE_BTN_SEPARATE,
                             CONTACT_DETAIL_TREE_TITLE_SEPARATE_TOP,
                             CONTACT_DETAIL_TREE_TITLE_SEPARATE_WIDTH, CONTACT_DETAIL_TREE_TITLE_SEPARATE_HEIGHT);

            QPixmap pmSeparate = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_SEPARATE);

            if (!pmSeparate.isNull())
            {
                stylePainter.drawPixmap(rcSeparate, pmSeparate);
            }
        }
    }
}

void CRemoteListTitle::Relayout()
{
    QRect rcTitle = rect();
    int nTotalWidth = CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH * m_nBtnCount +
                      CONTACT_DETAIL_TITLE_BTN_SPACE * (m_nBtnCount - 1);
    int nLeft =  rcTitle.right() - CONTACT_DETAIL_TITLE_BTN_RIGHT - nTotalWidth;

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

        pBtn->SetRect(nLeft, CONTACT_DETAIL_TITLE_BTN_COMMON_TOP,
                      CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH, CONTACT_DETAIL_TITLE_BTN_COMMON_HEIGHT);

        pBtn->SetIconTop(CONTACT_DETAIL_TITLE_ICON_COMMON_TOP);
        pBtn->SetTextTop(CONTACT_DETAIL_TITLE_TEXT_COMMON_TOP);

        nLeft += (CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH + CONTACT_DETAIL_TITLE_BTN_SPACE);
    }
}

#ifdef IF_ENABLE_TESTABILITY
QString CRemoteListTitle::GetTestInfo()
{
    QString strTestinfo = CFrameTipTitle::GetTestInfo();
    xml_document doc;
    doc.load(strTestinfo.toLatin1().constData());
    if (doc.empty())
    {
        return strTestinfo;
    }
    xml_node nodeRoot = doc.child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    AddBtnNode(nodeRoot, m_btnNextPage, BTN_NEXT);
    AddBtnNode(nodeRoot, m_btnPrePage, BTN_PRE);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
