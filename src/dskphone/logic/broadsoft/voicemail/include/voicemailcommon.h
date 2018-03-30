#ifndef __VOICEMAIL_COMMON_H__
#define  __VOICEMAIL_COMMON_H__

#include <ylstring.h>
#include <ylvector.h>
#include <yllist.h>

enum eMediaType
{
    MEDIA_audio,
    MEDIA_video
};

enum ePlayStatus
{
    PLAYSTATUS_None,
    PLAYSTATUS_Play,
    PLAYSTATUS_Pause,
    PLAYSTATUS_Stop,
};

struct sVoiceMailData
{
    yl::string m_strName;
    yl::string m_strNumber;
    yl::string m_strTime;
    int m_nDuration;
    int m_nLine;

    bool m_bRead;

    bool m_bUrgent;

    bool m_bConfidential;

    bool m_bPrivacy;

    eMediaType m_eMediaType;

    yl::string m_strMessgId;

    sVoiceMailData()
    {
        m_strName = "";
        m_strNumber = "";
        m_strTime = "";
        m_nDuration = 0;
        m_nLine = -1;
        m_bRead = false;
        m_bUrgent = false;
        m_eMediaType = MEDIA_audio;
        m_strMessgId = "";
        m_bConfidential = false;
        m_bPrivacy = false;
    }
};


typedef YLVector<sVoiceMailData> VecVoiceMail;


struct VoiceMailListData
{
    YLList<sVoiceMailData> m_listData;  // BroadSoft contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count.

    VoiceMailListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listData.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

#define kszVoiceMailFilePath    TEMP_PATH "VoiceMail.wav"

#define kszNodeVoiceMessagingMessages "VoiceMessagingMessages"
#define kszNodemessageInfoList "messageInfoList"
#define kszNodemessageInfo "messageInfo"
#define kszNodeduration "duration"
#define kszNodecallingPartyInfo "callingPartyInfo"
#define kszNodeaddress "address"
#define kszNodetime "time"
#define kszNodename "name"
#define kszNodevideo "video"
#define kszNodeurgent "urgent"
#define kszNoderead "read"
#define kszNodemessageId "messageId"
#define kszNodemessageMediaContent "messageMediaContent"
#define kszNodemediaType "mediaType"
#define kszNodecontent "content"
#define kszNodeVoiceMessage "VoiceMessage"
#define kszNodeconfidential "confidential"
#define kszNodeprivacy "privacy"

#define kszReqMarkAsRead "/markAsRead"
#define kszReqMarkAsUnRead "/markAsUnread"




#endif
