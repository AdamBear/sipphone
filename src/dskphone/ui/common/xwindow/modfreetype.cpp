#include "modfreetype.h"
#include "xwindow/xfreetypemanager.h"

#if IF_USE_FREETYPE_LIB
void FreeType_SetFontPath(const yl::string& strFontPath)
{
    g_FreeTypeManager.SetFontPath(strFontPath);
}

void FreeType_Init()
{
    g_FreeTypeManager.Init();
}

chSize FreeType_TextOutBound(const xFont& font, const yl::string& strText)
{
    return g_FreeTypeManager.TextBound(font, strText);
}

void FreeType_DrawText(xGraphic& graphic, const chRect& rcBound, const char* pszText, int nAlign,
                       const xFont& font, const xColor& color, const xRegion& regionClip)
{
    g_FreeTypeManager.DrawGraphicText(graphic, rcBound, pszText, nAlign, font, color, regionClip);
}

#endif // IF_USE_FREETYPE_LIB
