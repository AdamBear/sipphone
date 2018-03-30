#include "xscreendevice.h"

#ifndef _WIN32
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include "xwindow/xgraphicproess.h"

xScreenDevice::xScreenDevice()
    : m_hFB(-1)
    , m_nBitsPerPixel(0)
#if !IF_ONLY_USE_GRADATION
    , m_nScreenSize(0)
    , m_nBufferSize(0)
#endif
    , m_nSyncType(SCREEN_SYNC_TYPE_NONE)
    , m_nWaitCmd(0)
    , m_nSyncCmd(0)
    , m_bFullScreenSync(false)
{
}

xScreenDevice::~xScreenDevice()
{
    Close();
}

bool xScreenDevice::Init(const char* pszFile)
{
    if (NULL == pszFile)
    {
        XWINDOW_ERR("NULL == pszFile in xScreenDevice::Init");
        return false;
    }

    chIniFileA file(pszFile);

    phraseConstTextA textSection(FB_SCREEN_PARAM_SECTION);
    phraseConstTextA textKeyDevName(FB_SCREEN_PARAM_KEY_DEVICE_NAME);
    phraseConstTextA textKeyMemName(FB_SCREEN_PARAM_KEY_MEM_NAME);

    chStringA strDevName = file.GetTextValue(textSection, textKeyDevName, "");
    chStringA strMemName = file.GetTextValue(textSection, textKeyMemName, "");

    if (strDevName == "" || strMemName == "")
    {
        XWINDOW_ERR("DevName:%s MemName:%s in xScreenDevice::Init", strDevName.c_str(), strMemName.c_str());
        return false;
    }

    m_strMemName = strMemName.c_str();

    phraseConstTextA textKeyFormat(FB_SCREEN_PARAM_KEY_FORMAT);
    phraseConstTextA textKeySync(FB_SCREEN_PARAM_KEY_SYNC);
    phraseConstTextA textKeyWaitCmd(FB_SCREEN_PARAM_KEY_WAIT_COMMAND);
    phraseConstTextA textKeySyncCmd(FB_SCREEN_PARAM_KEY_SYNC_COMMAND);
    phraseConstTextA textKeyRotate(FB_SCREEN_PARAM_KEY_ROTATE);
    phraseConstTextA textKeyFullScreenSync(FB_SCREEN_PARAM_KEY_FULLSCREEN_SYNC);

    m_nFormat = file.GetIntValue(textSection, textKeyFormat, GRAPHIC_FORMAT_BGRA32);
    m_nSyncType = file.GetIntValue(textSection, textKeySync, 0);
    m_nWaitCmd = file.GetIntValue(textSection, textKeyWaitCmd, 0);
    m_nSyncCmd = file.GetIntValue(textSection, textKeySyncCmd, 0);
#if IF_GRAPHIC_ROTATE
    int nRotate = file.GetIntValue(textSection, textKeyRotate, 0);

    xGraphicConverter::SetRotateAngle(nRotate);
#endif

    int nFullScreenSync = file.GetIntValue(textSection, textKeyFullScreenSync, 0);

    m_bFullScreenSync = (1 == nFullScreenSync);

    return Open(strDevName.c_str());
}

int xScreenDevice::GetScreenWidth()
{
    return m_nWidthPixels;
}

int xScreenDevice::GetScreenHeight()
{
    return m_nHeightPixels;
}

bool xScreenDevice::GetMemName(yl::string& strName)
{
    strName = m_strMemName;

    return true;
}

BYTE* xScreenDevice::GetCanvasBuffer(int& nBufferSize)
{
#if IF_ONLY_USE_GRADATION
    return NULL;
#else
    if (m_nBufferSize >= (m_nScreenSize << 1))
    {
        nBufferSize = m_nScreenSize;

        return (m_pPixmap + m_nScreenSize);
    }

    return NULL;
#endif
}

bool xScreenDevice::Open(LPCSTR lpszDevname)
{
    if (NULL == lpszDevname)
    {
        return false;
    }

    m_hFB = open(lpszDevname, O_RDWR);

    if (-1 == m_hFB)
    {
        XWINDOW_ERR("open(%s) fail!", lpszDevname);
        return false;
    }

    struct fb_var_screeninfo vinfo;

    memset(&vinfo, 0, sizeof(vinfo));

    /* Get fixed screen information */
#if IF_ONLY_USE_GRADATION
    int nResult = ioctl(m_hFB, FBIOGET_VSCREENINFO, &vinfo);

    if (0 == nResult)
    {
        m_nWidthPixels = vinfo.xres;
        m_nHeightPixels = vinfo.yres;
        m_nBitsPerPixel = vinfo.bits_per_pixel;

        int nSize = (m_nWidthPixels * m_nHeightPixels * m_nBitsPerPixel) >> 3;

        m_pPixmap = (BYTE*)mmap(0, nSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_hFB, 0);

        if ((POINTER)m_pPixmap != MAX_POINTER)
        {
            return true;
        }
    }

    XWINDOW_INFO("FBIOGET_VSCREENINFO's result=%d", nResult);
#else
    struct fb_fix_screeninfo finfo;

    memset(&finfo, 0, sizeof(finfo));

    int nResult1 = ioctl(m_hFB, FBIOGET_FSCREENINFO, &finfo);
    int nResult2 = ioctl(m_hFB, FBIOGET_VSCREENINFO, &vinfo);

    if (0 == nResult1 && 0 == nResult2)
    {
        chWARNING(vinfo.xoffset == 0 && vinfo.yoffset == 0);

        m_nBitsPerPixel = vinfo.bits_per_pixel;
        m_nWidthPixels = finfo.line_length * 8 / m_nBitsPerPixel;
        m_nHeightPixels = vinfo.yres;
        m_nScreenSize = m_nWidthPixels * m_nHeightPixels * (m_nBitsPerPixel >> 3);
        m_nBufferSize = finfo.smem_len;

        chWARNING(m_nWidthPixels == (int)vinfo.xres);
        chWARNING(m_nHeightPixels <= (int)(finfo.smem_len / finfo.line_length));

        m_pPixmap = (BYTE*)mmap(0, m_nBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_hFB, 0);

        if ((POINTER)m_pPixmap != MAX_POINTER)
        {
            memset(m_pPixmap, 0, m_nBufferSize);
            return true;
        }
    }

    XWINDOW_INFO("FBIOGET_FSCREENINFO's result=%d FBIOGET_VSCREENINFO's result=%d", nResult1, nResult2);
#endif

    return false;
}

void xScreenDevice::Close()
{
    if (-1 != m_hFB)
    {
        if (NULL != m_pPixmap)
        {
#if IF_ONLY_USE_GRADATION
            int nSize = (m_nWidthPixels * m_nHeightPixels * m_nBitsPerPixel) >> 3;

            munmap((void*)m_pPixmap, nSize);
#else
            munmap((void*)m_pPixmap, m_nBufferSize);
#endif
        }

        close(m_hFB);
    }

    m_hFB = -1;
    m_pPixmap = NULL;
    m_nWidthPixels = 0;
    m_nHeightPixels = 0;
    m_nBitsPerPixel = 0;
}

bool xScreenDevice::Flush(xGraphic& src, const chRect& rcFlush)
{
#if IF_ONLY_USE_GRADATION
    xGraphicOperate::tranGadation8B_2_1B(src, *this);
#else
    xGraphicOperate::quickCopy(src, *this, rcFlush, m_nScreenSize);
#endif

#if IF_SCREEN_SUPPORT_PARTLY_REFRESH
    m_rgnRefresh |= rcFlush;
#endif

    return true;
}

bool xScreenDevice::Flush(xGraphic& src, const xRegion& rgnFlush)
{
    const typeRectList& listRect = rgnFlush.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
#if !IF_ONLY_USE_GRADATION && IF_GRAPHIC_ROTATE
        chRect rcFlush = *it;

        xGraphicConverter::RotateRightAngle(rcFlush, m_nWidthPixels, m_nHeightPixels);

        Flush(src, rcFlush);
#else
        Flush(src, *it);
#endif
    }

    return true;
}

void xScreenDevice::OnBeginFlush()
{
    if (SCREEN_SYNC_TYPE_WAIT_VSYNC == (SCREEN_SYNC_TYPE_WAIT_VSYNC & m_nSyncType) && -1 != m_hFB)
    {
        int nValue = 0;

        ioctl(m_hFB, m_nWaitCmd, &nValue);
    }
}

void xScreenDevice::OnEndFlush()
{
    if (-1 == m_hFB)
    {
        return;
    }

    if (SCREEN_SYNC_TYPE_MANUAL_SYNC == (SCREEN_SYNC_TYPE_MANUAL_SYNC & m_nSyncType))
    {
#if IF_SCREEN_SUPPORT_PARTLY_REFRESH
        chRect rcBound = m_rgnRefresh.bound();
        struct fb_fillrect rcDirty;

        ioctl(m_hFB, m_nSyncCmd, &rcDirty);

        m_rgnRefresh.clear();
#else
        ioctl(m_hFB, m_nSyncCmd, NULL);
#endif
    }
}
#endif // _WIN32
