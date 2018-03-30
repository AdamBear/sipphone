#ifndef __XFONT_MANAGER_H__
#define __XFONT_MANAGER_H__

#include <ETLLib/ETLLib.hpp>
#include <singletonclassdefine.h>
#include <ylstl/ylhashmap.h>
#include <ylstl/ylstring.h>
#include "xGraphic.h"
#include <xwindow/xwindowdefine.h>

#if IF_USE_FREETYPE_LIB
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H


typedef struct FACE_ID_INFO
{
    int         nFaceIndex;
    yl::string  strFamily;
    yl::string  strFontPath;
    FACE_ID_INFO()
    {
        clear();
    }
    FACE_ID_INFO& operator = (const FACE_ID_INFO& stFaceIdInfo)
    {
        nFaceIndex = stFaceIdInfo.nFaceIndex;
        strFamily = stFaceIdInfo.strFamily;
        strFontPath = stFaceIdInfo.strFontPath;
        return *this;
    }

    void clear()
    {
        nFaceIndex = -1;
        strFamily = "";
        strFontPath = "";
    }

} ST_FACE_ID_INFO;


typedef struct SMALL_BITMAP_INFO
{
    FTC_Node    pNode;
    FTC_SBit    pFtcSBitMap;
    SMALL_BITMAP_INFO()
    {
        clear();
    }
    SMALL_BITMAP_INFO& operator = (const SMALL_BITMAP_INFO& stSBitInfo)
    {
        pNode = stSBitInfo.pNode;
        pFtcSBitMap = stSBitInfo.pFtcSBitMap;
        return *this;
    }

    void clear()
    {
        pNode = NULL;
        pFtcSBitMap = NULL;
    }

} ST_SMALL_BITMAP_INFO;

typedef YLHashMap<yl::string, ST_FACE_ID_INFO>          MAP_FACE_ID_INFO;
typedef YLHashMap<wchar_t, ST_SMALL_BITMAP_INFO>        MAP_SMALL_BITMAP_INFO;

class xFreeTypeManager
{
    DECLARE_SINGLETON_CLASS(xFreeTypeManager)

public:
    void Init();
    void Uninit();
    void SetFontPath(const yl::string& strFontPath);

    static FT_Error FaceRequesterCallBack(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* pFaceID);

public:
    chSize TextBound(const xFont& font, const yl::string& strText, int nSpace = 0);

    bool DrawGraphicText(xGraphic& graphicDst, const chRect& rcBound, const char* pszText, int nAlign,
                         const xFont& font, const xColor& color, const xRegion& regionClip);

private:
    void InitFontList();
    bool GetFontListInDirectory(MAP_FACE_ID_INFO& mapFaceIDInfo, const yl::string& strDirectory, const yl::string& strExtName);

    bool LoadCharIndex(wchar_t wChar, FTC_SBit* pFtcSBit);
    bool LoadAvailableCharIndex(wchar_t wChar, FTC_SBit* pFtcSBit);
    bool LoadCharIndexByFaceID(const ST_FACE_ID_INFO& stFaceIdInfo, wchar_t wChar, FTC_SBit* pFtcSBit);

    bool GetCharIndexByFaceID(const ST_FACE_ID_INFO& stFaceIdInfo, wchar_t wChar, FTC_SBit* pFtcSBit);
    bool GetFaceIDByName(const yl::string& strFontName, ST_FACE_ID_INFO& stFaceIdInfo);

    int DrawCharBitmapData(xGraphic& dst, const chRect& rcBound, const chPoint& ptDst,
                           wchar_t wChar, const xFont& font, const xColor& color,
                           const xRegion& regionClip, int nSpace = 0);
    chSize LineTextBound(const xFont& font, const yl::string& strText, int nSpace = 0);

private:
    yl::string          m_strFontPath;
    FT_Library          m_pFtLib;    /* 库的句柄  */

    FTC_Manager         m_CacheManager;
    FTC_CMapCache       m_CMapCache;
    FTC_SBitCache       m_SbitCache;

    ST_FACE_ID_INFO     m_stCurFaceID;
    xFont               m_CurFont;
    MAP_FACE_ID_INFO        m_mapFaceID;

    MAP_SMALL_BITMAP_INFO           m_mapFtcSBit;
    int                 m_nBaseLineOffset;
};

#define g_FreeTypeManager xFreeTypeManager::GetInstance()

#endif // __XFONT_MANAGER_H__

#endif // IF_USE_FREETYPE_LIB
