///////////////////////////////////////////////////////////
//  CHeadSetChannelMode.h
//  Implementation of the Class CHeadSetChannelMode
//  Created on:      18-12月-2015 16:46:33
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_HEADSETCHANNELMODE_H__
#define __VOICE_HEADSETCHANNELMODE_H__

#ifndef _DECT
#include "cbasechannelmode.h"

class CHeadSetChannelMode : public CBaseChannelMode
{

public:
    CHeadSetChannelMode(CBaseInitParam * pParam);
    virtual ~CHeadSetChannelMode();

    virtual REAL_CHANNEL GetRealChannel();

protected:
    virtual bool ProcessSwitchChannel(REAL_CHANNEL eRealChannel);

};

#endif
#endif // __VOICE_HEADSETCHANNELMODE_H__
