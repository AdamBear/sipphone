/************************************************************************
 * FileName  : CAnywhereController.h (header file)
 * Purpose   :
 * Date      : 2012/08/11 11:29:07
 ************************************************************************/

#ifndef _CANYWHERECONTROLLER_H_
#define _CANYWHERECONTROLLER_H_

#include "anywherecommon.h"

bool AnyWhere_RequestAddLocation(const SAYWLocation & sLoct, bool bCreatorbyModify = false);

bool AnyWhere_RequestAllLocationList();

bool AnyWhere_RequestModifyLocation(const yl::string & strOriginalPhoneNumber,
                                    const SAYWLocation & sLoct);

bool AnyWhere_RequestDeleteLocation(const yl::string & strPhoneNumber);

bool AnyWhere_RequestDeleteAllLocation();

bool AnyWhere_RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList,
        SAYWLocationArray & arry);

bool AnyWhere_RequestLocationDetail(const yl::string & strPhoneNumber);


//取消所有的命令请求
void AnyWhere_CancelAllRequest();

void AnyWhere_GetAllLocationsQueryResultList(SAYWLocationArray & arry);

bool AnyWhere_GetLocationDetailQueryResult(SAYWLocation & sLoct);

size_t AnyWhere_GetCachedLocationCount();

//设置哪个账号
void AnyWhere_SetLineID(int nLineID);

//获取账号
int AnyWhere_GetLineID();

//判断号码是否存在
bool AnyWhere_CheckNumberExit(const char * szNumber);

#endif // _CANYWHERECONTROLLER_H_
