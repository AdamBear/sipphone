#ifndef __MOD_MTSW_H_
#define __MOD_MTSW_H_

#if IF_FEATURE_METASWITCH
#include "mtswcommondefine.h"

class CMTSWActionCmdBase;

/*******************************************************************
** 函数描述: 模块初始化
** 参数：    无
** 返回:     无
********************************************************************/
void MTSW_Init();

void MTSW_PreInit();
/*******************************************************************
** 函数描述: 登入Commportal
** 参数：    无
** 返回:     无
********************************************************************/
void MTSW_Commprotal_Login();

/*******************************************************************
** 函数描述: 登出Commportal
** 参数：    无
** 返回:     无
********************************************************************/
void MTSW_Commprotal_Logout();

/*******************************************************************
** 函数描述: 是否已登录Commportal
** 参数：    无
** 返回:     true：已登录；false：未登录
********************************************************************/
bool MTSW_IsLogined();

/*******************************************************************
** 函数描述: Post
** 参数：
    pActionCallback: 各模块用于处理本次执行结果的回调函数
    strURL: URL
    strData: Body
** 返回:成功True，失败False
********************************************************************/
bool MTSW_ExecPost(CMTSWActionCmdBase * pCallback, yl::string strURL, yl::string strData,
                   MtswActionType nType);

/*******************************************************************
** 函数描述: Download
** 参数：
    pActionCallback: 各模块用于处理本次执行结果的回调函数
    strURL: URL
    strFilePath: 下载文件存放路径
    bAsyncParse: 是否异步解析
** 返回:成功True，失败False
********************************************************************/
bool MTSW_ExecDownLoad(CMTSWActionCmdBase * pCallback, yl::string strURL, MtswActionType nType,
                       yl::string strFilePath = "", int nExParm = -1);

bool MTSW_IsCommportalActive();

#endif

#endif  // __MOD_MTSW_H_
