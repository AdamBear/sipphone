#ifndef __X_SCREEN_DEVICE_H__
#define __X_SCREEN_DEVICE_H__

#include "xwindow/xGraphic.h"
#include "fbscreendefine.h"
#if IF_CUSTOM_SCREEN_DEVICE
#include "iscreendevice.h"
#endif

#ifndef _WIN32
class xScreenDevice : public xGraphic
#if IF_CUSTOM_SCREEN_DEVICE
    , public IScreenDevice
#endif
{
public:
    xScreenDevice();
    ~xScreenDevice();

public:
    bool Init(const char* pszFile);

#if IF_CUSTOM_SCREEN_DEVICE
    virtual bool Open(LPCSTR lpszDevname);
    virtual void Close();

    virtual bool Flush(xGraphic& src, const chRect& rcFlush);
    virtual bool Flush(xGraphic& src, const xRegion& rgnFlush);

    virtual void OnBeginFlush();
    virtual void OnEndFlush();

    virtual int GetScreenWidth();
    virtual int GetScreenHeight();

    virtual bool GetMemName(yl::string& strName);

    virtual bool IsFullScreenSync()
    {
        return m_bFullScreenSync;
    }

    virtual int GetColorFormat()
    {
        return m_nFormat;
    }

    virtual BYTE* GetCanvasBuffer(int& nBufferSize);

#else
    bool Open(LPCSTR lpszDevname);
    void Close();

    bool Flush(xGraphic& src, const chRect& rcFlush);
    bool Flush(xGraphic& src, const xRegion& rgnFlush);

    void OnBeginFlush();
    void OnEndFlush();

    int GetScreenWidth();
    int GetScreenHeight();

    bool GetMemName(yl::string& strName);

    inline bool IsFullScreenSync()
    {
        return m_bFullScreenSync;
    }

    inline int GetColorFormat()
    {
        return m_nFormat;
    }

    BYTE* GetCanvasBuffer(int& nBufferSize);
#endif

protected:
    friend class xFrameBufferScreen;

    int         m_hFB;
    int         m_nBitsPerPixel;
#if !IF_ONLY_USE_GRADATION
    int         m_nScreenSize;
    int         m_nBufferSize;
#endif

    enum
    {
        SCREEN_SYNC_TYPE_NONE           = 0,
        SCREEN_SYNC_TYPE_WAIT_VSYNC     = 1,            // 同步前等待VSync信号
        SCREEN_SYNC_TYPE_MANUAL_SYNC    = 2,            // 发送命令控制同步FB到显示屏上
    };

    int         m_nSyncType;
    int         m_nWaitCmd;
    int         m_nSyncCmd;
    bool        m_bFullScreenSync;
    yl::string  m_strMemName;
#if IF_SCREEN_SUPPORT_PARTLY_REFRESH
    xRegion     m_rgnRefresh;
#endif
};
#endif // _WIN32

#endif
