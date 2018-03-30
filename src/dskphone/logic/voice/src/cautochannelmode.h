///////////////////////////////////////////////////////////
//  CAutoChannelMode.h
//  Implementation of the Class CAutoChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_AUTOCHANNELMODE_H__
#define __VOICE_AUTOCHANNELMODE_H__

#ifndef _DECT
class CAutoChannelMode : public CBaseChannelMode
{
public:
    CAutoChannelMode(CBaseInitParam * pParam);
    virtual ~CAutoChannelMode();

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
#endif // __VOICE_AUTOCHANNELMODE_H__
