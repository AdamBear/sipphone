#include <QtGui>
#include "contactbaseitem.h"
#include "fontcolor.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/ilistitemdelegate.h"
#include "uimanager/rectdefine.h"
#include "framelistaction.h"
#include "t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
namespace
{
// 复选框位置信息
#define DEFAULT_CONTACT_POS_CHECK_LEFT 0
#define DEFAULT_CONTACT_POS_CHECK_TOP 16
#define DEFAULT_CONTACT_POS_CHECK_WIDTH 22
#define DEFAULT_CONTACT_POS_CHECK_HEIGHT 23

// 复选框显示时图标的偏移10
#define DEFAULT_CONTACT_POS_CHECK_ICON_X_OFFSET 10

// Detail位置信息
// Detail按钮离右侧的偏移位置
#define DEFAULT_CONTACT_POS_BTN_DETAIL_X_OFFSET 72
#define DEFAULT_CONTACT_POS_BTN_DETAIL_TOP 1
#define DEFAULT_CONTACT_POS_BTN_DETAIL_WIDTH 72
#define DEFAULT_CONTACT_POS_BTN_DETAIL_HEIGHT 53

// Detail图标离右侧的偏移位置
#define DEFAULT_CONTACT_POS_ICON_DETAIL_X_OFFSET 51
#define DEFAULT_CONTACT_POS_ICON_DETAIL_TOP 11
#define DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH 30
#define DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT 30
}

CContactBaseItem::CContactBaseItem(QWidget * parent/* = NULL*/,
                                   IListItemAction * pAction/* = NULL*/)
    : CSimpleInfoListItem(parent, pAction)
    , m_bEditing(false)
    , m_bChecked(false)
    , m_bDetail(false)
    , m_btnDetail(this)
    , m_nId(0)
#ifdef IF_ENABLE_TESTABILITY
    , m_rcBtnDetail(QRect())
    , m_rcCheckBox(QRect())
#endif
{
    m_nType |= LIST_ITEM_TYPE_CONTACT_BASE;

    Relayout();

    setObjectName("CContactBaseItem");
}

CContactBaseItem::~CContactBaseItem(void)
{
}

int CContactBaseItem::GetContactBaseItemType()
{
    return (LIST_ITEM_TYPE_SIMPLE_INFO | LIST_ITEM_TYPE_CONTACT_BASE);
}

// 清除内容
void CContactBaseItem::ClearContent()
{
    m_bEditing = false;
    m_bChecked = false;
    m_bDetail = false;
    m_bContactPicVisible = true;
    m_btnDetail.ResetDown();
    m_nId = 0;

    CSimpleInfoListItem::ClearContent();
}

void CContactBaseItem::SetEdit(bool bEdit)
{
    m_bEditing = bEdit;

    if (m_bEditing)
    {
        m_bDetail = false;
    }
}

void CContactBaseItem::SetChecked(bool bChecked, bool bRefresh/* = false*/)
{
    bool bUpdate = false;

    if (bChecked != m_bChecked)
    {
        bUpdate = true;
    }

    m_bChecked = bChecked;

    if (bUpdate && bRefresh)
    {
        RefreshUI();
    }
}

void CContactBaseItem::SetId(int nId)
{
    m_nId = nId;
}

void CContactBaseItem::SetDetail(bool bVisible)
{
    m_bDetail = bVisible;
}

bool CContactBaseItem::IsEdit() const
{
    return m_bEditing;
}

bool CContactBaseItem::IsChecked() const
{
    return m_bChecked;
}

bool CContactBaseItem::IsDetail() const
{
    return m_bDetail;
}

int CContactBaseItem::GetId() const
{
    return m_nId;
}

void CContactBaseItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_bDetail || (!m_btnDetail.ProcessMousePress(pEvent)))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}

void CContactBaseItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bDetailDown = false;
    bool bDetailClick = false;
    bool bEdit = m_bEditing;

    if (m_bDetail)
    {
        bDetailDown = m_btnDetail.IsDown();
        bDetailClick = m_btnDetail.ProcessMouseRelease(pEvent);
    }

    if (!bDetailClick)
    {
        if (bDetailDown)
        {
            m_btnDetail.ResetDown();
        }

        if (!ProcessMouseRelease(pEvent, true))
        {
            return;
        }
    }

    if (bEdit)
    {
        if (m_bChecked)
        {
            m_bChecked = false;
        }
        else
        {
            m_bChecked = true;
        }

        DoAction(CONTACT_ITEM_ACTION_CHECK);

        pEvent->accept();

        RefreshUI();
    }
    else if (bDetailClick)
    {
        DoAction(CONTACT_ITEM_ACTION_DETAIL);
    }
}

void CContactBaseItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    QPoint ptContact;

    PaintContact(stylePainter, ptContact);
}

void CContactBaseItem::Relayout()
{
    QRect rcItem = rect();

    int nLeft = rcItem.right() - DEFAULT_CONTACT_POS_BTN_DETAIL_X_OFFSET;

    m_btnDetail.SetRect(nLeft, DEFAULT_CONTACT_POS_BTN_DETAIL_TOP,
                        DEFAULT_CONTACT_POS_BTN_DETAIL_WIDTH, DEFAULT_CONTACT_POS_BTN_DETAIL_HEIGHT);
}

void CContactBaseItem::PaintContact(QStylePainter & stylePainter, QPoint & ptContact)
{
    PaintDownBg(stylePainter);

    QRect rc = rect();
    QPoint pt(rc.left(), rc.top());

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        pt.setX(pt.x() + nLeftMargin);
    }

    PaintCheckBox(stylePainter, pt);

    ptContact = pt;

    int nTextRight = rc.width();
    const QRect & rcDetail = m_btnDetail.GetRect();

    if (!rcDetail.isNull())
    {
        nTextRight = rcDetail.left();
    }
    if (m_bContactPicVisible)
    {
        PaintDefault(stylePainter, pt, nTextRight - pt.x(), PIC_CONTACTS_ICON_DEFAULT);
    }
    else
    {
        PaintDefault(stylePainter, pt, nTextRight - pt.x(), "");
    }

    PaintDetailButton(stylePainter, rc.right());
}

void CContactBaseItem::PaintCheckBox(QStylePainter & stylePainter, QPoint & ptBegin)
{
    if (!m_bEditing)
    {
        return;
    }

    ptBegin.setX(ptBegin.x() + DEFAULT_CONTACT_POS_CHECK_LEFT);

    QPixmap pm;

    if (m_bChecked)
    {
        pm = THEME_GET_BMP(PIC_CHECK_BOX_SELECT);
    }
    else
    {
        pm = THEME_GET_BMP(PIC_CHECK_BOX_NORMAL);
    }

    if (!pm.isNull())
    {
        QRect rcCheckBox(ptBegin.x(), ptBegin.y() + DEFAULT_CONTACT_POS_CHECK_TOP,
                         DEFAULT_CONTACT_POS_CHECK_WIDTH, DEFAULT_CONTACT_POS_CHECK_HEIGHT);
#ifdef IF_ENABLE_TESTABILITY
        m_rcCheckBox = rcCheckBox;
#endif
        stylePainter.drawPixmap(rcCheckBox, pm);
    }

    ptBegin.setX(ptBegin.x() + DEFAULT_CONTACT_POS_CHECK_WIDTH +
                 DEFAULT_CONTACT_POS_CHECK_ICON_X_OFFSET);
}

void CContactBaseItem::PaintDetailButton(QStylePainter & stylePainter, int nRight)
{
    if (m_bDetail)
    {
        QRect rcIcon(nRight - DEFAULT_CONTACT_POS_ICON_DETAIL_X_OFFSET, DEFAULT_CONTACT_POS_ICON_DETAIL_TOP,
                     DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH, DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT);
#ifdef IF_ENABLE_TESTABILITY
        m_rcBtnDetail = rcIcon;
#endif
        m_btnDetail.PaintButton(stylePainter, PIC_GENERAL_LIST_ITEM_INFO, PIC_GENERAL_LIST_ITEM_INFO_CLICK,
                                NULL, rcIcon);
    }
}

void CContactBaseItem::SetContactPicVisible(bool bVisible)
{
    m_bContactPicVisible = bVisible;
}

#ifdef IF_ENABLE_TESTABILITY
QString CContactBaseItem::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // checkbox
    ptAbsolute = mapToGlobal(m_rcCheckBox.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = BTN_CHECK;
    testInfo.text = "";
    testInfo.type = VIRTUAL_BTN;

    AddNode(nodeRoot, testInfo);


    //icon
    ptAbsolute = mapToGlobal(m_rcIcon.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = LABEL_ICON;
    testInfo.text = "";
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    // title
    ptAbsolute = mapToGlobal(m_rcTitle.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcTitle.width();
    testInfo.height = m_rcTitle.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strTitle);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // SubTitle
    ptAbsolute = mapToGlobal(m_rcSubTitle.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcSubTitle.width();
    testInfo.height = m_rcSubTitle.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = fromQString(m_strSubTitle);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // info
    ptAbsolute = mapToGlobal(m_rcInfo.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcInfo.width();
    testInfo.height = m_rcInfo.height();
    testInfo.id = LABEL_INFOINDEX;
    testInfo.text = fromQString(m_strInfo);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // btn
    ptAbsolute = mapToGlobal(m_rcBtnDetail.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcBtnDetail.width();
    testInfo.height = m_rcBtnDetail.height();
    testInfo.id = BTN_DETAIL;
    testInfo.text = "";
    testInfo.type = VIRTUAL_BTN;
    AddNode(nodeRoot, testInfo);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif
