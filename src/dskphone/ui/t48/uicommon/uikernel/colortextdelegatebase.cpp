#include "colortextdelegatebase.h"
#include "qtcommon/qmisc.h"
#include "languagehelper.h"
#include "uicommon.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

const int MAX_PAINT = 3;//绘制最多次数（因仅进行一次的匹配）

const int STRING_NO_FOUND = -1;
//const int CUTOFF_TEXT_LENGTH = 3;//字符串切除替代部分的长度，目前为“...”，因此为3

CColorTextDelegateBase::CColorTextDelegateBase()
    : m_strLight("")
    , m_bIsNeedCutOff(false)
    , m_iAlignV(0)
    , m_bFirstMatch(false)
{

}

CColorTextDelegateBase::~CColorTextDelegateBase()
{

}
void CColorTextDelegateBase::SetHighLightString(const QString & strHighLight)
{
    m_strLight = strHighLight;

    //SetIsFirstMatch(configParser_GetConfigInt(kszDirSearchMatchtype) == 1);
}

QString CColorTextDelegateBase::GetHighLightString() const
{
    return m_strLight;
}

void CColorTextDelegateBase::SetIsNeedCutOff(bool bIsNeedCutOff)
{
    m_bIsNeedCutOff = bIsNeedCutOff;
}

void CColorTextDelegateBase::SetVAlign(int eAlignV)
{
    m_iAlignV = eAlignV;
}

void CColorTextDelegateBase::PaintHighLightText(QPainter & paintText,
        const QString & strShowText,
        QRect rcText,
        const QColor & colorText)
{
    QString strLanShowText = LANG_PARSE(strShowText);

    RTL_GetRealString(strLanShowText);

    int iHighLightLength = m_strLight.size();

    if (0 >= iHighLightLength || configParser_GetConfigInt(kszEnableHighLight) == 0
       )
    {
        if (m_bIsNeedCutOff)
        {
            QString strCut = CutOffText(strLanShowText, QFontMetrics(paintText.font()),
                                        rcText.width() - CUTOffTEXT_OFFSET);//减3为兼容stylepainter里的cutoff
            paintText.drawText(rcText, m_iAlignV, strCut);
        }
        else
        {
            paintText.drawText(rcText, m_iAlignV, strLanShowText);
        }
        return;
    }

    int iIndex = SearchIndex(strLanShowText);

    QString strCutOff = CutOffText(strLanShowText, QFontMetrics(paintText.font()),
                                   rcText.width() - CUTOffTEXT_OFFSET);//减3为兼容stylepainter里的cutoff

    bool bIsNeedCutOff = m_bIsNeedCutOff && (strCutOff != strLanShowText);

    if (bIsNeedCutOff)
    {
        PaintCutOffHighLightText(paintText, strCutOff, rcText, colorText, iIndex);
    }
    //通用场景
    else
    {
        PaintNormalHighLightText(paintText, strLanShowText, rcText, colorText, iIndex);
    }
}

void CColorTextDelegateBase::PaintHighLightScrollText(QPainter & paintText,
        const QString & strTotalText,
        const QString & strShowText,
        QRect rcText,
        const QColor & colorText)
{
    const QString strHightLight = m_strLight;

    int iIndex = SearchIndex(strTotalText);
    int iHighLightLength = m_strLight.size();
    int iShowTextIndex = strTotalText.indexOf(strShowText);

    //如若高亮字符已经滚动出屏幕外（即已经被切除）
    if (iShowTextIndex >= iIndex + iHighLightLength && iIndex != STRING_NO_FOUND)
    {
        m_strLight = "";
    }
    //如若高亮字符滚动时被截断
    else if (iShowTextIndex > iIndex && iShowTextIndex < iIndex + iHighLightLength
             && iIndex != STRING_NO_FOUND)
    {
        m_strLight = strTotalText.mid(iShowTextIndex, iIndex + iHighLightLength - iShowTextIndex);
    }
    else
    {
        //
    }

    PaintHighLightText(paintText, strShowText, rcText, colorText);
    m_strLight = strHightLight;
}

void CColorTextDelegateBase::PaintCutOffHighLightText(QPainter & paintText,
        const QString & strCutOff,
        QRect rcText,
        const QColor & colorText,
        int iIndex)
{
    PaintTextInfo objPaintInfo;


    int iHighLightLength = m_strLight.size();
    int iCutOffSize = strCutOff.size();

    //高亮部分在截断部分的场景
    if (iIndex > iCutOffSize - CUTOffTEXT_LENGTH - iHighLightLength)
    {
        bool bIsAllInCut = false;//代表是否全在截断部分
        if (iIndex >= iCutOffSize - CUTOffTEXT_LENGTH)
        {
            bIsAllInCut = true;
        }

        int iLastIndex = 0; //搜寻的下一个匹配字符串的开始索引
        objPaintInfo.m_bIsHighLight = false;
        int iNextIndex = bIsAllInCut ? (iCutOffSize - CUTOffTEXT_LENGTH) : iIndex;

        PruneString(iLastIndex, iNextIndex, objPaintInfo, strCutOff);
        CutPaintText(objPaintInfo, rcText, paintText.font());
        PaintTextByRect(paintText, objPaintInfo, colorText, iLastIndex);

        iLastIndex = iNextIndex;

        objPaintInfo.m_bIsHighLight = true;
        PruneString(iLastIndex, -1, objPaintInfo, strCutOff);
        CutPaintText(objPaintInfo, rcText, paintText.font());
        PaintTextByRect(paintText, objPaintInfo, colorText, iLastIndex);
    }
    //通用场景
    else
    {
        //走通用流程
        PaintNormalHighLightText(paintText, strCutOff, rcText, colorText, iIndex);
    }
}

void CColorTextDelegateBase::PaintNormalHighLightText(QPainter & paintText,
        const QString & strTotalText,
        QRect rcText,
        const QColor & colorText,
        int iIndex)
{
    int iHighLightLength = m_strLight.size();

    PaintTextInfo objPaintInfo;
    int iLastIndex = 0; //搜寻的下一个匹配字符串的开始索引

    int iEndSign = MAX_PAINT -
                   1;//代表绘制结束的标志，即此次绘制后不再进行绘制，则此时应为循环的个数-1;

    objPaintInfo.m_bIsHighLight = true;
    for (int i = 0; i < MAX_PAINT; ++i)
    {
        objPaintInfo.m_bIsHighLight = !(objPaintInfo.m_bIsHighLight);

        PruneString(iLastIndex, (i == iEndSign) ? -1 : iIndex, objPaintInfo,
                    strTotalText);//如为要结束时，则将其剩余的字符串切除进去

        CutPaintText(objPaintInfo, rcText, paintText.font());
        PaintTextByRect(paintText, objPaintInfo, colorText, iLastIndex);

        if (objPaintInfo.m_bIsHighLight)
        {
            iLastIndex = iLastIndex + iHighLightLength;
        }
        else
        {
            iLastIndex = iIndex;
        }

        if (-1 == iIndex) //说明不需要继续绘制
        {
            break;
        }
    }
}

void CColorTextDelegateBase::CutPaintText(PaintTextInfo & objInfo, QRect & rcTotal,
        const QFont & fontText)
{
    QFontMetrics fontMetricsText(fontText);
    int iTextPaintLength = fontMetricsText.width(objInfo.m_strPaint);

    objInfo.m_rcPaint.setRect(rcTotal.x(), rcTotal.y(),
                              iTextPaintLength < rcTotal.width() ? iTextPaintLength : rcTotal.width(), rcTotal.height());

    //减去传递过来的rect，以便其能直接应用于下次计算
    rcTotal.setX(rcTotal.x() + iTextPaintLength);
}

int CColorTextDelegateBase::SearchIndex(const QString & strTotalText) const
{
    int iIndex = strTotalText.indexOf(m_strLight, 0, Qt::CaseInsensitive);

    if (GetIsFirstMatch() && 0 != iIndex)
    {
        return -1;
    }

    return iIndex;
}

void CColorTextDelegateBase::PruneString(int iLastIndex,
        int iIndex,
        PaintTextInfo & objInfo,
        const QString & strTotalText) const
{
    int iHightLightLength = m_strLight.size();

    if (-1 == iIndex)
    {
        objInfo.m_strPaint = strTotalText.mid(iLastIndex, iIndex);
        return;
    }

    if (objInfo.m_bIsHighLight)
    {
        objInfo.m_strPaint = strTotalText.mid(iIndex, iHightLightLength);
    }
    else
    {
        objInfo.m_strPaint = strTotalText.mid(iLastIndex, iIndex - iLastIndex);
    }
}


void CColorTextDelegateBase::PaintTextByRect(QPainter & paintText,
        const PaintTextInfo & objInfo,
        const QColor & colorText,
        int iLastIndex) const
{
    //计算绘制布局方式
    if (objInfo.m_rcPaint.width() < 0)
    {
        return;
    }
    int iTextAlign = (m_iAlignV);

    //进行高亮绘制
    if (objInfo.m_bIsHighLight)
    {
        paintText.save();

        QPen p = paintText.pen();
        p.setColor(colorText);
        paintText.setPen(p);

        paintText.drawText(objInfo.m_rcPaint, iTextAlign, objInfo.m_strPaint);

        paintText.restore();
    }
    else
    {
        paintText.drawText(objInfo.m_rcPaint, iTextAlign, objInfo.m_strPaint);
    }
}

bool CColorTextDelegateBase::GetIsFirstMatch() const
{
    return m_bFirstMatch;
}
