#ifndef CXML_LABEL_H
#define CXML_LABEL_H

#include <QLabel>
#include <QFrame>
#include <string>
#include <list>
#include <QtGui>
#include "uicommon.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////
struct LabDisplayData
{
    int               iFontSize;
    QColor            fontColor;
    Qt::AlignmentFlag fontAlignment;
    yl::string        strText;
    yl::string        strIconPath;

    LabDisplayData()
    {
        iFontSize = 0;
        fontAlignment = Qt::AlignHCenter;
    }

    ~LabDisplayData() {}
};
///////////////////////////////////////////////////////////////////////////////
// 随着设置的Text会自动跟随字体大小变化高度的Lab
class AutoResizeLab : public QLabel
{
public:
    AutoResizeLab(QWidget * parent = 0);
    AutoResizeLab(int iFixWidth, int iRowHeight, QWidget * parent = 0);
    ~AutoResizeLab();

    // 设置Lab的Text，只有通过这个接口设置Lab才会自适应大小
    void SetLabText(const QString & strText);

    // 是否开启自适应大小
    void OpenAutoResize(bool bOpen);

    // 设置固定的宽度
    void SetFixWidth(int iFixWidth);

    // 设置单元行的高度
    void SetCellRowHeight(int iCellRowHeight);

    // 设置控件的最小高度
    void SetMinimumHeight(int iMinimumHeight);

    // Lab中每行文字的高度
    int FontlineSpacing();

    // 设置图片的路径
    void SetIconPath(const yl::string & strIconPath);

    // 是否绘制小图标
    void SetDrawIcon(bool bDraw);

    // 根据高度重新设置控件的Text
    // 即让控件的Text显示在iHeight的高度内
    void ResetSetTextByHeight(int iHeight);

    void paintEvent(QPaintEvent * pEvent);

private:
    // 获得第一行的高度
    int GetFirstRowHeight();

    // 计算Text在lab内的高度
    int CountTextHeight(const QString & strText);

private:
    // 固定的宽度
    int     m_iFixWidth;

    // 每个单元行的高度
    // 设置为0时Lab的高度就为Text的高度
    // 设置为非0是Lab为可容纳Text的最小的n * m_iCellRowHeight(n为大于0的整数)
    int     m_iCellRowHeight;

    // 是否开启自适应大小
    bool    m_bOpenAutoResize;

    // 最小的高度
    int     m_iMinimumHeight;

    // 小图标的路径
    yl::string  m_strIconPath;

    // Icon
    QPixmap m_pmIcon;

    // 判断是否绘制Icon
    bool    m_bDrawIcon;
};

/************************************************************************/
/*                     about  AutoResizeListView                        */
/************************************************************************/
class AutoResizeListView : public QFrame
{
public:
    typedef std::list<AutoResizeLab *> labelList;
    typedef labelList::iterator       labeliter;

public:
    AutoResizeListView(QWidget * parent = 0);
    AutoResizeListView(int ItemFixWidth, QWidget * parent = 0);
    ~AutoResizeListView();

    // 设置数据
    void SetViewData(const list<StatusItem> & listStatusItem, bool bClear = true);

    // 设置数据
    void SetViewData(const list<LineData *> & listLineData, bool bClear = true);

    // 设置是否打断
    void SetWrapWord(bool bWrap);

    // 设置Item的固定高度
    void SetItemFixWidth(int iWidth);

    // 设置每行的高度
    void SetRowHeight(int iHeight);

    // 显示小于高度iHeight的所有Lab
    void ShowThePartLabByHeight(int iHeight);

    // 根据高度重新设置Lab的位置和控件的高度,防止文字被截断
    void ResetLabPosByHeight(int iHeight);

    // 设置控件的最小高度
    void SetLabMinimumHeight(int iMinimumHeight);

    //设置最大显示行数
    void SetMaxRow(int iMaxRow);

private:
    // 设置Lab数据
    void SetViewData(const list<LabDisplayData> & listLineData, bool bClear = true);

    // 设置单个Lab的风格
    void SetLabStyle(const LabDisplayData & objDisplayData, AutoResizeLab * pLabel);

    // 重新改变每个Item的宽度
    void ResetItemWidth(int iWidth);

    // 清空链表上的数据
    void ClearListData();

private:
    labelList   m_ListLabel;     // 存放Lab指针的链表
    static const int DEFAULT_ROW_HEIGHT = 30; // 默认的行高
    int         m_iRowHeight;    // 每行的高度
    int         m_nItemFixWidth; // 每个Item的固定宽度
    int         m_iItemMiniHeight;// 每个Item最小的高度
    bool        m_bWrap;         // 是否打断lab中的文字
    int         m_iMaxRow;
};

#endif // CXML_LABEL_H
