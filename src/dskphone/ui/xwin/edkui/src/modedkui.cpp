#include "edkuimanager.h"
#include "edkuidatamanager.h"
#include "../include/modedkui.h"
#include "edk/include/modedk.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/modsettingui.h"
#include "edkkeytimer.h"
#include "globalinputhook/modinputhook.h"

#if IF_FEATURE_EDK

bool EdkUI_Init()
{
    return _EDKUIManager.Init();
}

bool EdkUI_GetSoftkeyIDByType(SoftKey_TYPE eType, yl::string & strSoftkeyID)
{
    return _EDKUIDataMgr.GetSoftkeyIDByType(eType, strSoftkeyID);
}

yl::string EdkUI_GetPageNameByMenuID(const yl::string & strMenuID)
{
    yl::string strPageName;

    _EDKUIDataMgr.GetPageNameByMenuID(strMenuID, strPageName);

    return strPageName;
}

yl::string EdkUI_GetMenuIDByPageData(const yl::string & strPageName,
                                     const yl::string & strNextPageName, const int & nID)
{
    yl::string strMenuID = "";

    yl::string strPageMapName = _EDKUIManager.GetMapNameByPageData(strPageName, strNextPageName, nID);

    if (EDK_IsTipModeEnable())
    {
        _EDKUIDataMgr.GetMenuIDByPageName(strPageMapName, strMenuID);
    }

    return strMenuID;
}

int EdkUI_GetDirTypeByMenuID(const yl::string & strMenuID)
{
    return _EDKUIDataMgr.GetDirTypeByMenuID(strMenuID);
}

yl::string EdkUI_GetMenuIDByDirType(int DirType)
{
    yl::string strMenuID = "";

    if (EDK_IsTipModeEnable())
    {
        _EDKUIDataMgr.GetMenuIDByDirType(DirType, strMenuID);
    }

    return strMenuID;
}

bool EdkUI_GetKeyTipName(int iKeyType, yl::string & strTipName, TipNameType eType)
{
    if (!EDK_IsTipModeEnable())
    {
        return false;
    }

    if (eType == TIPNAME_SOFTKEY)
    {
        return _EDKUIManager.GetTipName((SoftKey_TYPE)iKeyType, strTipName);
    }
    else if (eType == TIPNAME_PROGRAMKEY)
    {
        return _EDKUIDataMgr.GetProgramkeyIDByType((DssKey_Type)iKeyType, strTipName);
    }

    return false;
}

bool EdkUI_GetMenuTipName(yl::string & strMenuID, const yl::string strPageName, int iDirType)
{
    if (!EDK_IsTipModeEnable())
    {
        return false;
    }

    CBaseDialog* pDialog = UIManager_GetTopWindow();
    if (NULL == pDialog)
    {
        return false;
    }

    if (eWT_Directory == pDialog->GetWindowType()
            || eWT_CallLog == pDialog->GetWindowType())
    {
        _EDKUIDataMgr.GetMenuIDByPageName(strPageName, strMenuID);;
    }
    else if (eWT_Menu == pDialog->GetWindowType()
             || eWT_PageMenu == pDialog->GetWindowType())
    {
        _EDKUIDataMgr.GetMenuIDByDirType(iDirType, strMenuID);
    }

    return !strMenuID.empty();
}

bool EdkUI_OnExcuteSoftkey(const yl::string & strSoftkeyID)
{
    return _EDKUIManager.OnExcuteSoftkey(strSoftkeyID);
}

bool EdkUI_EnterMenu(const yl::string & strMenuiD, int iIndex/* = 0*/)
{
    return _EDKUIManager.EnterMenu(strMenuiD);
}

bool EdkUI_ProgramkeyIDByType(DssKey_Type eType, yl::string & strKeyID)
{
    return _EDKUIDataMgr.GetProgramkeyIDByType(eType, strKeyID);
}

DssKey_Type EdkUI_ProgramkeyTypeByID(const yl::string & strKeyID)
{
    return _EDKUIDataMgr.GetProgramkeyTypeByID(strKeyID);
}

bool EdkUI_SetAllowEnterUIFunc(FuncIsAllowEnterMenu pAllowFunc,
                               const yl::string & strPageName/* = ""*/,
                               const int iDirType/* = 0*/)
{
    return _EDKUIDataMgr.SetAllowEnterUIFunc(pAllowFunc, strPageName, iDirType);
}


void EdkUI_ShowEdkPrompt(int iIndex, int iId, FuncEdkActionCallBack pFun)
{
    // iIndex为外部传来的输入框索引，根据该index获取输入框数据
    ST_EDK_DATA stEdkData;
    stEdkData.iIndex = iIndex;
    stEdkData.iId = iId;
    stEdkData.pCallBackFun = pFun;
    SettingUI_EnterPage(kszPageIDEdkPrompt, (void *)(&stEdkData));
}

void EdkUI_UpdateSoftkey()
{
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (NULL != pBaseDlg)
    {
        pBaseDlg->RefreshSoftkey();
    }
}

void EdkUI_StartKeyTimer()
{
    _EDKKeyTimer.StartKeyTimer();
}

void EdkUI_StopKeyTimer()
{
    _EDKKeyTimer.StopKeyTimer();
}

void EdkUI_SetHintMode(bool bShowMode)
{
    if (EDK_IsTipModeEnable() == bShowMode)
    {
        return;
    }

    if (EDK_SetTipMode(bShowMode))
    {
        CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
        if (NULL != pBaseDlg)
        {
            pBaseDlg->RefreshSoftkey();
            pBaseDlg->RefreshTitle();
        }
    }
}

void EdkUI_GetEDKSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    YLList<SoftKey_Data> listSoftKey;
    Edk_GetEdkSoftKeyList(listSoftKey);

    YLList<SoftKey_Data>::ListIterator iter = listSoftKey.begin();
    for (; iter != listSoftKey.end(); ++iter)
    {
        SoftKey_Data & dataSoftKey = *iter;
        CSoftKeyBarData SoftkeyData;
        SoftkeyData.m_iType = dataSoftKey.eType;
        SoftkeyData.m_strSoftkeyTitle = Edk_GetEdkSoftKeyLabel(dataSoftKey.eType);

        if (dataSoftKey.iPos >= objSoftkeyData.size())
        {
            objSoftkeyData.push_back(SoftkeyData);
        }
        else
        {
            objSoftkeyData.insertAt(dataSoftKey.iPos, SoftkeyData);
        }
    }

    //根据提示模式是否开启显示不同的值
    if (EDK_IsTipModeEnable())
    {
        for (int i = 0; i < objSoftkeyData.size(); ++i)
        {
            CSoftKeyBarData & dataSoftKey = objSoftkeyData[i];

            yl::string strHintTmp;
            EdkUI_GetKeyTipName(dataSoftKey.m_iType, strHintTmp, TIPNAME_SOFTKEY);

            //避免m_strSoftkeyTitle后置为空的softkey在提示模式上显示
            if (!strHintTmp.empty())
            {
                dataSoftKey.m_strSoftkeyTitle = strHintTmp;
            }
        }
    }
}

#else

bool EdkUI_Init()
{
    return false;
}

bool EdkUI_GetSoftkeyIDByType(SoftKey_TYPE eType, yl::string & strSoftkeyID)
{
    return false;
}

yl::string EdkUI_GetPageNameByMenuID(const yl::string & strMenuID)
{
    return "";
}

int EdkUI_GetDirTypeByMenuID(const yl::string & strMenuID)
{
    return false;
}

yl::string EdkUI_GetMenuIDByDirType(int DirType)
{
    return "";
}

bool EdkUI_GetKeyTipName(int iType, yl::string & strTipName, TipNameType eType)
{
    return false;
}

bool EdkUI_GetMenuTipName(yl::string & strMenuID, const yl::string strPageName, int iDirType)
{
    return false;
}

bool EdkUI_OnExcuteSoftkey(const yl::string & strSoftkeyID)
{
    return false;
}

bool EdkUI_EnterMenu(const yl::string & strMenuiD, int iIndex/* = 0*/)
{
    return false;
}

bool EdkUI_ProgramkeyIDByType(DssKey_Type eType, yl::string & strKeyID)
{
    return false;
}

DssKey_Type EdkUI_ProgramkeyTypeByID(const yl::string & strKeyID)
{
    return DT_NONE;
}

bool EdkUI_SetAllowEnterUIFunc(FuncIsAllowEnterMenu pAllowFunc,
                               const yl::string & strPageName/* = ""*/,
                               const int iDirType/* = 0*/)
{

}

void EdkUI_ShowEdkPrompt(int iIndex, int iId, FuncEdkActionCallBack pFun)
{

}

void EdkUI_UpdateSoftkey()
{

}

bool EDKUI_KeyTimerHook(int nKeyCode, bool bPress)
{

}

void EdkUI_SetHintMode(bool bShowMode)
{

}

void EdkUI_VlomueKeyStartTimer()
{

}

void EdkUI_VlomueKeyStopTimer()
{

}

void EdkUI_GetEDKSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{

}

#endif
