#ifndef __MODIMAGECACHEMANAGER_H__
#define __MODIMAGECACHEMANAGER_H__

#include <ETLLib/ETLLib.hpp>
#include "xwindow/xGraphic.h"
#include "imageclassifier.h"
#include "imagecachedefine.h"


/************************************************************************/
/* 接口  ： Image_Init()                                             */
/* 功能  ： 初始化图片缓存                                                */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_Init();

/************************************************************************/
/* 接口  ： Image_UnInit()                                               */
/* 功能  ： 释放图片缓存                                               */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_UnInit();

/************************************************************************/
/* 接口  ： Image_RegisterClassifier(CImageClassifier*)                  */
/* 功能  ： 注册分类器                                                  */
/* 参数说明 ：CImageClassifier* 分类器                                  */
/* 返回值 ：true 注册成功，false 注册失败                              */
/************************************************************************/
bool Image_RegisterClassifier(CImageClassifier* pClassifier);

/************************************************************************/
/* 接口  ： Image_CheckImageSize(const yl::string&)                      */
/* 功能  ： 判断图片大小是否超过话机最大支持                         */
/* 参数说明 ：const yl::string& 图片路径                               */
/* 返回值 ： bool 是否符合限制                                            */
/************************************************************************/
IMAGE_SIZE_TYPE Image_CheckImageSize(const yl::string& strPath);

/************************************************************************/
/* 接口  ： Image_LoadImage(const yl::string&， int)                    */
/* 功能  ： 加载图片                                                 */
/* 参数说明 ：const yl::string& 图片路径                               */
/*          ：int 图片分类类型                                           */
/*          ：bool 是否重新加载图片                                        */
/* 返回值 ： bool 是否加载成功                                            */
/************************************************************************/
bool Image_LoadImage(const yl::string& strPath, int nClassifyType, bool bReload = false);

/************************************************************************/
/* 接口  ： Image_LoadImage(const yl::string&，int, xPixmap&)           */
/* 功能  ： 加载图片                                                 */
/* 参数说明 ：const yl::string& 图片路径                               */
/*          ：int 图片分类类型                                           */
/*          ：xPixmap& 加载后的图片对象                                    */
/*          ：bool 是否重新加载图片                                        */
/* 返回值 ： bool 是否加载成功                                            */
/************************************************************************/
bool Image_LoadImage(const yl::string& strFilePath, int nClassifyType, xPixmap& pic, bool bReload = false);

/************************************************************************/
/* 接口  ： Image_AddImage(const std::string&, int, const char*, int)    */
/* 功能  ： 使用给定的二进制流，添加图片                               */
/* 参数说明 ：const std::string& 图片路径                              */
/*          ：int 图片分类类型                                           */
/*          ：const char* 给定的二进制流                                */
/*          ：int 给定流大小                                          */
/* 返回值 ： bool 是否加载成功                                            */
/************************************************************************/
bool Image_AddImage(const yl::string& strFilePath, int nClassifyType, const char* pFlow, int nFlowSize, bool bReload = false);

/************************************************************************/
/* 接口  ： Image_RemoveImage(const yl::string&)                     */
/* 功能  ： 删除图片                                                 */
/* 参数说明 ：const yl::string& 图片路径                               */
/*            int 分类器类型，                                            */
/*                值为IMG_CLASSIFY_NONE时，删除所有为strPath的图片      */
/* 返回值 ： bool 是否删除成功                                            */
/************************************************************************/
bool Image_RemoveImage(const yl::string& strPath, int nClassifyType = IMG_CLASSIFY_NONE);

/************************************************************************/
/* 接口  ： Image_ClearImage                                         */
/* 功能  ： 清空图片IMG_CLASSIFY_NONE                                    */
/* 参数说明 ：int 分类类型                                             */
/* 返回值 ： 无                                                            */
/************************************************************************/
void Image_ClearImage(int nClassifyType);

/************************************************************************/
/* 接口  ： Image_GetImage(LPCSTR)                                       */
/* 功能  ： 获取图片，获取不到根据默认缓存方式加载图片                  */
/* 参数说明 ：const yl::string& 图片路径                               */
/* 返回值 ： xPixmap 图片                                             */
/* 备注  ： IMG_CLASSIFY_NOCACHE类型的图片不能使用这个                    */
/************************************************************************/
xPixmap Image_GetImage(const yl::string& strFilePath);

/************************************************************************/
/* 接口  ： Image_GetImage(xPixmap, LPCSTR, int)                     */
/* 功能  ： 获取图片保存在xPixmap，                                      */
/*          获取不到根据nClassifyType对应分配器中配置的缓存方式加载图片  */
/* 参数说明 ：xPixmap& 图片对象                                            */
/*            const yl::string& 图片路径                                */
/*            int 分类器类型                                           */
/* 返回值 ： bool 与否成功获取图片                                      */
/************************************************************************/
bool Image_GetImage(xPixmap& pic, const yl::string& strFilePath, int nClassifyType);

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
/************************************************************************/
/* 接口  ： Image_ClearScreenGraphic(int)                                */
/* 功能  ： 清除Screen插件中的缓存                                       */
/* 参数说明 ：int 分类器类型                                          */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_ClearScreenGraphic(int nClassifyType);

/************************************************************************/
/* 接口  ： Image_ClearScreenGraphic(const yl::string&)                  */
/* 功能  ： 清除Screen插件中的缓存                                       */
/* 参数说明 ：const yl::string& 图片路径                               */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_ClearScreenGraphic(const yl::string& strFilePath);
#endif

#if IF_IMAGE_MEMORY_DUMP
/************************************************************************/
/* 接口  ： Image_Dump(int)                                              */
/* 功能  ： 输出缓存图片信息                                         */
/* 参数说明 ：int 内存分页大小                                           */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_Dump(int nPageSize);
#endif

#endif // __MODIMAGECACHEMANAGER_H__
