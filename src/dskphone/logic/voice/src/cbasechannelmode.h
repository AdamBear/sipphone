///////////////////////////////////////////////////////////
//  CBaseChannelMode.h
//  Implementation of the Class CBaseChannelMode
//  Created on:      17-12月-2015 16:57:41
//  Original author: Song
///////////////////////////////////////////////////////////

#ifndef __VOICE_BASECHANNELMODE_H__
#define __VOICE_BASECHANNELMODE_H__
#include "ylstl/ylsmartptr.h"
#include "cbasechannelhandle.h"
#include "voicechannelparam.h"

//#define VOICE_INFO_DEBUG

#ifndef _DECT
class CBaseChannelParam;
class CBaseInitParam;

class CBaseChannelMode
{
public:
    CBaseChannelMode(CBaseInitParam * pParam);
    virtual ~CBaseChannelMode();

    //获取物理通道
    REAL_CHANNEL GetRealChannelParam();

    //切换通道
    bool SwitchChannel(REAL_CHANNEL eRealChannel);

    // 通道切换处理
    virtual bool ProcessSwitchChannel(REAL_CHANNEL eRealChannel);

    virtual bool BeforeSwitchChannel(REAL_CHANNEL eRealChannel);

    virtual bool AfterSwitchChannel(REAL_CHANNEL eRealChannel);

    //获取通道参数
    virtual void GenrateChannelParam(REAL_CHANNEL eRealChannel, CHANNEL_MODE eChannelMode);

    // 获取物理通道
    virtual REAL_CHANNEL GetRealChannel();

    // 判断是否同一初始化参数对象
    bool IsSameInitParam(CBaseInitParam * pInitParam);

    // 判断是否同一通话参数对象
    bool IsSameChannelHandleParam(CBaseChannelParam * pHandleParam);

    // 判断是否同一通话处理对象
    bool IsSameChannelHandle(CBaseChannelHandle * pHandle);

    virtual CBaseChannelParam * GetChannelParam()
    {
        return m_pChannelHandleParam;
    }

protected:
    // 获取物理通道处理类
    CBaseChannelHandle * GetRealChannelHandle(REAL_CHANNEL eRealChannel);

    CHANNEL_MODE CorrectChannelMode(REAL_CHANNEL eRealChannel, CHANNEL_MODE eMode);

    yl::string GetParamTypeString(CBaseChannelParam * pParam);

    //禁止默认构造函数
    CBaseChannelMode() {;}
protected:

    //初始化参数
    CBaseInitParam * m_pChannelInitParam;

    // 通道参数
    CBaseChannelParam * m_pChannelHandleParam;

    //物理通道处理类
    CBaseChannelHandle * m_pChannelHandle;

    CHANNEL_MODE m_eChannelMode;
};

#endif
#endif // __VOICE_BASECHANNELMODE_H__
