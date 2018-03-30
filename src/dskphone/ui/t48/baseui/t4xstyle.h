#ifndef _T4X_STYLE_H_
#define _T4X_STYLE_H_

/*
 * T4XStyle
 * T4XPhone上的自定义样式
 * 用于实现一些全局的的控件样式
 */
#include "uikernel/stylebase.h"
#include <qcommonstyle.h>

//是否统计翻译长度（内部使用，对外版本此项应为0）
#define TEST_TRAN_LEN 0

class QStyleOptionSlider;

// The qT4XStyle class subclasses from QcleanlooksStyle. you can use it to
// custom your Qt look and feel just by calling setStyle before QApplication initilizes.
class qT4XStyle : public QCommonStyle
{
    typedef QCommonStyle BaseClass;

    Q_OBJECT

public:
    enum ePxKey
    {
        PxNone = 0,
        PxComboBoxArrow,                // 组合框箭头
        PxComboBoxArrowDown,            // 组合框箭头向下
        PxComboBoxArrowUp,              // 组合框箭头向上
        PxComboBoxArrowFocus,           // 组合框箭头选中状态
        PxComboBoxArrowDownFocus,       // 组合框箭头向下选中状态
        PxComboBoxArrowUpFocus,         // 组合框箭头向上选中状态
        PxRadioButtonOn,                // 单选框被选中
        PxRadioButtonOff,               // 单选框没被选中
        PxRadioButtonDisable,           // 单选框不可用
        PxCheckBoxOn,                   // 复选框被选中
        PxCheckBoxOff,                  // 复选框没被选中
        PxScrollBarVertBackGound,   // 竖直滚动条背景图片
        PxScrollBarUpArrow,         // 竖直滚动条向上箭头
        PxScrollBarDownArrow,       // 竖直滚动条向下箭头
        PxScrollBarVertSlider,          // 竖直滚动条滑块
        PxScrollBarHorzBackGound,   // 水平滚动条背景图片
        PxScrollBarLeftArrow,           // 水平滚动条向左箭头
        PxScrollBarRightArrow,      // 水平滚动条向右箭头
        PxScrollBarHorzSlider,          // 水平滚动条滑块
        PxIconViewCover,                // IconView点击时候的遮盖图标
        PxPopMenuBackground,        //弹出菜单的背景
        PxPoPMenuSeletcedItem,      //弹出菜单选中条

        PxMax   //个数
    };

public:
    qT4XStyle();
    virtual ~qT4XStyle();

    // override function

    // 绘制控制
    void drawControl(ControlElement element, const QStyleOption * option,
                     QPainter * painter, const QWidget * widget) const;

    // 绘制一个原始的对象
    void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
                       QPainter * painter, const QWidget * widget = 0) const;

    // 绘制组合框
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
                            QPainter * painter, const QWidget * widget) const;

    // 绘制Item的文本
    void drawItemText(QPainter * painter, const QRect & rect,
                      int flags, const QPalette & pal, bool enabled,
                      const QString & text, QPalette::ColorRole textRole = QPalette::NoRole) const;

    // 标准像素的查询
    int pixelMetric(PixelMetric m, const QStyleOption * opt = 0, const QWidget * widget = 0) const;

    // 控件属性的查询
    int styleHint(StyleHint sh, const QStyleOption * opt, const QWidget * widget,
                  QStyleHintReturn * hret) const;

    // 获得子控件的矩形区域
    QRect subElementRect(SubElement sr, const QStyleOption * opt,
                         const QWidget * widget) const;

    // 获得文本内容的大小
    QSize sizeFromContents(ContentsType type, const QStyleOption * option,
                           const QSize & size, const QWidget * widget) const;

    // 获得子组合控件的矩形区域
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex * opt,
                         SubControl sc, const QWidget * widget) const;
#if TEST_TRAN_LEN
public slots:
    //保存翻译的计时器到时，保存翻译长度到文件
    void OnTranSaveTimeout();
#endif
private:
    // 绘制PushBtn的label
    bool myDrawPushButtonLabel(const QStyleOption * option,
                               QPainter * painter, const QWidget * widget) const;

    // 绘制cmb的箭头图标
    bool myDrawComboBoxArrow(ComplexControl control, const QStyleOptionComplex * option,
                             QPainter * painter, const QWidget * widget) const;

    // 绘制CheckBox指示符
    bool myDrawCheckBoxIndicator(PrimitiveElement element, const QStyleOption * option,
                                 QPainter * painter, const QWidget * widget = 0) const;

    // 绘制RadioButton指示符
    bool myDrawRadioButtonIndicator(PrimitiveElement element, const QStyleOption * option,
                                    QPainter * painter, const QWidget * widget = 0) const;

    // 绘制滚动条
    bool myDrawScrollBar(ComplexControl control, const QStyleOptionComplex * option,
                         QPainter * painter, const QWidget * widget) const;

    // 绘制滚动条向上或向左箭头
    bool myDrawScrollBarSubLine(ComplexControl control, const QStyleOptionSlider * optSlider,
                                QPainter * painter, const QWidget * widget) const;

    // 绘制滚动条向下或向右箭头
    bool myDrawScrollBarAddLine(ComplexControl control, const QStyleOptionSlider * optSlider,
                                QPainter * painter, const QWidget * widget) const;

    // 绘制滚动条的滑块
    bool myDrawScrollBarSlider(ComplexControl control, const QStyleOptionSlider * optSlider,
                               QPainter * painter, const QWidget * widget) const;


    // 绘制菜单
    bool myDrawPnaelMenu(PrimitiveElement element, const QStyleOption * option,
                         QPainter * painter, const QWidget * widget) const;

    bool myDrawMenuItem(ControlElement element, const QStyleOption * option,
                        QPainter * painter, const QWidget * widget) const;

    // 获取某种状态对应的颜色
    QColor myGetStateColor(const QStyle::State st) const;
};

#endif // _T4X_STYLE_H_

