#include "Win32ScreenDevice.h"


#if IF_CUSTOM_SCREEN_DEVICE && defined(_WIN32)
extern HWND g_hMainWnd;

CWin32ScreenDevice::CWin32ScreenDevice()
    : m_nWidth(0)
    , m_nHeight(0)
    , m_pBits(NULL)
    , m_nLineBits(0)
{
    ::memset(&m_bmInfo, 0, sizeof(m_bmInfo));

    m_bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
}

CWin32ScreenDevice::~CWin32ScreenDevice()
{
    Close();
}

bool CWin32ScreenDevice::Open(LPCSTR lpszDevname)
{
    if (nullptr == g_hMainWnd)
    {
        return false;
    }

    RECT rcMain;

    ::GetClientRect(g_hMainWnd, &rcMain);

    m_nWidth = rcMain.right - rcMain.left;
    m_nHeight = rcMain.bottom - rcMain.top;

    m_bmInfo.bmiHeader.biWidth = m_nWidth;
    m_bmInfo.bmiHeader.biHeight = m_nHeight;
    m_bmInfo.bmiHeader.biBitCount = 24;
    m_bmInfo.bmiHeader.biCompression = BI_RGB;
    m_bmInfo.bmiHeader.biPlanes = 1;
    m_bmInfo.bmiHeader.biClrUsed = 0;
    m_bmInfo.bmiHeader.biClrImportant = 0;

    m_nLineBits = (m_nWidth * m_bmInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;

    m_bmInfo.bmiHeader.biSizeImage = m_nLineBits * m_nHeight;

    m_pBits = new char[m_bmInfo.bmiHeader.biSizeImage];

    memset((void*)m_pBits, 0, m_bmInfo.bmiHeader.biSizeImage);

    return true;
}

void CWin32ScreenDevice::Close()
{
    if (NULL != m_pBits)
    {
        delete[] m_pBits;

        m_pBits = NULL;
    }
}

bool CWin32ScreenDevice::Flush(xGraphic& src, const chRect& rcFlush)
{
    const xPixel* pData = src.pixels();

    if (NULL == pData || NULL == m_pBits || m_nWidth <= 0 || m_nHeight <= 0)
    {
        return false;
    }

    CopyPixel2RGB(pData, m_pBits, rcFlush);

    m_rgnRefresh |= rcFlush;

    return true;
}

bool CWin32ScreenDevice::Flush(xGraphic& src, const xRegion& rgnFlush)
{
    const xPixel* pData = src.pixels();

    if (NULL == pData || NULL == m_pBits || m_nWidth <= 0 || m_nHeight <= 0)
    {
        return false;
    }

    const typeRectList& listRect = rgnFlush.GetRectList();

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        const chRect& rcFlush = *it;

        CopyPixel2RGB(pData, m_pBits, rcFlush);

        m_rgnRefresh |= rcFlush;
    }

    return true;
}

void CWin32ScreenDevice::CopyPixel2RGB(const xPixel* pPixel, char* pBits, const chRect& rcSrc)
{
    if (NULL == pPixel || NULL == pBits || rcSrc.left >= m_nWidth || rcSrc.top >= m_nHeight)
    {
        return;
    }

    int nX = rcSrc.left;
    int nY = rcSrc.top;

    int nRight = rcSrc.right;

    if (nRight > m_nWidth)
    {
        nRight = m_nWidth;
    }

    int nBottom = rcSrc.bottom;

    if (nBottom > m_nHeight)
    {
        nBottom = m_nHeight;
    }

    pPixel = pPixel + m_nWidth * nY + nX;
    pBits = pBits + m_nLineBits * (m_nHeight - nY - 1) + nX * 3;

    for (int i = nY; i < nBottom; ++i)
    {
        const xPixel* pLinePixel = pPixel;
        char* pLineBits = pBits;

        for (int j = nX; j < nRight; ++j)
        {
            *pLineBits = pLinePixel->b;
            *(pLineBits + 1) = pLinePixel->g;
            *(pLineBits + 2) = pLinePixel->r;
            pLineBits += 3;
            pLinePixel += 1;
        }

        pPixel += m_nWidth;
        pBits -= m_nLineBits;
    }
}

void CWin32ScreenDevice::OnBeginFlush()
{

}

void CWin32ScreenDevice::OnEndFlush()
{
    const chRect& rcBound = m_rgnRefresh.bound();
    RECT rcRefresh;

    rcRefresh.left = rcBound.left;
    rcRefresh.top = rcBound.top;
    rcRefresh.right = rcBound.right;
    rcRefresh.bottom = rcBound.bottom;

    m_rgnRefresh.clear();

    ::InvalidateRect(g_hMainWnd, &rcRefresh, TRUE);
}

bool CWin32ScreenDevice::IsFullScreenSync()
{
    return false;
}

int CWin32ScreenDevice::GetScreenWidth()
{
    return m_nWidth;
}

int CWin32ScreenDevice::GetScreenHeight()
{
    return m_nHeight;
}

bool CWin32ScreenDevice::GetMemName(yl::string& strName)
{
    strName = "sip_phone_win32_screen";

    return true;
}

void CWin32ScreenDevice::DrawScreen(HDC hDC)
{
    if (NULL == m_pBits)
    {
        return;
    }

    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hDC, &m_bmInfo, DIB_RGB_COLORS, &pBits, NULL, 0);

    memcpy((char*)pBits, m_pBits, m_bmInfo.bmiHeader.biSizeImage);

    HDC hMemDC = CreateCompatibleDC(hDC);

    ::SelectObject(hMemDC, hBitmap);

    BitBlt(hDC, 0, 0, m_bmInfo.bmiHeader.biWidth, m_bmInfo.bmiHeader.biHeight, hMemDC, 0, 0, SRCCOPY);

    ::DeleteObject(hBitmap);
    ::ReleaseDC(NULL, hMemDC);
}

#endif
