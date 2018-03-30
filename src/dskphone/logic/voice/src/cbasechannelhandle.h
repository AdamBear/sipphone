///////////////////////////////////////////////////////////
//  CBaseChannelHandle.h
//  Implementation of the Class CBaseChannelHandle
//  Created on:      17-12月-2015 17:10:14
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_BASECHANNELHANDLE_H__
#define __VOICE_BASECHANNELHANDLE_H__

#ifndef _DECT
class CBaseChannelParam;
class CBaseChannelHandle
{
public:
    CBaseChannelHandle(REAL_CHANNEL eRealChannel);
    ~CBaseChannelHandle();
    bool OpenRealChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam);
protected:
    bool OpenHeadSetChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam);
    bool OpenHandFreeChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam);
    bool OpenHandSetChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam);
    bool OpenDefaultChannel(REAL_CHANNEL eChannel, CBaseChannelParam * pParam);
    CBaseChannelHandle() {}
    REAL_CHANNEL m_eRealChannel;
};

#endif // _DECT
#endif // __VOICE_BASECHANNELHANDLE_H__
