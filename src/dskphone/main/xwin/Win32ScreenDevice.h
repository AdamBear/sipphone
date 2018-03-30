#ifndef __WIN32_SCREEN_DEVICE_H__
#define __WIN32_SCREEN_DEVICE_H__

#include "fbscreen/include/iscreendevice.h"


#if IF_CUSTOM_SCREEN_DEVICE && defined(_WIN32)
class CWin32ScreenDevice : public IScreenDevice
{
public:
    CWin32ScreenDevice();
    virtual ~CWin32ScreenDevice();

public:
    virtual bool Open(LPCSTR lpszDevname);
    virtual void Close();

    virtual bool Flush(xGraphic& src, const chRect& rcFlush);
    virtual bool Flush(xGraphic& src, const xRegion& rgnFlush);

    virtual void OnBeginFlush();
    virtual void OnEndFlush();

    virtual bool IsFullScreenSync();
    virtual int GetScreenWidth();
    virtual int GetScreenHeight();
    virtual bool GetMemName(yl::string& strName);

    void DrawScreen(HDC hDC);

protected:
    void CopyPixel2RGB(const xPixel* pPixel, char* pBits, const chRect& rcSrc);

protected:
    xRegion     m_rgnRefresh;

    BITMAPINFO  m_bmInfo;
    char*       m_pBits;
    int         m_nLineBits;
    int         m_nWidth;
    int         m_nHeight;
};
#endif

#endif
