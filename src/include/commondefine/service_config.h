/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-10-23
*
* for export APIs header file , message layer
*
****************************************************************** */
#ifndef __SRV_CFG_H__
#define __SRV_CFG_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>
#include "setting_type_define.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
    /***********************************************************************/
    /* notification  define  id from 0 to 64(for compatible usage)         */
    /***********************************************************************/
    /*
    * usage   :to notify that some config changed
    *          
    * id      :0
    * target  :
    * wparam  :SETTING_TYPE
    * lparam  :
    * data_ptr:none
    * data_len:none
    */
#define CONFIG_MSG_BCAST_CHANGED      MKSRV_MSG(MKSRV_ID_CONFIG,0)

	/*
	* usage   :to notify that all config reset to factory
	*          
	* id      :0
	* target  :
	* wparam  :
	* lparam  :
	* data_ptr:none
	* data_len:none
	*/
#define CONFIG_MSG_BCAST_RESET        MKSRV_MSG(MKSRV_ID_CONFIG,1)


	/***********************************************************************/
	/* notification  define  id from 64 to 256(for new design)             */
	/***********************************************************************/



	/************************************************************************/
	/* APIs for c                                                           */
	/************************************************************************/
	/**
	* @brief  :
	*
	* @param  :
	*
	* @warning:
	*          
	* @return :0 is success,otherwise failed.
	* 
	*/
	int config_init(mkit_handle_t caller);
	int config_exit(mkit_handle_t caller);

	/**
	* @brief  :basic get,string/decrypt/int/float
	*
	* @param  :
	*
	* @warning:
	*          
	* @return :0 is success,otherwise failed.
	* 
	*/
	int config_get_string (mkit_handle_t caller,const char*path,const char*section,const char*key,char *buff,int buff_max);
	int config_get_decrypt(mkit_handle_t caller,const char*path,const char*section,const char*key,char *buff,int buff_max);
	int config_get_int    (mkit_handle_t caller,const char*path,const char*section,const char*key,int  *value);
	int config_get_float  (mkit_handle_t caller,const char*path,const char*section,const char*key,float*value);
  
	/**
	* @brief  :basic set,string/encrypt/int/float
	*
	* @param  :
	*
	* @warning:
	*          
	* @return :0 is success,otherwise failed.
	* 
	*/
	int config_set_string (mkit_handle_t caller,const char*path,const char*section,const char*key,const char *buff,int buff_max);
	int config_get_encrypt(mkit_handle_t caller,const char*path,const char*section,const char*key,const char *buff,int buff_max);
	int config_set_int    (mkit_handle_t caller,const char*path,const char*section,const char*key,int   value);
	int config_set_float  (mkit_handle_t caller,const char*path,const char*section,const char*key,float value);

	/**
	* @brief  :compact APIs
	*
	* @param  :
	*
	* @warning:
	*          
	* @return :0 is success,otherwise failed.
	* 
	*/
   

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __SRV_CFG_H__ */

