#if IF_FEATURE_GENBAND_E911
#include "mode911address.h"
#include "e911addressmanager.h"
#include "modconfigparser.h"
#include "configiddefine.h"

void e911_Init()
{
    _E911AddressMgr.Init();
}

bool e911_ReqAddress()
{
    return _E911AddressMgr.ReqE911AddrestList();
}

bool e911_GetAddressListData(E911AddressListData* pData, int nID)
{
    return _E911AddressMgr.GetE911AddressListData(pData, nID);
}

E911AddressData* e911_GetAddressDetailByID(int nID)
{
    return _E911AddressMgr.GetAddressDetailByID(nID);
}

yl::string e911_GetCurrentLocation()
{
    return configParser_GetConfigString(kszE911LocationAddr);
}


bool e911_SetLocationID(int& nID, yl::string& strFullAddr)
{
    return _E911AddressMgr.SetLocationID(nID, strFullAddr);
}

int e911_GetAddressListCount(int nID)
{
    return _E911AddressMgr.GetAddressListCount(nID);
}

bool e911_GetFullLocationAddr(int& nID, yl::string& strFullAddr)
{
    return _E911AddressMgr.GetFullLocationAddr(nID, strFullAddr);
}

void e911_CancelDownload()
{
    _E911AddressMgr.CancelDownload();
}

void e911_ClearListData()
{
    _E911AddressMgr.ClearE911Address();
}

bool e911_IsGenbandE911On()
{
    return _E911AddressMgr.IsGenbandE911On();
}

int e911_GetIDByIndex(int nParentID, int nIndex)
{
    return _E911AddressMgr.GetIDByIndex(nParentID, nIndex);
}

int e911_GetParentIDByID(int nID)
{
    return _E911AddressMgr.GetParentIDByID(nID);
}

#endif