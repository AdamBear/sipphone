#include "pickupmenuprocessor.h"
//#include "talkui/include/modtalkui.h"
#include "pickupmenuuibase.h"
#include "blf/include/modblf.h"
#include "dsskey/include/moddsskey.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/directoryenumtypes.h"

#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

#include "adapterbox/include/modadapterbox.h"

CPickUPMenuProcessor::CPickUPMenuProcessor()
{
    m_pPickUPMenuUI = NULL;
}

CPickUPMenuProcessor::~CPickUPMenuProcessor()
{
    m_pPickUPMenuUI = NULL;
}

//构造完成后 绑定了UI后调用
void CPickUPMenuProcessor::Init()
{

}

//释放前解除UI绑定前调用
void CPickUPMenuProcessor::Uninit()
{

}

//设置绑定的UI
void CPickUPMenuProcessor::SetBindUI(CPickUPMenuUIBase * pPickUPMenuUI)
{
    m_pPickUPMenuUI = pPickUPMenuUI;
}

//获取绑定的UI
CPickUPMenuUIBase * CPickUPMenuProcessor::GetBindUI()
{
    return m_pPickUPMenuUI;
}

//获取 pickupmenu 数据
CPickUPMenuData * CPickUPMenuProcessor::GetPickUPMenuData()
{
    //先更新一下 pickupmenu 数据  然后再返回
    UpdatePickUPMenuData();

    return &m_objPickUPMenuData;

}


//获取softkey数据信息
void CPickUPMenuProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    VecSoftkey.clear();

    //固定显示这4个softkey
    VecSoftkey.push_back(ST_PICKUP);
    VecSoftkey.push_back(ST_DIAL);
    VecSoftkey.push_back(ST_NEWCALL);
    VecSoftkey.push_back(ST_CANCEL);
}

//softKey 按键处理
bool CPickUPMenuProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bResult = false;

    switch (eSoftkey)
    {
    case ST_PICKUP:
        {
            OnPickUpLine();
            bResult = true;
        }
        break;
    case ST_DIAL:
        {
            OnDial();
            bResult = true;
        }
        break;
    case ST_NEWCALL:
        {
            OnNewCall();
            bResult = true;
        }
        break;
    case ST_CANCEL:
        {
            OnCancel();
            bResult = true;
        }
        break;
    default:
        bResult = false;
        break;
    }

    return bResult;

}

//获取title
yl::string CPickUPMenuProcessor::GetTitle()
{
    return _UILOGIC_LANG_TRAN(TRID_CALL_LIST_FOR_PICKUP);
}

//处理pickup事件
void CPickUPMenuProcessor::OnPickUpLine()
{
    if (m_objPickUPMenuData.vectorMenuName.size() <= 0)
    {
        return;
    }

    if (NULL != m_pPickUPMenuUI)
    {
        //获取焦点序号
        int iFocusIndex = m_pPickUPMenuUI->GetFocusPickUPMenuItemIndex();
        int nDsskeyId = blf_GetPickupDsskeyIdByIndex(iFocusIndex);

        if (nDsskeyId == -1)
        {
            TALKUI_WARN("Failed to Get Pickup Menu Item ! Index[%d]", iFocusIndex);
            return;
        }

        // 获取LineId
        int iLineID = dsskey_GetDsskeyLine(nDsskeyId);

        // 获取呼出的号码
        yl::string strCallNum;
        dsskey_GetCallOutNumber(nDsskeyId, strCallNum);

        // 组装扩展数据
        CallOut_Param tCallParam;
        tCallParam.eCallOutType = TCT_PICKUP;
        tCallParam.iPickUpBLFIndex = -1;

        int index = blf_GetDialogIndexIdByIndex(iFocusIndex);

        if ((DT_BLF == dsskey_GetDsskeyType(nDsskeyId)
#ifdef IF_FEATURE_GENBAND
                || DT_BLFGROUP == dsskey_GetDsskeyType(nDsskeyId)
#endif
            )
                && blf_IsSupportAutopPickUpByDsskey(nDsskeyId, index)
                && configParser_GetCfgItemIntValue(kszAccountDialogPickUp, iLineID) == 1)
        {
            DialgInfo refData;
            blf_GetDialogInfoByDsskeyId(refData, nDsskeyId, BLF_Manager, index);

            // 远方的名字, 用于监控方多路来电时具体截答
            tCallParam.strRemoteName = refData.m_strDisplayName;
            tCallParam.iPickUpBLFIndex = dsskey_GetDsskeyIndexByID(nDsskeyId);
        }

        TALKUI_INFO("pickup，strCallOut[%s]，iPickupBLFIndex[%d]", strCallNum.c_str(),
                    tCallParam.iPickUpBLFIndex);
        talklogic_CallOut(strCallNum, "", iLineID, &tCallParam);
    }
}

//处理dial事件
void CPickUPMenuProcessor::OnDial()
{
    if (m_objPickUPMenuData.vectorMenuName.size() <= 0)
    {
        return;
    }

    if (NULL != m_pPickUPMenuUI)
    {
        int iFocusIndex = m_pPickUPMenuUI->GetFocusPickUPMenuItemIndex();
        int nDsskeyId = blf_GetPickupDsskeyIdByIndex(iFocusIndex);

        if (nDsskeyId == -1)
        {
            TALKUI_WARN("Failed to Get Pickup Menu Item ! Index[%d]", iFocusIndex);
            return;
        }


        int iLineID = dsskey_GetDsskeyLine(nDsskeyId);
        //兼容之前的blf，这里初值付为这个，之前的这个就是号码，blfList是name
        yl::string strNumber;
#ifndef _WIN32
        DssKey_Type dsskeyTypeGet = dsskey_GetDsskeyType(nDsskeyId);

        if (DT_BLF == dsskeyTypeGet
#ifdef IF_FEATURE_GENBAND
                || DT_BLFGROUP == dsskeyTypeGet
#endif
           )
        {
            strNumber = dsskey_GetDsskeyValue(nDsskeyId);
        }
        else if (DT_BLFLIST == dsskeyTypeGet)
        {
            strNumber = dsskey_GetDsskeyExtension(nDsskeyId);
            if (strNumber.empty())
            {
                strNumber = dsskey_GetDsskeyValue(nDsskeyId);
            }
        }

#endif

        TALKUI_INFO("[CDlgPickupMenu::OnDial] strNumber[%s]", strNumber.c_str());
        //对来电的号码呼出
        yl::string strName;
        Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR);
        talklogic_CallOut(strNumber, strName, iLineID);
    }
}

//处理new call事件
void CPickUPMenuProcessor::OnNewCall()
{
    if (m_objPickUPMenuData.vectorMenuName.size() <= 0)
    {
        return;
    }

    if (NULL != m_pPickUPMenuUI)
    {
        int iFocusIndex = m_pPickUPMenuUI->GetFocusPickUPMenuItemIndex();
        int nDsskeyId = blf_GetPickupDsskeyIdByIndex(iFocusIndex);

        if (nDsskeyId == -1)
        {
            TALKUI_WARN("Failed to Get Pickup Menu Item ! Index[%d]", iFocusIndex);
            return;
        }

        talklogic_EnterDialUI("", dsskey_GetDsskeyLine(nDsskeyId));
    }

}

//处理cancle事件
void CPickUPMenuProcessor::OnCancel()
{
    //主动退出的时候清数据
    blf_CancelPickupMenuShow();

    //调用UI的接口退出
    AdapterBox_ExitPickupMenu();
//  talkui_ExitPickupMenu();
}

//更新pickupmenu 数据
void CPickUPMenuProcessor::UpdatePickUPMenuData()
{
    YLList<yl::string> listMenuName;

    //先清除列表信息
    m_objPickUPMenuData.vectorMenuName.clear();

    // 获取列表
    if (blf_GetPickupMenuList(listMenuName))
    {
        //测试数据
#ifdef IF_TALKUI_DEBUG
        if (listMenuName.size() == 0)
        {
            for (int i = 1; i < 10; i++)
            {
                listMenuName.push_back((QString("testitem") + QString::number(i)).toUtf8().data());
            }
        }
#endif


        //设置数据
        for (YLList<yl::string>::iterator iter = listMenuName.begin();
                iter != listMenuName.end(); iter++)
        {
            yl::string strMenuName = *iter;

            m_objPickUPMenuData.vectorMenuName.push_back(strMenuName);
        }
    }

    //为空 要显示 空
    //由framelist在中央显示none
//  if(m_objPickUPMenuData.vectorMenuName.size() <= 0)
//  {
//      m_objPickUPMenuData.vectorMenuName.push_back(_UILOGIC_LANG_TRAN(TRID_NONE));
//  }

}

