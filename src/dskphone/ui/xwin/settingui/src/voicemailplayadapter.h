#ifndef __VOICE_MAIL_PLAY_ADAPTER_H__
#define __VOICE_MAIL_PLAY_ADAPTER_H__

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL

#include "settinguiadapter.h"
#include "voicemailtimeritem.h"

class xTimerPieceView;
class CVoiceMailPlayAdapter : public CSettingUIAdapter
{

public:
    CVoiceMailPlayAdapter();
    virtual ~CVoiceMailPlayAdapter();
    static CSettingUIAdapter* CreateVoiceMailPlayAdapter();
    virtual xListViewItem* PrepareItem(xListView* pListView, int nDataIndex, xListViewItem* pItem);
    void OnTimerChanged(xSignal* sender, WPARAM wParam,
                        LPARAM lParam, int nDataBytes, LPVOID pData);

private:
    xTimerPieceView* m_pTimerPiece;

};

#endif // IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#endif // __VOICE_MAIL_PLAY_ADAPTER_H__