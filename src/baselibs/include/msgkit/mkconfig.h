/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2013-03-02
*
* for internal config header file , MUST NOT export!
*
****************************************************************** */
#ifndef __MK_CONFIG_H__
#define __MK_CONFIG_H__

/************************************************************************/
/*                         version config                               */
/************************************************************************/
/*
* enable pthread key for private handle
*/
#define  MKIT_CONFIG_TASKKEY     1

/*
* enable handle lock inside
*/
#define  MKIT_CONFIG_TASKLOCK    1

/*
* enable handle safe check
*/
#define  MKIT_CONFIG_SAFECHECK   1

/*
* enable error log
*/
#define  MKIT_CONFIG_ERRLOG      1

/************************************************************************/
/* common helper micro                                                  */
/************************************************************************/
/*enable instance lock*/
#if MKIT_CONFIG_TASKLOCK
#define INST_LOCK_GET(_inst)  MUX_LOCK(_inst->lock)
#define INST_LOCK_PUT(_inst)  MUX_UNLOCK(_inst->lock)
#else
#define INST_LOCK_GET(_inst)
#define INST_LOCK_PUT(_inst)
#endif

/*safe check for handle*/
#if MKIT_CONFIG_SAFECHECK
#define INST_CHECK(_inst,_ret,_magic) \
    do {  \
    if((_inst)==NULL || (_inst)->magic!=_magic) \
    return _ret;\
    } while (0)
#else
#define INST_CHECK(_inst,_ret,_magic)
#endif

/************************************************************************/
/*                         version log                                  */
/************************************************************************/
/*
* 1.0.0.0  -> 1.0.0.0:第一次版本
*/

#if defined(_DEBUG)
#define  MKIT_CONFIG_RELEASE "debug"
#else
#define  MKIT_CONFIG_RELEASE "release"
#endif


#define  MKIT_CONFIG_LIBRARY   "mkservice"
#define  MKIT_CONFIG_VERSION   "1.0.0.0"
#define  MKIT_CONFIG_BUILD     MKIT_CONFIG_VERSION"-"MKIT_CONFIG_RELEASE"("__DATE__","__TIME__")"


/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __MK_CONFIG_H__ */
