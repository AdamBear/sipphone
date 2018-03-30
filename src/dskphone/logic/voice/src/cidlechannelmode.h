///////////////////////////////////////////////////////////
//  CIdleChannelMode.h
//  Implementation of the Class CIdleChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_IDLECHANNELMODE_H__
#define __VOICE_IDLECHANNELMODE_H__

#ifndef _DECT

class CIdleChannelMode : public CBaseChannelMode
{

public:
    CIdleChannelMode(CBaseInitParam * pParam);
    virtual ~CIdleChannelMode();
    virtual REAL_CHANNEL GetRealChannel();

protected:
    virtual bool BeforeSwitchChannel(REAL_CHANNEL eRealChannel);
    virtual bool AfterSwitchChannel(REAL_CHANNEL eRealChannel);

};

#endif
#endif // __VOICE_IDLECHANNELMODE_H__
