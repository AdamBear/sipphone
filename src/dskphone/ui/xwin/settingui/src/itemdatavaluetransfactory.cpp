#include "itemdatavaluetransfactory.h"

IMPLEMENT_GETINSTANCE(CItemDataValueTransFactory)

CItemDataValueTransFactory::CItemDataValueTransFactory()
{
}

CItemDataValueTransFactory::~CItemDataValueTransFactory()
{
    MAP_CREATE_DATATRANS_FUN::iterator it = m_mapCreateDataTrans.begin();
    for (; it != m_mapCreateDataTrans.end(); ++it)
    {
        CItemDataValueTransBase * pDataTrans = it->second;
        if (NULL != pDataTrans)
        {
            delete pDataTrans;
            pDataTrans = NULL;
        }
    }

    m_mapCreateDataTrans.clear();
}

void CItemDataValueTransFactory::RegisterItemDataValueTrans(int nItemType,
        FUN_CREATE_DATATRANS pFunCreateDataTrans)
{
    if (NULL == pFunCreateDataTrans)
    {
        return;
    }

    CItemDataValueTransBase * pDataTrans = (*pFunCreateDataTrans)();
    m_mapCreateDataTrans[nItemType] = pDataTrans;
}

CItemDataValueTransBase * CItemDataValueTransFactory::GetItemDataValueTrans(int nItemType)
{
    CItemDataValueTransBase * pDataTrans = NULL;
    MAP_CREATE_DATATRANS_FUN::iterator it = m_mapCreateDataTrans.find(nItemType);
    if (it != m_mapCreateDataTrans.end())
    {
        pDataTrans = it->second;
    }

    return pDataTrans;
}


