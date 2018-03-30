///////////////////////////////////////////////////////////
//  CRingChannelMode.h
//  Implementation of the Class CRingChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_RINGCHANNELMODE_H__
#define __VOICE_RINGCHANNELMODE_H__

#ifndef _DECT

class CRingChannelMode : public CBaseChannelMode
{

public:
    CRingChannelMode(CBaseInitParam * pParam);
    virtual ~CRingChannelMode();

    virtual REAL_CHANNEL GetRealChannel();

protected:
    virtual bool BeforeSwitchChannel(REAL_CHANNEL eRealChannel);
    virtual bool ProcessSwitchChannel(REAL_CHANNEL eRealChannel);
    virtual bool AfterSwitchChannel(REAL_CHANNEL eRealChannel);
};

#endif
#endif // __VOICE_RINGCHANNELMODE_H__
