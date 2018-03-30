#include "networkconfmgritem.h"
#include <QStylePainter>
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "uimanager/rectdefine.h"
#include "uikernel/quipushbutton.h"

namespace
{
#define TALK_NW_CONF_MGR_ITEM_FONT_SIZE 16
#define TALK_NW_CONF_MGR_ITEM_FONT_SIZE_MIN 12
#define TALK_NW_CONF_MGR_ITEM_FONT_SIZE_MAX 20
#define TALK_NW_CONF_MGR_ITEM_LEFT_MARGIN 10
#define TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_RIGHT_MARGIN  70
#define TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_WIDTH 46
#define TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_HEIGHT    46
#define TALK_NW_CONF_MGR_ITEM_WIDTH 478
#define TALK_NW_CONF_MGR_ITEM_HEIGHT 57
const QColor ITME_DEFAULT_COLOR(Qt::black);
}


CNetworkMgrItem::CNetworkMgrItem(QWidget * parent/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_NW_CONF_MGR, parent, NULL)
    , m_fontColor(ITME_DEFAULT_COLOR)
    , m_strName("")
    , m_bRemove(false)
    , m_pBtnRemove(NULL)
{
    setObjectName("CNetworkMgrItem");
    qWidgetUtility::setFontStyle(this, TALK_NW_CONF_MGR_ITEM_FONT_SIZE);

    m_pBtnRemove = new qUIPushButton(this);

    if (NULL == m_pBtnRemove)
    {
        return;
    }

    m_pBtnRemove->setGeometry(TALK_NW_CONF_MGR_ITEM_WIDTH - TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_RIGHT_MARGIN - \
                              TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_WIDTH, \
                              (TALK_NW_CONF_MGR_ITEM_HEIGHT - TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_HEIGHT) / 2, \
                              TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_WIDTH, \
                              TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_HEIGHT);
    m_pBtnRemove->set3StatePic(PIC_ICON_NETWORK_CONF_REMOVE, PIC_ICON_NETWORK_CONF_REMOVE_PRESS, \
                               PIC_ICON_NETWORK_CONF_REMOVE_PRESS);

    m_pBtnRemove->hide();

    connect(m_pBtnRemove, SIGNAL(clicked()), this, SLOT(OnMemberRemove()));
}

CNetworkMgrItem::~CNetworkMgrItem()
{
    disconnect(m_pBtnRemove, SIGNAL(clicked()), this, SLOT(OnMemberRemove()));
}

void CNetworkMgrItem::SetName(QString strName)
{
    if (m_strName != strName)
    {
        m_strName = strName;
        update();
    }
}

void CNetworkMgrItem::SetUserID(QString strID)
{
    m_strID = strID;
}

QString CNetworkMgrItem::GetUserID()
{
    return m_strID;
}

QString CNetworkMgrItem::GetUserName()
{
    return m_strName;
}

void CNetworkMgrItem::SetAllowRemove(bool bRemove)
{
    if (m_bRemove != bRemove)
    {
        m_bRemove = bRemove;
        if (m_bRemove)
        {
            m_pBtnRemove->show();
        }
        else
        {
            m_pBtnRemove->hide();
        }
    }
}

void CNetworkMgrItem::OnMemberRemove()
{
    emit RequestRemoveMember(GetUserID(), GetUserName());
}

void CNetworkMgrItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    stylePainter.setPen(m_fontColor);

    QRect rectName = this->rect();
    rectName.setLeft(rectName.left() + TALK_NW_CONF_MGR_ITEM_LEFT_MARGIN);
    rectName.setWidth(RECT_TALK_PAGE.width() - 2 * TALK_NW_CONF_MGR_ITEM_LEFT_MARGIN - TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_RIGHT_MARGIN \
                      - TALK_NW_CONF_MGR_ITEM_REMOVE_ICON_WIDTH);
    //draw name
    stylePainter.drawItemText(rectName, Qt::AlignLeft | Qt::AlignVCenter, this->palette(), true,
                              m_strName);
}

void CNetworkMgrItem::SetFontSize(int iFontSize)
{
    if (IsFontSizeValid(iFontSize))
    {
        qWidgetUtility::setFontStyle(this, iFontSize);
    }
}

void CNetworkMgrItem::SetFontColor(QColor clr)
{
    if (clr != m_fontColor)
    {
        m_fontColor = clr;
        if (this->isVisible())
        {
            update();
        }
    }
}

bool CNetworkMgrItem::IsFontSizeValid(int iFontSize)
{
    if (TALK_NW_CONF_MGR_ITEM_FONT_SIZE_MIN <= iFontSize
            && TALK_NW_CONF_MGR_ITEM_FONT_SIZE_MAX >= iFontSize)
    {
        return true;
    }

    return false;
}

