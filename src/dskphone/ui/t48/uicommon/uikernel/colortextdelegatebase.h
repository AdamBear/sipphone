#ifndef _H_COLORTEXT_DELEGATE_BASE
#define _H_COLORTEXT_DELEGATE_BASE

#ifndef _HIGHLIGHT
#define IF_FEATRUE_HIGHLIGHT_CLOSE
#endif

struct  PaintTextInfo
{
    QString m_strPaint;         //绘制的文字
    QRect   m_rcPaint;          //绘制的位置

    bool    m_bIsHighLight; //是否为高亮文字

    PaintTextInfo() : m_strPaint("")
        , m_rcPaint(QRect(-1, -1, -1, -1))
        , m_bIsHighLight(false)
    {
    }
};

class CColorTextDelegateBase
{
public:
    CColorTextDelegateBase();
    virtual ~CColorTextDelegateBase();

public:
    //设置高亮字段
    void SetHighLightString(const QString & strHighLight);

    //获取高亮字段
    QString GetHighLightString() const;

    //设置竖直方向上的布局
    void SetVAlign(int eAlignV);

    //设置是否需要字符串超长处理
    void SetIsNeedCutOff(bool bIsNeedCutOff);

    void SetIsFirstMatch(bool bFirstMatch)
    {
        m_bFirstMatch = bFirstMatch;
    }
    bool GetIsFirstMatch() const;

    //绘制带高亮文字
    virtual void PaintHighLightText(QPainter & paintText, const QString & strShowText, QRect rcText,
                                    const QColor & colorText);

    //绘制带高亮文字
    virtual void PaintHighLightScrollText(QPainter & paintText, const QString & strTotalText,
                                          const QString & strShowText, QRect rcText, const QColor & colorText);

protected:
    //绘制带截断的字符情形
    virtual void PaintCutOffHighLightText(QPainter & paintText, const QString & strCutOff, QRect rcText,
                                          const QColor & colorText, int iIndex);

    //绘制通用场景,iHighLightNum代表要高亮的个数
    virtual void PaintNormalHighLightText(QPainter & paintText, const QString & strTotalText,
                                          QRect rcText, const QColor & colorText, int iIndex);

    //剪切绘制位置,结构体内的QRect为计算出来的绘制位置，并在rcTotal里切除相应的位置
    virtual void CutPaintText(PaintTextInfo & objInfo, QRect & rcTotal, const QFont & fontText);

    //计算出搜索字符串的位置
    virtual int SearchIndex(const QString & strTotalText) const;

    //对搜索出来的字符串进行包装
    virtual void PruneString(int iLastIndex, int iIndex, PaintTextInfo & objInfo,
                             const QString & strTotalText) const;

    //根据绘制信息进行绘制
    virtual void PaintTextByRect(QPainter & paintText, const PaintTextInfo & objInfo,
                                 const QColor & colorText, int iLastIndex) const;
private:

    QString     m_strLight;     //高亮字符串
    bool        m_bIsNeedCutOff;    //字符超长时是否需要剪切字符串

    int         m_iAlignV;      //竖直方向上的布局

    bool                m_bFirstMatch;
};


#endif