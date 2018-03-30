#ifndef __MOD_CONFIG_TRANS_H__
#define __MOD_CONFIG_TRANS_H__

#include "configtrandefine.h"
#include <ylstl/ylstring.h>

#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/
/* functions                                                            */
/************************************************************************/

/**********************************************************************************************//**
 * @fn  int load_trans(const char* trans_file);
 *
 * @brief   Description: loadconfig translator
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @param   trans_file file path for config translator.
 *
 * @return  The transaction.
 **************************************************************************************************/

int load_trans(const char * trans_file);

/**********************************************************************************************//**
 * @fn  int release_trans();
 *
 * @brief   Releases the transaction.
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @return  An int.
 **************************************************************************************************/

int release_trans();

/**********************************************************************************************//**
 * @fn  config_info_t* get_config_item(const char* name);
 *
 * @brief   Describe: get config item by name
 *            | name | - config name id. returns: config_item pointer. !! Should be release by
 *            $release_config_item func.
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @param   name    The config name.
 *
 * @return  Null if it fails, else the configuration item.
 **************************************************************************************************/

/**********************************************************************************************//**
 * @fn  int get_config_name_by_path(const char* file, const char* sec, const char* key, char* buff, int buff_max, int* pattr);
 *
 * @brief   Gets configuration name by path.
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @param           file        The file.
 * @param           sec         The section.
 * @param           key         The key.
 * @param [in,out]  buff        If non-null, the buffer.
 * @param           buff_max    The buffer maximum.
 * @param [in,out]  pattr       If non-null, the pattr.
 *
 * @return  0:success otherwise:failed.
 **************************************************************************************************/

int get_config_name_by_path(const char * file, const char * sec, const char * key, char * buff, int buff_max, int * pattr);

/**********************************************************************************************//**
 * @fn   const config_info_t* get_config_info_t(const char* name);
 *
 * @brief   Gets config_info_t.
 * @brief   返回配置项信息
 * @author  Song
 * @date    2016/12/9
 *
 * @param   name    The name.
 *
 * @return  Null if it fails, else the point to config_info_t.
 **************************************************************************************************/

//const config_info_t * get_config_info_t(const char * name);

int get_config_simple_info_t(const char * name, config_simple_info_t & simple_info);
/**********************************************************************************************//**
 * @fn   int get_config_path(const char* name, yl::string& file, yl::string& sec, yl::string& key);
 *
 * @brief   Gets configuration path.
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @param           name    The name.
 * @param [in,out]  file    The file.
 * @param [in,out]  sec     The section.
 * @param [in,out]  key     The key.
 *
 * @return  The configuration path.
 **************************************************************************************************/

int get_config_path(const char * name, yl::string & file, yl::string & sec, yl::string & key);


/**********************************************************************************************//**
 * @typedef int (*item_callback)(config_info_t* pitem, int flag)
 *
 * @brief   Defines an alias representing the flag.
 **************************************************************************************************/

//typedef int (*item_callback)(config_info_t * pitem, int flag);
typedef int(*simple_item_callback)(config_simple_info_t * pitem, int flag);

/**********************************************************************************************//**
 * @fn  int traverse_config_items(item_callback item_func, int flag);
 *
 * @brief   Traverse configuration items.
 *
 * @author  Song
 * @date    2016/12/9
 *
 * @param   item_func   The item function.
 * @param   flag        The flag.
 *
 * @return  An int.
 **************************************************************************************************/

//int traverse_config_items(item_callback item_func, int flag);
int traverse_simple_config_items(simple_item_callback item_func, int flag);

#ifdef __cplusplus
};
#endif

#endif // __CONFIG_TRANS_H__


