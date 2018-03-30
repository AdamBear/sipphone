#include <QtGui>
#include "contactupdatelisttitle.h"
#include "translateiddefine.h"
#include "uikernel/languagehelper.h"
#include "baseui/t4xpicpath.h"

#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

#include "contacts/directory/include/moddirinterface.h"

namespace
{
// 按钮的位置信息
#define CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_TOP 1
#define CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH 84
#define CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_HEIGHT 54
#define CONTACT_UPDATE_LIST_TITLE_BTN_RIGHT 4

#define CONTACT_UPDATE_LIST_TITLE_FONT_SIZE_BTN_BAR 14
#define CONTACT_TEXT_MAX_LENGTH 350
}

CContactUpdateListTitle::CContactUpdateListTitle(CFrameList * pFrameList/* = NULL*/,
        IFrameTitleAction * pAction/* = NULL*/)
    : CFrameTipTitle(pFrameList, pAction)
    , m_btnUpdate(this)
    , m_btnSort(this)
    , m_bUpdatable(false)
    , m_bSortable(false)
{
    setObjectName("CContactUpdateListTitle");
    m_nType = FRAME_LIST_TITLE_TYPE_CONTACT_UPDATE_TITLE;
}

CContactUpdateListTitle::~CContactUpdateListTitle(void)
{
}

bool CContactUpdateListTitle::IsContactUpdateListTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_CONTACT_UPDATE_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CContactUpdateListTitle::SetGroupName(const QString & strGroupName)
{
    m_strGroupName = strGroupName;

    QString strTip = m_strGroupName + QLatin1String("( 0 )");

    SetTip(strTip);
}

void CContactUpdateListTitle::SetContactCount(int nCount)
{
    QString strTip = m_strGroupName;

    QFont f;
    f.setPointSize(16);

    QString strTipCut = CutOffText(strTip, QFontMetrics(f), CONTACT_TEXT_MAX_LENGTH);
    if (strTipCut.length() != strTip.length())
    {
        SetTip(strTipCut + QString("( %1 )").arg(nCount));
    }
    else
    {
        SetTip(strTip + QString("( %1 )").arg(nCount));
    }
}

#if IF_FEATURE_METASWITCH_DIRECTORY
void CContactUpdateListTitle::SetUpdatable(bool bUpdatable, bool bSortable /*= false*/)
#else
void CContactUpdateListTitle::SetUpdatable(bool bUpdatable)
#endif
{
    m_bUpdatable = bUpdatable;
#if IF_FEATURE_METASWITCH_DIRECTORY
    m_bSortable = bSortable;
#endif

    Relayout();
}

#if IF_FEATURE_METASWITCH_DIRECTORY
void CContactUpdateListTitle::SetSortable(bool bSortable)
{
    m_bSortable = bSortable;

    Relayout();
}
#endif

void CContactUpdateListTitle::mousePressEvent(QMouseEvent * pEvent)
{
    if (m_bUpdatable)
    {
        m_btnUpdate.ProcessMousePress(pEvent);
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (m_bSortable)
    {
        m_btnSort.ProcessMousePress(pEvent);
    }
#endif
}

void CContactUpdateListTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (m_bUpdatable && m_btnUpdate.ProcessMouseRelease(pEvent))
    {
        DoAction(LIST_TITLE_ACTION_CONTACT_UPDATE);
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (m_bSortable && m_btnSort.ProcessMouseRelease(pEvent))
    {
        DoAction(LIST_TITLE_ACTION_CONTACT_SORT);
    }
#endif
}

void CContactUpdateListTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    int nRight = 0;
    const QRect & rcTip = m_btnUpdate.GetRect();

    if (rcTip.isValid())
    {
        nRight = rcTip.left();
    }
    else
    {
        QRect rcTitle = rect();

        nRight = rcTitle.right();
    }

    PaintTip(stylePainter, nRight);
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (!m_bUpdatable && !m_bSortable)
    {
        return;
    }

    if (m_bSortable)
    {
        QString strTitle = LANG_TRANSLATE(TRID_MTSW_SORT_FIRSTNAME);
        if (MtswDir_GetSortType() == SORT_BY_LASTNAME)
        {
            strTitle = LANG_TRANSLATE(TRID_MTSW_SORT_FIRSTNAME);
        }
        else
        {
            strTitle = LANG_TRANSLATE(TRID_MTSW_SORT_LASTNAME);
        }

        QFont fontBtn = stylePainter.font();

        fontBtn.setPointSize(CONTACT_UPDATE_LIST_TITLE_FONT_SIZE_BTN_BAR);
        stylePainter.setFont(fontBtn);

        m_btnSort.PaintVButton(stylePainter, PIC_DIRECTORY_DIRECTORY_BTN_CLICK,
                               PIC_DIRECTORY_DIRECTORY_DIRSORT, strTitle);
    }

    if (m_bUpdatable)
    {
        QString strTitle = LANG_TRANSLATE(TRID_UPDATE);
        QFont fontBtn = stylePainter.font();

        fontBtn.setPointSize(CONTACT_UPDATE_LIST_TITLE_FONT_SIZE_BTN_BAR);
        stylePainter.setFont(fontBtn);

        m_btnUpdate.PaintVButton(stylePainter, PIC_DIRECTORY_DIRECTORY_BTN_CLICK,
                                 PIC_DIRECTORY_DIRECTORY_UPDATE, strTitle);
    }
#else
    if (!m_bUpdatable)
    {
        return;
    }

    QString strTitle = LANG_TRANSLATE(TRID_UPDATE);
    QFont fontBtn = stylePainter.font();

    fontBtn.setPointSize(CONTACT_UPDATE_LIST_TITLE_FONT_SIZE_BTN_BAR);
    stylePainter.setFont(fontBtn);

    m_btnUpdate.PaintVButton(stylePainter, PIC_DIRECTORY_DIRECTORY_BTN_CLICK,
                             PIC_DIRECTORY_DIRECTORY_UPDATE, strTitle);
#endif
}

void CContactUpdateListTitle::Relayout()
{
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (!m_bUpdatable && !m_bSortable)
    {
        return;
    }

    int iIconWidth = CONTACT_UPDATE_LIST_TITLE_BTN_RIGHT;

    if (!m_bSortable)
    {
        m_btnSort.SetRect(0, 0, 0, 0);
    }
    else
    {
        iIconWidth += CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH;
    }

    if (!m_bUpdatable)
    {

        m_btnUpdate.SetRect(0, 0, 0, 0);
    }
    else
    {
        iIconWidth += CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH;
    }



    QRect rcTitle = rect();
    int nLeft =  rcTitle.right() - iIconWidth;

    if (m_bSortable)
    {
        m_btnSort.SetRect(nLeft, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_TOP,
                          CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_HEIGHT);
        m_btnSort.SetDefaultPos();

        nLeft += CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH;
    }

    if (m_bUpdatable)
    {
        m_btnUpdate.SetRect(nLeft, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_TOP,
                            CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_HEIGHT);
        m_btnUpdate.SetDefaultPos();
    }

#else
    if (!m_bUpdatable)
    {
        m_btnUpdate.SetRect(0, 0, 0, 0);
        return;
    }

    QRect rcTitle = rect();
    int nLeft =  rcTitle.right() - CONTACT_UPDATE_LIST_TITLE_BTN_RIGHT -
                 CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH;

    m_btnUpdate.SetRect(nLeft, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_TOP,
                        CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_WIDTH, CONTACT_UPDATE_LIST_TITLE_BTN_COMMON_HEIGHT);
    m_btnUpdate.SetDefaultPos();
#endif
}

#ifdef IF_ENABLE_TESTABILITY
QString CContactUpdateListTitle::GetTestInfo()
{
    QString strTestinfo = CFrameTipTitle::GetTestInfo();
    xml_document doc;
    doc.load(strTestinfo.toLatin1().constData());
    if (doc.empty())
    {
        return strTestinfo;
    }
    xml_node nodeRoot = doc.child("testinfo");

    AddBtnNode(nodeRoot, m_btnUpdate, BTN_UPDATE, fromQString(LANG_TRANSLATE(TRID_UPDATE)));
    AddBtnNode(nodeRoot, m_btnSort, BTN_SORT, fromQString(LANG_TRANSLATE(TRID_MTSW_SORT_FIRSTNAME)));

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
