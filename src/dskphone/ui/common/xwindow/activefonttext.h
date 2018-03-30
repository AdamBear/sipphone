#ifndef _DYNAMIC_FONT_TEXT_H_
#define _DYNAMIC_FONT_TEXT_H_

#include "ylvector.h"
#include <ETLLib/ETLLib.hpp>

typedef YLVector<int> VEC_ACTIVE_FONT;
class CActiveFontText
{
public:
    CActiveFontText();
    ~CActiveFontText();
public:
    //在自动字体中选择适合的字体
    int GetFitFontInDynamicFont(const chRect& rcText, const yl::string& strText);
    void AddActiveFont(int nFontSize);
    void AddActiveFontVec(VEC_ACTIVE_FONT& vecActiveFonts);
    void DeleteActiveFont(int nFontSize);
    void ClearActiveFont();

private:
    bool IsOverLengthOrHeight(const chRect& rcText, const yl::string& strText, int nFontSize);
    chSize GetTextSize();

private:
    VEC_ACTIVE_FONT m_vecActiveFonts;            //自动字体列表
};
#endif //_DYNAMIC_FONT_TEXT_H_
