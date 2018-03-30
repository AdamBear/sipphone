///////////////////////////////////////////////////////////
//  CSelectChannelMode.h
//  Implementation of the Class CSelectChannelMode
//  Created on:      17-12月-2015 16:57:42
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_SELECTEDCHANNELMODE_H__
#define __VOICE_SELECTEDCHANNELMODE_H__

#ifndef _DECT

class CSelectChannelMode : public CBaseChannelMode
{

public:
    CSelectChannelMode(CBaseInitParam * pParam);
    virtual ~CSelectChannelMode();
    virtual REAL_CHANNEL GetRealChannel();
    virtual CBaseChannelParam * GetChannelParam();

protected:
    virtual bool BeforeSwitchChannel(REAL_CHANNEL eRealChannel);
    virtual bool ProcessSwitchChannel(REAL_CHANNEL eRealChannel);
    virtual bool AfterSwitchChannel(REAL_CHANNEL eRealChannel);
private:
    CBaseChannelMode * m_pSubChannelMode;
};

#endif
#endif // __VOICE_SELECTEDCHANNELMODE_H__
