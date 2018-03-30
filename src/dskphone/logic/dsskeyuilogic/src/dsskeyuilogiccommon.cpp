#include "dsskey_inc.h"


void DsskeyPageInfo::SetIndex(int iIndex, int iMoudleIndex)
{
    m_iIndex = iIndex;
    DSSKEY_MODULE_TYPE moduleType = CDsskeyUIData::MapMoudle2Logic(m_eUIType);

    if (moduleType != DMT_NON)
    {
        m_iDsskeyID = dsskey_GetDsskeyID(moduleType, iMoudleIndex, m_iIndex);
    }

    DSSKEY_INFO("SetIndex index=[%d] module=[%d][%d] ui=[%d] id=[%d]",
                iIndex, iMoudleIndex, moduleType, m_eUIType, m_iDsskeyID);
}

CDsskeyInrInfo::~CDsskeyInrInfo()
{
    for (VecAttrItem::iterator it = vecInrAttr.begin();
            it != vecInrAttr.end(); ++it)
    {
        delete *it;
    }

    vecInrAttr.clear();
}

CDsskeyInrInfo::CDsskeyInrInfo() : iClassType(BASE_DIR), iRedirectType(DT_NA)
{
    memset(chAuth, '0', MAX_MODE_LENGTH);
}

CDsskeyItemAttr * CDsskeyInrInfo::Find(int iAttrType)
{
    for (VecAttrItem::iterator it = vecInrAttr.begin();
            it != vecInrAttr.end(); ++it)
    {
        if ((*it)->iAttrType == iAttrType)
        {
            return (*it);
        }
    }

    return NULL;
}

CDsskeyItemAttr * CDsskeyInrInfo::CreateItem(int iAttrType)
{
    CDsskeyItemAttr * pAttr = Find(iAttrType);
    if (pAttr == NULL)
    {
        pAttr = new CDsskeyItemAttr;
        pAttr->iAttrType = iAttrType;
        vecInrAttr.push_back(pAttr);
    }

    return pAttr;
}

yl::string CDsskeyItemAttr::strDefaultValue;

bool dsskey_data_check_imp(int dsskeyID, const DssKeyUIData & dsskeyData, CDsskeyPair & errorTips)
{
    /* 一些特殊的校验函数，为了兼容原有流程，同时不影响现有流程，故将其加入特殊过滤函数中 */
    switch (dsskeyData.m_eType)
    {
#ifdef IF_SUPPORT_BLUETOOTH
    case DT_BTLINE:
        {
            if (!Bluetooth_IsMobileConnected())
            {
                errorTips.strValue = TRID_MOBILE_ACCOUNT_IS_NOT_CONNECTED;
                return false;
            }
        }
        break;
#endif
    default:
        break;
    }

    return true;
}

bool CDsskeyItemAttr::IsReadOnly()
{
    return strShow ? _DsskeyUIData.GetFunctionResult(*strShow) : false;
}

bool CDsskeyItemAttr::IsVisible()
{
    return strRead ? _DsskeyUIData.GetFunctionResult(*strRead) : false;
}

const yl::string & CDsskeyItemAttr::GetIme()
{
    return strImeType ? *strImeType : strDefaultValue;
}

const yl::string & CDsskeyItemAttr::GetName()
{
    return strName ? *strName : strDefaultValue;
}

const yl::string & CDsskeyItemAttr::GetRegexp()
{
    return strRegexpID ? *strRegexpID : strDefaultValue;
}

const yl::string & CDsskeyItemAttr::GetDefault()
{
    return strDefault ? *strDefault : strDefaultValue;
}

DSSKEY_BUTTON_TYPE CDsskeyItemAttr::GetButtonType()
{
	return m_eButtonType;
}
