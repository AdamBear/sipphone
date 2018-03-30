#ifndef _RES_INTERFACE_HEADER_
#define _RES_INTERFACE_HEADER_

#include <ylstl/ylsmartptr.h>
#include <ylstl/yllist.h>
#include "resourcedata.h"

// c compatiable interface
#define RES_TYPE const char *

/**********************************************************************************************//**
 * @fn  int res_reload(RES_TYPE resType);
 *
 * @brief   重新加载指定类型的文件 resType : all时表示加载所有资源 .
 *
 * @param   resType Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在
 **************************************************************************************************/

int res_reload(RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_init();
 *
 * @brief   初始化资源模块.
 *
 * @return   0 成功 其余 错误码.
 **************************************************************************************************/

int res_init();

/**********************************************************************************************//**
 * @fn  int res_save();
 *
 * @brief   保存资源文件 .
 *
 * @return  0 成功 其余 错误码.
 **************************************************************************************************/

int res_save();

/**********************************************************************************************//**
 * @fn  int res_uninit();
 *
 * @brief   释放资源管理模块所用资源 .
 *
 * @return  0 成功 其余 错误码.
 **************************************************************************************************/

int res_uninit();

/**********************************************************************************************//**
 * @fn  int res_import(const char *path, const char *resName, RES_TYPE resType, int forceReplace);
 *
 * @brief   导入资源项 .
 *
 * @param   path            Full pathname of the file.
 * @param   resName         Name of the resource.
 * @param   resType         Type of the resource.
 * @param   forceReplace    1 名称强制覆盖, 其余 不进行覆盖
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在 2 名称重复 3 路径已经存在.
 **************************************************************************************************/

int res_import(const char * path, const char * resName, RES_TYPE resType, int forceReplace);

/**********************************************************************************************//**
 * @fn  int res_remove(const char * resName, RES_TYPE resType);
 *
 * @brief   删除资源文件.
 *
 * @param   resName Name of the resource.
 * @param   resType Type of the resource.
 *
 * @return   0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_remove(const char * resName, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_clear(RES_TYPE resType);
 *
 * @brief   清空指定的资源，保留系统资源 仅从资源配置文件删除，不删除文件 .
 *
 * @param   resType Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_clear(RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_importPath(const char *pStrPath, RES_TYPE resType);
 *
 * @brief   导入路径下所有资源, 用扩展名区分，扩展名由资源类型指定 .
 *
 * @param   pStrPath    Full pathname of the string file.
 * @param   resType     Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_importPath(const char * pStrPath, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_isResExisted(const char * resItemName, RES_TYPE res_Type);
 *
 * @brief   资源是否存在.
 *
 * @param   resItemName Name of the resource item.
 * @param   res_Type    Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_isResExisted(const char * resItemName, RES_TYPE res_Type);

/**********************************************************************************************//**
 * @fn  int res_setResDispName(const char *dispName, const char * restItemName, RES_TYPE res_Type);
 *
 * @brief   设置资源显示名称 .
 *
 * @param   dispName        Name of the disp.
 * @param   restItemName    Name of the REST item.
 * @param   res_Type        Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_setResDispName(const char * dispName, const char * restItemName, RES_TYPE res_Type);

/**********************************************************************************************//**
 * @fn  int res_setResPath(const char *path, const char * restItemName, RES_TYPE res_Type);
 *
 * @brief   设置资源路径 .
 *
 * @param   path            Full pathname of the file.
 * @param   restItemName    Name of the REST item.
 * @param   res_Type        Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在 3 路径已经存在.
 **************************************************************************************************/

int res_setResPath(const char * path, const char * restItemName, RES_TYPE res_Type);

/**********************************************************************************************//**
 * @fn  int res_getResItemsByResType(YLList<YLSmartPtr<BaseResItem> > &listOutput, RES_TYPE resType);
 *
 * @brief    获取指定类型的资源项列表 .
 *
 *
 * @param [in,out]  listOutput  返回的文件列表.
 * @param           resType     资源类型.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getResItemsByResType(YLList<YLSmartPtr<BaseResItem> > & listOutput, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_getResItemsByResType(YLList<YLSmartPtr<BaseResItem> > &listOutput, RES_TYPE resType, bool bGetDefaul);
 *
 * @brief   获取指定类型的资源项列表.
 *
 * @param [in,out]  listOutput  返回的文件列表.
 * @param           resType     资源类型.
 * @param           bGetDefaul  是否需要包含有默认项.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getResItemsByResType(YLList<YLSmartPtr<BaseResItem> > & listOutput, RES_TYPE resType,
                             bool bGetDefaul);

/**********************************************************************************************//**
 * @fn  int res_getImportedResItemsByResType(YLList<YLSmartPtr<BaseResItem> > &listOutput, RES_TYPE resType);
 *
 * @brief   获取指定类型的导入资源项列表 .
 *
 * @param [in,out]  listOutput  The list output.
 * @param           resType     Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getImportedResItemsByResType(YLList<YLSmartPtr<BaseResItem> > & listOutput,
                                     RES_TYPE resType);


typedef const char * RES_ITEM_TYPE;

/**********************************************************************************************//**
 * @fn  int res_getResItemsByItemType(YLList<YLSmartPtr<BaseResItem> > &listOutput, RES_TYPE resType, RES_ITEM_TYPE itemType);
 *
 * @brief   获取指定类型的资源项列表.
 *
 * @param [in,out]  listOutput  The list output.
 * @param           resType     Type of the resource.
 * @param           itemType    Type of the item.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getResItemsByItemType(YLList<YLSmartPtr<BaseResItem> > & listOutput, RES_TYPE resType,
                              RES_ITEM_TYPE itemType);

/**********************************************************************************************//**
 * @fn  int res_getResTypeList(YLList<yl::string> &listOutput);
 *
 * @brief   获取资源类型列表 .
 *
 * @param [in,out]  listOutput  The list output.
 *
 * @return    0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getResTypeList(YLList<yl::string> & listOutput);

/**********************************************************************************************//**
 * @fn  int res_getResListByType(YLList<yl::string> &listOutput, RES_TYPE resType);
 *
 * @brief   获取指定类型下资源的名称 .
 *
 * @param [in,out]  listOutput  The list output.
 * @param           resType     Type of the resource.
 *
 * @return  0 成功 其余 错误码 1 资源类型不存在.
 **************************************************************************************************/

int res_getResListByType(YLList<yl::string> & listOutput, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  YLSmartPtr<BaseResItem> res_getResItem(const char *pItemName, RES_TYPE resType);
 *
 * @brief   根据类型和名称获取资源项 .
 *
 * @param   pItemName   Name of the item.
 * @param   resType     Type of the resource.
 *
 * @return  1 资源类型不存在;
 **************************************************************************************************/

YLSmartPtr<BaseResItem> res_getResItem(const char * pItemName, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  YLSmartPtr<BaseResItem> res_getResItembyIndex(int index, RES_TYPE resType);
 *
 * @brief   获取指定索引位置的资源项 .
 *
 * @param   index   Zero-based index of the.
 * @param   resType Type of the resource.
 *
 * @return  1 资源类型不存在;
 **************************************************************************************************/

YLSmartPtr<BaseResItem> res_getResItembyIndex(int index, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  YLSmartPtr<BaseResItem> res_getDefaultItem(RES_TYPE resType);
 *
 * @brief   获取默认值.
 *
 * @param   resType Type of the resource.
 *
 * @return  A YLSmartPtr<BaseResItem>;
 **************************************************************************************************/

YLSmartPtr<BaseResItem> res_getDefaultItem(RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_setDefault(const char *resItemName, RES_TYPE resType);
 *
 * @brief   设置默认资源项.
 *
 * @param   resItemName Name of the resource item.
 * @param   resType     Type of the resource.
 *
 * @return  An int.
 **************************************************************************************************/

int res_setDefault(const char * resItemName, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  int res_getResPath(yl::string &strOutput, const char *resItemName, RES_TYPE reType);
 *
 * @brief   获取资源路径 .
 *
 * @param [in,out]  strOutput   The output.
 * @param           resItemName Name of the resource item.
 * @param           reType      Type of the re.
 *
 * @return  0 成功 1：资源类型不存在 2：资源不存在.
 **************************************************************************************************/

int  res_getResPath(yl::string & strOutput, const char * resItemName, RES_TYPE reType);

/**********************************************************************************************//**
 * @fn  int res_getDefaultPath(yl::string &strOutput, RES_TYPE reType);
 *
 * @brief   获取默认的资源项路径 .
 *
 * @param [in,out]  strOutput   The output.
 * @param           reType      Type of the re.
 *
 * @return  0 成功 1：资源类型不存在 2：资源不存在.
 **************************************************************************************************/

int res_getDefaultPath(yl::string & strOutput, RES_TYPE reType);

/**********************************************************************************************//**
 * @fn  int res_getDispNameByName(yl::string &strOutput, const char *resName, RES_TYPE reType);
 *
 * @brief   获取资源的显示名 .
 *
 * @param [in,out]  strOutput   The output.
 * @param           resName     Name of the resource.
 * @param           reType      Type of the re.
 *
 * @return  0 成功 1：资源类型不存在 2：资源不存在.
 **************************************************************************************************/

int res_getDispNameByName(yl::string & strOutput, const char * resName, RES_TYPE reType);

/**********************************************************************************************//**
 * @fn  RES_ITEM_TYPE res_getResItemType(const char *pItemName, RES_TYPE resType);
 *
 * @brief   获取资源类型.
 *
 * @param   pItemName   Name of the item.
 * @param   resType     Type of the resource.
 *
 * @return  A RES_ITEM_TYPE.
 **************************************************************************************************/

RES_ITEM_TYPE res_getResItemType(const char * pItemName, RES_TYPE resType);


/**********************************************************************************************//**
 * @fn  bool res_toRealPath(yl::string& strPath, int nType);
 *
 * @brief   获取路径的全名.
 *
 * @param [in,out]  strPath Full pathname of the file.
 * @param           nType   The type.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool res_toRealPath(yl::string & strPath, int nType);


/**********************************************************************************************//**
 * @fn  bool res_toHidePath(yl::string& strPath, int nType);
 *
 * @brief   获取隐藏名, | strPath | [in out] 可以为全名, 也可以为隐藏名. 如果为全名则将全名替换
 *                  为隐藏名, 否则不替换..
 *
 * @param [in,out]  strPath Full pathname of the file.
 * @param           nType   The type.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool res_toHidePath(yl::string & strPath, int nType);

/**********************************************************************************************//**
 * @fn  bool res_getPathByResItemType(yl::string& strPath, RES_ITEM_TYPE itemType, RES_TYPE resType);
 *
 * @brief   Resource get path by resource item type.
 *
 * @param [in,out]  strPath     Full pathname of the file.
 * @param           itemType    Type of the item.
 * @param           resType     Type of the resource.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool res_getPathByResItemType(yl::string & strPath, RES_ITEM_TYPE itemType, RES_TYPE resType);

/**********************************************************************************************//**
 * @fn  yl::string res_getPathByName(const char* szName);
 *
 * @brief   获取配置路径.
 *
 * @param   szName  The name.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string res_getPathByName(const char * szName);

#endif // ! _RES_INTERFACE_HEADER_


