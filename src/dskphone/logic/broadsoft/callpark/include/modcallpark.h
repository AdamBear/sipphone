#ifndef _MOD_CALLPARK_H_
#define _MOD_CALLPARK_H_

#include <ylstring.h>
#include "broadsoft/xsiactionhelper/src/callidprocess.h"

/************************************************************************/
/* 接口  ： talklogic_CallPark()                                     */
/* 功能  ： 进行CallPark                                               */
/* 参数说明 ：                                                          */
/*     [in]1.iCallId：通话的本地CallId                                    */
/*     [in]2.strExtensionNum：Park目的地的分机号                        */
/*     [in]3.iAccountId：Park通话用的账号ID                             */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool callPark_CallPark(int iCallId, const yl::string & strExtensionNum, int iAccountId = -1);

/************************************************************************/
/* 接口  ： talklogic_GroupCallPark()                                    */
/* 功能  ： 进行GroupCallPark                                          */
/* 参数说明 ：                                                          */
/*     [in]1.iCallId：通话的本地CallId                                    */
/*     [in]2.iAccountId：GroupCallPark用的账号ID                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool callPark_GroupCallPark(int iCallId, int iAccountId = -1);

/************************************************************************/
/* 接口  ： talklogic_RetrieveCallPark()                             */
/* 功能  ： 取回Park的通话                                              */
/* 参数说明 ：                                                          */
/*     [in]1.strExtensionNum：分机号                                    */
/*     [in]2.iAccountId：取回通话所使用的帐号ID                         */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool callPark_RetrieveCallPark(const yl::string & strExtensionNum, int iAccountId = -1);

void callPark_Init();

#endif //_MOD_CALLPARK_H_
