#include "voicemailtimeritem.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#include "baseui/include/baseuicommon.h"
#include "baseui/include/timerpieceview.h"


CVoiceMailTimerItem::CVoiceMailTimerItem(IListViewItemAction* pAction/* = NULL*/)
    : CDetailListItemBase(pAction)
    , m_pIcon(NULL)
    , m_pTimerPiece(NULL)
    , m_pTextTotalTime(NULL)
    , m_pTextStatus(NULL)
{
    m_nType = LIST_ITEM_TYPE_VOICE_MAIL;
}

CVoiceMailTimerItem::~CVoiceMailTimerItem()
{

}

int CVoiceMailTimerItem::GetVoiceMailTimerItemType()
{
    return (LIST_ITEM_TYPE_VOICE_MAIL);
}

CVoiceMailTimerItem* CVoiceMailTimerItem::GetVoiceMailTimerItem(xListViewItem* pItem)
{
    if (NULL == pItem || GetVoiceMailTimerItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CVoiceMailTimerItem*)pItem;
}

void CVoiceMailTimerItem::loadChildrenElements(xml_node& node)
{
    xListViewItem::loadChildrenElements(node);
    m_pTextStatus = static_cast<xTextView*>(getViewById("txtStatus"));
    m_pTextTotalTime = static_cast<xTextView*>(getViewById("txtTotal"));
    m_pTimerPiece = static_cast<xTimerPieceView*>(getViewById("timerPiece"));
    m_pIcon = static_cast<xImageView*>(getViewById("imageStatus"));
}

xListViewItem* CVoiceMailTimerItem::CreateVoiceMailTimerItem()
{
    CVoiceMailTimerItem* pListItem = new CVoiceMailTimerItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/voicemailtimeritem.xml");
    }

    return pListItem;
}

void CVoiceMailTimerItem::SetTotalTime(const yl::string& strValue)
{
    if (NULL != m_pTextTotalTime)
    {
        m_pTextTotalTime->SetText(strValue);
    }
}

void CVoiceMailTimerItem::PauseTimer()
{
    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->PauseTimer();
    }
}

void CVoiceMailTimerItem::ResumeTimer()
{
    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->ResumeTimer();
    }
}

void CVoiceMailTimerItem::StartTimer()
{
    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->StartTimer();
    }
}

void CVoiceMailTimerItem::StopTimer()
{
    if (NULL != m_pTimerPiece)
    {
        m_pTimerPiece->StopTimer();
    }
}

void CVoiceMailTimerItem::SetPlayStatus(const yl::string& strPlayStatus)
{
    if (NULL == m_pTextStatus)
    {
        return;
    }

    yl::string strText;
    if (m_bShowColon)
    {
        strText = strPlayStatus + ":";
    }
    else
    {
        strText = strPlayStatus;
    }
    m_pTextStatus->SetText(strText);
}

xTimerPieceView*  CVoiceMailTimerItem::GetTimerPiece()
{
    return m_pTimerPiece;
}

void CVoiceMailTimerItem::SetTimerMaxTick(long lTime)
{
    if (NULL != m_pTimerPiece)
    {
        return m_pTimerPiece->SetMaxTick(lTime);
    }
}

void CVoiceMailTimerItem::SetIconShow(bool bShow)
{
    if (NULL == m_pIcon)
    {
        return;
    }

    if (bShow)
    {
        m_pIcon->show();
    }
    else
    {
        m_pIcon->hide(true);
    }
}

void CVoiceMailTimerItem::SetTextStatusShow(bool bShow)
{
    if (NULL == m_pTextStatus)
    {
        return;
    }

    if (bShow)
    {
        m_pTextStatus->show();
    }
    else
    {
        m_pTextStatus->hide(true);
    }
}

void CVoiceMailTimerItem::SetIcon(const yl::string& strIcon)
{
    if (NULL != m_pIcon)
    {
        m_pIcon->setPixmap(strIcon);
    }
}
#endif
