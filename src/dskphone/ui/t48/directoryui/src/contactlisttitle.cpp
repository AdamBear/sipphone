#include <QtGui>
#include "contactlisttitle.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "directorylistaction.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
// 按钮的位置信息
#define CONTACT_LIST_TITLE_BTN_COMMON_TOP 1
#define CONTACT_LIST_TITLE_BTN_COMMON_WIDTH 84
#define CONTACT_LIST_TITLE_BTN_COMMON_HEIGHT 54
#define CONTACT_LIST_TITLE_BTN_RIGHT 4
#define CONTACT_LIST_TITLE_BTN_SPACE 9

// 分隔线的位置信息
#define CONTACT_LIST_TITLE_SEPARATE_LEFT 4
#define CONTACT_LIST_TITLE_SEPARATE_TOP 12
#define CONTACT_LIST_TITLE_SEPARATE_WIDTH 1
#define CONTACT_LIST_TITLE_SEPARATE_HEIGHT 30

#define CONTACT_LIST_TITLE_FONT_SIZE_CHECK_BOX 16
#define CONTACT_LIST_TITLE_FONT_SIZE_BTN_BAR 14
}

CContactListTitle::CContactListTitle(CFrameList * pFrameList/* = NULL*/,
                                     IFrameTitleAction * pAction/* = NULL*/)
    : CFrameSelectTitle(pFrameList, pAction)
    , m_btnMove(this)
    , m_btnBlackList(this)
    , m_btnBar(this, false)
    , m_btnDel(this)
    , m_nBtnCount(0)
{
    m_nType = FRAME_LIST_TITLE_TYPE_CONTACT_LIST_TITLE;
    setObjectName("CContactListTitle");
}

CContactListTitle::~CContactListTitle(void)
{
}

bool CContactListTitle::IsContactListTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_CONTACT_LIST_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CContactListTitle::SetBtnCount(int nCount)
{
    if (nCount > MAX_BTN_COUNT)
    {
        return;
    }

    m_btnBar.SetBtnCount(nCount);

    m_nBtnCount = 0;

    m_btnMove.SetRect(0, 0, 0, 0);
    m_btnBlackList.SetRect(0, 0, 0, 0);
    m_btnDel.SetRect(0, 0, 0, 0);

    m_btnMove.ResetDown();
    m_btnBlackList.ResetDown();
    m_btnDel.ResetDown();

    for (int i = 0; i < MAX_BTN_COUNT; ++i)
    {
        m_arrTitle[i] = "";
    }
}

void CContactListTitle::AddButton(int nAction, const QString & strTitle)
{
    if (m_nBtnCount >= MAX_BTN_COUNT)
    {
        return;
    }

    CTitleButton * pBtn = NULL;

    switch (nAction)
    {
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            pBtn = &m_btnDel;
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            pBtn = &m_btnMove;
        }
        break;
    case LIST_TITLE_ACTION_ADD_BLACKLIST:
        {
            pBtn = &m_btnBlackList;
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

void CContactListTitle::mousePressEvent(QMouseEvent * pEvent)
{
    CFrameSelectTitle::mousePressEvent(pEvent);

    m_btnBar.ProcessMousePress(pEvent);
}

void CContactListTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CFrameSelectTitle::mouseReleaseEvent(pEvent);

    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        return;
    }

    if (pBtn == &m_btnDel)
    {
        DoAction(LIST_TITLE_ACTION_DEL_CONTACT);
    }
    else if (pBtn == &m_btnMove)
    {
        DoAction(LIST_TITLE_ACTION_MOVE_CONTACT);
    }
    else if (pBtn == &m_btnBlackList)
    {
        DoAction(LIST_TITLE_ACTION_ADD_BLACKLIST);
    }
}

void CContactListTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QFont fontTitle = stylePainter.font();

    fontTitle.setPointSize(CONTACT_LIST_TITLE_FONT_SIZE_CHECK_BOX);
    fontTitle.setWeight(QFont::Normal);
    fontTitle.setBold(false);
    stylePainter.setFont(fontTitle);

    PaintCheckBox(stylePainter);

    fontTitle.setPointSize(CONTACT_LIST_TITLE_FONT_SIZE_BTN_BAR);
    stylePainter.setFont(fontTitle);

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

        if (pBtn == &m_btnMove)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_MOVE_CONTACT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);
        }
        else if (pBtn == &m_btnBlackList)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_BLACKLIST, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);
        }
        else if (pBtn == &m_btnDel)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DEL_CONTACT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);
        }

        // 画分隔线
        if ((i + 1) != nBtnCount)
        {
            QRect rcBtn = pBtn->GetRect();
            QRect rcSeparate(rcBtn.right() + CONTACT_LIST_TITLE_SEPARATE_LEFT, CONTACT_LIST_TITLE_SEPARATE_TOP,
                             CONTACT_LIST_TITLE_SEPARATE_WIDTH, CONTACT_LIST_TITLE_SEPARATE_HEIGHT);

            QPixmap pmSeparate = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_SEPARATE);

            if (!pmSeparate.isNull())
            {
                stylePainter.drawPixmap(rcSeparate, pmSeparate);
            }
        }
    }
}

void CContactListTitle::Relayout()
{
    QRect rcTitle = rect();
    int nTotalWidth = CONTACT_LIST_TITLE_BTN_COMMON_WIDTH * m_nBtnCount + CONTACT_LIST_TITLE_BTN_SPACE
                      * (m_nBtnCount - 1);
    int nLeft =  rcTitle.right() - CONTACT_LIST_TITLE_BTN_RIGHT - nTotalWidth;

    for (int i = 0; i < m_nBtnCount; ++i)
    {
        CTitleButton * pBtn = (CTitleButton *)m_btnBar.GetBtn(i);

        if (NULL == pBtn)
        {
            continue;
        }

        pBtn->SetRect(nLeft, CONTACT_LIST_TITLE_BTN_COMMON_TOP,
                      CONTACT_LIST_TITLE_BTN_COMMON_WIDTH, CONTACT_LIST_TITLE_BTN_COMMON_HEIGHT);
        pBtn->SetDefaultPos();

        nLeft += (CONTACT_LIST_TITLE_BTN_COMMON_WIDTH + CONTACT_LIST_TITLE_BTN_SPACE);
    }
}

#ifdef IF_ENABLE_TESTABILITY
QString CContactListTitle::GetTestInfo()
{
    QString strTestinfo = CFrameSelectTitle::GetTestInfo();
    xml_document doc;
    doc.load(strTestinfo.toLatin1().constData());
    if (doc.empty())
    {
        return strTestinfo;
    }
    xml_node nodeRoot = doc.child("testinfo");

    AddBtnNode(nodeRoot, m_btnMove, BTN_MOVE, fromQString(m_btnMove.GetText()));
    AddBtnNode(nodeRoot, m_btnBlackList, BTN_BLACKLIST, fromQString(m_btnBlackList.GetText()));
    AddBtnNode(nodeRoot, m_btnDel, BTN_DELETE, fromQString(m_btnDel.GetText()));

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif