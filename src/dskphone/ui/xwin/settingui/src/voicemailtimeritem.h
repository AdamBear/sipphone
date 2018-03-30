#ifndef __VOICE_MAIL_TIMER_ITEM_H__
#define __VOICE_MAIL_TIMER_ITEM_H__

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#include "baseui/include/detaillistitembase.h"

enum
{
    LIST_ITEM_TYPE_VOICE_MAIL = 0x00140000,
};

class xTextElement;
class xTimerPieceView;
class xImageView;
class CVoiceMailTimerItem : public CDetailListItemBase
{
public:
    explicit CVoiceMailTimerItem(IListViewItemAction* pAction = NULL);
    virtual ~CVoiceMailTimerItem();
    static int GetVoiceMailTimerItemType();
    static CVoiceMailTimerItem* GetVoiceMailTimerItem(xListViewItem* pItem);
    static xListViewItem* CreateVoiceMailTimerItem();

public:
    void loadChildrenElements(xml_node& node);
    void SetTotalTime(const yl::string& strValue);
    void SetPlayStatus(const yl::string& strPlayStatus);
    void StartTimer();
    void StopTimer();
    void PauseTimer();
    void ResumeTimer();
    void SetTimerMaxTick(long lTime);
    void SetIconShow(bool bShow);
    void SetIcon(const yl::string& strIcon);
    void SetTextStatusShow(bool bShow);

    xTimerPieceView* GetTimerPiece();

protected:
    xTimerPieceView*    m_pTimerPiece;
    xTextView*          m_pTextStatus;
    xTextView*          m_pTextTotalTime;
    xImageView*         m_pIcon;

};
typedef CVoiceMailTimerItem* CVoiceMailTimerItemPtr;
#endif

#endif