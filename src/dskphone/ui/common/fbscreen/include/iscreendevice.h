#ifndef __I_SCREEN_DEVICE_H__
#define __I_SCREEN_DEVICE_H__

#include <xwindow/xscreendefine.h>
#include "xwindow/xGraphic.h"

#if IF_CUSTOM_SCREEN_DEVICE
class IScreenDevice
{
public:
    IScreenDevice() {}
    virtual ~IScreenDevice() {}

public:
    virtual bool Open(LPCSTR lpszDevname)                       = 0;
    virtual void Close()                                        = 0;

    virtual bool Flush(xGraphic& src, const chRect& rcFlush)    = 0;
    virtual bool Flush(xGraphic& src, const xRegion& rgnFlush)  = 0;

    virtual void OnBeginFlush()                                 = 0;
    virtual void OnEndFlush()                                   = 0;

    virtual bool IsFullScreenSync()                             = 0;
    virtual int GetScreenWidth()                                = 0;
    virtual int GetScreenHeight()                               = 0;
    virtual bool GetMemName(yl::string& strName)                = 0;
    virtual int GetColorFormat()                                = 0;
    virtual BYTE* GetCanvasBuffer(int& nBufferSize)             = 0;
};
#endif

#endif
