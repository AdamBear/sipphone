#include "dialogviewcontroller.h"

#include <common/common_data_define.h>

#include "xwindow/xWindowManager.h"
#include "xwindow/xSoftkeyBar.h"
#include "dsskeyui/moddsskeyui.h"
#include "dsskeyui/clinekeyui.h"
#include "edk/include/modedk.h"
#include "uimanager/moduimanager.h"

#include "baseui/include/t2xpicpath.h"

/************************************************************************/
/*                         class CSoftKeyBarData                        */
/************************************************************************/
CSoftKeyBarData::CSoftKeyBarData(const CSoftKeyBarData & other)
    : m_iType(other.m_iType)
    , m_strSoftkeyTitle(other.m_strSoftkeyTitle)
    , m_strBtnPic(other.m_strBtnPic)
{

}

void CSoftKeyBarData::InitData()
{
    m_iType = -1;
    m_strSoftkeyTitle = "";
    m_strBtnPic = "";
}

CSoftKeyBarData & CSoftKeyBarData::operator=(const CSoftKeyBarData & other)
{
    // 证同测试
    if (this == &other)
    {
        return *this;
    }

    this->m_iType = other.m_iType;
    this->m_strSoftkeyTitle = other.m_strSoftkeyTitle;
    this->m_strBtnPic = other.m_strBtnPic;
    return *this;
}
///////////////////////////////////////////////////

/************************************************************************/
/*                         class CDialogViewController                        */
/************************************************************************/

bool CDialogViewController::s_bDefaultShowSoftKey = true;

CDialogViewController::CDialogViewController(xView * pDialogView)
    : m_pDialogView(pDialogView)
    , m_pDialogWindowObject(NULL)
    , m_bProcessed(false)
    , m_bDefaultShowDsskey(false)
    , m_eDsskeyDisplayMode(DDM_HIDE)
{
    m_SoftkeyData.clear();
}

CDialogViewController::~CDialogViewController()
{

}

void CDialogViewController::SetDefaultShowSoftkey(bool bShowSoftkey)
{
    s_bDefaultShowSoftKey = bShowSoftkey;
}

////////////////////xView///////////////////////
void CDialogViewController::CreateWindowObject(bool bWaitSetActive/* = false*/)
{
    if (NULL != m_pDialogView && NULL == m_pDialogWindowObject)
    {
        if (NULL != m_pDialogView->m_pWindow)
        {
            m_pDialogWindowObject = m_pDialogView->m_pWindow;
        }
        else
        {
            m_pDialogWindowObject = g_WorkStation.createWindow(m_pDialogView, TML_DEFAULT/* + 1*/,
                                    bWaitSetActive);
        }
    }
}

void CDialogViewController::BringWindowObjectTop()
{
    if (NULL != m_pDialogWindowObject)
    {
        g_WorkStation.popWindowObject(m_pDialogWindowObject);
        g_WorkStation.pushWindowObject(m_pDialogWindowObject);
    }
}

void CDialogViewController::PopWindowObject()
{
    if (NULL != m_pDialogWindowObject)
    {
        g_WorkStation.popWindowObject(m_pDialogWindowObject);

        m_pDialogWindowObject = NULL;
    }
}

void CDialogViewController::DialogShow(int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/)
{
    if (NULL != m_pDialogView)
    {
        m_pDialogView->show(nShowCmd);
    }
}

void CDialogViewController::DialogRaise()
{
    if (NULL != m_pDialogView)
    {
        if (NULL != (m_pDialogView->m_pParent))
        {
            m_pDialogView->m_pParent->moveChildToTop(m_pDialogView);
        }
        else if (NULL != m_pDialogView->m_pWindow && m_pDialogView->isVisible() && m_pDialogView->m_bAppear)
        {
            g_WorkStation.bringWindowToTop_helper(m_pDialogView);
        }
    }
}

void CDialogViewController::DialogUpdate()
{
    if (NULL != m_pDialogView)
    {
        m_pDialogView->update(true);
    }
}

void CDialogViewController::DialogSetActive()
{
    if (NULL != m_pDialogWindowObject)
    {
        g_WorkStation.setActiveWindowObject(m_pDialogWindowObject);
    }
}

void CDialogViewController::DialogHide()
{
    if (NULL != m_pDialogView && m_pDialogView->m_bAppear)
    {
        m_pDialogView->hide(true);
    }
}

bool CDialogViewController::DialogClose()
{
    if (NULL != m_pDialogView)
    {
        m_pDialogView->close();
        m_pDialogWindowObject = NULL;
        return true;
    }

    return false;
}

/////////////////////////////////////////////////

bool CDialogViewController::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        yl::string & strPix)
{
    return false;
}

void CDialogViewController::SetDefaultShowDsskey(bool bShowDsskey)
{
    m_bDefaultShowDsskey = bShowDsskey;
}

void CDialogViewController::UpdateDsskey(bool bShow)
{
    if (bShow && g_pLinekeyUI->IsEnable())
    {
        DSSKEY_DISPLAY_MODE eDsskeyDisplayMode = GetDsskeyDisplayMode();
        if (eDsskeyDisplayMode != DDM_HIDE)
        {
            g_pLinekeyUI->SetDisplayMode(eDsskeyDisplayMode);
            g_pLinekeyUI->show();
        }
        else
        {
            g_pLinekeyUI->hide();
        }
    }
    else
    {
        g_pLinekeyUI->hide();
    }
}

//获取Softkey数据
void CDialogViewController::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData = m_SoftkeyData;
}

#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
yl::string CDialogViewController::GetSoftkeyBG()
{
    return PIC_THEME1_SOFTKEY_NORMAL_BG;
}
#endif

// 设置Softkey数据
void CDialogViewController::SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData)
{
    if (NULL == m_pDialogView)
    {
        return;
    }

    clearViewSoftKey(m_pDialogView);

    m_SoftkeyData = objSoftkeyData;

    if (UIManager_IsEnableCustomSoftkey())
    {
        CArraySoftKeyBarData tmpSoftkeyData = m_SoftkeyData;
        UIManager_GetCustomSoftkeyData(tmpSoftkeyData);
        SetupSoftKey(tmpSoftkeyData);
    }
    else
    {
        SetupSoftKey(m_SoftkeyData);
    }
}

void CDialogViewController::slotOnSoftkeypress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    OnSoftkeyAction(wParam, false);
#if IF_FEATURE_EDK
    Edk_OnEdkSoftKeyPress((EDKSOFTKEYTYPE)wParam);
#endif
}

void CDialogViewController::slotOnSoftKeyLonngPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    OnSoftkeyAction(wParam, true);
}

xBrush CDialogViewController::GetBackGroundBrush()
{
    return xBrush();
}

void CDialogViewController::UpdateSoftkey(bool bShow)
{
    xView& barSoftKey = currentSoftKeyBar();

    if (bShow)
    {
#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
        xPixmap pixMap;
        g_WorkStation.GetImageByPath(GetSoftkeyBG(), pixMap);
        if (!pixMap.isNull())
        {
            barSoftKey.setBackground(pixMap);
        }
#endif

        if (!barSoftKey.checkVisible())
        {
            barSoftKey.show();
        }
    }
    else if (barSoftKey.checkVisible())
    {
        barSoftKey.hide();
    }
}

xView * CDialogViewController::GetDialogView()
{
    return m_pDialogView;
}

void CDialogViewController::SetupSoftKey(CArraySoftKeyBarData & objSoftkeyData)
{
    int nIndex = 0;
    CArraySoftKeyBarData::iterator iter = objSoftkeyData.begin();

    for (; iter != objSoftkeyData.end(); ++iter, ++nIndex)
    {
        const CSoftKeyBarData & SoftkeyData = (*iter);
        if (ST_EMPTY != SoftkeyData.m_iType)
        {
            setupViewSoftKey(m_pDialogView
                             , nIndex
                             , SoftkeyData.m_strSoftkeyTitle
                             , SoftkeyData.m_iType
                             , SLOT_METHOD(this, CDialogViewController::slotOnSoftkeypress)
                             , SLOT_METHOD(this, CDialogViewController::slotOnSoftKeyLonngPress));
        }
    }
}
