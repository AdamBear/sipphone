#ifndef _DEBUG_COMMON_FUNC_H_
#define _DEBUG_COMMON_FUNC_H_

#ifdef IF_FEATURE_TOOLS

/**********************************************************************************************//**
 * @fn  void debug_PrintAll();
 *
 * @brief   Debug print all.打印所有的信息：1、系统运行的信息（脚本） 2、各个目录空间大小 3、FD
 *          注意：由于业务模块，可能会需要debug模块的基础接口，因此代码放到commonUnit，但是不允许业务逻辑
 *          直接调用debug.h的接口。业务逻辑统一通过modCommonUnit封装出去
 * @author  Yl 0487
 * @date    2017/1/9
 **************************************************************************************************/

void debug_PrintAll();

/**********************************************************************************************//**
 * @fn  void debug_PrintTask();
 *
 * @brief   Debug print task.打印指定的任务信息
 *
 * @author  Yl 0487
 * @date    2017/1/9
 **************************************************************************************************/
void debug_PrintTask(int nPID);

/**********************************************************************************************//**
 * @fn  void debug_PrintTask(const char* pName);
 *
 * @brief   Debug print task.打印指定的任务信息
 *
 * @author  Yl 0487
 * @date    2017/1/9
 *
 * @param   pName   The name.
 **************************************************************************************************/

void debug_PrintTask(const char * pName);

/**********************************************************************************************//**
 * @fn  void debug_PrintSpace(const char* pName);
 *
 * @brief   Debug print space.打印指定目录的空间，如果是NULL，则统计根目录下空间大小（/tmp、/data、/config等）
 *
 * @author  Yl 0487
 * @date    2017/1/9
 *
 * @param   pName   The name.
 **************************************************************************************************/

void debug_PrintSpace(const char * pName);

/**********************************************************************************************//**
 * @fn  const char* debug_MapMesgToName(int nMesg);
 *
 * @brief   Debug map message to name.消息值映射消息名
 *
 * @author  Yl 0487
 * @date    2017/1/9
 *
 * @param   nMesg   The message.
 *
 * @return  Null if it fails, else a pointer to a const char.
 **************************************************************************************************/

const char * debug_MapMesgToName(unsigned int nMesg);

/**********************************************************************************************//**
 * @fn  void debug_PrintQueue();
 *
 * @brief   Debug print queue.打印消息队列：并且将其中的消息值映射成消息名
 *
 * @author  Yl 0487
 * @date    2017/1/9
 **************************************************************************************************/

void debug_PrintQueue();

/**********************************************************************************************//**
 * @fn  void debug_PrintTimer();
 *
 * @brief   Debug print timer.打印定时器队列
 *
 * @author  Yl 0487
 * @date    2017/1/9
 **************************************************************************************************/

void debug_PrintTimer();


void debug_PrintMemStatic();

void debug_PrintFlashStatic();

void debug_PrintNetInfo();

void debug_AnalysisLog(char * pLog);

void debug_Tool_Map_Msg(const char * pMsg);

void debug_Tool_Upgrade_Rom(const char * pPath);

void debug_Tool_Get_Cfg(const char * pKey);

void debug_dump_logic_info(unsigned long handle, const char* szType);
// 查询消息
//
// 查询st_xxx
//
// 查询dt_xxx
#endif //#ifdef IF_FEATURE_TOOLS

// 诊断功能也要使用
void debug_PrintUptime(char * szTime);

bool debug_GetTotalMemInfo(int & nTotal, int & nFree, int & nBuff, int & nCached);
#endif //_DEBUG_COMMON_FUNC_H_

