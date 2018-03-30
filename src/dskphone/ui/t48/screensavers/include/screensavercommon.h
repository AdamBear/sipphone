#ifndef __SCREEN_SAVER_COMMON_H__
#define __SCREEN_SAVER_COMMON_H__

// 是否支持用户自定义图片模式
#define SCREEN_SAVE_FEATRUE_CUSTOM_MODE     1

// 时钟的移动方式，开启：连续移动； 关闭：跳动
#define SCREEN_SAVE_FEATRUE_CLOCK_MOVING    0

#if !SCREEN_SAVE_FEATRUE_CLOCK_MOVING
// 当使用固定位置跳动方式时，是否从列表中选出几个固定的位置，否则使用完全随机的位置
#define SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST    0
#endif

// 是否支持不同颜色的切换
#define SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE             0

// 是否支持不同特效的切换
#define SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE     0

// 是否支持随机播放图片
#define SCREEN_SAVE_FEATURE_SHOW_PICTURE_BY_RANDOM      0

// 测试开关
#define SCREEN_SAVE_FEATURE_TEST                        0

#endif // #ifndef __SCREEN_SAVER_COMMON_H__
