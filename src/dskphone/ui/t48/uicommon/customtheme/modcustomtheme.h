#ifndef __MOD_CUSTOM_THEME_H__
#define __MOD_CUSTOM_THEME_H__

#include "customthemecommondefine.h"

/**************************************************************/
// 函数描述：模块初始化
// 参数说明：无
// 返回值说明：无
/**************************************************************/
void CustomTheme_Init();

/**************************************************************/
// 函数描述：通过键值获取自定义字体数据
// 参数说明：nKey : 键值
// 返回值说明：自定义字体数据对象
/**************************************************************/
const CustomFontData & CustomTheme_GetCustomFontDataByKey(int nKey);

/**************************************************************/
// 函数描述：通过键值获取自定义字体大小
// 参数说明：nKey : 键值
// 返回值说明：自定义字体的大小
/**************************************************************/
int CustomTheme_GetCustomFontSizeByKey(int nKey);

/**************************************************************/
// 函数描述：通过键值Key获取自定义字体颜色
// 参数说明：nFontKey : 键值； objColor : 自定义的颜色值； nFontStatus：何种状态的字体
// 返回值说明：Color 值
/**************************************************************/
QColor CustomTheme_GetCustomFontColorByKey(int nFontKey, int nFontStatus = FONT_NORMAL);

/**************************************************************/
// 函数描述：通过键值Key判断是否使用字体阴影
// 参数说明：nFontKey : 键值
// 返回值说明：是否使用阴影
/**************************************************************/
bool CustomTheme_IsShowFontShadowByKey(int nFontKey);

// 设置某个Widget字体属性
bool CustomTheme_SetWidgetFontProperty(QWidget * pWidget, int nFontKey, bool bBold = false,
                                       QFont::Weight objWeight = QFont::Normal, int nFontStatus = FONT_NORMAL);

#endif // #ifndef __MOD_CUSTOM_THEME_H__
