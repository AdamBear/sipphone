// 简单信息列表项：图标 + (Title/Detail)
#ifndef __SIMPLE_INFO_LIST_ITEM_H__
#define __SIMPLE_INFO_LIST_ITEM_H__

#include "uikernel/listitem.h"
#include <ylstring.h>
#include "uikernel/scrolltext.h"

class QRect;
class QPoint;
class QString;
class QStylePainter;

enum
{
    LIST_ITEM_TYPE_SIMPLE_INFO          = 0x03000000,
};

class CSimpleInfoListItem : public CListItem
{
    Q_OBJECT

public:
    explicit CSimpleInfoListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CSimpleInfoListItem(void);

// CListItem
public:
    // 清除内容
    virtual void ClearContent();

public:
    // 设置标题
    void SetTitle(const yl::string & strTitle);
    // 设置标题
    void SetTitle(const QString & strTitle);
    // 设置标题
    void SetSubTitle(const yl::string & strTitle);
    // 设置标题
    void SetSubTitle(const QString & strTitle);
    // 设置信息
    void SetInfo(const yl::string & strInfo);
    // 设置信息
    void SetInfo(const QString & strInfo);
    // 设置联系人头像
    void SetIcon(const yl::string & strIconPath);

    void SetTitleColor(QColor  color);
    void setShowSubTitle(bool bShow);
    void setShowInfo(bool bShow);

    // 设置标题字体大小
    static void SetTitleFontSize(int nFontSize, int nFontWeight = QFont::Normal, bool bBold = false);
    // 设置子标题字体大小
    static void SetSubTitleFontSize(int nFontSize, int nFontWeight = QFont::Normal, bool bBold = false);
    // 设置信息字体大小
    static void SetInfoFontSize(int nFontSize, int nFontWeight = QFont::Normal, bool bBold = false);

    //设置高亮字符
    void SetHighLightText(const QString & strHighLight)
    {
        m_colorTextDelegate.SetHighLightString(strHighLight);
    }

    // 是否有联系人头像
    bool m_bContactPicVisible;
protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    // 画默认列表信息
    void PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                      const char * pszDefaultIcon = NULL);
    void PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon, const QPoint & ptBegin,
                      int nInfoWidth = 0);

    void PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin, int nInfoWidth = 0,
                       const char * pszDefaultIcon = NULL);

    // 按默认偏移位置及大小画图标
    void PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                   const char * pszDefaultIcon = NULL);
    // 画图标
    void PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                   const char * pszDefaultIcon = NULL);
    // 按默认偏移位置及大小画标题和信息
    void PaintInfo(QStylePainter & stylePainter, const QPoint & ptText, int nRemainWidth);
    // 画标题和信息
    void PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle, const QRect & rcSubTitle,
                   const QRect & rcInfo, int nTitleAlign = Qt::AlignTop, int nInfoAlgin = Qt::AlignBottom);


protected:
    QString m_strTitle;
    QString m_strSubTitle;
    QString m_strInfo;
    yl::string m_strIconPath;

    bool    m_bShowSubTitle;
    bool    m_bShowInfo;

    static bool s_bFontInit;
    static QFont s_fontTitle;
    static QFont s_fontSubTitle;
    static QFont s_fontInfo;

    QRect m_rcIcon;
    QRect m_rcTitle;
    QRect m_rcSubTitle;
    QRect m_rcInfo;

    bool    m_bDefineTitleColor;
    QColor m_TitleColor;
    CColorTextDelegateT9 m_colorTextDelegate;

};

#endif
