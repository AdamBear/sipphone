#ifndef __IMAGE_CACHE_MANAGER_DEFINE_H__
#define __IMAGE_CACHE_MANAGER_DEFINE_H__
#include "innermsgdefine.h"

// 是否开启多线程
#define IF_SUPPORT_MULTI_THREAD 0
// 是否添加缓存算法
#define IS_SUPPORT_LRU_CACHE 1

#define IF_IMAGE_FILE_DEBUG 0

#define IF_IMAGE_MEMORY_DUMP 1

enum IMAGE_MESSAGE_CODE
{
    MSG_IMAGE_MESSAGE_FIRST = INTER_MSG_IMAGE_MESSAGE_BEGIN,
    // wparam: bSucceed, lparam: nClassifyType. ExtraData strImagePath
    MSG_IMAGE_MESSAGE_LOAD_FINISH,
    MSG_IMAGE_MESSAGE_END = INTER_MSG_IMAGE_MESSAGE_END,
};

// 图片缓存类型
enum IMAGE_CACHE_TYPE
{
    IMG_CACHE_TYPE_NONE         = 0,                        // 不缓存
    IMG_CACHE_TYPE_MANUAL       = 1,                        // 手动缓存(需要调用AddImage和RemoverImage)
#if IS_SUPPORT_LRU_CACHE
    IMG_CACHE_TYPE_LRU          = 2,                        // LRU缓存(使用时加载，按LRU算法释放)
#endif
};

// 图片分类类型
enum IMAGE_CLASSIFY_TYPE
{
    IMG_CLASSIFY_NONE           = 0,
    IMG_CLASSIFY_THEME          = 1,                        // 系统主题图片
    IMG_CLASSIFY_STOCK          = 2,                        // 系统非主题的图片
    IMG_CLASSIFY_NOCACHE        = 3,                        // 直接读取的图片
    IMG_CLASSIFY_CUSTOM         = 4,                        // 自定义范围的图片
};

// 图片大小类型
enum IMAGE_SIZE_TYPE
{
    IMG_SIZE_REGULAR            = 0,                        // 合格
    IMG_SIZE_INVALID            = 1,                        // 无效
    IMG_SIZE_OVER_LIMIT         = 2,                        // 超出范围
};

// 允许缓存到内存中的最大图片分辨率
// 1080p
#define MAX_CACHE_RESOLUTION            2073600

// 允许加入的最大图片分辨率
// 1080p*2 大于主流壁纸最大值2560x1600
#define MAX_PICTURE_RESOLUTION          4147200

#endif // __IMAGE_CACHE_MANAGER_DEFINE_H__
