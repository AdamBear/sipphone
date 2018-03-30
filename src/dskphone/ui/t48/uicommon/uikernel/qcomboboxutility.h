
#ifndef __Q_COMBOBOX_UTILITY_H__
#define __Q_COMBOBOX_UTILITY_H__

#include <QComboBox>
#include <QItemDelegate>

// ComboBox的最大显示数量，超过则出现滚动条
#define CMB_MAX_VISIBLE_ITEM 3
// ComboBoxItem的高度
#define CMB_ITEM_HEIGHT 47
// ComboBox滚动条箭头的高度
#define CMB_SCROLLBAR_ARROWS_HEIGHT 34

// 用于设置combobox行高
// 修改语言翻译的渲染类
class CMyComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit CMyComboBoxDelegate(QComboBox * pComboBox, int nHeight = CMB_ITEM_HEIGHT,
                                 QObject * parent = 0);
    virtual ~CMyComboBoxDelegate();

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
    // painting
    void paint(QPainter * painter,
               const QStyleOptionViewItem & option,
               const QModelIndex & index) const;
    virtual void drawDisplay(QPainter * painter, const QStyleOptionViewItem & option,
                             const QRect & rect, const QString & text) const;

public Q_SLOTS:
    void OnComboBoxDestroy(QObject * pComboBox);

private:
    int m_iHeight;
    QComboBox * m_pComboBox;
};

class qComboBoxUtility
{
public:
    qComboBoxUtility(void);
    ~qComboBoxUtility(void);

public:
    /****************************************************
    函数名称：SetCmbGeometry(QComboBox* pCombobox, QRect rcCmb);
    函数意义：设置QComboBox的位置
    参数意义：
    参数1：pCombobox --QComboBox*类型，所要修改属性的QComboBox 控件指针
    参数2：rcCmb 设置的位置
    返回值：无返回值
    ***************************************************/
    static void SetCmbGeometry(QComboBox * pCombobox, QRect rcCmb);

    /***************************************************
    SetComboBoxStyle(for T3X)
    ***************************************************/
    static void SetComboBoxStyle(QComboBox * pComboBox, const QColor & color);

    /****************************************************
    函数名称：SetUpComboBox(QComboBox* pCombobox);
    函数意义：设置QComboBox基本属性
    参数意义：
    参数1：pCombobox --QComboBox*类型，所要修改属性的QComboBox 控件指针
    返回值：无返回值
    ***************************************************/
    static void SetUpComboBox(QComboBox * pCombobox);

    static void SetComboBoxSelectText(QComboBox * pCombobox, const QString & strText);

    static void SwitchItem(QComboBox * pComboBox, bool bSelectPre);

    static bool OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent, QWidget * pFocusWidget);

    static bool OnRightPressEventFilter(QObject * pObj, QEvent * pEvent, QWidget * pFocusWidget);
};

#endif
