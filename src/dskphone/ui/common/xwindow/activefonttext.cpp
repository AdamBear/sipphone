#include "activefonttext.h"
#include "ylstringalgorithm.h"
#include "ylalgorithm.h"
#include "commonapi/stringhelperapi.h"
#include "xGraphic.h"

CActiveFontText::CActiveFontText()
{
    m_vecActiveFonts.clear();
}

CActiveFontText::~CActiveFontText()
{

}

int CActiveFontText::GetFitFontInDynamicFont(const chRect& rcText, const yl::string& strText)
{
    int nFontSize = m_vecActiveFonts.size();
    int nFontIndex = -1;

    for (int i = 0; i < nFontSize; ++i)
    {
        bool bOver = IsOverLengthOrHeight(rcText, strText, m_vecActiveFonts[i]);
        if (bOver)
        {
            break;
        }

        nFontIndex = i;
    }

    if (nFontIndex < nFontSize && nFontIndex >= 0)
    {
        return m_vecActiveFonts[nFontIndex];
    }

    return -1;
}

void CActiveFontText::AddActiveFont(int nFontSize)
{
    if (nFontSize <= 0)
    {
        return;
    }

    m_vecActiveFonts.push_back(nFontSize);
    yl::stable_sort(m_vecActiveFonts.begin(), m_vecActiveFonts.end());
}

void CActiveFontText::AddActiveFontVec(VEC_ACTIVE_FONT& vecActiveFonts)
{
    bool bAdd = false;

    for (int i = 0; i < vecActiveFonts.size(); ++i)
    {
        int nActiveFont = vecActiveFonts[i];
        if (nActiveFont <= 0)
        {
            continue;
        }

        m_vecActiveFonts.push_back(nActiveFont);
        bAdd = true;
    }

    if (bAdd)
    {
        yl::stable_sort(m_vecActiveFonts.begin(), m_vecActiveFonts.end());
    }
}

void CActiveFontText::DeleteActiveFont(int nFontSize)
{
    for (int i = 0; i < m_vecActiveFonts.size(); ++i)
    {
        int nActiveFont = m_vecActiveFonts[i];
        if (nActiveFont == nFontSize)
        {
            m_vecActiveFonts.removeAt(i);
            return;
        }
    }
}

void CActiveFontText::ClearActiveFont()
{
    m_vecActiveFonts.clear();
}

bool CActiveFontText::IsOverLengthOrHeight(const chRect& rcText, const yl::string& strText, int nFontSize)
{
    xFont font;
    font.setPointSize(nFontSize);
    chSize szText = font.textBound(strText);
    bool bOverLength = rcText.Width() < szText.cx;
    bool bOverHeight = rcText.Height() < szText.cy;

    return bOverLength || bOverHeight;
}
