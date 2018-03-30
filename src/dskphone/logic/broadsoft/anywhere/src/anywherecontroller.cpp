/************************************************************************
 * FileName  : CAnywhereController.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/08/11 11:29:17
 ************************************************************************/
#include "anywherecontroller.h"
#include "anywhereprocess.h"
#include <ETLLib.hpp>

bool AnyWhere_RequestAddLocation(const SAYWLocation & sLoct, bool bCreatorbyModify /*= false*/)
{
    return g_pAnywhereProcesser->RequestAddLocation(sLoct, bCreatorbyModify);
}

bool AnyWhere_RequestAllLocationList()
{
    return g_pAnywhereProcesser->RequestLocationList();
}

bool AnyWhere_RequestModifyLocation(const yl::string & strOriginalPhoneNumber,
                                    const SAYWLocation & sLoct)
{
    return g_pAnywhereProcesser->RequestModifyLocation(strOriginalPhoneNumber, sLoct);
}

bool AnyWhere_RequestDeleteLocation(const yl::string & strPhoneNumber)
{
    return g_pAnywhereProcesser->RequestDeleteLocation(strPhoneNumber);
}

bool AnyWhere_RequestDeleteAllLocation()
{
    return g_pAnywhereProcesser->RequestDeleteAllLocation();
}

bool AnyWhere_RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList,
        SAYWLocationArray & arry)
{
    return g_pAnywhereProcesser->RequestModifyMutilLocations(strOriginalPhoneNumberList, arry);
}

bool AnyWhere_RequestLocationDetail(const yl::string & strPhoneNumber)
{
    return g_pAnywhereProcesser->RequestLocationDetail(strPhoneNumber);
}


void AnyWhere_CancelAllRequest()
{
    g_pAnywhereProcesser->CancelAllRequest();
}

size_t AnyWhere_GetCachedLocationCount()
{
    return g_pAnywhereProcesser->GetCachedLocationCount();
}

void AnyWhere_GetAllLocationsQueryResultList(SAYWLocationArray & arry)
{
    g_pAnywhereProcesser->GetAllLocationsQueryResultList(arry);
}

bool AnyWhere_GetLocationDetailQueryResult(SAYWLocation & sLoct)
{
    return g_pAnywhereProcesser->GetLocationDetailQueryResult(sLoct);
}

//设置哪个账号
void AnyWhere_SetLineID(int nLineID)
{
    g_pAnywhereProcesser->SetLineID(nLineID);
}

//获取账号
int AnyWhere_GetLineID()
{
    return g_pAnywhereProcesser->GetLineID();
}

//判断号码是否存在
bool AnyWhere_CheckNumberExit(const char * szNumber)
{
    return g_pAnywhereProcesser->CheckNumberExit(szNumber);
}

