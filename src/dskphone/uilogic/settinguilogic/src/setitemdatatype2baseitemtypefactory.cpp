#include "setitemdatatype2baseitemtypefactory.h"
#include <ETLLib/ETLLib.hpp>
#include "baseui/include/detaillistitem.h"
#include "dsklog/log.h"

IMPLEMENT_GETINSTANCE(CItemDataTypeTransFactory)


CItemDataTypeTransFactory::CItemDataTypeTransFactory()
{
}

CItemDataTypeTransFactory::~CItemDataTypeTransFactory()
{
}

void CItemDataTypeTransFactory::RegisterItemDataTypeTrans(int eSetItemDataType, int eBaseItemType)
{
	if (m_mapCreateItemDataTypeTrans.end() != m_mapCreateItemDataTypeTrans.find(eSetItemDataType))
	{
#ifndef _WIN32
#warning assert
#endif
//      chASSERT(m_mapCreateItemDataTypeTrans.end() == m_mapCreateItemDataTypeTrans.find(eSetItemDataType)
//          , "[CItemDataTypeTransFactory::RegisterItemDataTypeTrans] repeat register same type item:%0x", eSetItemDataType);
	}

	m_mapCreateItemDataTypeTrans[eSetItemDataType] = eBaseItemType;
}

int CItemDataTypeTransFactory::GetItemDataTypeTrans(int eSetItemDataType)
{
	return m_mapCreateItemDataTypeTrans[eSetItemDataType];
}

int CItemDataTypeTransFactory::GetDefaultItemDataTypeTrans()
{
#ifndef _WIN32
#warning 逻辑不应包含UI
#endif
	return CDetailListItem::GetDetailListItemType();
}
