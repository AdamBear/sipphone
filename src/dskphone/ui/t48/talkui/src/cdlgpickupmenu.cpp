/************************************************************************
 * FileName  : CDlgPickupMenu.cpp (implementation file)
 * Purpose   :
 * Date      : 2013/01/15 20:41:23
 ************************************************************************/

#include "cdlgpickupmenu.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "modtalkui.h"
#include "dsskey/include/moddsskey.h"
#include "account/include/modaccount.h"
#include "mainwnd/mainwnd.h"

// mod-blf
#include "blf/include/modblf.h"
// #include "CommonAPI/CommonAPI.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/t4xpicpath.h"
#include "contacts/directory/include/moddirinterface.h"
#include "uimanager/rectdefine.h"
#include "imagemanager/modimagemanager.h"
#include "talk/talklogic/include/commonfunctions.h"

#define ACTION_DLG_PICKUP_PICKUP        "pickup"
#define ACTION_DLG_PICKUP_Dial              "dial"
#define ACTION_DLG_PICKUP_NEWCALL   "newcall"
#define ACTION_DLG_PICKUP_CANCEL        "cancel"

//blfdefines.h 中有定义 这边就不定义了
//#define size_USER_NAME        100
//typedef struct key_status
//{
//  int key;                            /* 当前按键 */
//  int line_id;                        /* 当前账号 */
//  int status;                         /* 当前按键状态 */
//  char remote_name[size_USER_NAME];   /* 远方用户名 */
//  char local_name[size_USER_NAME];    /* 被监控用户名 */
//}key_status_t;

CDlgPickupMenu::CDlgPickupMenu(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
{
    SetWindowType(eWT_SubPage);

    //设置顶层窗口属性
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_SETTING);

    InitData();

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_CALL_LIST_FOR_PICKUP));

    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SOME_PAGE_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
        m_pGeneralFrame->SetRect(132, 0, 536, 336);
    }

    connect(this, SIGNAL(ProcessPageAction(const QString &)), this,
            SLOT(OnProcessPageAction(const QString &)));

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

CDlgPickupMenu::~CDlgPickupMenu()
{

}

void CDlgPickupMenu::Init()
{

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgPickupMenu::Uninit()
{
    TALKUI_INFO("CDlgPickupMenu Stop Play Ring");
    _MainWND.SetVolumeBarVisible(false);
}

void CDlgPickupMenu::SetData(void * pData)
{
    LoadPageData();
}

bool CDlgPickupMenu::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = 0;
    iMax = 15;

    return true;
}

bool CDlgPickupMenu::LoadPageData()
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    ClearAllContent();

    YLList<yl::string> listMenuName;

    // 获取列表
    if (blf_GetPickupMenuList(listMenuName))
    {
        QWidgetPair qPair;
        qPair.eType = SETTING_ITEM_NORMAL;
        qPair.bAddIndex = true;

        //设置数据
        for (YLList<yl::string>::iterator iter = listMenuName.begin();
                iter != listMenuName.end(); iter++)
        {
            //这边要转码
            qPair.first = toQString((*iter).c_str());
            qPair.second = NULL;
            m_vecWidgetPair.push_back(qPair);
        }
    }

    if (m_vecWidgetPair.isEmpty())
    {
        //显示None
        m_pGeneralFrame->SetNote(LANG_TRANSLATE(TRID_NONE));
    }
    else
    {
        AddWidget(m_vecWidgetPair, true, false, true, true);
    }

    return true;
}

void CDlgPickupMenu::OnProcessPageAction(const QString & strAction)
{
    OnAction(strAction);
}

bool CDlgPickupMenu::IsStatusChanged()
{
    return false;
}

bool CDlgPickupMenu::SavePageData()
{
    return false;
}

//http://10.3.5.199/Bug.php?BugID=65420
//实际上  数据是不可能为空的
//获取Softkey数据
void CDlgPickupMenu::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData = m_SoftkeyData;
}

bool CDlgPickupMenu::OnAction(const QString & strAction)
{
    if (strAction == ACTION_DLG_PICKUP_PICKUP)
    {
        OnPickUpLine();
    }
    else if (strAction == ACTION_DLG_PICKUP_Dial)
    {
        OnDial();
    }
    else if (strAction == ACTION_DLG_PICKUP_NEWCALL)
    {
        OnNewCall();
    }
    else if (strAction == ACTION_DLG_PICKUP_CANCEL)
    {
        OnCancel();
    }
    else
    {
        return false;
    }

    return true;
}

//处理pickup事件
void CDlgPickupMenu::OnPickUpLine()
{
    if (m_pGeneralFrame != NULL
            && m_pGeneralFrame->GetTotalDataCount() > 0)
    {
        int iFocusIndex = m_pGeneralFrame->GetFocusItemDataIndex();
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
void CDlgPickupMenu::OnDial()
{
    if (m_pGeneralFrame != NULL
            && m_pGeneralFrame->GetTotalDataCount() > 0)
    {
        int iFocusIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        int nDsskeyId = blf_GetPickupDsskeyIdByIndex(iFocusIndex);

        if (nDsskeyId == -1)
        {
            TALKUI_WARN("Failed to Get Pickup Menu Item ! Index[%d]", iFocusIndex);
            return;
        }


        int iLineID = dsskey_GetDsskeyLine(nDsskeyId);
        //兼容之前的blf，这里初值付为这个，之前的这个就是号码，blfList是name
        yl::string strNumber;

        DssKey_Type dsskeyTypeGet = DT_NONE;

        dsskeyTypeGet = dsskey_GetDsskeyType(nDsskeyId);

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


        TALKUI_INFO("[CDlgPickupMenu::OnDial] strNumber[%s]", strNumber.c_str());
        //对来电的号码呼出
        yl::string strName;
        Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR);
        talklogic_CallOut(strNumber, strName, iLineID);
    }
}

//处理new call事件
void CDlgPickupMenu::OnNewCall()
{
    if (m_pGeneralFrame != NULL
            && m_pGeneralFrame->GetTotalDataCount() > 0)
    {
        int iFocusIndex = m_pGeneralFrame->GetFocusItemDataIndex();
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
void CDlgPickupMenu::OnCancel()
{
    //主动退出的时候清数据
    blf_CancelPickupMenuShow();

    talkui_ExitPickupMenu();
}

int CDlgPickupMenu::GetCurrentIndex()
{
    if (m_pGeneralFrame != NULL
            && m_pGeneralFrame->GetTotalDataCount() > 0)
    {
        return m_pGeneralFrame->GetFocusItemDataIndex();
    }
    else
    {
        return -1;
    }
}

void CDlgPickupMenu::OnBackButtonClick()
{
    OnAction(ACTION_DLG_PICKUP_CANCEL);
}

void CDlgPickupMenu::OnHomeButtonClick()
{
    OnAction(ACTION_DLG_PICKUP_CANCEL);
    CBaseDialog::OnHomeButtonClick();
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgPickupMenu::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

bool CDlgPickupMenu::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!CheckEventValid(pObject, pEvent))
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_OK:
                {
                    OnAction(ACTION_DLG_PICKUP_PICKUP);
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    OnAction(ACTION_DLG_PICKUP_CANCEL);
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgPickupMenu::InitData()
{
    m_SoftkeyData[0].m_strSoftkeyAction = ACTION_DLG_PICKUP_PICKUP;
    m_SoftkeyData[0].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_PICK_UP);
    m_SoftkeyData[0].m_strIconPath  = PIC_SOFTKEY_DPICKUP;

    m_SoftkeyData[1].m_strSoftkeyAction = ACTION_DLG_PICKUP_Dial;
    m_SoftkeyData[1].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DIAL);
    m_SoftkeyData[1].m_strIconPath  = PIC_SOFTKEY_SEND;

    m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_PICKUP_NEWCALL;
    m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
    m_SoftkeyData[2].m_strIconPath  = PIC_SOFTKEY_NEWCALL;

    m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_PICKUP_CANCEL;
    m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_CANCEL);
    m_SoftkeyData[3].m_strIconPath  = PIC_SOFTKEY_CANCEL;
}
