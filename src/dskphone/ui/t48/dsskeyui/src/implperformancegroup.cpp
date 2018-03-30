#include "implperformancegroup.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "contacts/directory/include/moddirinterface.h"
#include "uikernel/languagehelper.h"
#include "dsskeyuilogic/include/moddsskeyuilogic.h"
#include "uicommon/qtcommon/qmisc.h"

////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceGroupBase::ImplDsskeyPerformanceGroupBase(DssKey_Type type)
    : ImplDsskeyPerformanceBase(type)
{

}

ImplDsskeyPerformanceGroupBase::~ImplDsskeyPerformanceGroupBase()
{

}

// 获取灯显示
bool ImplDsskeyPerformanceGroupBase::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    yl::string strContactName = dsskeyuilogic_GetContactLabel(logicData);
    if (GetExtendFlag())
    {
        // 联系人群组名称获取时已经做了LANG_TRANSLATE,在扩展台必须直接LANG_PARSE进行翻译
        strContactName = fromQString(LANG_PARSE(toQString(strContactName)));
    }

    if (!strContactName.empty() && logicData.strLabel.empty())
    {
        LabelData.strFirstRowText = strContactName;
        return true;
    }
    return false;
}

// 获取图标显示
bool ImplDsskeyPerformanceGroupBase::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_LOCALGROUP;
    return true;
}

//////////////////////////DT_LOCALGROUP/////////////////////////////////////

ImplDsskeyPerformanceLocalGroup::ImplDsskeyPerformanceLocalGroup()
    : ImplDsskeyPerformanceGroupBase(DT_LOCALGROUP)
{

}

ImplDsskeyPerformanceLocalGroup::~ImplDsskeyPerformanceLocalGroup()
{

}

//////////////////////////DT_XMLGROUP/////////////////////////////////////

ImplDsskeyPerformanceXMLGroup::ImplDsskeyPerformanceXMLGroup()
    : ImplDsskeyPerformanceGroupBase(DT_XMLGROUP)
{

}

ImplDsskeyPerformanceXMLGroup::~ImplDsskeyPerformanceXMLGroup()
{

}

//////////////////////////DT_BSGROUP/////////////////////////////////////

ImplDsskeyPerformanceBrsfGroup::ImplDsskeyPerformanceBrsfGroup()
    : ImplDsskeyPerformanceGroupBase(DT_BSGROUP)
{

}

ImplDsskeyPerformanceBrsfGroup::~ImplDsskeyPerformanceBrsfGroup()
{

}

#if IF_BUG_PLCMDIR
///////////////DT_PLCM_PHONEBOOK/////////////////////////
ImplDsskeyPerformancePlcm::ImplDsskeyPerformancePlcm()
    : ImplDsskeyPerformanceBase(DT_PLCM_PHONEBOOK)
{

}

ImplDsskeyPerformancePlcm::~ImplDsskeyPerformancePlcm()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePlcm::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_LOCALGROUP;
    return true;
}

#endif
