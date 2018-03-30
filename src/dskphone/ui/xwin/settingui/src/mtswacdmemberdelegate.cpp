#include "mtswacdmemberdelegate.h"

#if IF_FEATURE_METASWITCH_ACD

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


namespace
{

#define ACDLIST_NOTE_MSGBOX_SHOW_TIME     1500
}

CMtswAcdMemberDelegate::CMtswAcdMemberDelegate()
{
}

CMtswAcdMemberDelegate::~CMtswAcdMemberDelegate()
{
}

CSettingUIDelegateBase* CMtswAcdMemberDelegate::CreatMtswAcdMemberDelegate()
{
    CMtswAcdMemberDelegate* pDelegate = new CMtswAcdMemberDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CMtswAcdMemberDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_SEND;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
}

bool CMtswAcdMemberDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_SEND == iType)
    {
        OnActionSend();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CMtswAcdMemberDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_OK:
            {
                OnActionSend();
                return true;
            }
            break;
        case PHONE_KEY_LEFT:
        case PHONE_KEY_RIGHT:
            {
                VEC_SMLGG_DATA vecMLGHData;
                MTSWACD_GetMLHGList(vecMLGHData);
                yl::string strCurMLHGName = MTSWACD_GetCurrentMLHG();
                int i = 0;
                for (; i < vecMLGHData.size(); ++i)
                {
                    if (vecMLGHData[i].strName == strCurMLHGName)
                    {
                        break;
                    }
                }

                int nSwitchIndex;
                bool bNext = false;
                if (PHONE_KEY_RIGHT == nKeyCode)
                {
                    bNext = true;
                }

                if (!SettingUI_GetSwitchIndex(vecMLGHData.size(), i, bNext, nSwitchIndex))
                {
                    return false;
                }

                // 重新设置当前mlhg组
                MTSWACD_SetCurrentMLHG(vecMLGHData[nSwitchIndex].strName);
                g_SetUICallBackFun.RefreshUI(true);
                return true;
            }
            break;
        default:
            break;
        }
    }


    return CSettinguiDataDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CMtswAcdMemberDelegate::GetMLHGMembers()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    VEC_MEMBER_DATA vecMemberData;
    //获取数据
    MTSWACD_GetMLHGMemberList(vecMemberData);
    for (int i = 0; i < vecMemberData.size(); ++i)
    {
        const yl::string& strDisp = vecMemberData[i].strDirectoryNumber;
        if (strDisp.empty())
        {
            continue;
        }

        yl::string strStatus = vecMemberData[i].bLoggedIn ? _LANG(TRID_MTSW_ACD_LOGIN)
                               : _LANG(TRID_MTSW_ACD_LOGOUT);

        AddBaseItemData(m_pPageData, strDisp, strStatus);
    }
}

void CMtswAcdMemberDelegate::OnActionSend()
{
    // 呼出列表成員
    CSettingUIItemDataBase* pBaseItemData = GetItemDataByDataIndex(GetFocusIndex());
    if (NULL == pBaseItemData)
    {
        return;
    }

    yl::string strDisplayName = pBaseItemData->GetDisplayName();
    if (!strDisplayName.empty())
    {
        talklogic_CallOut(strDisplayName, "");
    }
}

CSettingUIPageDataBase* CMtswAcdMemberDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        pPageData->SetPageTitle(MTSWACD_GetCurrentMLHG());
        GetMLHGMembers();
    }

    return pPageData;
}

#endif

