#include <QtGui>
#include "uikernel/ilistitemdelegate.h"
#include "callloglistitem.h"
#include "modcalllogui.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define DEFAULT_CALL_TYPE_POS_LEFT 0
#define DEFAULT_CALL_TYPE_POS_TOP 18
#define DEFAULT_CALL_TYPE_POS_WIDTH 21
#define DEFAULT_CALL_TYPE_POS_HEIGHT 21

// 联系人图标位置信息
#define DEFAULT_CALL_LOG_POS_ICON 6
}

CCallLogListItem::CCallLogListItem(QWidget * parent/* = NULL*/,
                                   IListItemAction * pAction/* = NULL*/)
    : CContactBaseItem(parent, pAction)
    , m_nCallType(-1)
{
    m_nType |= LIST_ITEM_SUB_TYPE_CALL_LOG;
    setObjectName("CCallLogListItem");
}

CCallLogListItem::~CCallLogListItem(void)
{
}

int CCallLogListItem::GetCallLogListItemType()
{
    return (LIST_ITEM_TYPE_SIMPLE_INFO | LIST_ITEM_TYPE_CONTACT_BASE | LIST_ITEM_SUB_TYPE_CALL_LOG);
}

void CCallLogListItem::SetCallType(int nType)
{
    m_nCallType = nType;
}

void CCallLogListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 背景
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

    // 复选框
    PaintCheckBox(stylePainter, pt);

    // 类型
    pt.setX(pt.x() + DEFAULT_CALL_TYPE_POS_LEFT);
    pt.setY(rc.top() + DEFAULT_CALL_TYPE_POS_TOP);

    QRect rcCallTypeIcon(pt.x(), pt.y(), DEFAULT_CALL_TYPE_POS_WIDTH, DEFAULT_CALL_TYPE_POS_HEIGHT);
    yl::string strCallLogType;

    if (IsDown())
    {
        strCallLogType = GetCallTypeSelectIcon(m_nCallType);
    }
    else
    {
        strCallLogType = GetCallTypeIcon(m_nCallType);
    }

    QPixmap pmCallType = THEME_GET_BMP(strCallLogType.c_str());

    if (!pmCallType.isNull())
    {
        stylePainter.drawPixmap(rcCallTypeIcon, pmCallType);
    }

    // 图标和信息
    pt.setX(rcCallTypeIcon.right() + DEFAULT_CALL_LOG_POS_ICON);
    pt.setY(rc.top());

    int nTextRight = rc.width();
    const QRect & rcDetail = m_btnDetail.GetRect();

    if (!rcDetail.isNull())
    {
        nTextRight = rcDetail.left();
    }

    PaintIconInfo(stylePainter, pt, nTextRight - pt.x(), PIC_CONTACTS_ICON_DEFAULT);

    // Detail按钮
    PaintDetailButton(stylePainter, rc.right());
}
