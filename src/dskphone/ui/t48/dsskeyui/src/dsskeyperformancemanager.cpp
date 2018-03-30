#include "dsskeyperformancemanager.h"
#include <ETLLib.hpp>
#include "baseui/t4xpicpath.h"
#include "commonfunc.h"
#include "dsskey/include/moddsskey.h"
#include "edk/include/modedk.h"

DsskeyPerformanceManager & Get_DsskeyPerfManagerInstance()
{
    static DsskeyPerformanceManager perfManagerinstance;
    return perfManagerinstance;
}

DsskeyPerformanceManager::DsskeyPerformanceManager()
{
    //Init();
}

DsskeyPerformanceManager::~DsskeyPerformanceManager()
{

}

// 获取文字显示
void DsskeyPerformanceManager::Init()
{
    DsskeyTypeString_Init();
    m_mappPerformance.clear();
}

// 设置实际的表现
bool DsskeyPerformanceManager::AddImplPerformance(DssKey_Type dsskeyType,
        ImplDsskeyPerformanceBase * pImplPerf)
{
    bool bRet = false;
    if (NULL != pImplPerf)
    {
        m_mappPerformance.put(dsskeyType, pImplPerf);
        bRet = true;
    }
    return bRet;
}

// 设置实际的表现
bool DsskeyPerformanceManager::DelImplPerformance(DssKey_Type dsskeyType,
        ImplDsskeyPerformanceBase * pImplPerf)
{
    bool bRet = false;

    if (NULL != pImplPerf)
    {
        map_DsskeyPerf::iterator it = m_mappPerformance.find(dsskeyType);
        if (it != m_mappPerformance.end())
        {
            ImplDsskeyPerformanceBase * pRealImplPerf = static_cast<ImplDsskeyPerformanceBase *>(it->second);
            if (pImplPerf == pRealImplPerf)
            {
                m_mappPerformance.erase(it);
            }
            else
            {
                DSSKEYUI_WARN("The Implement [%p] of m_vecpPerformance[%s] is not the pImplPerf [%p] ", \
                              pRealImplPerf, GetDsskeyTypeString(dsskeyType).c_str(), pImplPerf);
            }
            bRet = true;
        }
        else
        {
            DSSKEYUI_WARN("Cann't find %s to delete. map_DsskeyPerf Size is [%d] ", \
                          GetDsskeyTypeString(dsskeyType).c_str(), m_mappPerformance.size());
        }
    }

    return bRet;
}

// 获取背景显示
ImplDsskeyPerformanceBase * DsskeyPerformanceManager::GetDSSKeyPerformance(DssKey_Type dsskeyType)
{
    map_DsskeyPerf::iterator it = m_mappPerformance.find(dsskeyType);
    if (it != m_mappPerformance.end())
    {
        ImplDsskeyPerformanceBase * pPerf = static_cast<ImplDsskeyPerformanceBase *>(it->second);
        if (NULL == pPerf)
        {
            DSSKEYUI_WARN("GetDSSKeyPerformance %s Error.", GetDsskeyTypeString(dsskeyType).c_str());
        }
        return pPerf;
    }
    else
    {
        DSSKEYUI_WARN("Cann't find %s. map_DsskeyPerf Size is [%d] ", \
                      GetDsskeyTypeString(dsskeyType).c_str(), m_mappPerformance.size());
    }

    return NULL;
}

bool DsskeyPerformanceManager::ParseDefaultDisplayData(DsskeyDisplayData & displayData,
        const DssKeyLogicData & logicData)
{
    // 默认的字段处理
    displayData.eDssKeytype = logicData.eDKType;
    displayData.iDsskeyID = logicData.iDssKeyID;
    displayData.listAuxiliaryState = logicData.listAuxiliaryState;
    displayData.iIndex = dsskey_GetKeyIndex(logicData.iDssKeyID);
    displayData.eType = logicData.eType;
    return true;
}

bool DsskeyPerformanceManager::ParseDisplayData(DsskeyDisplayData & displayData,
        const DssKeyLogicData & logicData)
{
    bool bResult = false;
    DssKey_Type dsskeyType = logicData.eDKType;
    // 初始化默认数据
    displayData.clear();

    ParseDefaultDisplayData(displayData, logicData);

    // 判断是否扩展台 ，扩展台不翻译
    bool bExt = dsskey_IsExpModule(logicData.eType) && dsskey_IsSupportModuleType(logicData.eType);

    //EDK控制的DSSKey/////////////////////
    bool bShowEDKCtrlDsskey = false;
    EdkCtrlDsskeyDispData objEDKDisplayData;

    bShowEDKCtrlDsskey = EDK_GetEdkCtrlDsskeyDispData(logicData.iDssKeyID, objEDKDisplayData);
    ///////////////////////////////////////////

    DSSKEYUI_INFO("[%s] : Index is %d, Label is %s, iStatus is %d, DK_Type is %s.",
                  bExt ? "Exp key" : "Line key", \
                  displayData.iIndex, logicData.strLabel.c_str(), logicData.iStatus,
                  GetDsskeyTypeString(dsskeyType).c_str());

    if (dsskeyType == DT_NA && !bShowEDKCtrlDsskey)
    {
        return true;
    }

    // 获取一般Label的方法
    if (logicData.strLabel.empty())
    {
        // 为空需手动获取默认的名称
        displayData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(dsskey_GetDefaultLabel(
                                           logicData.eDKType).c_str(), bExt)).toUtf8().data();
    }
    else
    {
        displayData.strFirstRowText = logicData.strLabel;
    }

    ImplDsskeyPerformanceBase * pPerf = GetDSSKeyPerformance(dsskeyType);
    if (NULL != pPerf)
    {
        pPerf->SetExtendFlag(bExt);
        pPerf->GetPerformanceBegin(logicData);

        // 获取文字显示数据
        DsskeyLabelData labelData;
        bool bRet = pPerf->GetLabelPerformance(logicData, labelData);
        if (bRet)
        {
            displayData.eTextMode       = labelData.eTextMode;
            displayData.strFirstRowText = labelData.strFirstRowText;
            displayData.strSecRowText   = labelData.strSecRowText;
        }

        // 获取图标显示数据
        DsskeyIconData iconData;
        bRet = pPerf->GetIconPerformance(logicData, iconData);
        if (bRet)
        {
            displayData.strIconPath     = iconData.strIconPath;
            displayData.strDownIconPath = iconData.strDownIconPath;
            displayData.strFlashIcon1   = iconData.strFlashIcon1;
            displayData.strFlashIcon2   = iconData.strFlashIcon2;
            displayData.eRefreshType    = iconData.eRefreshType;
            displayData.eIconType       = iconData.eIconType;
            displayData.strPresenceIcon = iconData.strPresenceIcon;
        }

        // 获取对应灯显示数据
        DsskeyLightData lightData;
        bRet = pPerf->GetLightPerformance(logicData, lightData);
        if (bRet)
        {
            displayData.eLightColorType     = lightData.eLightColorType;
            displayData.eLightFlashType     = lightData.eLightFlashType;
            displayData.ePageColorType      = lightData.ePageColorType;
            displayData.ePageLightFlashType = lightData.ePageLightFlashType;
            displayData.iLightPriority      = lightData.iLightPriority;
        }

        // 获取背景显示数据
        DsskeyBkgroundData bkGroundData;
        bRet = pPerf->GetBkgroundPerformance(logicData, bkGroundData);
        if (bRet)
        {
            displayData.eGroundPhoto = bkGroundData.eGroundPhoto;
        }
        bResult = true;
    }

    //EDK控制的DSSKey/////////////////////
    if (bShowEDKCtrlDsskey)
    {
        SetEDKCtrlDsskeyDisplay(displayData, objEDKDisplayData);
        bResult = true;
    }
    ///////////////////////////////////////////

    return bResult;
}

void DsskeyPerformanceManager::SetEDKCtrlDsskeyDisplay(DsskeyDisplayData & displayData,
        const EdkCtrlDsskeyDispData & objEDKDisplayData)
{
    if (!objEDKDisplayData.m_strLable.empty())
    {
        displayData.eTextMode = TEXT_MODE_FIRST_ROW;
        displayData.strFirstRowText = objEDKDisplayData.m_strLable;
        displayData.strSecRowText = "";
    }

    if (COLOR_NONE <= objEDKDisplayData.m_eColorType
            && objEDKDisplayData.m_eColorType <= COLOR_ORANGE_PIC_IN)
    {
        if (COLOR_NONE == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_DOWN_BLF_IDLE;
//          displayData.strDownIconPath = "";
        }
        else if (COLOR_RED == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_TALKING;
//          displayData.strDownIconPath = "";
        }
        else if (COLOR_GREEN == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_IDLE;
//          displayData.strDownIconPath = "";
        }
        //根据需求，先不需要实现 (-_-|||)
//      else if (COLOR_ORANGE == objEDKDisplayData.m_eColorType)
//      {
//          displayData.strIconPath = "";
//          displayData.strDownIconPath = "";
//      }
        else if (COLOR_RED_PIC_IN == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_RING;
//          displayData.strDownIconPath = "";
        }
        else if (COLOR_RED_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_CALLOUT;
//          displayData.strDownIconPath = "";
        }
        else if (COLOR_GREEN_PIC_IN == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_IDLE_RING;
//          displayData.strDownIconPath = "";
        }
        else if (COLOR_GREEN_PIC_OUT == objEDKDisplayData.m_eColorType)
        {
            displayData.strIconPath = PIC_DSS_BLF_IDLE_CALLOUT;
//          displayData.strDownIconPath = "";
        }
        //根据需求，先不需要实现 (⊙﹏⊙)
//      else if (COLOR_ORANGE_PIC_OUT == objEDKDisplayData.m_eColorType)
//      {
//          displayData.strIconPath = "";
//          displayData.strDownIconPath = "";
//      }
//      else if (COLOR_ORANGE_PIC_IN == objEDKDisplayData.m_eColorType)
//      {
//          displayData.strIconPath = "";
//          displayData.strDownIconPath = "";
//      }

        displayData.strFlashIcon1 = "";
        displayData.strFlashIcon2 = "";
        displayData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        displayData.strPresenceIcon = "";
    }

    //根据需求，先不需要实现 (@_@)
//  if (BACKGROUND_RED == objEDKDisplayData.m_eBackGroundType)
//  {
//      displayData.eGroundPhoto = GROUNDPHOTO_MODE_RED;
//  }
//  else if (BACKGROUND_GREEN == objEDKDisplayData.m_eBackGroundType)
//  {
//      displayData.eGroundPhoto = GROUNDPHOTO_MODE_GREEN;
//  }
//  else
//  {
//      //
//  }

}

///////////////////////////////////////

