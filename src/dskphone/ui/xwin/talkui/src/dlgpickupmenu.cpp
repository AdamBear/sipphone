#include "dlgpickupmenu.h"

/////////////talkui////////////
#include "talkuicommonfunction.h"

namespace talkui
{

//取得实例
CBaseDialog * CDlgPickupMenu::CreateInstance()
{
    return new CDlgPickupMenu();
}

CDlgPickupMenu::CDlgPickupMenu()
    : CDlgTalkMenuBase(eWT_PickupMenu, DLG_DlgPickupMenu)
{

}

CDlgPickupMenu::~CDlgPickupMenu()
{

}

// 初始化
void CDlgPickupMenu::Init()
{
    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(&m_objPickUpAdapter);
        m_pListView->ClearContent();
    }

    m_objPickUpAdapter.Init();
}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgPickupMenu::Uninit()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    m_objPickUpAdapter.Uninit();
}

bool CDlgPickupMenu::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     yl::string & strBGPath)
{
    iMin = 0;
    iMax = 15;

    return true;
}

//设置数据
void CDlgPickupMenu::SetData(void * pData)
{
    //设置title
    //设置显示title
    if (NULL != m_pTitle)
    {
        m_pTitle->SetText(m_objPickUPMenuProcessor.GetTitle());
    }

    //先从逻辑层获取数据
    CPickUPMenuData * pPickUPMenuData = m_objPickUPMenuProcessor.GetPickUPMenuData();

    if (NULL != pPickUPMenuData)
    {
        if (pPickUPMenuData->vectorMenuName.size() > 0)
        {
            FocusList(true);
        }
        else
        {
            FocusList(false);
        }

        m_objPickUpAdapter.SetPickUpInfo(pPickUPMenuData);
        //刷新当前显示 焦点项 信息
        UpdatePageInfo(pPickUPMenuData->vectorMenuName.size());
    }
}

//获取焦点 pickupmenuitem 的序号
int CDlgPickupMenu::GetFocusPickUPMenuItemIndex()
{
    if (NULL != m_pListView)
    {
        return m_pListView->GetFocusItemDataIndex();
    }

    return -1;
}


//获取Softkey数据
void CDlgPickupMenu::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    YLVector<SoftKey_TYPE> vectorSoftkey;
    int iSoftkeyCount = 0;
    //从processor中获取 softkey 数据
    m_objPickUPMenuProcessor.GetSoftkey(vectorSoftkey);

    iSoftkeyCount = vectorSoftkey.size();
    for (int i = 0; i < iSoftkeyCount; i++)
    {
        CSoftKeyBarData softkeyBarData;

        GetSoftkeyBarDataFromSoftKeyType(softkeyBarData, vectorSoftkey[i]);
        objSoftkeyData.push_back(softkeyBarData);
    }
}

bool CDlgPickupMenu::onKeyPressedEvent(int key)
{
    switch (key)
    {
    case PHONE_KEY_OK:
        {
            m_objPickUPMenuProcessor.OnPickUpLine();
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            m_objPickUPMenuProcessor.OnCancel();
        }
        break;
    default:
        {
            return CDlgTalkMenuBase::onKeyPressedEvent(key);
        }
        break;
    }

    return true;
}

//处理softkey 继承自 talkbaseui
bool CDlgPickupMenu::OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    TALKUI_INFO("CDlgPickupMenu-OnSoftkeyPress(%d)", eSoftType);

    return m_objPickUPMenuProcessor.OnSoftkeyPress(eSoftType);
}

}       //namespace talkui


