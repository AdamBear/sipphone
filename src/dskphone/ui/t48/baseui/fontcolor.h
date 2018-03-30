#ifndef _FONT_COLOR_
#define _FONT_COLOR_
#include <QtGui>

#define THEME_TEXT_COLOR Qt::black
// Edit高亮背景颜色
#define THEME_EDIT_HL_COLOR Qt::darkBlue
// Edit高亮字体颜色
#define THEME_EDIT_HL_TEXT_COLOR Qt::white

const QColor HIGH_LIGHT_COLOR(0, 144, 255);
// 文字类型
enum FONT_TYPE
{
    FONT_TYPE_TITLE = 0,
    FONT_TYPE_TEXT,
    FONT_TYPE_BUTTON,
    FONT_TYPE_MENU,
    FONT_TYPE_TALK_TITLE,
    FONT_TYPE_TALK_TEXT,
    FONT_TYPE_TALK_BUTTON,
    FONT_TYPE_TALK_MENU,
    FONT_TYPE_TEXT_SELECTED,
    FONT_TYPE_FOREGROUND_TEXT,
    FONT_TYPE_FOREGROUND_TEXT_SELECTED  //QComboBox,QLineEdit,QTextEdit等控件的字体颜色
};

// 设置控件的颜色
// | pWgt | in 控件的指针
// | eType | in 文字的类型
// | qtColor | in 设置的颜色,若使用默认值将根据文字的类型和当前的主题自动匹配颜色
void SetWgtFontColor(QWidget * pWgt, FONT_TYPE eType, Qt::GlobalColor qtColor = Qt::color0);

void SetWidgetColor(QWidget * pWgt, QPalette::ColorRole qtColorRole, QColor objColor);

class FontColorManager
{
public:
    FontColorManager();
    ~FontColorManager();

    // 设置控件上文字的颜色
    void SetWidgetColor(QWidget * pWgt, FONT_TYPE eType, Qt::GlobalColor qtColor = Qt::color0);

private:
    QPalette::ColorRole GetColorRoleByFontType(FONT_TYPE eType);
    QColor GetColorByFontType(FONT_TYPE eType);
};

#endif // _FONT_COLOR_
