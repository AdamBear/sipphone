#ifndef _SYSTEM_COMMON_API_HEADER_
#define _SYSTEM_COMMON_API_HEADER_

#include <stdio.h>
#include <ETLLib/ETLLib.hpp>
#include <ylstl/yllist.h>
#include <ylstl/ylstring.h>
#include "common_api_enumtypes.h"

#define IF_COMMONAPI_SYSTEM  1

/*******************************************************************
** 函数描述:   生成密码
** 参数：      [in] strURL: http头信息
** 返回:       无
********************************************************************/
yl::string commonAPI_EncryptPassword(const char * strpwd, const char * salt);

yl::string commonAPI_EncryptPassword(LPCSTR lpNewPassword);

/*******************************************************************
** 函数描述:   检查密码是否和用户szUser的密码匹配
** 参数：      [in] szUser: 用户名
**             [in] szPwd: 密码
** 返回:       TRUE:匹配成功, FALSE:匹配失败
********************************************************************/
BOOL commonAPI_CheckPassword(const char * szUser, const char * szPwd);

/*******************************************************************
** 函数描述:   获取时间
** 参数：      无
** 返回:       当前时间
********************************************************************/
SYSTEMTIME commonAPI_GetTime();

SYSTEMTIME commonAPI_GetTime(time_t tTime);

/*******************************************************************
** 函数描述:   获取随机数
** 参数：      无
** 返回:       随机数
********************************************************************/
UINT commonAPI_GetRand();

/*******************************************************************
** 函数描述:   获取文件大小
** 参数：      文件名
** 返回:       文件大小
********************************************************************/
LRESULT commonAPI_GetFileSize(LPCSTR FileName);

/*******************************************************************
** 函数描述:   判断传入的路径是否是文件夹
** 参数：      strDirectory 路径
** 输出：
** 返回:      true 是文件夹 false 不是文件夹
********************************************************************/
bool commonAPI_isValidDirectory(const yl::string & strDirectory);

/*******************************************************************
** 函数描述:   根据扩展名获取指定目录下的文件列表
** 参数：      strDirectory 待查找目录
               strExtName 扩展名,空或者'*'获取所有
               strExtName可以为"png,xml"或"*.png;*.xml"或".png;.xml" ""和"*"表示所有文件
               isFullPath 是否返回全路径
               isCaseSensitive 扩展名是否区分大小写
               isRecursion 是否递归
** 输出：        plistFileFound: 文件字符串集合
** 返回:
********************************************************************/
bool commonAPI_getFilesInDirectoryByExt(YLList<yl::string> * plistFileFound,
                                        const yl::string & strDirectory,
                                        const yl::string & strExtName = "", bool isFullPath = false, bool isCaseSensitive = false,
                                        bool isRecursion = false);

/*******************************************************************
** 函数描述:   根据扩展名获取指定目录下的目录列表
** 参数：      strDirectory 待查找目录
               isFullPath 是否返回全路径
** 输出：        plistDirectory: 返回数据集合
** 返回:
********************************************************************/
bool commonAPI_getDirectoryInDirectory(YLList<yl::string> * plistDirectory,
                                       const yl::string & strDirectory, bool isFullPath = false);

/*******************************************************************
** 函数描述:   根据扩展名获取指定目录下的所有项
** 参数：      strDirectory 待查找目录
               isFullPath 是否返回全路径
** 输出：        plistDirectory: 返回数据集合
** 返回:
********************************************************************/
bool commonAPI_getItemsInDirectory(YLList<yl::string> * plistDirectory,
                                   const yl::string & strDirectory, bool isFullPath = false);

/*******************************************************************
** 函数描述:   将获取到的文件名按升序排列
** 参数：      plistFileFound 文件名List
** 输出：        排序完后的plistFileFound
** 返回:      有进行排序 true  没有进行排序 false
********************************************************************/
bool commonAPI_sortItemASC(YLList<yl::string> & plistFileFound);

/*******************************************************************
** 函数描述:   将获取到的文件名按降序排列
** 参数：      plistFileFound 文件名List
** 输出：        排序完后的plistFileFound
** 返回:      有进行排序 true  没有进行排序 false
********************************************************************/
bool commonAPI_sortItemDESC(YLList<yl::string> & plistFileFound);

/*******************************************************************
** 函数描述:   生成crc校验码
** 参数：
** 输出：
** 返回:      生成的 crc 校验码
********************************************************************/
unsigned long commonAPI_crc32(unsigned long crc, const char * buf, size_t len);

/*******************************************************************
** 函数描述:   线程安全的system系统调用
** 参数：      命令字符串
** 输出：
** 返回:        执行结果
********************************************************************/
bool commonAPI_System(const char * lpszSystemCmd);

/* 保存密码接口 */
bool commonAPI_SavePassword(const char * szUser, const char * szPwd);

#ifdef WIN32
/* Baselib 通知外壳时间接口 */
bool commonAPI_NotifySimulator(int iEventType = 0, int iEventParam = 0);
#endif

#endif
