#include "wifi_inc.h"
#include "settingui/src/cdlgaskpassword.h"


Q_DECLARE_METATYPE(WifiAddData);

namespace
{
// test
//#define TEST_WIFI
// page action
#define ENTER_WIFI_ADD_EDIT_PAGE        "OpenSubPage(wifiaddedit)"

#define OPEN_PASSWORD_ACTION    "OpenSubPage(enter_password)"
// action
#define CONNECT_WIFI                    "Connect_Wifi"
#define DISCONNECT_WIFI                 "DisConnect_Wifi"
// softkey
#define WIFI_SOFTKEY_ADD                "Add"
// signal strong
// #define MIN_SIGNAL              -85
// #define LOW_SIGNAL              -75
// #define MID_SIGNAL              -65
// #define HIGH_SIGNAL             -55
// popup id
#define PopupBoxID_Detail       90
};

// 处理消息.
static LRESULT OnWifiStorageUIMessage(msgObject & refMessage)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CDlgWifiStorage::OnWifiStorageUIMessage] message[%d], lParam[%d], wParam[%d], UpUI[%d]!!"
                   , refMessage.message, refMessage.lParam, refMessage.wParam, WIFI_MSG_UPDATE_UI);

    CDlgWifiStorage * pWifiStorage = qobject_cast<CDlgWifiStorage *>(UIManager_GetPrivateDlg(
                                         DLG_CDlgWifiStorage));
    if (NULL == pWifiStorage || !pWifiStorage->IsTopPage())
    {
        SETTINGUI_INFO("CDlgWifiStorage not top page");
        return TRUE;
    }

    if (WIFI_MSG_UPDATE_UI != refMessage.message)
    {
        return TRUE;
    }

    WirelessOperationMsg * wifiMsg = (WirelessOperationMsg *)refMessage.GetExtraData();
    if (NULL != wifiMsg)
    {
        switch (wifiMsg->iOperationType)
        {
        case OP_UPDATE_WIFI_DATA:
            {
                pWifiStorage->OnReShow();
            }
            break;
        case OP_CONNECT_WIFI_RESULT:
            {
                _UIWifiManager.SetNeedUpdate(true);

                //连接结果  lparam :0/1/2 success/wrong pwd/failed
                if (-1 != wifiMsg->lSessionID)
                {
                    if (WIRELESS_OP_CONNECT_FAILED == wifiMsg->iOperationResult)
                    {
                        pWifiStorage->SetConnectResultNote(wifiMsg->iOperationResult, wifiMsg->lSessionID);
                    }
                    else
                    {
                        pWifiStorage->OnReShow();
                    }
                }
            }
            break;
        case OP_DISCONNECT_WIFI_RESULT:
            {
                _UIWifiManager.SetNeedUpdate(true);

                //断开连接结果lparam:0/1 failed/success
                if (-1 != wifiMsg->lSessionID)
                {
                    pWifiStorage->SetDisConnectResultNote(wifiMsg->iOperationResult, wifiMsg->lSessionID);
                }
            }
            break;
        case OP_ENTER_WIFI_PWD:
            {
                SETTINGUI_INFO("[SettingUI][OnWifiStorageUIMessage::OP_ENTERPWD]");

                if (wifiMsg->lSessionID != WIFI_SECTION_ID_INVAILED  && !_UIWifiManager.IsOperatering())
                {
                    _UIWifiManager.SetOperatering(true);
                    pWifiStorage->SetWifiPopId(wifiMsg->lSessionID);

                    bool bAuthentication = (refMessage.lParam == 1);

                    pWifiStorage->PopPwdDialog(bAuthentication);
                }
            }
            break;
        default:
            break;
        }
    }
#endif
    return TRUE;
}


/******************************************* CDlgWifiStorage ********************************************/
CDlgWifiStorage::CDlgWifiStorage(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_iPreWifiID(-1)
    , m_bConnecting(false)
    , m_iCurDetailID(-1)
    , m_pWifiSavedDelegate(NULL)
    , m_iPopWifiID(WIFI_SECTION_ID_INVAILED)
#ifdef _T49
    , m_pWifiDetailSelector(NULL)
#endif
{
    m_mpIDDataIndex.clear();
    InitData();

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_STORAGE_NETWORK));
#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiStorageUIMessage);
#endif
}

CDlgWifiStorage::~CDlgWifiStorage(void)
{
#ifdef IF_SUPPORT_WIFI
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, OnWifiStorageUIMessage);
#endif

#ifdef _T49
    if (NULL != m_pWifiDetailSelector)
    {
        delete m_pWifiDetailSelector;
        m_pWifiDetailSelector = NULL;
    }
#endif

    if (_UIWifiManager.IsOperatering())
    {
        OnCancelPassword();
    }
}

void CDlgWifiStorage::InitData()
{
    m_pWifiSavedDelegate = new WifiSavedDelegate();
#ifdef _T49
    m_pWifiDetailSelector = new WifiDetailSelector();
#endif
    if (NULL == m_pWifiSavedDelegate
#ifdef _T49
            || NULL == m_pWifiDetailSelector
#endif
       )
    {
        return;
    }
    m_pWifiSavedDelegate->setParent(this);
    m_pWifiSavedDelegate->BindFramelist(m_pGeneralFrame);
    m_pWifiSavedDelegate->InitSettingListStyle();
    m_pListDelegate = m_pWifiSavedDelegate;
    m_pWifiSavedDelegate->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                         PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                         PIC_GENERAL_LIST_ITEM_CLICK_BG);
    m_pWifiSavedDelegate->SetItemParent(this);
    m_pListDelegate->SetItemParent(this);
    QObject::connect(m_pWifiSavedDelegate, SIGNAL(ItemDetailAction(CListItem *)), this,
                     SLOT(OnItemDetailAction(CListItem *)));
    QObject::connect(m_pWifiSavedDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));

    SetSoftkeyType(STYLE_TYPE_DIVIDE);

    DoRelayout();
}

/*************************************** 继承CListSubPage *****************************************/
bool CDlgWifiStorage::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("[CDlgWifiStorage::OnAction()][action[%s]]"
                   , strAction.toUtf8().data());
    if (CONNECT_WIFI == strAction)
    {
        _UIWifiManager.SetNeedUpdate(true);

        if (-1 == m_iPreWifiID
                || m_mpIDDataIndex[m_iPreWifiID] - 1 != m_pGeneralFrame->GetFocusItemDataIndex()
                || !m_bConnecting)
        {
            if (-1 != m_iPreWifiID && m_mpIDDataIndex[m_iPreWifiID] - 1 < m_vecWidgetPair.size())
            {
                // 消除之前连接的提示
                CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(m_pGeneralFrame->GetItemByDataIndex(
                                                   m_mpIDDataIndex[m_iPreWifiID] - 1));
                if (NULL != pWifiItem)
                {
                    pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(""), INT_NONE);
                }
            }
            // 更新连接提示
            m_iPreWifiID = -1;
            CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();
            if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
            {
                CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
                if (NULL != pWifiItem)
                {
                    m_iPreWifiID = pWifiItem->GetNetID();
                    m_bConnecting = true;
                    pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(TRID_CONNECTING), INT_ANIMATION);
                    if (OR_INTERFACE_FAIL == _UIWifiManager.ConnectWifi("CC", pWifiItem->GetNetID()))
                    {
                        pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(TRID_CONNECT_FAILED), INT_DISAPPEAR);
                    }
                }
            }
        }
    }
    else if (DISCONNECT_WIFI == strAction)
    {
        _UIWifiManager.SetNeedUpdate(true);

        CListItemPtr pNowItem = m_pGeneralFrame->GetFocusItem();
        if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
        {
            CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
            if (NULL != pWifiItem
                    && OR_INTERFACE_SUCCESS == _UIWifiManager.DisconnectWifi("CC", pWifiItem->GetNetID()))
            {
                pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(""), INT_NONE);
            }
        }
    }
    else if (WIFI_SOFTKEY_ADD == strAction)
    {
#ifdef IF_SUPPORT_WIFI
        if (MAX_SAVED_NUM <= WIFI_GetSavedNetworkNum())
        {
            _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                 LANG_TRANSLATE(TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD)
                                                 , MESSAGEBOX_Cancel, SHORT_TIME);
        }
        else
#endif
        {
            CreatePageByString(ENTER_WIFI_ADD_EDIT_PAGE, ENUM_WIFI_ADD_PAGE_TYPE);
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool CDlgWifiStorage::GetSoftkeyData(CArraySoftKeyBarData & objWndData
#ifdef _T49
                                     , QObject * pSignalReceiver
#endif
                                    )
{
    for (int i = 0; i < SOFTKEY_NUM; ++i)
    {
        objWndData[i].m_strSoftkeyTitle.clear();
        objWndData[i].m_strSoftkeyAction.clear();
        objWndData[i].m_strIconPath.clear();
    }

    objWndData[0].m_strSoftkeyAction = WIFI_SOFTKEY_ADD;
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(WIFI_SOFTKEY_ADD);
    objWndData[0].m_strIconPath = PIC_SOFTKEY_ADD;

    return true;
}

void CDlgWifiStorage::OnReShow()
{
    SETTINGUI_INFO("[SettingUI][CDlgWifiStorage::OnReShow()]");
    m_mpIDDataIndex.clear();
    m_bConnecting = false;
    DoRelayout();
}

bool CDlgWifiStorage::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pObj || NULL == pEvent)
    {
        return false;
    }
    if (QEvent::KeyPress == pEvent->type())
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        switch (pKeyEvent->key())
        {
        case PHONE_KEY_1:
        case PHONE_KEY_2:
        case PHONE_KEY_3:
        case PHONE_KEY_4:
        case PHONE_KEY_5:
        case PHONE_KEY_6:
        case PHONE_KEY_7:
        case PHONE_KEY_8:
        case PHONE_KEY_9:
            {
                return true;
            }
            break;
        default:
            break;
        }
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

/********************************************************************************************/
void CDlgWifiStorage::DoRelayout()
{
    m_iPreWifiID = -1;
//#ifdef TEST_WIFI
#ifdef TEST_T48_WIFI
    MakeReadyForRelayout();
    int iTot = 10;
    for (int i = 0; i < iTot; ++i)
    {
        // 添加扫描到的网络
        QWidgetPair qPair;

        qPair.first = "";
        qPair.second = NULL;
        qPair.third = CONNECT_WIFI;
        qPair.eType = SETTING_ITEM_WIFI;
        qPair.bAddIndex = false;
        //
        WifiAddData wifiData("Test", i % 2 ? 2 : 1, ""
                             , i % 2, i % 3);
        wifiData.bAutoPair = true;
        wifiData.iSecurity = i % 2;
        qPair.appendVariantData.setValue(wifiData);
        //
        m_vecWidgetPair.push_back(qPair);
    }
#endif
#ifdef IF_SUPPORT_WIFI
    MakeReadyForRelayout();

    WiFiNetInfo connectNet;
    bool bNet = WIFI_IsConnected();
    WIFI_GetCurrentNetConnectInfo(connectNet);

    UI_WIFI_NETINFO_LIST wifiSaveList;
    WIFI_GetSavedNetworkList(wifiSaveList);

    WiFiNetInfoItem it;
    WiFiNetInfoItem itEnd = wifiSaveList.end();
    // Map默认值为0，故值从1开始，区别不存在ID
    int iIdx = 1;
    for (it = wifiSaveList.begin(); it != itEnd; ++it, ++iIdx)
    {
        QWidgetPair qPair;

        qPair.first = "";
        qPair.second = NULL;
        qPair.third = CONNECT_WIFI;
        qPair.eType = SETTING_ITEM_WIFI;
        qPair.bAddIndex = false;

        WifiAddData wifiData;
        wifiData.iLeftIcon = -1;
        QString strDis = (*it).strProfileName.empty()
                         ? toQString((*it).strSSID)
                         : toQString((*it).strProfileName);
        wifiData.strDisName = strDis;
        wifiData.iSecurity = (*it).iSercureMode;
        wifiData.iNetID = (*it).iNetID;
        wifiData.bAutoPair = true;
        wifiData.eType = INT_FOREVER;
        // 设置Map
        m_mpIDDataIndex[(*it).iNetID] = iIdx;

        if (bNet)
        {
            if (connectNet.iNetID == (*it).iNetID)
            {
                wifiData.strNote = LANG_TRANSLATE(TRID_CONNECT_SUCCESS).toUtf8().data();
                qPair.third = DISCONNECT_WIFI;
            }
        }
        else if (OR_INTERFACE_SUCCESS == _UIWifiManager.IsConnectingWifiDevice((*it).iNetID))
        {
            wifiData.strNote = LANG_TRANSLATE(TRID_CONNECTING).toUtf8().data();
            wifiData.eType = INT_ANIMATION;
            m_iPreWifiID = (*it).iNetID;
            m_bConnecting = true;
        }

        qPair.appendVariantData.setValue(wifiData);
        m_vecWidgetPair.push_back(qPair);
    }
#endif

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}

// 更新连接结果提示语
void CDlgWifiStorage::SetConnectResultNote(int iResult, int iCurID /*= -1*/)
{
    SETTING_INFO("CDlgWifiStorage::SetConnectResultNote(result[%d], curID[%d])"
                 , iResult, iCurID);

    if (-1 == iCurID || m_mpIDDataIndex[iCurID] - 1 >= m_vecWidgetPair.size())
    {
        return;
    }
    m_bConnecting = false;
    // 设置新Item提示
    CListItemPtr pNowItem = m_pGeneralFrame->GetItemByDataIndex(m_mpIDDataIndex[iCurID] - 1);
    if (NULL != pNowItem && pNowItem->inherits("CSettingWifiItem"))
    {
        CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pNowItem);
        if (NULL != pWifiItem)
        {
#ifdef IF_SUPPORT_WIFI
            if (WIRELESS_OP_CONNECT_SUCCESS == iResult)
            {
                std::string strNote = TRID_CONNECT_SUCCESS;
                pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_FOREVER);
                pWifiItem->SetClickAction(DISCONNECT_WIFI);
            }
            else
#endif
            {
                std::string strNote = TRID_CONNECT_FAILED;
                pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_DISAPPEAR);
            }
        }
    }
}

void CDlgWifiStorage::SetDisConnectResultNote(int iResult, int iCurID /*= -1*/)
{
    SETTING_INFO("CDlgWifiStorage::SetDisConnectResultNote(result[%d], iCurID[%d])"
                 , iResult, iCurID);

    if (-1 != iCurID && m_mpIDDataIndex[iCurID] - 1 < m_vecWidgetPair.size())
    {
        // 消除之前连接的提示
        CListItemPtr pPreItem = m_pGeneralFrame->GetItemByDataIndex(m_mpIDDataIndex[iCurID] - 1);
        if (NULL != pPreItem && pPreItem->inherits("CSettingWifiItem"))
        {
            CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pPreItem);
            if (NULL != pWifiItem)
            {
#ifdef IF_SUPPORT_WIFI
                if (WIRELESS_OP_DISCONNECT_FAILED == iResult)
                {
                    std::string strNote = TRID_DISCONNECT_FAILED;
                    pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_DISAPPEAR);
                }
                else
#endif
                {
                    std::string strNote = TRID_DISCONNECT_SUCCESS;
                    pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(strNote.c_str()), INT_DISAPPEAR);
                    pWifiItem->SetClickAction(CONNECT_WIFI);
                } // end if if (0 == iResult)
            } // end if (NULL != pWifiItem)
        } // end if (NULL != pPreItem && pPreItem->inherits("CSettingWifiItem"))
    }
}

void CDlgWifiStorage::CreatePageByString(const char * strPageAction, ENUM_WIFI_PAGE_TYPE eType,
        int iCurId /*= -1*/)
{
    _UIWifiManager.SetCurID(iCurId);
    _UIWifiManager.SetPageType(eType);
    SettingUI_OpenPage(SubMenuData(strPageAction));
}

// 检测Wifi是否存在
bool CDlgWifiStorage::IsWifiExist(int iNetID /*= -1*/)
{
    return (iNetID < 0 ? false : m_mpIDDataIndex[iNetID]);
}

void CDlgWifiStorage::OnItemDetailAction(CListItem * pItem)
{
    if (NULL != pItem && pItem->inherits("CSettingWifiItem"))
    {
        CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(pItem);
        if (NULL != pWifiItem)
        {
#ifdef _T49
            MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
            VecMsgBoxSoftkey vecSoftKey;

            vecSoftKey.push_back(skCancel);

            m_pWifiDetailSelector->LoadDataByType();
            m_pWifiDetailSelector->SetCurNetID(pWifiItem->GetNetID());
            MessageBox_ShowSelectBox(this, vecSoftKey, SELECT_POP_BOX_TYPE_MIDDLE
                                     , m_pWifiDetailSelector, LANG_TRANSLATE(TRID_MORE), PopupBoxID_Detail);
#else
            m_iCurDetailID = pWifiItem->GetNetID();
            MessageBox_ShowWifiDetailBox(this, "", -1, PopupBoxID_Detail);
#endif
        }
    }
}

// 处理弹出框回调
void CDlgWifiStorage::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    SETTINGUI_INFO("[CDlgWifiStorage::ProcessMsgBoxCallBack()]");
    if (NULL == pMessageBox)
    {
        return;
    }
    SETTINGUI_INFO("Result[%d], ID[%d]", pMessageBox->GetResult(), pMessageBox->GetID());
    SETTINGUI_INFO("ID");
    SETTINGUI_INFO("PopupBoxID_Detail	= %d", PopupBoxID_Detail);
    SETTINGUI_INFO("\nresult");
    SETTINGUI_INFO("MessageBox_Cancel	= %d", MessageBox_Cancel);
    SETTINGUI_INFO("MessageBox_OK		= %d", MessageBox_OK);
    SETTINGUI_INFO("MessageBox_Destory	= %d", MessageBox_Destory);
    SETTINGUI_INFO("MessageBox_ERROR	= %d", MessageBox_ERROR);

    int nResult = pMessageBox->GetResult();
#ifdef _T49
    int iCurId = m_pWifiDetailSelector->GetCurNetID();
#else
    int iCurId = m_iCurDetailID;
#endif
    SETTINGUI_INFO("now operating ID [%d]", iCurId);
    switch (pMessageBox->GetID())
    {
    case PopupBoxID_Detail:
        {
            switch (nResult)
            {
            case MessageBox_MoveUp:
                {
                    if (OR_INTERFACE_SUCCESS == _UIWifiManager.AdjustWifiLocation(iCurId, true))
                    {
                        OnReShow();
                    }
                    else
                    {
                        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_WIFI_OPERATOR_FAIL)
                                                             , MESSAGEBOX_Cancel, SHORT_TIME);
                    }
                }
                break;
            case MessageBox_MoveDown:
                {
                    if (OR_INTERFACE_SUCCESS == _UIWifiManager.AdjustWifiLocation(iCurId, false))
                    {
                        OnReShow();
                    }
                    else
                    {
                        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_WIFI_OPERATOR_FAIL)
                                                             , MESSAGEBOX_Cancel, SHORT_TIME);
                    }
                }
                break;
            case MessageBox_Delete:
                {
#ifdef IF_SUPPORT_WIFI
                    WiFiNetInfo wfInfo;
                    WIFI_GetCurrentNetConnectInfo(wfInfo);
                    if (wfInfo.iNetID == iCurId)
#endif
                    {
                        // 删除连接上的返回Base界面需要更新
                        _UIWifiManager.SetNeedUpdate(true);
                    }

                    if (OR_INTERFACE_SUCCESS == _UIWifiManager.DeleteWifiDevice("CC", iCurId))
                    {
                        OnReShow();
                    }
                    else
                    {
                        _UIWifiManager.Wifi_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_WIFI_OPERATOR_FAIL)
                                                             , MESSAGEBOX_Cancel, SHORT_TIME);
                    }
                }
                break;
            case MessageBox_Edit:
                {
                    CreatePageByString(ENTER_WIFI_ADD_EDIT_PAGE, ENUM_WIFI_EDIT_PAGE_TYPE, iCurId);
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
    // init
    m_iCurDetailID = -1;
}

void CDlgWifiStorage::SetWifiPopId(int nPopWifiId)
{
    m_iPopWifiID = nPopWifiId;
}

void CDlgWifiStorage::PopPwdDialog(bool bAuthentication /*= false*/)
{
    if (WIFI_SECTION_ID_INVAILED == m_iPopWifiID
            || m_mpIDDataIndex[m_iPreWifiID] - 1 >= m_vecWidgetPair.size())
    {
        return;
    }

    SubMenuData objSubMenu;
    objSubMenu.strMenuAction = OPEN_PASSWORD_ACTION;

    int nPwdType = CDlgAskPassword::ASK_PASSWORD_WIFI_COMMON;

    if (bAuthentication)
    {
        nPwdType = CDlgAskPassword::ASK_PASSWORD_WIFI_AUTHENTICATION;
    }

    objSubMenu.pBeforeSetSubpageData = (void *) & (nPwdType);

    emit openpage(objSubMenu);

    CBaseDialog * pDlgPswdBase = UIManager_GetPrivateDlg(DLG_CDlgAskPassword);

    if (pDlgPswdBase != NULL)
    {
        if (bAuthentication)
        {
            connect(pDlgPswdBase, SIGNAL(submitNameAndPassword(const yl::string &, const yl::string &)),
                    this, SLOT(OnSubmitNameAndPassword(const yl::string &, const yl::string &)));
        }
        else
        {
            connect(pDlgPswdBase, SIGNAL(SubmitPassword(const yl::string &)), this,
                    SLOT(OnSubmitPassword(const yl::string &)));
        }

        connect(pDlgPswdBase, SIGNAL(CancelInputPassword()), this, SLOT(OnCancelPassword()));
    }
}

void CDlgWifiStorage::OnSubmitNameAndPassword(const yl::string & strName,
        const yl::string & strPswd)
{
    if (m_iPopWifiID >= 0)
    {
        _UIWifiManager.SetOperatering(false);
        _UIWifiManager.AuthNetWork(m_iPopWifiID, strName, strPswd);
    }
}

void CDlgWifiStorage::OnSubmitPassword(const yl::string & strPswd)
{
    if (m_iPopWifiID >= 0)
    {
        _UIWifiManager.SetOperatering(false);
        _UIWifiManager.AskWifiPassword(m_iPopWifiID, strPswd.c_str());
    }
}

void CDlgWifiStorage::OnCancelPassword()
{
    if (m_iPopWifiID >= 0 && m_mpIDDataIndex[m_iPreWifiID] - 1 < m_vecWidgetPair.size())
    {
        _UIWifiManager.SetOperatering(false);

        CSettingWifiItem * pWifiItem = static_cast<CSettingWifiItem *>(m_pGeneralFrame->GetItemByDataIndex(
                                           m_mpIDDataIndex[m_iPopWifiID] - 1));
        if (NULL != pWifiItem)
        {
            pWifiItem->SetNoteDisplayAndType(LANG_TRANSLATE(""), INT_NONE);
        }

        m_iPopWifiID = WIFI_SECTION_ID_INVAILED;

        emit RefreshSoftkey();
    }
}
