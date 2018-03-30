///////////////////////////////////////////////////////////
//  CAudioManager.h
//  Implementation of the Class CAudioManager
//  Created on:      05-二月-2015 21:05:04
//  Original author: huangwz
///////////////////////////////////////////////////////////

#ifndef _AUDIO_MESSAGE_H_
#define _AUDIO_MESSAGE_H_

#ifdef _T49
#include <ETLLib.hpp>
#include "service_ipvp_types.h"
#include "commondefine.h"
#include "commondata.h"

class CCallInfo;

struct AudioInfo
{
    int iCallId;
    // 是否丢包状态
    bool bLostFlac;
    // 防抖计数
    int  iConut;
    ipvp_audio_t audioInfo;

    AudioInfo()
    {
        bLostFlac = false;
        iConut = 0;
    }
};

class CAudioManager : public CBaseMedia
{
public:
    CAudioManager();
    ~CAudioManager();

public:

    virtual bool OnEvent(MediaEventData * pData);

    virtual bool GetStats(int iCallId, MediaStatsInfo & statsInfo);

    bool OnStopAudio(int iCallId);

    bool OnStartAudio(int iCallID);
protected:
    AudioInfo * GetAudioInfo(int iCallID);

    bool DeleteAudioInfo(int iCallID);

    bool UpdateStats();

    bool UpdateStats(int iCallID, AudioInfo * pAudioInfo);

    // 请求比特率
    bool OnReqSrtpLifeTime(int iCallId);

private:
    YLList<AudioInfo> m_listAudioInfo;
};

#endif // _T49
#endif // _AUDIO_MESSAGE_H_
