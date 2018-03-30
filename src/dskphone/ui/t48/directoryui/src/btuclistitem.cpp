#include "btuclistitem.h"
#include "modbtucui.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace
{
#define BTUC_LIST_ITEM_POS_NEW_STATUS_LEFT 25
#define BTUC_LIST_ITEM_POS_NEW_STATUS_TOP 33
#define BTUC_LIST_ITEM_POS_STATUS_LEFT 30
#define BTUC_LIST_ITEM_POS_STATUS_TOP 38
#define BTUC_LIST_ITEM_POS_STATUS_WIDTH 16
#define BTUC_LIST_ITEM_POS_STATUS_HEIGHT 16
}


CBTUCListItem::CBTUCListItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CContactBaseItem(parent, pAction)
    , m_nPresence(_PRESENCE_TYPE_NONE)
{
    m_nType |= LIST_ITEM_SUB_TYPE_BTUC_CONTACT;
}

CBTUCListItem::~CBTUCListItem()
{

}

int CBTUCListItem::GetBTUCItemType()
{
    return (LIST_ITEM_TYPE_SIMPLE_INFO | LIST_ITEM_TYPE_CONTACT_BASE | LIST_ITEM_SUB_TYPE_BTUC_CONTACT);
}

void CBTUCListItem::ClearContent()
{
    m_nPresence = _PRESENCE_TYPE_NONE;
    m_pmPhoto = QPixmap();

    CContactBaseItem::ClearContent();
}

void CBTUCListItem::SetPhoto(const yl::string & strPhoto)
{
    if (strPhoto.empty())
    {
        QPixmap pmEmpty;

        m_pmPhoto = pmEmpty;
        return;
    }

    yl::string strFullPath = TEMP_PATH;

    strFullPath += strPhoto;

    if (!TMP_GET_BMP(m_pmPhoto, strFullPath.c_str()))
    {
        QPixmap pmEmpty;

        m_pmPhoto = pmEmpty;
    }
}

void CBTUCListItem::SetPresence(int nPresence)
{
    m_nPresence = nPresence;
}

void CBTUCListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

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

    int nTextRight = rc.width();
    const QRect & rcDetail = m_btnDetail.GetRect();

    if (!rcDetail.isNull())
    {
        nTextRight = rcDetail.left();
    }

    if (m_pmPhoto.isNull())
    {
        PaintDefault(stylePainter, pt, nTextRight - pt.x(), PIC_CONTACTS_ICON_DEFAULT);
    }
    else
    {
        PaintDefault(stylePainter, m_pmPhoto, pt, nTextRight - pt.x());
    }

    PaintStatus(stylePainter, pt);

    PaintDetailButton(stylePainter, rc.right());
}

void CBTUCListItem::PaintStatus(QStylePainter & stylePainter, const QPoint & ptContact)
{
    yl::string strIcon = BTUC_GetPresenceIcon(m_nPresence);

    if (strIcon.empty())
    {
        return;
    }

    QPixmap pmIcon = THEME_GET_BMP(strIcon.c_str());

    if (pmIcon.isNull())
    {
        return;
    }

    //是否显示新状态图标
    bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;
    int iPosLeft = bNewIconMode ? BTUC_LIST_ITEM_POS_NEW_STATUS_LEFT : BTUC_LIST_ITEM_POS_STATUS_LEFT;
    int iPosTop = bNewIconMode ? BTUC_LIST_ITEM_POS_NEW_STATUS_TOP : BTUC_LIST_ITEM_POS_STATUS_TOP;
    QRect rcIcon(ptContact.x() + iPosLeft, ptContact.y() + iPosTop,
                 BTUC_LIST_ITEM_POS_STATUS_WIDTH, BTUC_LIST_ITEM_POS_STATUS_HEIGHT);

    stylePainter.drawPixmap(rcIcon, pmIcon);
}
