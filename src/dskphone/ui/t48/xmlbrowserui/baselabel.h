#ifndef _BASE_LABEL_
#define _BASE_LABEL_

#include <QtGui>
#include <list>
#include <vector>

//////////////////////////////
//数据定义部分

#define LENGTH_LINE     360     //Item单行长度
#define HEIGHT_LINE     43      //Item单行高度
#define XB_BASE_lABEL_MIN_HEIGHT    43  //Item最小行高

#define HEIGHT_FRAME    HEIGHT_LINE * MAX_NUM_ITEM      //TextMenu窗口高度

enum TEXT_STYLE
{
    TEXT_STYLE_SINGLE   = 0,    //单行
    TEXT_STYLE_MULTI            //多行
};



/******************************
所有Item所用Label的基类：
-----自适应显示控件
输入:
    1.一段字符串
    2.显示风格:分行或不分行
输出:
    1.根据风格显示文字:
        1.分行:将文字分为多行显示
        2.不分行:文字在一行显示，过长部分以...显示
    2.获得获取时发出信号
************************************/
class CBaseLabel : public QLabel
{
    Q_OBJECT
public:
    CBaseLabel(QWidget * parent = 0);
    ~CBaseLabel();

public:
    //设置文字内容
    void SetTextContent(QString strText = "");

    //设置label是否分行
    void SetTextWrap(bool bWrap = false);

    //设置文字字体
    void SetTextFont(QFont font);

    //设置文字颜色
    void SetTextColor(QColor color = Qt::white);

    //设置label长度
    void SetLabLength(int nLength = LENGTH_LINE);

    //设置显示label，默认长度为
    void UpdateDiaplay();

    //根据label的所有属性及长度，获取显示时label需要的自适应高度
    int GetAutoResizeHeight();

    //清除显示效果
    void ClearDisplay();

    //根据控件长度及文字属性，获取文本显示的高度
    int GetTextHeight(const QString & strText);

signals:
    void clicked();

protected:
    bool eventFilter(QObject * pObj, QEvent * pEvent);

    void mousePressEvent(QMouseEvent * pMouseEvent);
    void mouseReleaseEvent(QMouseEvent * pMouseEvent);

private:
    //进行label属性的初始化
    void Init();

private:
    QString     m_strTextContent;   //文字内容

    QFont       m_TextFont;         //文字字体

    QColor      m_TextColor;        //文字颜色

    bool        m_bIsWrap;          //是否分行显示

    int         m_nLabLength;       //label默认长度

    bool m_bIsPressed;
};

#endif
