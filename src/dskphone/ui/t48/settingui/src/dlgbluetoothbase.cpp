#include "dlgbluetoothbase.h"
#include "cdlgbluetoothdetail.h"
#include "bluetoothdelegate.h"
#include "cdlgaskpassword.h"
#include "bluetoothdetailselector.h"

#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/qdatatimer.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcheckbox.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "mainwnd/mainwnd.h"
#include "messagebox/modmessagebox.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "setting/include/common.h"
#include "settingui/src/bluetoothitem.h"
#include "settingui/src/settingpage.h"
#include "commonunits/modcommonunits.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "wireless/common/wirelessdefines.h"


#include "include/idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "uimanager/rectdefine.h"
#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_common.h"

#include "messagebox/selectmessagebox.h"

#include "settinganimationitem.h"

#include "talkui/src/qpictureanimation.h"

namespace
{
// action
#ifndef BLUETOOTH_USED_NEW_VERSION
#define BLUETOOTH_UI_ACITON_DETAIL          "Detail"
#endif
#define BLUETOOTH_UI_ACITON_SCAN            "Scan"
#define BLUETOOTH_UI_ACITON_CONNECT         "Connect"
#define BLUETOOTH_UI_ACITON_DISCONNECT      "Disconnect"
#define BLUETOOTH_UI_ACITON_DELETE_ALL      "deleteall"
#define BLUETOOTH_UI_ACITON_SAVE            "Save"
// #define BLUETOOTH_UI_ACITON_BACK            "Back"
#define BLUETOOTH_UI_ACITON_ENTER           "Enter"

// PopupBox ID
#define BT_UI_POPBOX_ID_CANCEL_SCAN         90
#define BT_UI_POPBOX_ID_DELITEM             91
#define BT_UI_POPBOX_ID_DELALLITEM          92
#define BT_UI_POPBOX_ID_DETAIL              93
#ifdef SUPPORT_PHONE_DEVICE
#define BT_UI_POPBOX_ID_MOBILEPIN           94
#define BT_UI_POPBOX_ID_MOBILESYNC          95
#endif

// Test multi page
//#define Test_Multi_Page

#define BLUETOOTH_SCANNING_ANIMATION_FREQUENT 270

#define RECT_ANIMATION_PIC  QRect(20, 9, 30, 30)
#define RECT_ANIMATION_TEXT QRect(60, 9, BLUETOOTH_UI_ITEM_WIDTH - 60, 30)

#ifndef BLUETOOTH_USED_NEW_VERSION
enum
{
    BT_CONNECT_STATE_TYPE_NONE = 0,
    BT_CONNECT_STATE_TYPE_CONNECTING,
    BT_CONNECT_STATE_TYPE_DISCONNECT,
};
#endif
}

Q_DECLARE_METATYPE(BluetoothAddData);
Q_DECLARE_METATYPE(SSettingAnimationItemData);

// 处理消息.
static LRESULT OnBluetoothUIMessage(msgObject & refMessage)
{
    SETTINGUI_INFO("OnBluetoothUIMessage message[%d]!!", refMessage.message);

    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();

    if (pBaseDlg == NULL)
    {
        return TRUE;
    }

    if (pBaseDlg->inherits("CSettingWindow"))
    {
        CSettingWindow * pSubMenu = static_cast<CSettingWindow *>(pBaseDlg);

        if (NULL != pSubMenu
                && pSubMenu->GetCurrentSubPage()->inherits("CDlgBluetoothBase"))
        {
            CDlgBluetoothBase * pView = static_cast<CDlgBluetoothBase *>(pSubMenu->GetCurrentSubPage());

            if (NULL != pView)
            {
                pView->OnUIMessage(refMessage);
            } // end if (NULL != pView)
        } // end if (NULL != pSubMenu)
    }

    return TRUE;
}

QString CDlgBluetoothBase::s_strPinMac = "";

bool CDlgBluetoothBase::GetSyncPinMac(QString & strPinMac)
{
    if (s_strPinMac.isEmpty())
    {
        return false;
    }

    strPinMac = s_strPinMac;
    return true;
}

LRESULT CDlgBluetoothBase::OnUIMessage(msgObject & refMessage)
{
    SETTINGUI_INFO("OnUIMessage message[%d]!!", refMessage.message);
    SETTINGUI_INFO("BLUETOOTH_MSG_DEVICE_POWER	= %d", BLUETOOTH_MSG_DEVICE_POWER);
    SETTINGUI_INFO("BLUETOOTH_MSG_UPDATE_LIST	= %d", BLUETOOTH_MSG_UPDATE_LIST);
    SETTINGUI_INFO("BLUETOOTH_MSG_REQ_PIN		= %d", BLUETOOTH_MSG_REQ_PIN);
#ifdef BLUETOOTH_USED_NEW_VERSION
    SETTINGUI_INFO("BLUETOOTH_MSG_STATE_CHANGE	= %d", BLUETOOTH_MSG_STATE_CHANGE);
#endif
#ifdef SUPPORT_PHONE_DEVICE
    SETTINGUI_INFO("BLUETOOTH_MSG_SHOW_PIN		= %d", BLUETOOTH_MSG_SHOW_PIN);
    SETTINGUI_INFO("BLUETOOTH_MSG_REQ_SYNC		= %d", BLUETOOTH_MSG_REQ_SYNC);
#endif


    switch (refMessage.message)
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
    case BLUETOOTH_MSG_STATE_CHANGE:
        {
            CheckSwitchChange();
        }
        break;
#endif
    case BLUETOOTH_MSG_DEVICE_POWER:
        {
#ifdef BLUETOOTH_USED_NEW_VERSION
            if (IsOnScanning())
            {
                PreDealWithScan();
            }
            else
#endif
            {
                OnAction(BLUETOOTH_UI_ACITON_SCAN);
            }
        }
        break;
    case BLUETOOTH_MSG_UPDATE_LIST:
        {
            UpdateDeviceList(refMessage.wParam, refMessage.lParam
#ifdef BLUETOOTH_USED_NEW_VERSION
                             , (const char *)refMessage.GetExtraData()
#endif
                            );
        }
        break;

    case BLUETOOTH_MSG_REQ_PIN:
        {
            SetPinMac((const char *)refMessage.GetExtraData());
            ShowPasswordMesaageBox();
        }
        break;
#ifdef SUPPORT_PHONE_DEVICE
    // 蓝牙手机PIN码
    case BLUETOOTH_MSG_SHOW_PIN:
        {
            QString strPINCode = "";
            m_strAskPinMac = (const char *)refMessage.GetExtraData();
            QStringList strList = m_strAskPinMac.split("&");
            QStringList::iterator itList = strList.begin();
            for (int i = 0; itList != strList.end() && i < 2; ++itList, ++i)
            {
                if (i)
                {
                    m_strAskPinMac = *itList;
                }
                else
                {
                    strPINCode = *itList;
                }
            }

            if (m_strAskPinMac.isNull() || m_strAskPinMac.isEmpty())
            {
                SETTINGUI_WARN("Bluetooth UI:Logic push a null mac device!");
                return TRUE;
            }

            DeviceInfo bluetoothDeviceInfo;
            Bluetooth_GetBluetoothInfoByMac(m_strAskPinMac.toUtf8().data(), bluetoothDeviceInfo);

            QString strDis = QString("%1%2%3%4%5%6%7").arg(LANG_TRANSLATE(TRID_CONNECT_TO_BELOW_MOBILE_DEVICE))
                             .arg("\n").arg(toQString(bluetoothDeviceInfo.strName)).arg("\n")
                             .arg(LANG_TRANSLATE(TRID_MAKE_SURE_THE_DEVICE_IS_SHOWING_THIS_PASSKEY)).arg("\n")
                             .arg(strPINCode);

            SETTINGUI_INFO("Bluetooth UI:Logic PIN Mac[%s], code[%s], name[%s], ShowMsg[%s].",
                           m_strAskPinMac.toUtf8().data(), strPINCode.toUtf8().data(),
                           toQString(bluetoothDeviceInfo.strName).toUtf8().data(),
                           strDis.toUtf8().data());

            MessageBox_ShowCommonMsgBox(this, strDis, MESSAGEBOX_OkCancel, -1,
                                        "", "", BT_UI_POPBOX_ID_MOBILEPIN);


            // 记录全局访问的PinMac
            s_strPinMac = m_strAskPinMac;
        }
        break;
    // 同步蓝牙手机联系人询问提示框
    case BLUETOOTH_MSG_REQ_SYNC:
        {
            s_strPinMac = (const char *)refMessage.GetExtraData();

            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_WOULD_YOU_LIKE_TO_SYNC_PHONE_CONTACTS_TEMPORARILY)
                                        , MESSAGEBOX_OkCancel, -1, "", "", BT_UI_POPBOX_ID_MOBILESYNC);
        }
        break;
#endif
    case BLUETOOTH_MSG_DONGLE_CHANGE:
        {
            if (refMessage.wParam == 0)
            {
                // 移除设备时需要刷新
                LoadDeviceList();
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

CDlgBluetoothBase::CDlgBluetoothBase(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBox(NULL)
    , m_pBlueDelegate(NULL)
    , m_pBlueDetailSelector(NULL)
    , m_bClearStatus(true)
    , m_bAskingPwd(false)
    , m_strAskPinMac("")
    , m_strDetailMac("")
    , m_strConnectedMac("")
    , m_strConnectedDeviceName("")
    , m_iPairedBtCounts(0)
#ifdef BLUETOOTH_USED_NEW_VERSION
    , m_strCurOperatorMac("")
    , m_bInScanning(false)
    , m_bInConnecting(false)
#else
    , m_pItem(NULL)
    , m_iConnectState(BT_CONNECT_STATE_TYPE_NONE)
    , m_iAutoConnet(-1)
    , m_iMoveUp(-1)
#endif
{
    SetWindowType(eWT_SubPage);

    InitData();
}

CDlgBluetoothBase::~CDlgBluetoothBase()
{
    if (m_pCheckBox != NULL)
    {
        disconnect(m_pCheckBox->GetRadioButtonOn(), SIGNAL(toggled(bool)), this,
                   SLOT(OnSwitchDeviceStatus(bool)));
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    SetCurOperatorMAC("");

    m_bInConnecting = false;
    m_bInScanning = false;
#endif

    if (m_pBlueDetailSelector != NULL)
    {
        delete m_pBlueDetailSelector;
        m_pBlueDetailSelector = NULL;
    }
}

void CDlgBluetoothBase::Init()
{
    RegisterMsg();

    m_bClearStatus = true;
}

void CDlgBluetoothBase::Uninit()
{
    UnRegisterMsg();

    ResetAskStatus();

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_bInScanning = false;
#endif

    // 停止扫描设备
    RequestStopScan();

    // 清空搜索数据
    Bluetooth_ClearScanedList();
}

//继承自CDlgBaseSubPage
bool CDlgBluetoothBase::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    for (int i = 0; i < SOFTKEY_NUM; ++i)
    {
        objWndData[i].m_strSoftkeyTitle.clear();
        objWndData[i].m_strSoftkeyAction.clear();
        objWndData[i].m_strIconPath.clear();
    }

    bool bBluthOn = false;
    if (NULL != m_pCheckBox)
    {
        bBluthOn = CheckDongle(false) && Bluetooth_IsEnable() && m_pCheckBox->GetData();
    }

    if (bBluthOn)
    {
        if (m_vecBlueWgtPair.size() > 0 || Bluetooth_IsConnected())
        {
            objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
            objWndData[0].m_strSoftkeyAction = BLUETOOTH_UI_ACITON_DELETE_ALL;
            objWndData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;
        }

        if (DS_SCAN != Bluetooth_GetDeviceState() && !m_bInScanning)
        {
            objWndData[1].m_strSoftkeyAction = BLUETOOTH_UI_ACITON_SCAN;
            objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SCAN);
            objWndData[1].m_strIconPath = PIC_SOFTKEY_SCAN;
        }
    }

    m_SoftkeyData = objWndData;

    return true;
}

bool CDlgBluetoothBase::LoadPageData()
{
    if (m_pCheckBox != NULL)
    {
        bool bEnable = Bluetooth_IsEnable();
#ifdef BLUETOOTH_USED_NEW_VERSION
        m_pCheckBox->SetData(bEnable);

        if (bEnable)
        {
            OnAction(BLUETOOTH_UI_ACITON_SCAN);
        }
#else
        if (!CheckDongle(false))
        {
            Bluetooth_SetEnable(false);
            m_pCheckBox->SetData(false);
        }
        else
        {
            m_pCheckBox->SetData(bEnable);

            OnAction(BLUETOOTH_UI_ACITON_SCAN);
        }
#endif
    }

    return true;
}

bool CDlgBluetoothBase::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("[CDlgBluetoothBase::OnAction][%s]", strAction.toUtf8().data());

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_bInScanning = false;
#else
    m_iConnectState = BT_CONNECT_STATE_TYPE_NONE;

    m_iMoveUp = -1;
    m_iAutoConnet = -1;
#endif

    QString strMac = GetCurrentBluetoothMac();

    if (strAction == BLUETOOTH_UI_ACITON_SAVE)
    {
        OnBtnSaveClick();
    }
    else if (strAction == BLUETOOTH_UI_ACITON_ENTER)
    {
        OnBtnEnterClick();
    }
    else if (strAction == BLUETOOTH_UI_ACITON_DELETE_ALL)
    {
        m_bClearStatus = false;
        OnBtnDeleteAll();
    }
    else if (strAction == BLUETOOTH_UI_ACITON_SCAN)
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        m_bInScanning = true;
#endif
        OnBtnScanClick(true);
    }
    else if (strAction == BLUETOOTH_UI_ACITON_CONNECT)
    {
#ifndef BLUETOOTH_USED_NEW_VERSION
        m_iConnectState = BT_CONNECT_STATE_TYPE_CONNECTING;
        OnBtnConnect(strMac);
#endif

#ifdef BLUETOOTH_USED_NEW_VERSION
        // remove pre connecting item note
        QString strPreMac = GetCurOperatorMAC();
        SETTINGUI_INFO("Bluetooth UI:Pre Mac[%s], Cur Mac[%s], bConnecting[%d]",
                       strPreMac.toUtf8().data(), strMac.toUtf8().data(), m_bInConnecting);

        if (!m_bInConnecting
                || (!strMac.isEmpty() && 0 != strMac.compare(strPreMac)))
        {
            if (!strPreMac.isEmpty() && m_mapMACIndex.end() != m_mapMACIndex.find(strPreMac))
            {
                int iDateIdx = m_mapMACIndex[strPreMac];
                if (-1 < iDateIdx && iDateIdx < m_vecWidgetPair.size())
                {
                    SetBlueItemSavedDataByDataIndex(iDateIdx, "", INT_NONE, SETTING_ITEM_BLUETOOTH,
                                                    BLUETOOTH_UI_ACITON_CONNECT);
                }
            }
            SetCurOperatorMAC("");

            OnBtnConnect(strMac);
            m_bInConnecting = true;
        }
#endif
    }
    else if (strAction == BLUETOOTH_UI_ACITON_DISCONNECT)
    {
#ifndef BLUETOOTH_USED_NEW_VERSION
        m_iConnectState = BT_CONNECT_STATE_TYPE_DISCONNECT;
#endif

        OnBtnDisconnect(strMac);
    }
#ifndef BLUETOOTH_USED_NEW_VERSION
    else if (strAction == BLUETOOTH_UI_ACITON_DETAIL)
    {
        strMac = m_strDetailMac;

        m_iConnectState = BT_CONNECT_STATE_TYPE_NONE;
        if (strMac.isEmpty())
        {
            return false;
        }
        DeviceInfo bluetoothDeviceInfo;
        Bluetooth_GetBluetoothInfoByMac(strMac.toUtf8().data(), bluetoothDeviceInfo);

        MessageBox_ShowBluetoothBox(this, LANG_TRANSLATE(TRID_MORE)
                                    , bluetoothDeviceInfo.bAutoConnect, -1, BT_UI_POPBOX_ID_DETAIL);
    }
#endif
    else
    {
        return false;
    }

    return true;
}

void CDlgBluetoothBase::OnItemDetailAction(CListItem * pItem)
{
    CSettingItem * pSetItem = (CSettingItem *)pItem;

#ifndef BLUETOOTH_USED_NEW_VERSION
    SetDetailMac("");
#endif

    CSettingBluetoothItem * pBlueItem = NULL;
    if (NULL != pSetItem)
    {
        pBlueItem = static_cast<CSettingBluetoothItem *>(pSetItem);
        if (NULL != pBlueItem)
        {
            SetDetailMac(toQString(pBlueItem->GetDeviceMac()));

#ifdef BLUETOOTH_USED_NEW_VERSION
#ifdef SUPPORT_PHONE_DEVICE
            SETTINGUI_INFO("detail [%s]", GetDetailMAC().toUtf8().data());
            if (!GetDetailMAC().isEmpty())
            {
                MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
                VecMsgBoxSoftkey vecSoftKey;

                vecSoftKey.push_back(skCancel);

                if (pBlueItem->IsMobileDevice())
                {
                    m_pBlueDetailSelector->LoadDataByType(BLUETOOTH_DETAIL_POPUP_DLG_MOBILE);
                    m_pBlueDetailSelector->SetMobileSync(pBlueItem->IsAutoSyncDevice());
                    MessageBox_ShowSelectBox(this, vecSoftKey, SELECT_POP_BOX_TYPE_TINY//SELECT_POP_BOX_TYPE_TWO_LINE
                                             , m_pBlueDetailSelector, LANG_TRANSLATE(TRID_DETAIL), BT_UI_POPBOX_ID_DETAIL);
                }
                else
                {
                    m_pBlueDetailSelector->LoadDataByType(BLUETOOTH_DETAIL_POPUP_DLG_COMMON);
                    MessageBox_ShowSelectBox(this, vecSoftKey, SELECT_POP_BOX_TYPE_TINY//SELECT_POP_BOX_TYPE_ONE_LINE
                                             , m_pBlueDetailSelector, LANG_TRANSLATE(TRID_DETAIL), BT_UI_POPBOX_ID_DETAIL);
                }
            }
#else
            OnBtnDelete(m_strDetailMac);
#endif

            m_pGeneralFrame->FocusItemByDataIndex(pBlueItem->GetDataIndex());
#endif
        }
    }

#ifndef BLUETOOTH_USED_NEW_VERSION
    if (NULL != pSetItem)
    {
        SetCurrentIndex(pSetItem->GetDataIndex());
    }
    this->UpdateWnd();

    m_pItem = cc;
    //m_pBluetoothDeviceList->OnAction(BLUETOOTH_UI_ACITON_DETAIL);

    OnAction(BLUETOOTH_UI_ACITON_DETAIL);
#endif
}

#ifndef BLUETOOTH_USED_NEW_VERSION
void CDlgBluetoothBase::AfterDetail()
{
    if (NULL != m_pItem && -1 != GetAutoConnet())
    {
        ////---- Beta版本需要整改
        this->setFocus();
        m_pItem->m_bluetoothData.bAutoConnect = GetAutoConnet();
        m_pItem->update();
    }
    emit RefreshSoftkey();
}
#endif

void CDlgBluetoothBase::PreDealWithScan()
{
    RequestStartScan();
}

void CDlgBluetoothBase::OnBtnScanClick(bool bClearStatus)
{
    if (!CheckDongle())
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        m_bInScanning = false;
#else
        if (NULL != m_pCheckBox)
        {
            m_pCheckBox->SetData(false);
        }
#endif

        return;
    }

    // 容错，Dongle插入，逻辑层会发Power_On让Ui进行扫描，开关需要打开
    if (NULL == m_pCheckBox || !m_pCheckBox->GetData() || !Bluetooth_IsEnable())
    {
        return;
    }

    m_bClearStatus = bClearStatus;

    RequestStartScan();
}

void CDlgBluetoothBase::OnBtnSaveClick()
{
    if (NULL != m_pCheckBox)
    {
        bool bBluetoothOn = m_pCheckBox->GetData();

        if (bBluetoothOn && !CheckDongle())
        {
            return;
        }

        Bluetooth_SetEnable(bBluetoothOn);

        LoadMenuList(bBluetoothOn);
        emit RefreshSoftkey();

        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVED), MESSAGEBOX_Cancel, SHORT_TIME);
    }
}

void CDlgBluetoothBase::OnBtnEnterClick()
{
    if (!CheckDongle())
    {
        return;
    }

    if (NULL != m_pGeneralFrame)
    {
        int iCurrIdx = m_pGeneralFrame->GetFocusItemDataIndex();

        if (iCurrIdx == 0)
        {
            SubMenuData objSubMenu;
            objSubMenu.strMenuAction = "OpenSubPage(my_device_information)";
            emit openpage(objSubMenu);
        }
    }
}

void CDlgBluetoothBase::InitData()
{
    m_pBlueDelegate = new BluetoothDelegate();
    m_pBlueDetailSelector = new BluetoothDetailSelector();

    m_pCheckBox = new qCheckBox(this);
    m_pPicAnimation = new qMultiPicAnimation(this);

    if (NULL == m_pCheckBox || NULL == m_pBlueDelegate || NULL == m_pPicAnimation
            || NULL == m_pBlueDetailSelector)
    {
        return;
    }

    m_pBlueDelegate->setParent(this);
    m_pBlueDelegate->BindFramelist(m_pGeneralFrame);
    m_pBlueDelegate->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                    PIC_GENERAL_LIST_ITEM_CLICK_BG);
    m_pBlueDelegate->InitSettingListStyle();
    m_pListDelegate = m_pBlueDelegate;
    connect(m_pBlueDelegate, SIGNAL(ItemDetailAction(CListItem *)), this,
            SLOT(OnItemDetailAction(CListItem *)));
    connect(m_pBlueDelegate, SIGNAL(listItemAction(int, QString)), this, SLOT(OnListItemAction(int,
            QString)));

    m_pCheckBox->SetData(Bluetooth_IsEnable());
    connect(m_pCheckBox->GetRadioButtonOn(), SIGNAL(toggled(bool)), this,
            SLOT(OnSwitchDeviceStatus(bool)));
    m_pCheckBox->installEventFilter(this);

    LoadMenuList(Bluetooth_IsEnable());

    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_1);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_2);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_3);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_4);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_5);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_6);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_7);
    m_pPicAnimation->AddPicture(PIC_BLUE_SEARCHING_ITEM_8);
    m_pPicAnimation->SetPlaySpeed(BLUETOOTH_SCANNING_ANIMATION_FREQUENT);
    m_pPicAnimation->Stop();
    m_pPicAnimation->hide();
}

void CDlgBluetoothBase::LoadMenuList(bool bBtState)
{
    // 开关打开且有适配器才显示配对过的蓝牙
    if (bBtState && CheckDongle(false))
    {
        LoadPairedDevice();
        DoReLayout(bBtState);
    }
    else
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        DoReLayout(bBtState);
#else
        Bluetooth_SetEnable(false);
        DoReLayout(false);
#endif
    }
}

//on/off值变化
void CDlgBluetoothBase::OnSwitchDeviceStatus(bool iIdx)
{
    // On->Off不提示没有Dongle；Off->On才提示
    if (!CheckDongle(iIdx))
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        m_bInScanning = false;
        DoReLayout(false);
        Bluetooth_SetEnable(iIdx);
#else
        if (NULL != m_pCheckBox && m_pCheckBox->GetData())
        {
            m_pCheckBox->SetData(false);
        }
#endif

        return;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_bInScanning = false;
    if (iIdx)
    {
        m_bInScanning = true;
    }
#endif

    bool bBluthOn = false;
    if (NULL != m_pCheckBox)
    {
        bBluthOn = m_pCheckBox->GetData();
    }

    SETTINGUI_DEBUG("OnSwitchDeviceStatus(%s)", bBluthOn ? "On" : "Off");
    SETTINGUI_INFO("OnSwitchDeviceStatus(%s)", bBluthOn ? "On" : "Off");

    Bluetooth_SetEnable(bBluthOn);

    if (!bBluthOn)
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        m_mapMACIndex.clear();
#endif
        DoReLayout(false);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);
#ifdef BLUETOOTH_USED_NEW_VERSION
        if (RequestStartScan())
        {
            m_bClearStatus = true;
        }
#endif
    }
}

void CDlgBluetoothBase::DoReLayout(bool bBtState)
{
    SETTINGUI_INFO("Bluetooth UI:[DlgBluetoothBase::DoReLayout()], bSwitchOn [%d]", bBtState);

    // 清除数据时，需要记录当前的Index，以便刷新数据时，焦点正确
    int nCurIndex = GetCurrentIndex();

    MakeReadyForRelayout();

    SetCurrentIndex(nCurIndex);

    InitResidentWgtPair(bBtState);

    if (bBtState)
    {
        // add connnected bluetooth
        if (Bluetooth_IsConnected())
        {
            DeviceInfo objDevInfo;
            if (Bluetooth_GetBluetoothInfoByMac(m_strConnectedMac.toUtf8().data(), objDevInfo))
            {
                QWidgetPair qPair;

                if (FillBtWgtPairByBtDevice(qPair, &objDevInfo))
                {
                    m_mapMACIndex[toQString(objDevInfo.strMac)] = 1 +  GetResidentItemCount();
                    m_vecWidgetPair.push_back(qPair);
                }
            }
        }

#ifndef Test_Multi_Page
        m_vecWidgetPair += m_vecBlueWgtPair;
#else
        for (int i = 0; i < 15; ++i)
        {
            QWidgetPair qPair;
            QString strLab;

            strLab = LANG_TRANSLATE(QString("Test %d").arg(i));
            qPair.first = "";
            qPair.second = NULL;
            qPair.third = BLUETOOTH_UI_ACITON_CONNECT;//"";
            BluetoothAddData bluetoothData;
            bluetoothData.strInfo = strLab;
            bluetoothData.strMac = "00:11:22:33:44:55:66:77:88:99:aa:bb:cc:dd:ee:ff:gg:hh:ii:jj:kk:ll:mm";
            bluetoothData.bIsPairOrNot = true;//i%3;
            //bluetoothData.bExistNoteOrNot = false;
            bluetoothData.bAutoConnect = i % 3;
            bluetoothData.bMobileDevice = i % 2;

            qPair.appendVariantData.setValue(bluetoothData);
            qPair.eType = SETTING_ITEM_BLUETOOTH;
            m_vecWidgetPair.push_back(qPair);
        }
#endif
    }

    SETTINGUI_INFO("Bluetooth UI:[DlgBluetoothBase::DoReLayout()] End, WgtPair size[%d]",
                   m_vecWidgetPair.size());

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}

QString CDlgBluetoothBase::GetCurrentBluetoothMac()
{
    if (NULL == m_pGeneralFrame)
    {
        return QString("");
    }

    CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();

    if (NULL == pNowItem)
    {
        return QString("");
    }

    if (pNowItem->inherits("CSettingBluetoothItem"))
    {
        CSettingBluetoothItem * pBtItem = static_cast<CSettingBluetoothItem *>(pNowItem);

        if (NULL != pBtItem)
        {
            return pBtItem->GetDeviceMac().c_str();
        }
    }

    return QString("");
}

void CDlgBluetoothBase::SetBluetooth(bool bBluetoothOpen /* = false */)
{
    if (NULL != m_pCheckBox && !m_pCheckBox->GetData())
    {
        m_pCheckBox->SetData(bBluetoothOpen);
        emit RefreshSoftkey();
    }
    else
    {
        OnAction(BLUETOOTH_UI_ACITON_SCAN);
    }
}

/////////////////////////////////////// fromBluetoothDeviceList
void CDlgBluetoothBase::ResetAskStatus()
{
    if (!m_bAskingPwd)
    {
        return;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    Bluetooth_AnswerPin(m_strAskPinMac.toUtf8().data(), "");
#else
    QString strMac = GetCurrentBluetoothMac();

    if (!strMac.isEmpty())
    {
        Bluetooth_AnswerPin(strMac.toUtf8().data(), "");
    }
#endif
}

void CDlgBluetoothBase::UpdateDeviceList(int wParam, int lParam, const char * pMac /*= ""*/)
{
    SETTINGUI_INFO("UpdateDeviceList[%d][%d]", wParam, lParam);

    switch (wParam)
    {
    case OP_CONNECT_RESULT:
        {
            SetConnectResultNote(lParam, pMac);

            if (1 == lParam)
            {
                LoadDeviceList();
            }
        }
        break;
    case OP_PAIR_RESULT:
        {
            if (0 == lParam)
            {
                MessageBox_RemoveMessageBox(this);

                SetConnectResultNote(lParam, pMac);
            }
        }
        break;
    case OP_DISCONNECT_RESULT:
        {
#ifdef SUPPORT_PHONE_DEVICE
            MessageBox_RemoveMessageBox(this, MessageBox_Cancel, BT_UI_POPBOX_ID_MOBILESYNC);
#endif
            SetDisConnectResultNote(lParam, pMac);
        }
        break;
    case OP_SCAN_RESULT:
        {
            m_bInScanning = false;

            if (NULL != m_pCheckBox && m_pCheckBox->GetData())
            {
                LoadDeviceList();
            }

            emit RefreshSoftkey();
        }
        break;
    default:
        {
            if (NULL != m_pCheckBox && m_pCheckBox->GetData())
            {
                LoadDeviceList();
            }
        }
        break;
    }
}

void CDlgBluetoothBase::AcceptMessageBox()
{
    MessageBox_RemoveMessageBox(this);
}

void CDlgBluetoothBase::ShowPasswordMesaageBox()
{
    m_bAskingPwd = true;

    SubMenuData objSubMenu;
    objSubMenu.strMenuAction = "OpenSubPage(enter_password)";
    emit openpage(objSubMenu);

    // 创建密码输入界面
    CBaseDialog * pDlgPswdBase = UIManager_GetPrivateDlg(DLG_CDlgAskPassword);

    if (NULL != pDlgPswdBase)
    {
        disconnect(pDlgPswdBase, SIGNAL(SubmitPassword(const yl::string &)), 0, 0);
        connect(pDlgPswdBase, SIGNAL(SubmitPassword(const yl::string &)), this,
                SLOT(OnSubmitBTPassword(const yl::string &)));
        connect(pDlgPswdBase, SIGNAL(CancelInputPassword()), this, SLOT(OnCancelBTPassword()));
    }
}

void CDlgBluetoothBase::OnReturnToIdle()
{
    ResetAskStatus();
}

void CDlgBluetoothBase::LoadDeviceList(bool bClearStatus /*= true*/)
{
    UI_LIST_BT_DEVICE listDeviceInfo;
    Bluetooth_GetDeviceList(listDeviceInfo);

    m_strConnectedDeviceName.clear();
    m_strConnectedMac.clear();

#ifdef BLUETOOTH_USED_NEW_VERSION
    m_bInConnecting = false;
#endif

    SETTINGUI_INFO("[CDlgBluetoothBase::LoadDeviceList][Num = %d]", listDeviceInfo.size());

    if (NULL != m_pGeneralFrame)
    {
        if (NULL != &listDeviceInfo)
        {
            m_vecBlueWgtPair.clear();

#ifdef BLUETOOTH_USED_NEW_VERSION
            SetCurOperatorMAC("");
#endif

            // http://10.2.1.199/Bug.php?BugID=110373
            if (Bluetooth_GetDeviceState() != DS_NO_DONGLE)
            {
                // LoadPairedDeviceList
                AddPairedDevice(listDeviceInfo);

                // LoadAllDeviceList
                AddNotPairedDevice(listDeviceInfo);
            }
        }
    }

    DoReLayout(true);

    SETTINGUI_INFO("[CDlgBluetoothBase::LoadDeviceList][end]");
}

void CDlgBluetoothBase::DeleteNotPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo)
{
    DeviceInfo * pDevInfo = NULL;

    for (UI_LIST_BT_DEVICE::iterator iter = listDeviceInfo.begin();
            iter != listDeviceInfo.end(); ++iter)
    {
        pDevInfo = &(*iter);
        if (NULL == pDevInfo)
        {
            break;
        }

        if (!IsPairedDeivces(pDevInfo->eState))
        {
#ifdef BLUETOOTH_USED_NEW_VERSION
            Bluetooth_DeleteDevice(pDevInfo->strMac.c_str());
#else
            Bluetooth_DeleteDevice(pDevInfo->szMAC);
#endif
        }
    }
}

void CDlgBluetoothBase::LoadPairedDevice()
{
    SETTINGUI_INFO("Bluetooth UI:LoadPairedDevice begin.");

    UI_LIST_BT_DEVICE listDeviceInfo;
    Bluetooth_GetDeviceList(listDeviceInfo);

    // 删除未配对蓝牙
    DeleteNotPairedDevice(listDeviceInfo);

    m_strConnectedDeviceName.clear();
    m_strConnectedMac.clear();

    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->ClearContent(true);

        if (NULL != &listDeviceInfo)
        {
            m_vecBlueWgtPair.clear();

            // LoadPairedDeviceList
            AddPairedDevice(listDeviceInfo);
        }
    }

    SETTINGUI_INFO("Bluetooth UI:LoadPairedDevice end");
}

void CDlgBluetoothBase::AddPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo)
{
    SETTINGUI_INFO("[CDlgBluetoothBase::AddPairedDevice][start]");

    DeviceInfo * pDevInfo = NULL;

#ifdef BLUETOOTH_USED_NEW_VERSION
    int iNum = Bluetooth_IsConnected() ? 1 : 0;
#else
    int iNum = 0;
#endif

    for (UI_LIST_BT_DEVICE::iterator iter = listDeviceInfo.begin();
            iter != listDeviceInfo.end(); ++iter)
    {
        pDevInfo = &(*iter);
        if (NULL == pDevInfo)
        {
            break;
        }

#ifdef BLUETOOTH_USED_NEW_VERSION
        SETTINGUI_INFO("Bluetooth UI: Add Paired Device Mac = [%s]", pDevInfo->strMac.c_str());
#else
        SETTINGUI_INFO("Bluetooth UI: Add Paired Device Mac = [%s]", pDevInfo->szMAC);
#endif
        if (IsPairedDeivces(pDevInfo->eState))
        {
            if (BluetoothUI_IsConnectedDevice(pDevInfo))
            {
                m_strConnectedDeviceName = toQString(pDevInfo->strName);
                m_strConnectedMac = toQString(pDevInfo->strMac);
            }
            else
            {
                QWidgetPair qPair;

                if (FillBtWgtPairByBtDevice(qPair, pDevInfo))
                {
                    ++iNum;
                    m_mapMACIndex[toQString(pDevInfo->strMac)] = iNum + GetResidentItemCount();
                    m_vecBlueWgtPair.push_back(qPair);
                }
            }
        }
    }

    SETTINGUI_INFO("[CDlgBluetoothBase::AddPairedDevice][end][%d]", iNum);
    if (iNum != m_iPairedBtCounts)
    {
        m_iPairedBtCounts = iNum;
    }
}

void CDlgBluetoothBase::AddNotPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo)
{
    SETTINGUI_INFO("[CDlgBluetoothBase::AddNotPairedDevice][start]");
    DeviceInfo * pDevInfo = NULL;

    int iNum = 0;
    for (UI_LIST_BT_DEVICE::iterator iter = listDeviceInfo.begin();
            iter != listDeviceInfo.end(); ++iter)
    {
        pDevInfo = &(*iter);

        if (NULL == pDevInfo)
        {
            break;
        }

        if (!IsPairedDeivces(pDevInfo->eState))
        {
            QWidgetPair qPair;

            if (FillBtWgtPairByBtDevice(qPair, pDevInfo))
            {
                ++iNum;
                m_mapMACIndex[toQString(pDevInfo->strMac)] = m_iPairedBtCounts + iNum + GetResidentItemCount();
                m_vecBlueWgtPair.push_back(qPair);
            }
        }
    }

    SETTINGUI_INFO("[CDlgBluetoothBase::AddNotPairedDevice][end][%d]", iNum);
}

void CDlgBluetoothBase::DealWithDetailOperator(const int iOp, const QString & strMac)
{
    SETTINGUI_INFO("CDlgBluetoothBase::DealWithDetailOperator : [%d]", iOp);

#ifndef BLUETOOTH_USED_NEW_VERSION
    m_iAutoConnet = -1;
#endif

    switch (iOp)
    {
    case MessageBox_MoveUp:
        {
            OnBtnMoveUp(strMac.toUtf8().data());
        }
        break;
    case MessageBox_MoveDown:
        {
            OnBtnMoveDown(strMac.toUtf8().data());
        }
        break;
    case MessageBox_Delete:
        {
            OnBtnDelete(strMac.toUtf8().data());
        }
        break;
    case MessageBox_Cancel:
        {
        }
        break;
    case MessageBox_On:// auto connect On
        {
            Bluetooth_SetAutoConnect(strMac.toUtf8().data(), true);
#ifndef BLUETOOTH_USED_NEW_VERSION
            m_iAutoConnet = 1;
#endif
        }
        break;
    case MessageBox_Off:// auto connect Off
        {
            Bluetooth_SetAutoConnect(strMac.toUtf8().data(), false);
#ifndef BLUETOOTH_USED_NEW_VERSION
            m_iAutoConnet = 0;
#endif
        }
        break;
    default:
        break;
    }
}

void CDlgBluetoothBase::OnSubmitBTPassword(const yl::string & strPswd)
{
    SETTINGUI_INFO("BT Password: %s", strPswd.c_str());

    m_bAskingPwd = false;

#ifdef BLUETOOTH_USED_NEW_VERSION
    Bluetooth_AnswerPin(m_strAskPinMac.toUtf8().data(), strPswd.c_str());

#else
    QString strMac = GetCurrentBluetoothMac();

    if (!strMac.isEmpty())
    {
        Bluetooth_AnswerPin(strMac.toUtf8().data(), strPswd.c_str());
    }
#endif
}

void CDlgBluetoothBase::OnCancelBTPassword()
{
    m_bAskingPwd = false;
#ifdef BLUETOOTH_USED_NEW_VERSION
    if (!m_strAskPinMac.isEmpty())
    {
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[m_strAskPinMac], "", INT_NONE, SETTING_ITEM_BLUETOOTH,
                                        BLUETOOTH_UI_ACITON_CONNECT);
        Bluetooth_CancelPair(m_strAskPinMac.toUtf8().data());
    }

    SetCurOperatorMAC("");
    m_bInConnecting = false;
#else
    if (m_pGeneralFrame != NULL)
    {
        QString strMac = GetCurrentBluetoothMac();

        Bluetooth_AnswerPin(strMac.toUtf8().data(), "");
    }
#endif
}

void CDlgBluetoothBase::OnBtnConnect(const QString & strMac)
{
    if (!CheckDongle())
    {
        return;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    SetCurOperatorMAC(strMac);
#endif

    DeviceInfo bluetoothDeviceInfo;
    Bluetooth_GetBluetoothInfoByMac(strMac.toUtf8().data(), bluetoothDeviceInfo);

    if (BluetoothUI_IsConnectedDevice(&bluetoothDeviceInfo))
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_HAS_CONNECTED).toUtf8().data(),
                                        INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_DISCONNECT);
#else
        SetBluetoothItemNote(LANG_TRANSLATE(TRID_HAS_CONNECTED));
#endif
        return;
    }

    if (!Bluetooth_ConnectDevice(strMac.toUtf8().data()))
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_CONNECT_FAILED).toUtf8().data(),
                                        INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT);
#else
        SetBluetoothItemNote(LANG_TRANSLATE(TRID_CONNECT_FAILED));
#endif
    }
    else
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_OLD_CONNECTING).toUtf8().data(),
                                        INT_ANIMATION, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT);
#else
        SetBluetoothItemNote(LANG_TRANSLATE(TRID_OLD_CONNECTING));
#endif
    }
}

void CDlgBluetoothBase::OnBtnDisconnect(const QString & strMac)
{
    if (!CheckDongle())
    {
        return;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                    LANG_TRANSLATE(TRID_DISCONNECTING).toUtf8().data(),
                                    INT_ANIMATION, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_DISCONNECT);

    SetCurOperatorMAC("");
#endif

    if (!Bluetooth_DisconnectDevice(strMac.toUtf8().data()))
    {
#ifdef BLUETOOTH_USED_NEW_VERSION
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_DISCONNECT_FAILED).toUtf8().data(),
                                        INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_DISCONNECT);
#else
        SetBluetoothItemNote(LANG_TRANSLATE(TRID_DISCONNECT_FAILED));
#endif
        return;
    }

#ifndef BLUETOOTH_USED_NEW_VERSION
    SetBluetoothItemNote(LANG_TRANSLATE(TRID_DISCONNECTING));
#endif
}

void CDlgBluetoothBase::OnBtnDelete(const QString & strMac)
{
    if (CheckDongle())
    {
        QString strNote = TRID_IF_DELETE_SELECTED_BLUETOOTH_DEVICE;

        // 在搜索过程中删除需要停止搜索，否则逻辑层可能会有问题
        // 对用户进行提示
        if (IsBlueboothdeviceScan())
        {
            strNote = TRID_SCANNING_WILL_STOP_IF_CONTINUE_THIS_OPERATION;
        }

        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(strNote), MESSAGEBOX_OkCancel, -1,
                                    "", "", BT_UI_POPBOX_ID_DELITEM, QVariant(strMac.toUtf8().data()));
    }
}

void CDlgBluetoothBase::OnBtnDeleteAll()
{
    if (CheckDongle())
    {
        QString strNote = TRID_IF_DELETE_ALL_BLUETOOTH_ITEM;

        if (IsBlueboothdeviceScan())
        {
            strNote = TRID_SCANNING_WILL_STOP_IF_CONTINUE_THIS_OPERATION;
        }

        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(strNote), MESSAGEBOX_OkCancel, -1,
                                    "", "", BT_UI_POPBOX_ID_DELALLITEM);
    }
}

void CDlgBluetoothBase::OnBtnMoveUp(const QString & strMac)
{
#ifndef BLUETOOTH_USED_NEW_VERSION
    m_iMoveUp = 1;
#endif

    Bluetooth_AdjustDevicePos(strMac.toUtf8().data(), true);
    m_bClearStatus = false;
}

void CDlgBluetoothBase::OnBtnMoveDown(const QString & strMac)
{
#ifndef BLUETOOTH_USED_NEW_VERSION
    m_iMoveUp = 0;
#endif
    Bluetooth_AdjustDevicePos(strMac.toUtf8().data(), false);
    m_bClearStatus = false;
}

// set bluetooth item tip
void CDlgBluetoothBase::SetBluetoothItemNote(const QString & strNote)
{
    CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();
    if (NULL != pNowItem)
    {
        if (pNowItem->inherits("CSettingBluetoothItem"))
        {
            CSettingBluetoothItem * cc = static_cast<CSettingBluetoothItem *>(pNowItem);
            if (NULL != cc)
            {
                cc->m_bluetoothData.strNote = LANG_TRANSLATE(strNote).toUtf8().data();
                cc->update();
            }
        }
    }
}

#ifdef BLUETOOTH_USED_NEW_VERSION
// 更新连接结果提示语   0 : connect 1 : disconnect
void CDlgBluetoothBase::SetConnectResultNote(int iResult, QString strMac /*= ""*/)
{

    SETTINGUI_INFO("Bluetooth UI:[DlgBluetoothBase::SetConnectResultNote()],Connect result[%d], strMac[%s].",
                   iResult, strMac.toUtf8().data());

    if (strMac.isEmpty() || m_mapMACIndex[strMac] >= m_vecWidgetPair.size())
    {
        return;
    }

    if (0 == iResult)
    {
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_CONNECT_FAILED).toUtf8().data()
                                        , INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT);
    }
    else if (1 == iResult)
    {
        SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                        LANG_TRANSLATE(TRID_CONNECT_SUCCESS).toUtf8().data()
                                        , INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_DISCONNECT, true);
    }

    m_bInConnecting = false;
}

void CDlgBluetoothBase::SetDisConnectResultNote(int iResult, QString strMac /*= ""*/)
{
    SETTINGUI_INFO("Bluetooth UI:[DlgBluetoothBase::SetDisConnectResultNote],Disconnect result[%d], strMac[%s].",
                   iResult, strMac.toUtf8().data());

    if (strMac.isEmpty() || m_mapMACIndex[strMac] >= m_vecWidgetPair.size())
    {
        return;
    }

    UI_LIST_BT_DEVICE listDeviceInfo;
    Bluetooth_GetDeviceList(listDeviceInfo);
    QString str = QString("%1 %2").arg(listDeviceInfo.size()).arg(LANG_TRANSLATE(
                      TRID_BLUETOOTH_DEVICES));

    SetBlueItemSavedDataByDataIndex(GetResidentItemCount(), str.toUtf8().data(), INT_FOREVER,
                                    SETTING_ITEM_ANIMATION);

    SetBlueItemSavedDataByDataIndex(m_mapMACIndex[strMac],
                                    LANG_TRANSLATE(TRID_DISCONNECT_SUCCESS).toUtf8().data(),
                                    INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT, false);
}

// set bluetooth item ui saved data
void CDlgBluetoothBase::SetBlueItemSavedDataByDataIndex(int iDataIdx, std::string strNote,
        Item_Note_Type eType
        , int eItemType /*= SETTING_ITEM_BLUETOOTH*/
        , const QString & strAction /*= ""*/
        , bool bConnected /*= false*/)
{
    if (iDataIdx < 0 || iDataIdx >= m_vecWidgetPair.size())
    {
        return;
    }

    SETTINGUI_INFO("[DlgBluetoothBase::SetBlueItemSavedDataByDataIndex] idx[%d], note[%s], noteT[%d], itemT[%d], action[%s], connet[%d]"
                   , iDataIdx, strNote.c_str(), eType, eItemType, strAction.toUtf8().data(), bConnected);

    if (SETTING_ITEM_BLUETOOTH == eItemType)
    {
        BluetoothAddData blueData = m_vecWidgetPair[iDataIdx].appendVariantData.value<BluetoothAddData>();
        blueData.strNote = strNote.c_str();
        blueData.bExistNoteOrNot = bConnected || INT_ANIMATION == eType || INT_FOREVER == eType;
        m_vecWidgetPair[iDataIdx].appendVariantData.setValue(blueData);
    }
    else if (SETTING_ITEM_ANIMATION == eItemType)
    {
        SSettingAnimationItemData searchData =
            m_vecWidgetPair[iDataIdx].appendVariantData.value<SSettingAnimationItemData>();
        searchData.m_bAnimationSwitch = (INT_ANIMATION == eType) ? true : false;
        searchData.m_strLabel = toQString(strNote.c_str());
        m_vecWidgetPair[iDataIdx].appendVariantData.setValue(searchData);
    }

    m_vecWidgetPair[iDataIdx].third = strAction;

    m_pBlueDelegate->SetItemDataByDataIndex(m_vecWidgetPair[iDataIdx], iDataIdx, eItemType);
}

// get/set current operator device MAC
void CDlgBluetoothBase::SetCurOperatorMAC(const QString & strMac)
{
    m_strCurOperatorMac = strMac;
}

QString CDlgBluetoothBase::GetCurOperatorMAC()
{
    return m_strCurOperatorMac;
}

// set/get detail MAC
QString CDlgBluetoothBase::GetDetailMAC()
{
    return m_strDetailMac;
}
#endif // #ifdef BLUETOOTH_USED_NEW_VERSION

void CDlgBluetoothBase::SetDetailMac(QString strDetailMac)
{
    m_strDetailMac = strDetailMac;
}

#ifndef BLUETOOTH_USED_NEW_VERSION
// 是否是移动操作
int CDlgBluetoothBase::GetMoveUpOrNot()
{
    int iTmp = m_iMoveUp;
    m_iMoveUp = -1;      //多次刷新会出错，所以初始为-1，保证只操作一次
    return iTmp;
}
#endif

void CDlgBluetoothBase::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    SETTINGUI_INFO("CDlgBluetoothBase::MessageBoxCallBack()");
    if (NULL == pMessageBox)
    {
        return;
    }
    SETTINGUI_INFO("Result[%d], ID[%d]", pMessageBox->GetResult(), pMessageBox->GetID());
    SETTINGUI_INFO("ID");
    SETTINGUI_INFO("BT_UI_POPBOX_ID_CANCEL_SCAN	= %d", BT_UI_POPBOX_ID_CANCEL_SCAN);
    SETTINGUI_INFO("BT_UI_POPBOX_ID_DELITEM		= %d", BT_UI_POPBOX_ID_DELITEM);
    SETTINGUI_INFO("BT_UI_POPBOX_ID_DELALLITEM	= %d", BT_UI_POPBOX_ID_DELALLITEM);
    SETTINGUI_INFO("BT_UI_POPBOX_ID_DETAIL		= %d", BT_UI_POPBOX_ID_DETAIL);
    SETTINGUI_INFO("\nresult");
    SETTINGUI_INFO("MessageBox_Cancel	= %d", MessageBox_Cancel);
    SETTINGUI_INFO("MessageBox_OK		= %d", MessageBox_OK);
    SETTINGUI_INFO("MessageBox_Destory	= %d", MessageBox_Destory);
    SETTINGUI_INFO("MessageBox_ERROR	= %d", MessageBox_ERROR);

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case BT_UI_POPBOX_ID_CANCEL_SCAN:
        {
#ifdef BLUETOOTH_USED_NEW_VERSION
            m_bInScanning = false;
#endif

            RequestStopScan();
        }
        break;
    case BT_UI_POPBOX_ID_DELITEM:
        {
            if (MessageBox_OK == nResult)
            {
                RequestStopScan();

                QString strMac = pMessageBox->GetExtraData().toString();
                Bluetooth_DeleteDevice(strMac.toUtf8().data());
                m_bClearStatus = false;
#ifdef BLUETOOTH_USED_NEW_VERSION
                SetDetailMac("");
#endif

                UpdateDeviceList(OP_UPDATE_LIST, 0);
            }
        }
        break;
    case BT_UI_POPBOX_ID_DELALLITEM:
        {
            if (MessageBox_OK == nResult)
            {
                RequestStopScan();

                Bluetooth_DeleteDevice();
                m_bClearStatus = true;
#ifdef BLUETOOTH_USED_NEW_VERSION
                SetCurOperatorMAC("");
#endif

                UpdateDeviceList(OP_UPDATE_LIST, 0);
            }
        }
        break;
#ifndef BLUETOOTH_USED_NEW_VERSION
    case BT_UI_POPBOX_ID_DETAIL:
        {
            DealWithDetailOperator(nResult, m_strDetailMac);
            // set DetailMac Null
            SetDetailMac(QString(""));

            AfterDetail();
        }
        break;
#endif
#ifdef SUPPORT_PHONE_DEVICE
    // 蓝牙手机Pin
    case BT_UI_POPBOX_ID_MOBILEPIN:
        {
            SETTINGUI_INFO("Mobile AskPIN mac[%s]", m_strAskPinMac.toUtf8().data());
            if (MessageBox_OK == nResult)
            {
                if (false == Bluetooth_AnswerMobilePin(m_strAskPinMac.toUtf8().data()))
                {
                    SetBlueItemSavedDataByDataIndex(m_mapMACIndex[m_strAskPinMac],
                                                    LANG_TRANSLATE(TRID_CONNECT_FAILED).toUtf8().data()
                                                    , INT_DISAPPEAR, SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT);
                }
            }
            else if (MessageBox_Cancel == nResult)
            {
                SetBlueItemSavedDataByDataIndex(m_mapMACIndex[m_strAskPinMac], "", INT_NONE
                                                , SETTING_ITEM_BLUETOOTH, BLUETOOTH_UI_ACITON_CONNECT);
                SetCurOperatorMAC("");
                Bluetooth_CancelPair(m_strAskPinMac.toUtf8().data());
            }
            else
            {
                Bluetooth_CancelPair(m_strAskPinMac.toUtf8().data());
            }
        }
        break;
    // 同步联系人
    case BT_UI_POPBOX_ID_MOBILESYNC:
        {
            bool bSync = false;

            if (MessageBox_OK == pMessageBox->GetResult())
            {
                bSync = true;
            }

            if (pMessageBox->GetResult() == MessageBox_Destory)
            {
                idleScreen_SetStatus(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY);
            }
            else
            {
                idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY);
            }

            SETTINGUI_INFO("Bluetooth UI:bluetoothbase msgCallBack request sync contacts [%s], sync[%d]",
                           s_strPinMac.toUtf8().data(), bSync);

            Bluetooth_SetSyncContact(s_strPinMac.toUtf8().data(), bSync);

            UpdateDeviceList(OP_UPDATE_LIST, 0);
        }
        break;
    case BT_UI_POPBOX_ID_DETAIL:
        {
            QString strMac = GetDetailMAC();
            bool bUpdate = true;
            SETTINGUI_INFO("move or delete [%s]", GetDetailMAC().toUtf8().data());
            switch (nResult)
            {
            case MessageBox_Delete:
                {
                    bUpdate = false;
                    OnBtnDelete(strMac);
                }
                break;
            case MessageBox_MobileSyncOn:
                {
                    Bluetooth_SetSyncContact(strMac.toUtf8().data(), true);
                }
                break;
            case MessageBox_MobileSyncOff:
                {
                    Bluetooth_SetSyncContact(strMac.toUtf8().data(), false);
                }
                break;
            default:
                {
                    bUpdate = false;
                    SetDetailMac("");
                }
                break;
            }

            if (m_pBlueDetailSelector != NULL)
            {
                m_pBlueDetailSelector->ResetCheckBox();
            }

            if (bUpdate)
            {
                SetDetailMac("");
                UpdateDeviceList(OP_UPDATE_LIST, 0);
            }
        }
        break;
#endif
    default:
        break;
    }
}

// 检查蓝牙Dongle是否存在，及是否有弹框提示语
bool CDlgBluetoothBase::CheckDongle(bool bPopDlgTip /* = true */)
{
    if (Bluetooth_GetDeviceState() == DS_NO_DONGLE)
    {
        if (bPopDlgTip)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NO_BLUETOOTH_ADAPTER_FOUND),
                                        MESSAGEBOX_Cancel, SHORT_TIME);
        }

        return false;
    }

    return true;
}

#ifdef BLUETOOTH_USED_NEW_VERSION
bool CDlgBluetoothBase::CheckSwitchChange()
{
    bool bResult = false;

    bool bStatus = Bluetooth_IsEnable();

    if (!bStatus)
    {
        MessageBox_RemoveMessageBox(this);
    }

    if (bStatus != m_pCheckBox->GetData())
    {
        m_pCheckBox->SetData(bStatus);
        bResult = true;
    }

    emit RefreshSoftkey();

    return bResult;
}
#endif

void CDlgBluetoothBase::RegisterMsg()
{
    etl_RegisterMsgHandle(BLUETOOTH_MSG_DONGLE_CHANGE, BLUETOOTH_MSG_DONGLE_CHANGE,
                          OnBluetoothUIMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_DEVICE_POWER, BLUETOOTH_MSG_DEVICE_POWER, OnBluetoothUIMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_UPDATE_LIST, BLUETOOTH_MSG_UPDATE_LIST, OnBluetoothUIMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_REQ_PIN, BLUETOOTH_MSG_REQ_PIN, OnBluetoothUIMessage);

#ifdef BLUETOOTH_USED_NEW_VERSION
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE, OnBluetoothUIMessage);
#endif

#ifdef SUPPORT_PHONE_DEVICE
    etl_RegisterMsgHandle(BLUETOOTH_MSG_SHOW_PIN, BLUETOOTH_MSG_SHOW_PIN, OnBluetoothUIMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_REQ_SYNC, BLUETOOTH_MSG_REQ_SYNC, OnBluetoothUIMessage);
#endif
}

void CDlgBluetoothBase::UnRegisterMsg()
{
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_DONGLE_CHANGE, BLUETOOTH_MSG_DONGLE_CHANGE,
                            OnBluetoothUIMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_DEVICE_POWER, BLUETOOTH_MSG_DEVICE_POWER,
                            OnBluetoothUIMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_UPDATE_LIST, BLUETOOTH_MSG_UPDATE_LIST, OnBluetoothUIMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_REQ_PIN, BLUETOOTH_MSG_REQ_PIN, OnBluetoothUIMessage);

#ifdef BLUETOOTH_USED_NEW_VERSION
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                            OnBluetoothUIMessage);
#endif

#ifdef SUPPORT_PHONE_DEVICE
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_SHOW_PIN, BLUETOOTH_MSG_SHOW_PIN, OnBluetoothUIMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_REQ_SYNC, BLUETOOTH_MSG_REQ_SYNC, OnBluetoothUIMessage);
#endif
}

void CDlgBluetoothBase::InitResidentWgtPair(bool bBTSwitchOn/* = false*/)
{
    QWidgetPair qPair;
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    qPair.bChangeStyleOnSelect = false;
#endif

    // Edit Device Information
    if (!GetOpenByTestMode())
    {
        qPair.first = LANG_TRANSLATE(TRID_EDIT_MY_BLUETOOTH_DEVICE_INFORMATION);
        qPair.second = NULL;
        qPair.third = BLUETOOTH_UI_ACITON_ENTER;
        qPair.eType = SETTING_ITEM_MENU;
        qPair.bAddIndex = false;
        qPair.bAddMark = false;
        m_vecWidgetPair.push_back(qPair);
    }

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
    qPair.bChangeStyleOnPress = false;
#endif
#endif

    qPair.bAddMark = true;

    // Bluetooth Device Switch
    qPair.first = LANG_TRANSLATE(TRID_BLUE_TOOTH);
    qPair.second = m_pCheckBox;
    qPair.third = "";
    qPair.eType = SETTING_ITEM_NORMAL;
    qPair.bAddIndex = false;
    m_vecWidgetPair.push_back(qPair);

    // Bluetooth Device Search State
    if (bBTSwitchOn)
    {
        QString strLabel;
        GetSearchTitleText(strLabel);

        qPair.first = strLabel;
        qPair.second = m_pPicAnimation;
        qPair.third = "";
        qPair.eType = SETTING_ITEM_ANIMATION;

        SSettingAnimationItemData itemData;
        itemData.m_bHideAnimationWhenStop = true;

        if (IsBlueboothdeviceScan())
        {
            itemData.m_bAnimationSwitch = true;
        }
        else
        {
            itemData.m_bAnimationSwitch = false;
        }

        itemData.m_rcAnimationWidget = RECT_ANIMATION_PIC;
        itemData.m_rcText = RECT_ANIMATION_TEXT;

        qPair.appendVariantData.setValue(itemData);
        m_vecWidgetPair.push_back(qPair);
    }
}

bool CDlgBluetoothBase::FillBtWgtPairByBtDevice(QWidgetPair & qPair, DeviceInfo * pDeviceInfo)
{
    if (NULL == pDeviceInfo)
    {
        SETTINGUI_WARN("Bluetooth UI:[CDlgBluetoothBase::FillBtWgtPairByBtDevice()] fail, pDeviceInfo[NULL].");
        return false;
    }

    qPair.first = "";
    qPair.second = NULL;
    qPair.eType = SETTING_ITEM_BLUETOOTH;

    BluetoothAddData bluetoothData;
    bluetoothData.strInfo = toQString(pDeviceInfo->strName);
    bluetoothData.strMac = pDeviceInfo->strMac;
    bluetoothData.bIsPairOrNot = IsPairedDeivces(pDeviceInfo->eState);
#ifdef SUPPORT_PHONE_DEVICE
    bluetoothData.bMobileDevice = (pDeviceInfo->eType == BT_PHONE);
    bluetoothData.bAutoSync = pDeviceInfo->bSyncContact;
#endif
    bluetoothData.bAutoConnect = pDeviceInfo->bAutoConnect;

    if (WIRE_DEVICE_CONNECTED_STATE == pDeviceInfo->eState)
    {
        qPair.third = BLUETOOTH_UI_ACITON_DISCONNECT;
    }
    else
    {
        qPair.third = BLUETOOTH_UI_ACITON_CONNECT;
    }

    bluetoothData.strNote = "";

    switch (pDeviceInfo->eState)
    {
    case WIRE_DEVICE_CONNECTING_STATE:
    case WIRE_DEVICE_PAIRING_STATE:
    case WIRE_DEVICE_ANSWERPIN_STATE:
        {
#ifdef BLUETOOTH_USED_NEW_VERSION
            m_bInConnecting = true;
#endif
            bluetoothData.bExistNoteOrNot = true;
            bluetoothData.strNote = LANG_TRANSLATE(TRID_OLD_CONNECTING).toUtf8().data();
            SETTINGUI_INFO("Bluetooth UI:Set Cur connecting Device MAC[%s]", pDeviceInfo->strMac.c_str());
            SetCurOperatorMAC(pDeviceInfo->strMac.c_str());
        }
        break;
    case WIRE_DEVICE_CONNECTED_STATE:
        {
            bluetoothData.bExistNoteOrNot = true;
            bluetoothData.strNote = LANG_TRANSLATE(TRID_CONNECT_SUCCESS).toUtf8().data();
        }
        break;
    default:
        break;
    }

    qPair.appendVariantData.setValue(bluetoothData);

    return true;
}

int CDlgBluetoothBase::GetResidentItemCount()
{
    return (GetOpenByTestMode() ? BLUETOOTH_UI_RESIDENT_ITEM_COUNT_TESTMODE :
            BLUETOOTH_UI_RESIDENT_ITEM_COUNT_DEFAULT);
}

void CDlgBluetoothBase::GetSearchTitleText(QString & strTitle)
{
    int nConnectedCount = Bluetooth_IsConnected() ? 1 : 0;

    if (m_strConnectedDeviceName.isEmpty())
    {
        strTitle = QString("%1 %2:").arg(m_vecBlueWgtPair.size() + nConnectedCount)
                   .arg(LANG_TRANSLATE(TRID_BLUETOOTH_DEVICES));
    }
    else
    {
        strTitle = QString("%1 %2%3%4").arg(m_vecBlueWgtPair.size() + nConnectedCount)
                   .arg(LANG_TRANSLATE(TRID_BLUETOOTH_DEVICE_PARENTHESES_CONNECTING_WITH))
                   .arg(m_strConnectedDeviceName).arg("):");
    }
}

bool CDlgBluetoothBase::IsBlueboothdeviceScan() const
{
    return m_bInScanning || DS_SCAN == Bluetooth_GetDeviceState();
}

void CDlgBluetoothBase::OnStartScan()
{
    LoadDeviceList();

    emit RefreshSoftkey();
}

void CDlgBluetoothBase::OnStopScan()
{
    UI_LIST_BT_DEVICE listDeviceInfo;
    Bluetooth_GetDeviceList(listDeviceInfo);
    QString str = QString("%1 %2").arg(listDeviceInfo.size()).arg(LANG_TRANSLATE(
                      TRID_BLUETOOTH_DEVICES));

    SetBlueItemSavedDataByDataIndex(GetResidentItemCount(), str.toUtf8().data(), INT_FOREVER,
                                    SETTING_ITEM_ANIMATION);

    emit RefreshSoftkey();
}

bool CDlgBluetoothBase::RequestStartScan()
{
    if (Bluetooth_StartScanDevice())
    {
        OnStartScan();

        return true;
    }

    OnStopScan();

    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SCAN_FAILED),
                                MESSAGEBOX_Cancel, SHORT_TIME);

    return false;
}

bool CDlgBluetoothBase::RequestStopScan()
{
    if (Bluetooth_StopScanDevice())
    {
        OnStopScan();
    }
    return true;
}

bool CDlgBluetoothBase::IsPairedDeivces(enum WIRELESS_DEVICE_STATE eState)
{
#ifdef BLUETOOTH_USED_NEW_VERSION
    return (eState == WIRE_DEVICE_PAIRED_STATE || eState == WIRE_DEVICE_CONNECTING_STATE
            || eState == WIRE_DEVICE_CONNECTED_STATE || eState == WIRE_DEVICE_DISCONNECT_STATE);
#else
    return (eState == REMOTEDEV_PAIRED_STATE || eState == REMOTEDEV_CONNECTING_STATE
            || eState == REMOTEDEV_CONNECTED_STATE || eState == REMOTEDEV_DISCONNECT_STATE);
#endif

}

bool CDlgBluetoothBase::IsRejectReturnToIdle(RETURN_IDLE_TYPE
        eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    if (RETURN_IDLE_TYPE_TIME_OUT == eReturnIdle)
    {
        return true;
    }

    return false;
}
