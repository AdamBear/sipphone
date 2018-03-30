///////////////////////////////////////////////////////////
//  voicechannelparam.h
//
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_CHANNELPARAM_H__
#define __VOICE_CHANNELPARAM_H__
#include "voicecommon.h"

//通道设置参数
class CBaseChannelParam
{
public:
    CBaseChannelParam(CHANNEL_MODE eChannelMode)
    {
        m_eChannelMode = eChannelMode;
    }
    virtual ~CBaseChannelParam()
    {
    }

    CHANNEL_MODE m_eChannelMode;

protected:
    CBaseChannelParam() {}
};

class CHeadsetChannelParam : public CBaseChannelParam
{
public:
    CHeadsetChannelParam(CHANNEL_MODE eChannelMode, bool bEHS = false)
        : CBaseChannelParam(eChannelMode)
    {
        m_bEHS = bEHS;
    }

    bool m_bEHS;
};

//初始化参数
class CBaseInitParam
{
public:
    CBaseInitParam(CHANNEL_MODE eChannelMode)
    {
        m_eChannelMode = eChannelMode;
    }
    virtual ~CBaseInitParam() {}
    CHANNEL_MODE m_eChannelMode;

protected:
    CBaseInitParam() {}
};

class CHeadsetInitParam : public CBaseInitParam
{
public:
    CHeadsetInitParam(CHANNEL_MODE eChannelMode, bool bEHS = false)
        : CBaseInitParam(eChannelMode)
    {
        m_bEHS = bEHS;
    }

    bool m_bEHS;
};

class CGroupChannelInitParam : public CBaseInitParam
{
public:
    CGroupChannelInitParam(CHANNEL_MODE eChannelMode, bool bForceOpen = false)
        : CBaseInitParam(eChannelMode)
    {
        m_bForceOpen = bForceOpen;
    }

    bool m_bForceOpen;
};

#endif //__VOICE_CHANNELPARAM_H__
