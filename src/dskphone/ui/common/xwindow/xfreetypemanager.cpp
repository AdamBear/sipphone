#include "xfreetypemanager.h"

#if IF_USE_FREETYPE_LIB
#include "commonapi/systemhelperapi.h"
#include "ylstringalgorithm.h"
#include "xwindowcommon.h"
#include "xgraphicproess.h"

//#define FREETYPE_DEBUG

#ifdef FREETYPE_DEBUG
#define XFREETYPE_DEBUG(...)        XWINDOW_INFO(__VA_ARGS__)
#else
#define XFREETYPE_DEBUG(...)
#endif

#ifdef FREETYPE_DETAIL_DEBUG
#define XFREETYPE_DETAIL_DEBUG(...)     XWINDOW_INFO(__VA_ARGS__)
#else
#define XFREETYPE_DETAIL_DEBUG(...)
#endif

namespace
{
#define MAX_CACHE_FACES_SIZE            0
#define MAX_CACHE_SIZES_SIZE            0
#define MAX_CACHE_NODES_SIZE            20 * 1000
#define SIZE_EACH_FONT_POINT            1  // Size is expressed as 1/64th of points.[pixel]

#define FONT_EXTEND_HEIGHT              0
#define FONT_OFFSET_Y                   0
}

xFreeTypeManager::xFreeTypeManager()
    : m_pFtLib(NULL)
    , m_nBaseLineOffset(0)
{
}

xFreeTypeManager::~xFreeTypeManager()
{
    Uninit();
}

IMPLEMENT_GETINSTANCE(xFreeTypeManager)

void xFreeTypeManager::Init()
{
    m_mapFaceID.clear();
    m_mapFtcSBit.clear();

    FT_Error iError = 0;

    iError = FT_Init_FreeType(&m_pFtLib);  /* 初始化FreeType库 */
    if (iError)   /* 当初始化库时发生了一个错误 */
    {
        m_pFtLib = NULL;
        XWINDOW_ERR("FT_Init_FreeType Failed[%d]!", iError);
        return;
    }

    iError = FTC_Manager_New(m_pFtLib, MAX_CACHE_FACES_SIZE, MAX_CACHE_SIZES_SIZE, MAX_CACHE_NODES_SIZE,
                             xFreeTypeManager::FaceRequesterCallBack, (FT_Pointer)NULL, &m_CacheManager);
    if (iError)
    {
        XWINDOW_ERR("FTC_Manager_New Failed[%d]!", iError);
        return;
    }

    iError = FTC_CMapCache_New(m_CacheManager, &m_CMapCache);
    if (iError)
    {
        XWINDOW_ERR("FTC_CMapCache_New Failed[%d]!", iError);
        return;
    }

    iError = FTC_SBitCache_New(m_CacheManager, &m_SbitCache);
    if (iError)
    {
        XWINDOW_ERR("FTC_SBitCache_New Failed[%d]!", iError);
    }

    InitFontList();
}

void xFreeTypeManager::Uninit()
{
    FTC_Manager_Done(m_CacheManager);

    /* 释放FreeType Lib占用的内存 */
    if (NULL != m_pFtLib)
    {
        FT_Done_FreeType(m_pFtLib);
    }
}

void xFreeTypeManager::SetFontPath(const yl::string& strFontPath)
{
    m_strFontPath = strFontPath;
}

void xFreeTypeManager::InitFontList()
{
    yl::string strExtName = "ttf";
    GetFontListInDirectory(m_mapFaceID, m_strFontPath, strExtName);
}

bool xFreeTypeManager::GetFontListInDirectory(MAP_FACE_ID_INFO& mapFaceIDInfo, const yl::string& strDirectory, const yl::string& strExtName)
{
    bool bRet = false;

    LIST_STRING listFileName;
    if (!commonAPI_getFilesInDirectoryByExt(&listFileName, strDirectory, strExtName, true))
    {
        return bRet;
    }

    LIST_STRING::iterator itr = listFileName.begin();
    for (; itr != listFileName.end(); ++itr)
    {
        const yl::string& strFontPath = *itr;
        yl::string::size_type nFound = strFontPath.find_last_of('/');
        if (yl::string::npos == nFound)
        {
            continue;
        }

        ST_FACE_ID_INFO stFaceIdInfo;
        yl::string strTrim = "." + strExtName;
        yl::string strFontName = strFontPath.substr(nFound + 1);
        strFontName.trim_right(strTrim);
        stFaceIdInfo.strFamily = strFontName;
        stFaceIdInfo.strFontPath = strFontPath;
        stFaceIdInfo.nFaceIndex = 0;            // 默认选择字体文件的第一种字体。

        mapFaceIDInfo.put(strFontName, stFaceIdInfo);
    }

    if (mapFaceIDInfo.size() > 0)
    {
        bRet = true;
    }

    return bRet;
}

bool xFreeTypeManager::GetFaceIDByName(const yl::string& strFontName, ST_FACE_ID_INFO& stFaceIdInfo)
{
    MAP_FACE_ID_INFO::iterator itr = m_mapFaceID.find(strFontName);
    if (itr != m_mapFaceID.end())
    {
        stFaceIdInfo = itr->second;
        return true;
    }
    return false;
}

bool xFreeTypeManager::GetCharIndexByFaceID(const ST_FACE_ID_INFO& stFaceIdInfo, wchar_t wChar, FTC_SBit* pFtcSBit)
{
    if (NULL == pFtcSBit)
    {
        return false;
    }

    int nGlyphIndex = FTC_CMapCache_Lookup(m_CMapCache, (FTC_FaceID)&stFaceIdInfo, 1, wChar);

    if (0 == nGlyphIndex)
    {
        XFREETYPE_DETAIL_DEBUG("FTC_CMapCache_Lookup failed, wChar[0x%x], stFaceIdInfo.strFamily[%s].", wChar, stFaceIdInfo.strFamily.c_str());
        return false;
    }

    int nFontsPointSize = m_CurFont.getPointSize();

    FTC_ImageTypeRec ftcImageRec;
    ftcImageRec.face_id = (FTC_FaceID)&stFaceIdInfo;
    ftcImageRec.width = nFontsPointSize * SIZE_EACH_FONT_POINT;
    ftcImageRec.height = nFontsPointSize * SIZE_EACH_FONT_POINT;
    ftcImageRec.flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP;

    int iError;
    FTC_Node pFtcNode = NULL;
    iError = FTC_SBitCache_Lookup(m_SbitCache, &ftcImageRec, nGlyphIndex, pFtcSBit, NULL);

    if (iError)
    {
        XFREETYPE_DEBUG("FTC_SBitCache_Lookup failed iError[%d], wChar[0x%x], stFaceIdInfo.strFamily[%s].",
                        iError, wChar, stFaceIdInfo.strFamily.c_str());
        return false;
    }
    else if (NULL == *pFtcSBit)
    {
        return false;
    }

    return true;
}

bool xFreeTypeManager::LoadAvailableCharIndex(wchar_t wChar, FTC_SBit* pFtcSBit)
{
    if (NULL == pFtcSBit)
    {
        return false;
    }

    // 默认字体
    MAP_FACE_ID_INFO::iterator itr = m_mapFaceID.begin();
    for (; itr != m_mapFaceID.end(); ++itr)
    {
        const ST_FACE_ID_INFO& facID = itr->second;
        if (facID.strFamily != m_stCurFaceID.strFamily && LoadCharIndexByFaceID(facID, wChar, pFtcSBit))
        {
            return true;
        }
    }
    return false;
}

chSize xFreeTypeManager::LineTextBound(const xFont& font, const yl::string& strText, int nSpace/* = 0*/)
{
    XFREETYPE_DETAIL_DEBUG("TextBound text[%s] begin.", strText.c_str());
    chSize szText;
    yl::string strFamilyName = font.getFamily();
    int nFontsPointSize = font.getPointSize();
    int nChIdx, nTextLen;
    int nStartX = 0;
    int nChWidth = 0;
    int nTextWidth = 0;

    m_CurFont = font;
    m_stCurFaceID.clear();
    GetFaceIDByName(strFamilyName, m_stCurFaceID);

    yl::wstring wStrText = yl::to_utf16(strText);
    nTextLen = wStrText.GetLength(); /* 记录字符的数量 */

    int nBgHeight = nFontsPointSize + FONT_EXTEND_HEIGHT; /* 背景图形的高度，这个高度要大于字体的高度 */
    for (nChIdx = 0; nChIdx < nTextLen; ++nChIdx)
    {
        /* 开始遍历unicode编码的字符串中的每个元素 */
        if (' ' == wStrText[nChIdx])
        {
            nChWidth   = (nFontsPointSize - 2) >> 1;
            nTextWidth = nStartX + nChWidth;
            nStartX += (nChWidth + nSpace); /* 画笔向右边移动 */
        }
        else
        {
            FTC_SBit pFtcSBitmap = NULL;
            bool bRet = LoadCharIndex(wStrText[nChIdx], &pFtcSBitmap);

            if (bRet && NULL != pFtcSBitmap)
            {
                nChWidth = pFtcSBitmap->xadvance;
                nTextWidth = nStartX + nChWidth;
                nStartX += (nChWidth + nSpace); /* 画笔向右边移动 */
                nBgHeight = chMAX(nBgHeight, pFtcSBitmap->height);
            }
            else
            {
                XWINDOW_ERR("FreeType2 LoadCharIndex bRet[%d], pFtcSBit[%p].", bRet, pFtcSBitmap);
            }
        }
    }

    szText = chSize(nTextWidth, nBgHeight);
    XFREETYPE_DETAIL_DEBUG("TextBound strText[%s], width[%d], height[%d].", strText.c_str(), nTextWidth, nBgHeight);

    return szText;
}


bool xFreeTypeManager::LoadCharIndexByFaceID(const ST_FACE_ID_INFO& stFaceIdInfo, wchar_t wChar, FTC_SBit* pFtcSBit)
{
    if (NULL == pFtcSBit)
    {
        return false;
    }

    if (stFaceIdInfo.strFamily.empty())
    {
        XFREETYPE_DETAIL_DEBUG("LoadFaceByFaceID stFaceIdInfo.strFamily is empty.");
        return false;
    }

    /* 从字体库文件中获取字形索引 */
    if (GetCharIndexByFaceID(stFaceIdInfo, wChar, pFtcSBit))
    {
        return true;
    }

    return false;
}

bool xFreeTypeManager::LoadCharIndex(wchar_t wChar, FTC_SBit* pFtcSBit)
{
    if (NULL == pFtcSBit)
    {
        return false;
    }

    if (LoadCharIndexByFaceID(m_stCurFaceID, wChar, pFtcSBit))
    {
        return true;
    }

    // 加载可用字符索引
    if (LoadAvailableCharIndex(wChar, pFtcSBit))
    {
        return true;
    }

    return false;
}

bool xFreeTypeManager::DrawGraphicText(xGraphic& graphicDst, const chRect& rcBound, const char* pszText, int nAlign,
                                       const xFont& font, const xColor& color, const xRegion& regionClip)
{
    if (NULL == pszText)
    {
        return false;
    }

    yl::string strText = pszText;


    if (strText.empty())
    {
        return false;
    }

    int nYOffset = 0;
    int iTmpAlign = nAlign & gravity_y_mask;
    int nLineHeight = 0;
    VEC_STRING vecLines = yl::string_split<yl::string::value_type>(strText, "\n", true);

    for (VEC_STRING::iterator itr = vecLines.begin(); itr != vecLines.end(); ++itr)
    {
        yl::string& strLine = *itr;
        nLineHeight += LineTextBound(font, strLine).cy;
    }

    if (0 != iTmpAlign)
    {
        switch (iTmpAlign)
        {
        case align_v_center:
            nYOffset = (rcBound.Height() - nLineHeight) >> 1;
            break;
        case align_bottom:
            nYOffset = rcBound.Height() - nLineHeight;
            break;
        default:
            break;
        }
    }

    int nLineTop = rcBound.top + nYOffset;

    FTC_SBit pFtcSBitmap = NULL;
    bool bRet = LoadCharIndex(L'D', &pFtcSBitmap);
    if (bRet && NULL != pFtcSBitmap)
    {
        m_nBaseLineOffset = (font.getPointSize() - pFtcSBitmap->height) / 2;
        m_nBaseLineOffset = chMAX(m_nBaseLineOffset, 0);
    }

    for (VEC_STRING::iterator itr = vecLines.begin(); itr != vecLines.end(); ++itr)
    {
        yl::string& strLine = *itr;

        if (nLineTop > rcBound.bottom)
        {
            // 竖直方向位置超出屏幕
            break;
        }

        XFREETYPE_DETAIL_DEBUG("DrawGraphicText : TextBound text[%s] begin.", strLine.c_str());
        chSize sizeLine = LineTextBound(font, strLine);
        XFREETYPE_DETAIL_DEBUG("DrawGraphicText : TextBound text[%s] end.", strLine.c_str());

        int nWidth =  sizeLine.cx;
        int nHeight = sizeLine.cy;

        iTmpAlign = nAlign & gravity_x_mask;
        int nXOffset = 0;

        if (0 != iTmpAlign)
        {
            switch (iTmpAlign)
            {
            case align_h_center:
                nXOffset = (rcBound.Width() - nWidth) >> 1;
                break;
            case align_right:
                nXOffset = rcBound.Width() - nWidth;
                break;
            default:
                break;
            }
        }

        nXOffset = rcBound.left + nXOffset;

        chPoint posOffset = chPoint(nXOffset, nLineTop);
        XFREETYPE_DEBUG("DrawGraphicText : string[%s] nWidth[%d], nHeight[%d], nXOffset[%d], nYOffset[%d], nLineTop[%d].",
                        pszText, nWidth, nHeight, nXOffset, nYOffset, nLineTop);

        int nChIdx, nTextLen;
        yl::wstring wStrText = yl::to_utf16(strLine);
        nTextLen = wStrText.GetLength(); /* 记录字符的数量 */
        int nStartX = 0;

        for (nChIdx = 0; nChIdx < nTextLen; ++nChIdx)
        {
            nStartX = DrawCharBitmapData(graphicDst, rcBound, posOffset, wStrText[nChIdx], font, color, regionClip);
            posOffset.OffsetPoint(nStartX, 0);
            if (posOffset.x > rcBound.right)
            {
                // 水平方向位置超出屏幕
                break;
            }
        }

        nLineTop += nHeight;
    };

    XFREETYPE_DETAIL_DEBUG("DrawGraphicText end.");
    return true;
}

int xFreeTypeManager::DrawCharBitmapData(xGraphic& dst, const chRect& rcBound, const chPoint& ptDst,
        wchar_t wChar, const xFont& font, const xColor& color,
        const xRegion& regionClip, int nSpace/* = 0*/)
{
    int nFontsPointSize = font.getPointSize();
    int nStartX = 0;
    int nChWidth = 0;
    int nTextWidth = 0;
    /* 开始遍历unicode编码的字符串中的每个元素  */
    if (' ' == wChar)
    {
        nChWidth   = (nFontsPointSize - 2) >> 1;
        nTextWidth = nStartX + nChWidth;
        nStartX += (nChWidth + nSpace); /* 画笔向右边移动 */
    }
    else
    {
        FTC_SBit pFtcSBitmap = NULL;

        bool bRet = LoadCharIndex(wChar, &pFtcSBitmap);

        if (bRet && NULL != pFtcSBitmap)
        {
            nChWidth = pFtcSBitmap->xadvance;
            nTextWidth = nStartX + nChWidth;
            nStartX += (nChWidth + nSpace); /* 画笔向右边移动 */

            int nDstLeft = ptDst.x + pFtcSBitmap->left;
            int nDstTop = ptDst.y + nFontsPointSize - m_nBaseLineOffset - pFtcSBitmap->top + FONT_OFFSET_Y;

            chRect rcDst(nDstLeft, nDstTop, nDstLeft + pFtcSBitmap->width, nDstTop + pFtcSBitmap->height);

            const typeRectList& listRect = regionClip.GetRectList();

            for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
            {
                chRect rcTarget = rcDst & (*it);

                if (rcTarget.IsEmpty())
                {
                    continue;
                }

                chPoint ptOffset(rcTarget.left - rcDst.left, rcTarget.top - rcDst.top);

                xGraphicOperate::fillPolygon(pFtcSBitmap->buffer, ptOffset, pFtcSBitmap->width,
                                             dst, rcTarget, color);
            }
        }
        else
        {
            XWINDOW_ERR("FreeType2 LoadCharIndex bRet[%d], pFtcSBit[%p].", bRet, pFtcSBitmap);
        }
    }
    return nStartX;
}

FT_Error xFreeTypeManager::FaceRequesterCallBack(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* pFaceID)
{
    if (NULL == pFaceID)
    {
        return FT_Err_Invalid_Argument;
    }

    ST_FACE_ID_INFO* pFace = (ST_FACE_ID_INFO*)face_id;   // simple typecase
    if (NULL == pFace)
    {
        return FT_Err_Invalid_Argument;
    }

    FT_Error iError = FT_New_Face(library, pFace->strFontPath.c_str(), pFace->nFaceIndex, pFaceID);
    if (!iError)
    {
        iError = FT_Select_Charmap(*pFaceID, FT_ENCODING_UNICODE);    /* 设定为UNICODE，默认的也是 */
    }

    return iError;
}

chSize xFreeTypeManager::TextBound(const xFont& font, const yl::string& strText, int nSpace /*= 0*/)
{
    int wTotal = 0;
    int hTotal = 0;
    int wLine = 0;
    chSize szLine;
    VEC_STRING vecLines = yl::string_split<yl::string::value_type>(strText, "\n", true);
    //处理每一行
    for (VEC_STRING::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::string& str = *begin;
        szLine = LineTextBound(font, str, nSpace);
        wTotal = chMAX(wTotal, szLine.cx);
        hTotal += szLine.cy;
    }
    return chSize(wTotal, hTotal);
}

#endif
