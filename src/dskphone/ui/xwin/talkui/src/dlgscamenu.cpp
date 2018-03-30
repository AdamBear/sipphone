#include "dlgscamenu.h"

/////////////talkui////////////
#include "talkuicommonfunction.h"

namespace talkui
{
//取得实例
CBaseDialog * CDlgSCAMenu::CreateInstance()
{
    return new CDlgSCAMenu();
}

CDlgSCAMenu::CDlgSCAMenu()
    : CDlgTalkMenuBase(eWT_Unknown, DLG_DlgSCAMenu)
{

}

CDlgSCAMenu::~CDlgSCAMenu()
{

}

// 初始化
void CDlgSCAMenu::Init()
{
    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(&m_objSCAMenuAdapter);
        m_pListView->ClearContent();
    }

    m_objSCAMenuAdapter.Init();
}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgSCAMenu::Uninit()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    m_objSCAMenuAdapter.Uninit();
}

//设置数据
void CDlgSCAMenu::SetData(void * pData)
{
    //设置title
    //设置显示title
    if (NULL != m_pTitle)
    {
        m_pTitle->SetText(m_objSCAMenuProcessor.GetTitle());
    }

    //先给processor 设置数据
    m_objSCAMenuProcessor.SetData(pData);

    //从processor中获取数据来显示
    //先给代理设置数据
    CSCAMenuUIData * pSCAMenuUIData = m_objSCAMenuProcessor.GetSCAMenuUIData();

    if (NULL != pSCAMenuUIData)
    {
        if (pSCAMenuUIData->vectorSCAMenuItemData.size() > 0)
        {
            FocusList(true);
        }
        else
        {
            FocusList(false);
        }

        m_objSCAMenuAdapter.SetSCAMenuInfo(pSCAMenuUIData);

        //刷新当前显示 焦点项 信息
        UpdatePageInfo(pSCAMenuUIData->vectorSCAMenuItemData.size());
    }
}

//获取焦点 pickupmenuitem 的序号
int CDlgSCAMenu::GetFocusSCAMenuItemIndex()
{
    if (NULL != m_pListView)
    {
        return m_pListView->GetFocusItemDataIndex();
    }

    return -1;
}

//获取Softkey数据
void CDlgSCAMenu::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    YLVector<SoftKey_TYPE> vectorSoftkey;
    int iSoftkeyCount = 0;
    //从processor中获取 softkey 数据
    m_objSCAMenuProcessor.GetSoftkey(vectorSoftkey);

    iSoftkeyCount = vectorSoftkey.size();
    for (int i = 0; i < iSoftkeyCount; i++)
    {
        CSoftKeyBarData softkeyBarData;

        GetSoftkeyBarDataFromSoftKeyType(softkeyBarData, vectorSoftkey[i]);
        objSoftkeyData.push_back(softkeyBarData);
    }
}

bool CDlgSCAMenu::onKeyPressedEvent(int key)
{
    if (m_objSCAMenuProcessor.OnKeyPress(key))
    {
        return true;
    }

    return CDlgTalkMenuBase::onKeyPressedEvent(key);
}

//处理softkey 继承自 talkbaseui
bool CDlgSCAMenu::OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }
    TALKUI_INFO("CDlgSCAMenu-OnSoftkeyPress(%d)", eSoftType);

    return m_objSCAMenuProcessor.OnSoftkeyPress(eSoftType);
}

}       //namespace talkui


