#ifndef __MOD_FREETYPE_H__
#define __MOD_FREETYPE_H__

#include <xwindow/xwindowdefine.h>
#include "xWindow.h"

#if IF_USE_FREETYPE_LIB

// 设置字体库所在的路径
void FreeType_SetFontPath(const yl::string& strFontPath);

void FreeType_Init();

chSize FreeType_TextOutBound(const xFont& font, const yl::string& strText);

void FreeType_DrawText(xGraphic& graphic, const chRect& rcBound, const char* pszText, int nAlign,
                       const xFont& font, const xColor& color, const xRegion& regionClip);

#endif // IF_USE_FREETYPE_LIB

#endif // __MOD_FREETYPE_H__
