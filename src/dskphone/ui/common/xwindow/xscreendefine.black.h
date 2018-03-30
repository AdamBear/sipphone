#ifndef __XSCREEN_DEFINE_H__
#define __XSCREEN_DEFINE_H__


// 是否用插件（动态库）使用屏幕显示模块（为0时直接编译进xWindow库）
#define IF_SUPPORT_SCREEN_PLUGIN 0

// 是否自定义显示设备(外部自己控制显示输出)
#ifdef _WIN32
#define IF_CUSTOM_SCREEN_DEVICE 1
#else
#define IF_CUSTOM_SCREEN_DEVICE 0
#endif

// 是否支持设备像素缩放
#define IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE 0
// 是否支持显存图像缓存
#define IF_SCREEN_SUPPORT_DEVICE_GRAPHIC 0

// 是否支持局部刷新
#define IF_SCREEN_SUPPORT_PARTLY_REFRESH 0

#define IF_SCREEN_SUPPORT_BG_RENDER 0

// 是否支持双缓存
#define IF_SCREEN_SUPPORT_DOUBLE_BUFFER 0

#endif
