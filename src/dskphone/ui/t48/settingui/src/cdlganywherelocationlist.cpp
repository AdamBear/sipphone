#include "model.h"
#include "cdlganywherelocationlist.h"
#include "canywhereuilogic.h"
#include "cdlganywherelocationedit.h"
#include "actiondefinecommon.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/modsettingui.h"
#include "account/include/modaccount.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"
#include "baseui/t4xpicpath.h"

namespace
{
#define LOCATION_LIST_NEED_ORDER    0
//开关多个location的时候是否只发送改变的location的列表, 0表示发送所有location的列表
//#define MULTI_EIDT_LOCATIONS_SEND_ONLY_CHANGED    0
enum
{
    MSGBOX_ID_COMFIRM_DELETE_ALL_LOCATIONS = 100
};

}
CDlgAnyWhereLocationList::CDlgAnyWhereLocationList(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_nAccountID(-1)
{
    InitData();
}

void CDlgAnyWhereLocationList::InitData()
{
    //为界面自己安装事件过滤器
    this->installEventFilter(this);
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetNote(LANG_TRANSLATE(TRID_NONE));
    }

    SetData(NULL);
}

void CDlgAnyWhereLocationList::SetData(void * pData /* = NULL */)
{
    // 设置模板类型
    m_eWndType = eWT_SubPage;
    m_strTitle = LANG_TRANSLATE(TRID_LOCATIONS);
}

//事件过滤器
bool CDlgAnyWhereLocationList::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

// 设置页面的SoftKey
bool CDlgAnyWhereLocationList::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyAction = ACTION_ADD;
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ADD);
    objWndData[0].m_strIconPath = PIC_SOFTKEY_ADD;

    if (m_DataList.size() == 0)
    {
        objWndData[3].m_strSoftkeyAction.clear();
        objWndData[3].m_strSoftkeyTitle.clear();
    }
    else
    {
        objWndData[3].m_strSoftkeyAction = OPTION_ACTION_DELETEALL;
        objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
        objWndData[3].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }

    return true;
}

//清除所有item
void CDlgAnyWhereLocationList::ClearPageData()
{
    m_vecLabel.clear();
    m_DataList.clear();
    m_vecbActiveChanged.clear();

    ClearAllContent();
}

//设置页面数据
void CDlgAnyWhereLocationList::SetLocationList(SAYWLocationArray & objList)
{
    ClearPageData();

    //按号码字符排序，与服务器一样
    std::vector<SAYWLocation>  orderVector;
    std::vector<SAYWLocation>::iterator orderIter;

    SAYWLocationArray::LocationArray::iterator iter = objList.m_locatArr.begin();
    SETTINGUI_INFO("LocationList::SetLocationList  size [%d]", objList.m_locatArr.size());

    //遍历array，排序
    for (; iter != objList.m_locatArr.end(); ++iter)
    {
#if LOCATION_LIST_NEED_ORDER
        yl::string strLoctFist = g_pAnywhereUILogic->FormatContryCodePhoneNumber(
                                     iter->m_strCountryCode.c_str(),
                                     iter->m_strPhoneNum.c_str()).toUtf8().data();
        //从头对比排序列表并插入
        for (orderIter = orderVector.begin(); orderIter != orderVector.end(); orderIter++)
        {
            if (orderVector.size() == 0)
            {
                orderVector.push_back(*iter);
                break;
            }
            yl::string strOrderFirst = g_pAnywhereUILogic->FormatContryCodePhoneNumber(
                                           orderIter->m_strCountryCode.c_str(),
                                           orderIter->m_strPhoneNum.c_str()).toUtf8().data();
            if (strcmp(strLoctFist.c_str(), strOrderFirst.c_str()) < 0)
            {
                orderVector.insert(orderIter, *iter);
                orderIter = orderVector.begin();
                break;
            }
        }

        //如果顺序列表中没有比当前所比的location的number大或者为空，那么将其插入列表尾部
        if (orderIter == orderVector.end())
        {
            orderVector.push_back(*iter);
        }
#else
        orderVector.push_back(*iter);
#endif
    }

    //将排序后的数据设置到界面上
    SetListData(orderVector);
}

//获取制定列的数据
bool CDlgAnyWhereLocationList::getLocationData(SAYWLocation & propSet, int idx) const
{
    propSet.clear();
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    if (idx > -1
            && idx < m_pGeneralFrame->GetTotalDataCount())
    {
        //此处直接从链表中获取，因为页面当前数据没有保存，而列表始终是最新正确的
        propSet = m_DataList[idx];

        return true;
    }
    else
    {
        return false;
    }
}

//获取当前选中项的数据
bool CDlgAnyWhereLocationList::GetSelectedLocationData(SAYWLocation & propSet) const
{
    propSet.clear();
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }
    int idx = m_pGeneralFrame->GetFocusItemDataIndex();
    if (idx < 0)
    {
        return false;
    }
    else
    {
        return getLocationData(propSet, idx);
    }
}


void  CDlgAnyWhereLocationList::RemoveLocationItem(const yl::string & strPhoneNum)
{
    std::vector<SAYWLocation>::iterator iter = m_DataList.begin();

    for (; iter != m_DataList.end();)
    {
        if (strPhoneNum == (*iter).m_strPhoneNum)
        {
            iter = m_DataList.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

bool CDlgAnyWhereLocationList::IsStatusChanged()
{
    return false;
}

bool CDlgAnyWhereLocationList::LoadPageData()
{
    return true;
}

bool CDlgAnyWhereLocationList::SavePageData()
{
    /*//获取active属性改变的列的数据的集合
    CStringVector changedVec;
    SAYWLocationArray arr;
    for (int i = 0; i < m_vecLabel.size() && i < m_DataList.size(); i++)
    {
        qCheckBox* pCheckBox = m_vecLabel[i];
    #if  MULTI_EIDT_LOCATIONS_SEND_ONLY_CHANGED
        //判断对应行的数据是否有变化，有变化则加入保存链表
        if ((pCheckBox->GetData()) != m_DataList[i].m_bActive)
        {
            changedVec.push_back(m_DataList[i].m_strPhoneNum);

            SAYWLocation temp = m_DataList[i];
            temp.m_bActive = !temp.m_bActive;
            arr.push_back(temp);
        }
    #else
        changedVec.push_back(m_DataList[i].m_strPhoneNum);

        SAYWLocation temp = m_DataList[i];
        if ((pComboBox->itemData(pComboBox->currentIndex(), Qt::UserRole).value<bool>()) != m_DataList[i].m_bActive)
        {
            temp.m_bActive = !temp.m_bActive;
        }

        arr.push_back(temp);
    #endif
    }
    SETTINGUI_INFO("%d items are changed!!!",(int)(changedVec.size()));
    g_pAnywhereUILogic->RequestModifyMutilLocations(changedVec, arr);*/

    return true;
}


//设置数据
void CDlgAnyWhereLocationList::SetListData(std::vector<SAYWLocation> & orderVector)
{
    std::vector<SAYWLocation>::iterator iter;
    //设置数据
    for (iter = orderVector.begin(); iter != orderVector.end(); ++iter)
    {
        //保存到页面数据链表中
        m_DataList.push_back(*iter);
        m_vecbActiveChanged.push_back(false);

        QLabel * pItemLabel = new QLabel(this);
        if (pItemLabel == NULL)
        {
            return;
        }

        pItemLabel->setText(iter->m_bActive ? LANG_TRANSLATE(TRID_ON) : LANG_TRANSLATE(
                                TRID_OFF));      //设置Combox控件

        QWidgetPair qPair;

        QString strLab;
        strLab = toQString(iter->m_strDescription.c_str());   //设置姓名
        if (strLab.isEmpty())
        {
            // 如果名字为空则显示号码
            strLab = toQString(iter->m_strPhoneNum.c_str());
        }
        qPair.first = strLab;
        qPair.second = pItemLabel;
        qPair.third = OPTION_ACTION_EDIT;
        qPair.eType = SETTING_ITEM_MENU;
        qPair.bWithStatus = true;
        m_vecWidgetPair.push_back(qPair);

        m_vecLabel.push_back(pItemLabel);
    }

    AddWidget(m_vecWidgetPair, true, false, true, false);

    UpdateWnd();
}

bool CDlgAnyWhereLocationList::OnAction(const QString & strAction)
{
    if (strAction == ACTION_ADD)
    {
        //进入CDlgAnyWhereLocationNew界面
        emit openpage(SubMenuData("anywhere_new"));
    }
    else if (strAction == OPTION_ACTION_EDIT)
    {
        OnEditAnyWhere();                   //AnyWhere界面Option(Edit)
    }
    /*else if (strAction == OPTION_ACTION_DELETE)
    {
        OnDeleteAnyWhere();                 //AnyWhere界面Option(Delete)
    }*/
    else if (strAction == OPTION_ACTION_DELETEALL)
    {
        OnDeleteAllAnyWhere();              //AnyWhere界面Option(Delete All Contacts)
    }
    else
    {
        return false;
    }

    return true;
}

void CDlgAnyWhereLocationList::OnEditAnyWhere()
{
    SAYWLocation propSet;
    GetSelectedLocationData(propSet);
    //进入编辑页面
    emit openpage(SubMenuData("OpenSubPage(anywhere_Edit)"));

    CDlgAnyWhereLocationEdit * pTheSupPage = qobject_cast<CDlgAnyWhereLocationEdit *>
            (UIManager_GetPrivateDlg(DLG_CDlgAnyWhereEdit));

    if (pTheSupPage != NULL)
    {
        pTheSupPage->LoadPageData(propSet.m_strPhoneNum);
    }
}

/*void CDlgAnyWhereLocationList::OnDeleteAnyWhere()
{
    int iRet = MsgBox_PopupMessageBox(LANG_TRANSLATE(TRID_IF_DELETE_LOCATION), PB_TYPE_OkCancel);
    if (iRet == PopupBox_OK)
    {
        //获取选中行location的数据
        SAYWLocation currentItem;
        GetSelectedLocationData(currentItem);
        //调用删除单个location的接口，删除选中行的location
        g_pAnywhereUILogic->RequestDeleteLocation(currentItem.m_strPhoneNum, true);
    }
}*/

void CDlgAnyWhereLocationList::OnDeleteAllAnyWhere()
{
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_IF_DELETE_ALL_LOCATIONS),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSGBOX_ID_COMFIRM_DELETE_ALL_LOCATIONS);
}


CDlgAnyWhereLocationList::~CDlgAnyWhereLocationList()
{
}

// 设置子页面前的操作
void CDlgAnyWhereLocationList::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        m_nAccountID = acc_GetDefaultAccount();
    }
    else
    {
        m_nAccountID = *(int *)pData;
    }

    g_pAnywhereUILogic->ChangeDefaultAccountID(m_nAccountID);
}

void CDlgAnyWhereLocationList::showEvent(QShowEvent * e)
{

}

void CDlgAnyWhereLocationList::ExtraDialogInitData()
{
    //请求数据
    g_pAnywhereUILogic->RequestLocationList(true);
}

void CDlgAnyWhereLocationList::OnCustomBack()
{
    // 用于自定义退出界面操作（只回退一个界面）
    emit BtnBackClick();
}

void CDlgAnyWhereLocationList::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID =  pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();

    if (iMsgID == MSGBOX_ID_COMFIRM_DELETE_ALL_LOCATIONS
            && iRet == MessageBox_OK)
    {
        //调用删除多个location的接口
        g_pAnywhereUILogic->RequestDeleteAllLocation(true);
    }
}
