///////////////////////////////////////////////////////////
//  CGroupChannelMode.h
//  Implementation of the Class CGroupChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_GROUPCHANNELMODE_H__
#define __VOICE_GROUPCHANNELMODE_H__

#ifndef _DECT
class CGroupChannelMode : public CBaseChannelMode
{

public:
    CGroupChannelMode(CBaseInitParam * pParam);
    virtual ~CGroupChannelMode();
    virtual REAL_CHANNEL GetRealChannel();

protected:
    virtual bool BeforeSwitchChannel(REAL_CHANNEL eRealChannel);
private:
    bool m_bIsNeedPreProcess;
};

#endif
#endif // __VOICE_GROUPCHANNELMODE_H__
