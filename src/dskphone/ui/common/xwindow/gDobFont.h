#ifndef __gDobFont_h__
#define __gDobFont_h__

#include <xwindow/xwindowdefine.h>
#include <ETLLib/ETLLib.hpp>
#include <ylstring.h>
#include "xGraphic.h"


#if IF_USE_DOB_FONT
struct dotCharGraphy;
struct dotGraphySet;
struct shareFont;

class XWINDOW_EXPORT dobFont
{
public:
    void setPixelSize(int pointSize);
    void setFamily(const yl::string& family);

    int getPixelSize() const
    {
        return m_nPixelSize;
    }

public:
    chSize graphyBound(WCHAR c);
    chSize textBound(LPCSTR pText, int nLength = -1);
    // layout text on center-left of rectBound
    bool layoutText32(LPCSTR pText, const BYTE* pColor, BYTE* pCanvas, const chSize& sizeCanvas,
                      const chPoint& ptTextBottomLeft, const chSize& szLine, const chRect& rectClip);
    bool layoutText32Bound(const chRect& rectTextBound, LPCSTR pText, int align, const BYTE* pColor,
                           BYTE* pCanvas, const chSize& sizeCanvas, const chRect& rectClip);
    bool layoutText32Bound(const chRect& rectTextBound, LPCSTR pText, int align, chPoint offset,
                           const BYTE* pColor, BYTE* pCanvas, const chSize& sizeCanvas,
                           const chRect& rectClip);
private:
    dotCharGraphy* graphyAt(WCHAR c);
public:
    dobFont();

    static void SetFontPath(const yl::string& strFontPath);
    static const yl::string& GetFontPath();

private:
    int                 m_nPixelSize;
    shareFont*          m_pFontData;
    dotGraphySet*       m_pCharSetTable;
    dotCharGraphy*      m_pCharTable;
    dotGraphySet*       m_pCacheSet;
    int                 m_nCacheOffset;
};

class dobFile : private chByteArray, public chFileStdio
{
public:
    dobFile();
    ~dobFile();

public:
    bool Open(LPCSTR lpszFileName, UINT nOpenFlags);
    UINT Read(void* lpBuf, int nItemSize, UINT nCount);
    void Close();
    LPBYTE GetBuffer();
    int GetBufferSize();

private:
    LPBYTE  m_pReadPoint;
    int     m_nSize;
};
#endif

#endif // __gDobFont_h__
