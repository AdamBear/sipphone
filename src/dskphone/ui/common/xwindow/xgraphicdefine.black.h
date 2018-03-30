#ifndef __X_GRAPHIC_DEFINE_BLACK_H__
#define __X_GRAPHIC_DEFINE_BLACK_H__

////////////////////////////////////////////////////////////////////////////
// 图形相关
////////////////////////////////////////////////////////////////////////////
// 是否支持灰度
#define IF_USE_GRADATION 1
// 是否只支持灰度图
#define IF_ONLY_USE_GRADATION 1
// 是否支持ColorKey
#define IF_GRAPHIC_SUPPORT_COLOR_KEY 0
// 是否支持Alpha预乘
#define IF_GRAPHIC_SUPPORT_PRE_MULT 0
// 默认图片格式
#define GRAPHIC_DEFAULT_FORMAT GRAPHIC_FORMAT_GRADATION_8B
// 是否使用RGBA格式
#define IF_GRAPHIC_SUPPORT_RGBA_PAINT 0
// 是否使用BGR24格式
#define IF_GRAPHIC_SUPPORT_BGR24_PAINT 0
// 是否支持BGR24格式缩放
#define IF_GRAPHIC_SUPPORT_BGR24_SCALE 0

// 是否使用FreeType字体库
#define IF_USE_FREETYPE_LIB 0
// 是否使用dob字体库
#define IF_USE_DOB_FONT 1
// byte 表示的像素是列存放 bit0~bit7 <=> Y0~Y7(像素)
#define IF_USE_FAST_FONT 0
// 默认字体大小
#define DEFAULT_FONT_SIZE 10

// 是否支持透明度
#define IF_PIXEL_ALPHA 0
// 是否支持背景透明度
#define IF_STATION_BG_TRANSPARENT 0
// 是否支持图片缩放
#define IF_GRAPHIC_ZOOM 0
// 是否支持旋转
#define IF_GRAPHIC_ROTATE 0
// 是否支持标记Alpha通道
#define IF_GRAPHIC_MARK_ALPHA_CHANNEL 0
// 是否支持标记旋转角度
#define IF_GRAPHIC_MARK_ROTATE 0
// 是否支持.9图
#define IF_PIXMAP_SUPPORT_DOT_9 0
// 是否支持图片拼接
#define IF_PIXMAP_PARTLY_SHARED 0
// 是否使用图片缓存
#define IF_USED_IMAGE_CACHE_MANAGER 0

// 是否支持圆角窗口
#define IF_ROUND_RECTANGLE 1
// 是否支持反色绘制
#define IF_INVERT_EFFECT 1
// 是否支持画单点
#define IF_SCREEN_SET_PIXEL 1

#endif // __X_GRAPHIC_DEFINE_BLACK_H__
