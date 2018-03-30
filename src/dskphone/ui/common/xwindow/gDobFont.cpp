#include "ETLLib/ETLLib.hpp"

#include "gDobFont.h"
#include "dsklog/log.h"
#include "commonapi/stringhelperapi.h"


#if IF_USE_DOB_FONT
#define FONT_FORMAT_VERSION             0x1001

//#pragma pack(2)
#pragma pack(1)
struct dotCharGraphy
{
    BYTE            byteWidth;
    BYTE            byteHeight;
    POINTER         offsetBitsData;
};

struct dotGraphySet
{
    WORD            wMinChar;
    WORD            wMaxChar;
};

struct shareFont
{
    WORD            wVersion;
    WORD            wSetCount;
    WORD            wCharCount;
    WORD            wForByteAlignment;
};
#pragma pack()
typedef YLList<dotGraphySet *> LIST_GRAPHY_SET;
typedef YLList<BYTE *> LIST_CHAR;

static shareFont nullFont = {FONT_FORMAT_VERSION, 0, 0};

//////////////////////////////////////////////////////////////////////////
dobFile::dobFile()
    : m_pReadPoint(NULL)
{
}

dobFile::~dobFile()
{
}

LPBYTE dobFile::GetBuffer()
{
    return m_pReadPoint;
}

int dobFile::GetBufferSize()
{
    return chByteArray::GetSize();
}

bool dobFile::Open(LPCSTR lpszFileName, UINT nOpenFlags)
{
    if (!chFileStdio::Open(lpszFileName, nOpenFlags))
    {
        return false;
    }

    UINT nFileSize = chFileStdio::SeekToEnd();
    chFileStdio::SeekToBegin();

    chByteBlock byteBlock;
    byteBlock.alloc(nFileSize);
    chByteArray::AttachBlock(byteBlock);

    chFileStdio::Read(chByteArray::GetBuffer(), nFileSize, 1);
    chFileStdio::Close();
    m_pReadPoint = chByteArray::GetBuffer();

    return true;
}

UINT dobFile::Read(void * lpBuf, int nItemSize, UINT nCount)
{
    chASSERT(m_pReadPoint != NULL);
    if (nCount == 0 || nItemSize == 0)
    {
        return nCount;   // avoid Win32 "null-read"
    }

    chASSERT(lpBuf != NULL);
    UINT nReadBytes = nItemSize * nCount;
    if (m_pReadPoint - chByteArray::GetBuffer() + nReadBytes <= (UINT)chByteArray::GetSize())
    {
        memcpy(lpBuf, m_pReadPoint, nReadBytes);
        m_pReadPoint += nReadBytes;
        return nCount;
    }
    return 0;
}

void dobFile::Close()
{
    chByteArray::RemoveAll();
    m_pReadPoint = NULL;
}

//////////////////////////////////////////////////////////////////////////
class dobFontHost : protected chMapFile
{
    friend class dobFontManager;
public:
    dobFontHost();
    ~dobFontHost();

    shareFont * LoadShareFont(int nPixelSize);

private:
    shareFont * loadFontFromFile(LPCSTR lpszFilePath, LPCSTR lpszShareName);
};
typedef YLHashMap<int, dobFontHost *> MAP_DOB_FONT_HOST;

dobFontHost::dobFontHost()
{
}

dobFontHost::~dobFontHost()
{
    chMapFile::CloseMapFile();
}

shareFont * dobFontHost::LoadShareFont(int nPixelSize)
{
    shareFont * pShareFont = (shareFont *)chMapFile::getData();
    // already loaded return the share data direct
    if (pShareFont != NULL)
    {
        return pShareFont;
    }

    yl::string strShareName;

#if IF_USE_FAST_FONT
    strShareName = commonAPI_FormatString("fast-font-%.d", nPixelSize);
#else
    strShareName = commonAPI_FormatString("font-%d", nPixelSize);
#endif

    pShareFont = (shareFont *)chMapFile::OpenMapFile(strShareName.c_str());

    if (pShareFont != NULL)
    {
        chASSERT(pShareFont != NULL);
        chASSERTx(pShareFont->wVersion == FONT_FORMAT_VERSION, "version=%x expect=%x", pShareFont->wVersion, FONT_FORMAT_VERSION);
    }
    else
    {
        yl::string strDobFile = dobFont::GetFontPath();

#if IF_USE_FAST_FONT
        strDobFile += commonAPI_FormatString("ext/Font-Tahoma-%.2d.dob", nPixelSize);
#else
        strDobFile += commonAPI_FormatString("Font-%d.1b", nPixelSize);
#endif

        pShareFont = loadFontFromFile(strDobFile.c_str(), strShareName.c_str());
    }
    return pShareFont != NULL ? pShareFont : &nullFont;
}

shareFont * dobFontHost::loadFontFromFile(LPCSTR lpszFilePath, LPCSTR lpszShareName)
{
    dobFile file;

    if (!file.Open(lpszFilePath, chFileStdio::modeRead | chFileStdio::typeBinary))
    {
        XWINDOW_WARN("Can NOT open FONT %s", lpszFilePath);

        return NULL;
    }

    int nFileSize = file.GetBufferSize();
    if (nFileSize <= 0)
    {
        XWINDOW_WARN("FONT %s is empty", lpszFilePath);

        return NULL;
    }

    LPBYTE pDataBegin = file.GetBuffer();
    LPBYTE pDataEnd = pDataBegin + nFileSize;
    dotGraphySet * pSet = (dotGraphySet *)pDataBegin;
    int nTotalGraphyBytes = 0;
    LIST_GRAPHY_SET lsGraphySet;
    LIST_CHAR lsChar;

    while ((LPBYTE)pSet < pDataEnd)
    {
        chASSERT((LPBYTE)(pSet + 1) < pDataEnd);
        // 计算set的个数
        pSet->wMinChar = chBE2Host(pSet->wMinChar);
        pSet->wMaxChar = chBE2Host(pSet->wMaxChar);

        // 计算graphy char的个数
        int nCharCount = pSet->wMaxChar - pSet->wMinChar + 1;

        chASSERT(nCharCount > 0);

        BYTE * pChar = (LPBYTE)(pSet + 1);
        for (int i = 0; i < nCharCount; i++)
        {
            chASSERT(pChar + 2 < pDataEnd);
            int nBitCount = pChar[0] * pChar[1];
            // (8 - 1)
            int nByteCeil = nBitCount + 7;
            // ((nBitCount + 8 - 1) - ((nBitCount + 8 - 1) % 8)) / 8
            int nByteCount = ((nByteCeil - (nByteCeil & 7)) >> 3);

            lsChar.push_back(pChar);
#if IF_USE_FAST_FONT
            int nNewByteCount = chCEIL(pChar[1], 8) / 8 * pChar[0];
            nTotalGraphyBytes += nNewByteCount;
#else
            nTotalGraphyBytes += nByteCount;
#endif
            pChar += (2 + nByteCount);
        }
        lsGraphySet.push_back(pSet);
        pSet = (dotGraphySet *)pChar;
    }
    chASSERT((LPBYTE)pSet == pDataEnd);

    int nGraphySetSize = lsGraphySet.size();
    int nCharSize = lsChar.size();

    int nShareBytes = sizeof(shareFont) + sizeof(dotGraphySet) * nGraphySetSize
                      + sizeof(dotCharGraphy) * nCharSize + nTotalGraphyBytes;
    // Load to share memory

    shareFont * pShareFont = (shareFont *)chMapFile::OpenMapFile(lpszShareName, nShareBytes);

    chASSERT(chMapFile::IsCreateByMe());

    pShareFont->wVersion = FONT_FORMAT_VERSION;
    pShareFont->wSetCount = nGraphySetSize;
    pShareFont->wCharCount = nCharSize;

    pSet = (dotGraphySet *)(pShareFont + 1);

    LIST_GRAPHY_SET::iterator iterSet = lsGraphySet.begin();

    for (; iterSet != lsGraphySet.end(); ++iterSet)
    {
        *(pSet++) = *(*iterSet);
    }

    dotCharGraphy * pChar = (dotCharGraphy *)pSet;
    LPBYTE pGraphyData = (LPBYTE)pSet + sizeof(dotCharGraphy) * nCharSize;
    LIST_CHAR::iterator iterChar = lsChar.begin();

    for (; iterChar != lsChar.end(); ++iterChar)
    {
        LPBYTE pSrc = *iterChar;
        pChar->byteWidth = pSrc[0];
        pChar->byteHeight = pSrc[1];
        pChar->offsetBitsData = (POINTER)pGraphyData - (POINTER)pChar;

#if IF_USE_FAST_FONT
        int nNewByteCount = chCEIL(pSrc[1], 8) / 8 * pSrc[0];
        for (int y = 0; y < pChar->byteHeight; ++y)
        {
            int indexBits = pChar->byteWidth * y;
            LPBYTE pNewBits = pGraphyData + (y / 8) * pChar->byteWidth;
            BYTE maskNew = 1 << (y % 8);

            for (int x = 0; x < pChar->byteWidth; ++x)
            {
                LPBYTE pLineBits = pSrc + 2 + (indexBits + x) / 8;
                BYTE mask = 1 << (7 - (indexBits + x) % 8);

                chSET_BITS(*pNewBits, maskNew, *pLineBits & mask);
                ++pNewBits;
            }
        }
        pGraphyData += nNewByteCount;
#else
        int nBitCount = pSrc[0] * pSrc[1];
        // (8 - 1)
        int nByteCeil = nBitCount + 7;
        // ((nBitCount + 8 - 1) - ((nBitCount + 8 - 1) % 8)) / 8
        int nByteCount = ((nByteCeil - (nByteCeil & 7)) >> 3);
        memcpy(pGraphyData, pSrc + 2, nByteCount);
        pGraphyData += nByteCount;
#endif
        ++pChar;
    }

    return pShareFont;
}

//////////////////////////////////////////////////////////////////////////
class dobFontManager
{
    friend class dobFont;

public:
    dobFontManager();
    ~dobFontManager();

    static dobFontManager & GetInstance();

    shareFont * getShareFont(int nPixelSize);

protected:
    MAP_DOB_FONT_HOST   m_mapDobFontHost;
    yl::string          m_strFontPath;
};
#define g_DobFontMgr dobFontManager::GetInstance()

dobFontManager::dobFontManager()
{
}

dobFontManager::~dobFontManager()
{
    MAP_DOB_FONT_HOST::iterator iter = m_mapDobFontHost.begin();

    for (; iter != m_mapDobFontHost.end(); ++iter)
    {
        dobFontHost * pFontHost = iter->second;

        if (NULL != pFontHost)
        {
            iter->second = NULL;

            delete pFontHost;
        }
    }

    m_mapDobFontHost.clear();
}

dobFontManager & dobFontManager::GetInstance()
{
    static dobFontManager s_objFontManager;

    return s_objFontManager;
}

shareFont * dobFontManager::getShareFont(int nPixelSize)
{
    MAP_DOB_FONT_HOST::iterator iter = m_mapDobFontHost.find(nPixelSize);
    dobFontHost * pFontHost = NULL;

    if (iter != m_mapDobFontHost.end())
    {
        pFontHost = iter->second;

        if (NULL == pFontHost)
        {
            return NULL;
        }
    }
    else
    {
        pFontHost = new dobFontHost;

        if (NULL == pFontHost)
        {
            return NULL;
        }

        m_mapDobFontHost[nPixelSize] = pFontHost;
    }

    return pFontHost->LoadShareFont(nPixelSize);
}

//////////////////////////////////////////////////////////////////////////
dobFont::dobFont()
{
    m_nPixelSize = -1;
    m_pFontData = NULL;
    m_pCharSetTable = NULL;
    m_pCharTable = NULL;
    m_pCacheSet = NULL;
    m_nCacheOffset = 0;
}

void dobFont::SetFontPath(const yl::string & strFontPath)
{
    g_DobFontMgr.m_strFontPath = strFontPath;
}

const yl::string & dobFont::GetFontPath()
{
    return g_DobFontMgr.m_strFontPath;
}

void dobFont::setPixelSize(int pixelSize)
{
    m_nPixelSize = pixelSize;
    m_pFontData = g_DobFontMgr.getShareFont(pixelSize);
    m_pCharSetTable = (dotGraphySet *)(m_pFontData + 1);
    m_pCharTable = (dotCharGraphy *)((LPBYTE)m_pCharSetTable + sizeof(dotGraphySet) * m_pFontData->wSetCount);
    m_pCacheSet = NULL;
    m_nCacheOffset = 0;
}

void dobFont::setFamily(const yl::string &)
{

}

dotCharGraphy * dobFont::graphyAt(WCHAR wWord)
{
    chASSERT(m_pCharSetTable != NULL && m_pCharTable != NULL);

    dotGraphySet * pSet = m_pCharSetTable;
    dotCharGraphy * pChar = m_pCharTable;
    if (m_pCacheSet != NULL && wWord <= (int)m_pCacheSet->wMaxChar && wWord >= (int)m_pCacheSet->wMinChar)
    {
        pChar += m_nCacheOffset;
        pChar += ((UINT)wWord - m_pCacheSet->wMinChar);
        return pChar;
    }

    int nSetCount = m_pFontData->wSetCount;
    int iOffsetOfChar = 0;
    for (int i = 0; i < nSetCount; i++)
    {
        if (wWord <= (int)pSet->wMaxChar && wWord >= (int)pSet->wMinChar)
        {
            m_pCacheSet = pSet;
            m_nCacheOffset = iOffsetOfChar;

            iOffsetOfChar += (UINT)wWord - pSet->wMinChar;
            pChar += iOffsetOfChar;
            return pChar;
        }
        else
        {
            iOffsetOfChar += (pSet->wMaxChar - pSet->wMinChar + 1);
        }
        ++ pSet;
    }

    chASSERT(wWord != 0x003F);
    return graphyAt(0x003F);
}

chSize dobFont::graphyBound(WCHAR c)
{
    dotCharGraphy * pChar = graphyAt(c);
    return chSize(pChar->byteWidth, pChar->byteHeight);
}

static WORD UTF8Inc(LPCSTR & lpszTextA)
{
    WORD result = 0xFFFF;
    if (*lpszTextA != '\0')
    {
        BYTE byte = *lpszTextA;
        if ((byte >> 7) == 0x00)
        {
            DWORD dw0 = (DWORD)(BYTE) * lpszTextA;
            result = (WCHAR)dw0;
            lpszTextA += 1;
        }
        else if ((byte >> 5) == 0x06)
        {
            DWORD dw0 = (DWORD)(BYTE)lpszTextA[0];
            DWORD dw1 = (DWORD)(BYTE)lpszTextA[1];
            result = (WCHAR)((dw1 & 0x3F) | ((dw0 & 0x1F) << 6));
            lpszTextA += 2;
        }
        else if ((byte >> 4) == 0x0E)
        {
            DWORD dw0 = (DWORD)(BYTE)lpszTextA[0];
            DWORD dw1 = (DWORD)(BYTE)lpszTextA[1];
            DWORD dw2 = (DWORD)(BYTE)lpszTextA[2];
            result = (WCHAR)((dw2 & 0x3F) | ((dw1 & 0x3F) << 6) | ((dw0 & 0x0F) << 12));
            lpszTextA += 3;
        }
        else
        {
            //chASSERT(FALSE);
            result = '?';
            lpszTextA += 1;
        }
    }
    return result;
}

chSize dobFont::textBound(LPCSTR pText, int nLength)
{
    int wTotal = 0;
    int hTotal = 0;
    int wLine = 0;
    int hLine = m_nPixelSize;
    LPCSTR pTextEnd = pText + nLength;
    for (UINT nCount = 0; *pText != 0 && nCount < (UINT)nLength; nCount ++)
    {
        if (nLength >= 0
                && pText >= pTextEnd)
        {
            break;
        }

        if (*pText == '\n')
        {
            wTotal = chMAX(wTotal, wLine);
            hTotal += hLine;
            wLine = 0;
            hLine = m_nPixelSize;
            pText ++;
        }
        else
        {
            dotCharGraphy * pChar = graphyAt(UTF8Inc(pText));
            wLine += pChar->byteWidth;
            hLine = chMAX(hLine, pChar->byteHeight);
        }
    }
    wTotal = chMAX(wTotal, wLine);
    hTotal += hLine;
    return chSize(wTotal, hTotal);
}

// layout text on center-left of rectBound
bool dobFont::layoutText32(LPCSTR pText, const BYTE* pColor, BYTE* pCanvas, const chSize& sizeCanvas,
                           const chPoint& ptTextBottomLeft, const chSize& szLine, const chRect& rectClip)
{
    if (textEMPTY(pText))
    {
        return true;    // text is empty
    }

    chRect rectTextClip = chRect(chPoint(0, 0), sizeCanvas) & rectClip;
    if (rectTextClip.IsEmpty() || ptTextBottomLeft.y <= rectTextClip.top || ptTextBottomLeft.x >= rectTextClip.right)
    {
        // all text cliped out
        return true;
    }
    chPoint pt = ptTextBottomLeft;
    int nCanvasHeight = szLine.cy;
    int nLastCharHeight = 0;
    int nCharTop = 0;
    int nCharBottom = 0;

    while (*pText != 0 && *pText != '\n')
    {
        dotCharGraphy * pChar = graphyAt(UTF8Inc(pText));
        LPBYTE pBits = (LPBYTE)pChar + pChar->offsetBitsData;

        if (nLastCharHeight != pChar->byteHeight)
        {
            nLastCharHeight = pChar->byteHeight;

            int nOffset = (nCanvasHeight - nLastCharHeight) >> 1;

            nCharTop = pt.y - nLastCharHeight - nOffset;
            nCharBottom = pt.y - nOffset;
        }

        chRect rectOnCanvas = chRect(pt.x, nCharTop, pt.x + pChar->byteWidth, nCharBottom) & rectTextClip;
        chRect rectOnText = OffsetRect(rectOnCanvas, -pt.x, -nCharTop);
        int w = rectOnText.Width();
        int h = rectOnText.Height();
#if IF_USE_FAST_FONT
        for (int y = 0; y < h; ++y)
        {
            int yOnText = y + rectOnText.top;
            LPBYTE pLineBits = pBits + yOnText / 8 * pChar->byteWidth + rectOnText.left;

            int yOnCanvas = y + rectOnCanvas.top;
            BYTE* pPixel = pCanvas + sizeCanvas.cx * yOnCanvas + rectOnCanvas.left;

            BYTE mask = 1 << (yOnText % 8);
            for (int x = 0; x < w; ++x)
            {
                if (*pLineBits & mask)
                {
                    *pPixel = *pColor;
                }
                ++pPixel;
                ++pLineBits;
            }
        }
#else
        for (int y = 0; y < h; ++y)
        {
            int yOnText = y + rectOnText.top;

            int yOnCanvas = y + rectOnCanvas.top;
            BYTE* pPixel = pCanvas + sizeCanvas.cx * yOnCanvas + rectOnCanvas.left;

            int indexBits = pChar->byteWidth * yOnText + rectOnText.left;
            for (int x = 0; x < w; ++x)
            {
                LPBYTE pLineBits = pBits + ((indexBits + x) >> 3);// ((indexBits+x) / 8)
                BYTE mask = 1 << (7 - (indexBits + x) % 8);

                if (*pLineBits & mask)
                {
                    *pPixel = *pColor;
                }
                ++pPixel;
            }
        }
#endif
        pt.x += pChar->byteWidth;
    }
    return true;
}

static int alignOffset(const int widthText, const int aligh, int widthBound)
{
    int xOffset = 0;
    switch (aligh & 0x0f)
    {
    case 0x02:
        {
            int diff = widthBound - widthText;
            xOffset = diff / 2 + diff % 2;
            break;
        }
    case 0x04:
        xOffset = widthBound - widthText;
        break;
    default:
        xOffset = 0;
    }
    return xOffset;
}

bool dobFont::layoutText32Bound(const chRect& rectTextBound, LPCSTR pText, int align, const BYTE* pColor,
                                BYTE* pCanvas, const chSize& sizeCanvas, const chRect& rectClip)
{
    return layoutText32Bound(rectTextBound, pText, align, chPoint(0, 0), pColor, pCanvas, sizeCanvas, rectClip);
}

bool dobFont::layoutText32Bound(const chRect& rectTextBound, LPCSTR pText, int align, chPoint offset,
                                const BYTE* pColor, BYTE* pCanvas, const chSize& sizeCanvas,
                                const chRect& rectClip)
{
    LPCSTR lpszLineBegin = pText;
    int yLineTop = rectTextBound.top;
    if (!textEMPTY(pText))
        do
        {
            LPCSTR lpszEnding = ACC::chFind(lpszLineBegin, '\n');
            chSize sizeLine;
            if (lpszEnding == NULL)
            {
                sizeLine = textBound(lpszLineBegin);
            }
            else
            {
                sizeLine = textBound(lpszLineBegin, lpszEnding - lpszLineBegin);
            }
            int xOffset = alignOffset(sizeLine.cx, align, rectTextBound.Width());
            chPoint ptBottomLeft = chPoint(rectTextBound.left + xOffset - offset.x, yLineTop + sizeLine.cy - offset.y);

            layoutText32(lpszLineBegin, pColor, pCanvas, sizeCanvas, ptBottomLeft, sizeLine, rectClip);

            if (lpszEnding != NULL)
            {
                lpszLineBegin = lpszEnding + 1;
                yLineTop += sizeLine.cy;
            }
            else
            {
                return true;
            }
        }
        while (true);
    return true;
}
#endif
