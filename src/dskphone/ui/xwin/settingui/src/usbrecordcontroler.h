#ifndef __USB_RECORD_CONTROLER_H__
#define __USB_RECORD_CONTROLER_H__

#include "ETLLib/ETLLib.hpp"
#include "xwindow/xwindowcommon.h"

#ifdef IF_USB_SUPPORT
class CUSBRecordControler
{
public:
    CUSBRecordControler();
    ~CUSBRecordControler();

    static CUSBRecordControler & GetInstance();
    static void ReleaseControler();

    enum
    {
        RECORD_STATUS_NONE      = 0,
        RECORD_STATUS_START     = 1,
        RECORD_STATUS_PAUSE     = 2,
        RECORD_STATUS_RESMUE    = 3,
        RECORD_STATUS_STOP      = 4,
    };

    enum
    {
        AUDIO_PLAYER_ACTION_NONE        = 0,
        AUDIO_PLAYER_ACTION_PLAY        = 1,
        AUDIO_PLAYER_ACTION_PAUSE       = 2,
        AUDIO_PLAYER_ACTION_RESUME      = 3,
        AUDIO_PLAYER_ACTION_PLAY_LEFT   = 4,
        AUDIO_PLAYER_ACTION_PLAY_RIGHT  = 5,
        AUDIO_PLAYER_ACTION_STOP        = 6,
    };

public:
    void Init(const yl::string & strPageID);
    void UnInit(const yl::string & strPageID);

    bool IsUsbConnect();
    bool IsIdleRecordEnabled();
    bool IsPlayerRunning();

    UINT GetRecordingTime();
    long GetPlayTime();

    yl::string GetAudioDisplayName(const yl::string & strName);

    bool GetPlayAudioName(yl::string & strName);

    void SetAudioIndex(int nIndex);
    int GetAudioIndex() const;

    bool ProcessAudio(int nAction, long lData = 0);

    static LRESULT OnMessage(msgObject & msg);

protected:
    bool IsBindPage(const yl::string & strPageID);
    void OnStorageStateChange(msgObject & msg);
    void OnStorageVolumeWarning(msgObject & msg);
    void OnAudioPlayEnd(msgObject & msg);
    void OnRecordResult(msgObject & msg);

protected:
    static CUSBRecordControler * s_pUsbRecordControler;

    bool m_bConnect;
    int m_nAudioIndex;
    long m_lPlayTime;

    LIST_STRING m_listPageId;
};

#define g_UsbRecordControler CUSBRecordControler::GetInstance()

#endif

#endif
