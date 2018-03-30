#ifndef MODIMAGECACHEMANAGER_H
#define MODIMAGECACHEMANAGER_H

#include "imagecachemanagerdefine.h"
#include "imagecachemanager.h"
#include <QPixmap>
#include <ETLLib/ETLLib.hpp>

/************************************************************************/
/* 接口  ： Image_IsImageSizeReceivable(const std::string&)              */
/* 功能  ： 判断图片大小是否超过话机最大支持                         */
/* 参数说明 ：const std::string& 图片路径                              */
/* 返回值 ： bool 是否符合限制                                            */
/************************************************************************/
IMAGE_RECEIVABLE_TYPE Image_IsImageSizeReceivable(const std::string & strPath);

/************************************************************************/
/* 接口  ： Image_AddImage(const std::string&， IMAGE_TYPE)              */
/* 功能  ： 添加图片                                                 */
/* 参数说明 ：const std::string& 图片路径                              */
/*          ：IMAGE_TYPE 图片类型                                      */
/*          ：bool 是否删除之前图片                                        */
/* 返回值 ： bool 是否添加成功                                            */
/************************************************************************/
bool Image_AddImage(const std::string & strPath, IMAGE_TYPE eType, bool bDelBeforeImage = false);

/************************************************************************/
/* 接口  ： Image_AddImage(const std::string&，IMAGE_TYPE, QPixmap&)    */
/* 功能  ： 添加图片                                                 */
/* 参数说明 ：const std::string& 图片路径                              */
/*          ：IMAGE_TYPE 图片类型                                      */
/* 返回值 ： bool 是否添加成功                                            */
/************************************************************************/
bool Image_AddImage(const std::string & strPath, IMAGE_TYPE eType, QPixmap & pic);

/************************************************************************/
/* 接口  ： Image_RemoveImage(const std::string&)                        */
/* 功能  ： 删除图片                                                 */
/* 参数说明 ：const std::string& 图片路径                              */
/* 返回值 ： bool 是否删除成功                                            */
/************************************************************************/
bool Image_RemoveImage(const std::string & strPath);

/************************************************************************/
/* 接口  ： Image_ClearImage(bool)                                       */
/* 功能  ： 清空所有的图片(是否是Lync)                                 */
/* 参数说明 ：bool为真，只删除Lync图片                                 */
/* 返回值 ： 无                                                            */
/************************************************************************/
void Image_ClearImage(bool bClearLync = true);

/************************************************************************/
/* 接口  ： Image_ClearImageByImageType(IMAGE_TYPE)                      */
/* 功能  ： 清空类型为eType的图片资源                                  */
/* 参数说明 ：IMAGE_TYPE要删除的图片类型                               */
/* 返回值 ： 无                                                            */
/************************************************************************/
void Image_ClearImageByImageType(IMAGE_TYPE eType = DIT_NONE);

/************************************************************************/
/* 接口  ： Image_GetImage(LPCSTR, IMAGE_TYPE)                           */
/* 功能  ： 获取图片，获取不加根据IMAGE_TYPE加载图片                  */
/* 参数说明 ：LPCSTR 图片路径                                          */
/* 返回值 ： QPixmap 图片                                             */
/************************************************************************/
QPixmap Image_GetImage(LPCSTR strPath, IMAGE_TYPE eType = DIT_THEME);

/************************************************************************/
/* 接口  ： Image_GetImage(QPixmap, LPCSTR, IMAGE_TYPE)                  */
/* 功能  ： 获取图片保存在QPixmap，获取不加根据IMAGE_TYPE加载图片      */
/* 参数说明 ：LPCSTR 图片路径                                          */
/* 返回值 ： QPixmap 图片                                             */
/************************************************************************/
bool Image_GetImage(QPixmap & refPixmap, LPCSTR strPath, IMAGE_TYPE eType = DIT_NOCACHE);

/************************************************************************/
/* 接口  ： Image_LoadTheme(LPCSTR)                                      */
/* 功能  ： 加载主题图片                                               */
/* 参数说明 ：LPCSTR 主题路径                                          */
/* 返回值 ：                                                            */
/************************************************************************/
bool Image_LoadTheme(LPCSTR strPath);

/************************************************************************/
/* 接口  ： Image_UnloadTheme()                                          */
/* 功能  ：                                                          */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
bool Image_UnloadTheme();

#if ADD_CACHE_ALGORITHM
/************************************************************************/
/* 接口  ： Image_ClearLRUCache()                                        */
/* 功能  ：  清除LRU缓存内的数据                                     */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_ClearLRUCache();

/************************************************************************/
/* 接口  ： Image_InsertLRUCache()                                       */
/* 功能  ：  往LRU缓存内插入数据                                     */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
bool Image_InsertLRUCache(const std::string & strPath, QPixmap & pic);

/************************************************************************/
/* 接口  ： Image_IsExistInLRUCache()                                        */
/* 功能  ：  LRU缓存内是否存在该图片                                       */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
bool Image_IsExistInLRUCache(const std::string & strPath, QPixmap & pic);

#endif

/************************************************************************/
/* 接口  ： Image_LoadPixmapByPath()                                 */
/* 功能  ： 通过图片路径获取图片，图片不加入缓存                       */
/* 参数说明 ：refPixmap:QPixmap对象 strImagePath：图片路径              */
/* 返回值 ：获取图片是否成功                                            */
/************************************************************************/
bool Image_LoadPixmapByPath(QPixmap & refPixmap, const QString & strImagePath);

/************************************************************************/
/* 接口  ： Image_Draw_9_Pix()                                           */
/* 功能  ： 绘制点九格式主题图片                                   */
/* 参数说明 ：                                                         */
/* 返回值 ：                                                            */
/************************************************************************/
void Image_Draw_9_Pix(QPainter & refPainter, LPCSTR lpszFilePath, QRect PaintRect);

/************************************************************************/
/* 接口  ： Image_ConvertToThemePath()                                   */
/* 功能  ： 通过主题图片的名称，获取图片的全路径                       */
/* 参数说明 ：strPath(图片名称)     strFullPath(图片补全路径)                */
/* 返回值 ：返回是否成功                                              */
/************************************************************************/
bool Image_ConvertToThemePath(const QString & strPath, QString & strFullPath);

/************************************************************************/
/* 接口       ： THEME_LOAD()                                                */
/* 功能       ： 主题加载,会加载主题路径下的所有图片                    */
/* 参数说明 ：                                                         */
/*      1.themeName：路径名称                                      */
/* 返回值    ： 无                                                     */
/************************************************************************/
#define THEME_LOAD(themeName)                   Image_LoadTheme(themeName)

/************************************************************************/
/* 接口       ： THEME_UNLOAD()                                          */
/* 功能       ： 主题资源释放,释放主题所有的资源                     */
/* 参数说明 ： 无
*/
/* 返回值    ： 无                                                     */
/************************************************************************/
#define THEME_UNLOAD()                          Image_UnloadTheme()

/************************************************************************/
/* 接口       ： THEME_GET_BMP()                                         */
/* 功能       ： 获取加载的主题图片，获取不到则返回一张空的图片       */
/* 参数说明 ：                                                         */
/*      1.bmpName：图片路径                                        */
/* 返回值    ： 无                                                     */
/************************************************************************/
#define THEME_GET_BMP(bmpName)                  (Image_GetImage(bmpName))

/************************************************************************/
/* 接口       ： STOCK_GET_BMP()                                         */
/* 功能       ： 获取非主题的图片资源，获取不到则加载该图片         */
/* 参数说明 ：                                                         */
/*      1.bmpName：图片路径                                        */
/* 返回值    ： 无                                                     */
/************************************************************************/
#define STOCK_GET_BMP(bmpPath)                  (Image_GetImage(bmpPath, DIT_NOTTHEME))

/************************************************************************/
/* 接口       ： STOCK_FREE_BMP()                                            */
/* 功能       ： 从缓存池中移除对应的图片资源                          */
/* 参数说明 ：                                                         */
/*      1.bmpName：图片路径                                        */
/* 返回值    ： 无                                                     */
/************************************************************************/
#define STOCK_FREE_BMP(bmpPath)                 Image_RemoveImage(bmpPath)

#define TMP_GET_BMP(bmp, bmpPath)               (Image_GetImage(bmp, bmpPath))

#define Theme_Draw_9_Pix(refPainter, lpszFilePath, PaintRect) Image_Draw_9_Pix(refPainter, lpszFilePath, PaintRect)

#endif // _MOD_DIR_IMAGE_MANAGER_H_
