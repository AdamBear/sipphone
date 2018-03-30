#ifndef __VIRTUAL_VOICEMAIL_MANAGER_H__
#define __VIRTUAL_VOICEMAIL_MANAGER_H__

#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"
#include "voicemailcommon.h"

#define NEW_BASE64_DECODE   1

class CVirtualVoiceMailPrcocesser : public XsiBaseProcesser
{
public:

    static CVirtualVoiceMailPrcocesser * GetInstance();

    bool ReqVoicemessageList();

    bool ReqPlayContentByIndex(int index);

    bool ReqMarkAllRead();

    bool ReqMarkAllUnRead();

    bool ReqMarkByIndex(int index, bool bRead);

    bool ReqDeleteMessageByIndex(int index);

    void ClearListData();

    void CorrectTimeZone(yl::string & strTime);

    //void GetLocalTimeZone(int & iTimeZoneHour, int & iTimeZoneMin);

public:

    bool GetVoiceMailListData(VoiceMailListData * pData);

    bool GetDetailByIndex(sVoiceMailData & refData, int index);

    int GetTotalCount()
    {
        return m_vecMailData.size();
    }

public:
    bool Play();

    bool Pause();

    bool Resume();

    bool Seek(unsigned int locate);

    bool Stop();

    ePlayStatus GetPlayStatus()
    {
        return m_ePlayStatus;
    }

private:

    CVirtualVoiceMailPrcocesser();

    virtual ~CVirtualVoiceMailPrcocesser();

    virtual bool ProcessByType(XsiAction * pAction);

    bool PraseMesgList(const yl::string & strFile);

    bool PraseMesgDetail(const yl::string & strFile, int index);

    bool ProcessDelete(int index);

    bool ProcessMarkAll(bool bRead);

    bool ProcessMark(int index, bool bRead);
#if !NEW_BASE64_DECODE
    bool SaveVoiceFile(yl::string & strBase64Data, yl::string & strFileName);
#endif
    int getMaxDuration(eMediaType eType = MEDIA_audio);

    bool IsSupportVideoMail();

protected:
    //操作失败处理
    virtual void FailProcess(XsiAction * pAction);

private:

    VecVoiceMail    m_vecMailData;

    bool            m_bIsLoadingList;

    ePlayStatus     m_ePlayStatus;

    int             m_nPlayIndex;

};

#define g_pVoiceMailProcesser (CVirtualVoiceMailPrcocesser::GetInstance())


#endif
