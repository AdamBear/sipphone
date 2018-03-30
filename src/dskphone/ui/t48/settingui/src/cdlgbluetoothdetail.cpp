#include "cdlgbluetoothdetail.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/qdatatimer.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "mainwnd/mainwnd.h"
#include "uimanager/moduimanager.h"
#include "messagebox/modmessagebox.h"
#include "baseui/t4xpicpath.h"
#include "setting/include/common.h"
#include "include/extended/bluetoothmessagedefine.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "wireless/common/wirelessdefines.h"


#include "include/uicommon.h"

namespace
{
#define LINEEDIT_MAX_LENGTH         32

#define COMPLIE
// popupBoxID
#define PopupBoxID_CancelConnect    80
#define PopupBoxID_CancelDisconnect 81
#define PopupBoxID_Save             82
}

CDlgBluetoothDetail::CDlgBluetoothDetail(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_eDetailType(BDT_Local)
    , m_bAsynchReq(false)
    , m_eOpCode(OP_NONE)
    , m_pEditDeviceName(NULL)
    , m_pEditMACAddr(NULL)
    , m_pCmbBoxAutoConnect(NULL)
{
    SetWindowType(eWT_SubPage);
    //初始化控件等
    InitData();
}

CDlgBluetoothDetail::~CDlgBluetoothDetail()
{
}

//初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgBluetoothDetail::InitData()
{
    //初始化
    m_pCmbBoxAutoConnect = new QComboBox(this);
    m_pEditDeviceName = new QLineEdit(this);
    m_pEditMACAddr = new QLineEdit(this);

    //指针判空
    if (m_pCmbBoxAutoConnect == NULL || m_pEditDeviceName == NULL
            || m_pEditMACAddr == NULL)
    {
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxAutoConnect, Qt::black);

    m_pCmbBoxAutoConnect->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pCmbBoxAutoConnect->addItem(LANG_TRANSLATE(TRID_ON), true);

    //将autoconnect隐藏，本地信息不需要显示autoconnect
    m_pCmbBoxAutoConnect->hide();

    qLineEditUtility::SetLineEditStyle(m_pEditDeviceName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditMACAddr, Qt::black);

    m_pEditMACAddr->setEnabled(false);
    //安装输入法
    qInstallIME(m_pEditDeviceName, IME_Name);

    //设置输入框输入的最大长度
    m_pEditDeviceName->setMaxLength(LINEEDIT_MAX_LENGTH);
    m_pEditMACAddr->setMaxLength(LINEEDIT_MAX_LENGTH);

    //安装事件过滤器
    m_pCmbBoxAutoConnect->installEventFilter(this);
    m_pEditDeviceName->installEventFilter(this);
    m_pEditMACAddr->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//页面自己加载数据,用于设置本地详情数据
bool CDlgBluetoothDetail::LoadPageData()
{
    if (m_eDetailType == BDT_PairedDevice)
    {
        //非查看本地数据时，不做操作
        return true;
    }

    m_strDevMac.clear();

    Bluetooth_GetDeviceInfo(m_DeviceData);

    m_eDetailType = BDT_Local;

    //设置数据到页面上
    m_pEditDeviceName->setText(toQString(m_DeviceData.strName));
#ifdef BLUETOOTH_USED_OLD_VERSION
    m_pEditMACAddr->setText(toQString(m_DeviceData.szMAC));
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
    m_pEditMACAddr->setText(toQString(m_DeviceData.strMac));
#endif

    MakeReadyForRelayout();
    //设置要显示的控件，调用此函数将显示本地信息，不显示autoconnect项
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_DEVICE_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditDeviceName;
    m_vecWidgetPair.push_back(qPair);



    strLab = LANG_TRANSLATE(TRID_MAC_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pEditMACAddr;
    m_vecWidgetPair.push_back(qPair);

    //添加页面控件
    AddWidget(m_vecWidgetPair);

    this->UpdateWnd();

    SETTINGUI_INFO("CDlgBluetoothDetail::LoadPageData");

    return true;
}

//外部设置数据，用于设置已匹配设备的详情信息
bool CDlgBluetoothDetail::SetPageData(QString * pStrMac)
{
    SETTINGUI_INFO("CDlgBluetoothDetail::SetPageData");
    //判断数据的合法性
    if (NULL == pStrMac)
    {
        m_strDevMac.clear();
        return false;
    }

    m_strDevMac = *pStrMac;

    Bluetooth_GetBluetoothInfoByMac(m_strDevMac.toUtf8().data(), m_DeviceData);

    m_eDetailType = BDT_PairedDevice;

    //设置数据到页面上
    m_pEditDeviceName->setText(toQString(m_DeviceData.strName));
#ifdef BLUETOOTH_USED_OLD_VERSION
    m_pEditMACAddr->setText(toQString(m_DeviceData.szMAC));
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
    m_pEditMACAddr->setText(toQString(m_DeviceData.strMac));
#endif

    for (int i = 0; i < m_pCmbBoxAutoConnect->count(); ++i)
    {
        if (m_pCmbBoxAutoConnect->itemData(i).value<bool>() == m_DeviceData.bAutoConnect)
        {
            m_pCmbBoxAutoConnect->setCurrentIndex(i);
            break;
        }
    }

    //先清空
    m_vecWidgetPair.clear();
    //设置要显示的标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_DEVICE_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditDeviceName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MAC_ADDRESS);
    qPair.first = strLab;
    qPair.second = m_pEditMACAddr;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AUTO_CONNECT);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxAutoConnect;
    m_vecWidgetPair.push_back(qPair);

    //添加页面控件
    AddWidget(m_vecWidgetPair);

    this->UpdateWnd();

    return true;
}


//数据状态是否改变
bool CDlgBluetoothDetail::IsStatusChanged()
{
    DeviceInfo pageData;
    GetCurrentPageData(pageData);
    bool bchanged = pageData.strName != m_DeviceData.strName;
    //查看已配对蓝牙信息时才比较autoconnect
    if (m_eDetailType == BDT_PairedDevice && false == bchanged)
    {
        bchanged = bchanged || pageData.bAutoConnect != m_DeviceData.bAutoConnect;
    }

    return bchanged;
}

//取得当前页面数据
void CDlgBluetoothDetail::GetCurrentPageData(DeviceInfo & currentPageData)
{
    if (m_pEditDeviceName == NULL
            || m_pEditMACAddr == NULL
            || m_pCmbBoxAutoConnect == NULL)
    {
        return;
    }
    //处理预编辑字符
    ReturnAllEdit();
    //设置数据
    currentPageData.strName = m_pEditDeviceName->text().toUtf8().data();
#ifdef BLUETOOTH_USED_OLD_VERSION
    strcpy(currentPageData.szMAC, m_pEditMACAddr->text().toUtf8().data());
    currentPageData.bAutoConnect = m_pCmbBoxAutoConnect->itemData(
                                       m_pCmbBoxAutoConnect->currentIndex()).value<bool>();
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
    currentPageData.strMac = m_pEditMACAddr->text().toUtf8().data();
#endif
}

void CDlgBluetoothDetail::OnBtnIMEClick()
{
    //点击softbutton时，会进行输入法的切换，softbutton显示名也会随之变化
    _MainWND.SwitchNextIMEName(this->focusWidget());
}

void CDlgBluetoothDetail::OnBtnDeleteClick()
{
    QWidget * pFocusWidget = this->focusWidget();
    if (NULL == pFocusWidget)
    {
        return;
    }

    if (pFocusWidget->inherits("QLineEdit")
            || pFocusWidget->inherits("QTextEdit"))
    {
        //点击删除时，调用此函数，对当前焦点的文本输入框的内容进行退格删除
        qOnBackspaceClicked(this);
    }
    else if (pFocusWidget->inherits("QComboBox"))
    {
        QApplication::sendEvent(pFocusWidget, new QKeyEvent(QKeyEvent::KeyPress, PHONE_KEY_RIGHT,
                                Qt::NoModifier));
    }
}

void CDlgBluetoothDetail::UpdateSoftkeyFN4(int iIndex /*= -1*/)
{
    if (iIndex == -1)
    {
        if (m_pGeneralFrame != NULL)
        {
            iIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        }
    }

    if (m_eDetailType == BDT_PairedDevice)
    {
        //Auto connect项
        if (iIndex == 2)
        {
            m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
            m_SoftkeyData[3].m_strSoftkeyAction = "Save";
        }
        else
        {
            DeviceInfo deviceInfo;
            Bluetooth_GetBluetoothInfoByMac(m_strDevMac.toUtf8().data(), deviceInfo);
            SETTINGUI_INFO("OnSelectedItemChancged dev_state(%d)", deviceInfo.eState);
#ifdef BLUETOOTH_USED_OLD_VERSION
            if (deviceInfo.eState == REMOTEDEV_CONNECTED_STATE)
#endif
#ifdef BLUETOOTH_USED_NEW_VERSION
                if (WIRE_DEVICE_CONNECTED_STATE == deviceInfo.eState)
#endif
                {
                    m_SoftkeyData[3].m_strSoftkeyAction = "Disconnect";
                    m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DISCONNECT);
                    m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_DISCONNECT;
                }
                else
                {
                    m_SoftkeyData[3].m_strSoftkeyAction = "Connect";
                    m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CONNECT);
                    m_SoftkeyData[3].m_strSoftkeyAction = PIC_SOFTKEY_CONNECT;
                }
        }
    }//end if (m_eDetailType == BDT_PairedDevice)
    else
    {
        m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
        m_SoftkeyData[3].m_strSoftkeyAction = "Save";
    }
}

bool CDlgBluetoothDetail::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    DeviceInfo pageData;
    GetCurrentPageData(pageData);
    if (pageData.strName.empty())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_EDIT_DEVICE_NAME_CANNOT_BE_EMPTY);
        objErrorContent.pLineEdit = m_pEditDeviceName;
        return false;
    }
    return true;
}

//保存数据
bool CDlgBluetoothDetail::SavePageData()
{
    if (!IsStatusChanged())
    {
        return true;
    }

    DeviceInfo pageData;
    GetCurrentPageData(pageData);

    //调用逻辑成接口保存
    if (m_eDetailType == BDT_Local)
    {
        // 更新m_DeviceData
        m_DeviceData.strName = pageData.strName;

        return Bluetooth_SetDeviceName(pageData.strName.c_str());
    }
    else if (m_eDetailType == BDT_PairedDevice)
    {
        Bluetooth_SetAutoConnect(m_strDevMac.toUtf8().data(), pageData.bAutoConnect);
        return true;
    }

    return false;
}

//页面的选中行改变时的处理
void CDlgBluetoothDetail::OnCurrentIndexChange(int iIndex)
{
    if (m_eDetailType == BDT_PairedDevice)
    {
        UpdateSoftkeyFN4(iIndex);

        // 刷新softkey
        _MainWND.DrawSoftkey(this);

        // 刷新IME信息
        _MainWND.DisplayIMESoftkey(focusWidget());
    }
}


// 初始化数据
void CDlgBluetoothDetail::Init()
{
    //默认为查看本地信息
    m_eDetailType = BDT_Local;

    m_eOpCode = OP_NONE;
}

// 释放数据
void CDlgBluetoothDetail::Uninit()
{
    //还原控件属性
    if (NULL != m_pEditMACAddr)
    {
        m_pEditMACAddr->setEnabled(false);
    }
}

void CDlgBluetoothDetail::OnConnect()
{
    SETTINGUI_INFO("CDlgBluetoothDetail::OnConnect");
    if (Bluetooth_GetDeviceState() == DS_NO_DONGLE)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NO_BLUETOOTH_ADAPTER_FOUND)
                                    , MESSAGEBOX_Cancel, SHORT_TIME);
        return;
    }

    if (m_strDevMac.isEmpty())
    {
        return;
    }

    if (!Bluetooth_ConnectDevice(m_strDevMac.toUtf8().data()))
    {
        //失败
        //弹出Connect Failed提示框
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_FAILED)
                                    , MESSAGEBOX_Cancel, SHORT_TIME);
    }
    else
    {
        m_eOpCode = OP_CONNECT_RESULT;
        m_bAsynchReq = true;
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_OLD_CONNECTING)
                                    , MESSAGEBOX_Cancel, -1, "", "", PopupBoxID_CancelConnect);
    }
}

void CDlgBluetoothDetail::OnDisconnect()
{
    if (Bluetooth_GetDeviceState() == DS_NO_DONGLE)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NO_BLUETOOTH_ADAPTER_FOUND)
                                    , MESSAGEBOX_Cancel, SHORT_TIME);
        return;
    }

    if (!Bluetooth_DisconnectDevice(m_strDevMac.toUtf8().data()))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_DISCONNECT_FAILED)
                                    , MESSAGEBOX_Cancel, SHORT_TIME);
        return;
    }

    m_bAsynchReq = true;
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_DISCONNECTING)
                                , MESSAGEBOX_Cancel, -1, "", "", PopupBoxID_CancelDisconnect);
}

void CDlgBluetoothDetail::ConnectStatusChanged()
{
    if (m_bAsynchReq)
    {
        m_bAsynchReq = false;
        MessageBox_RemoveMessageBox(this);
#ifndef COMPLIE
        MsgBox_RemoveTopPopupBox(PopupBox_OK);
#endif

        //更新Softkey
        if (m_pGeneralFrame != NULL)
        {
            OnCurrentIndexChange(m_pGeneralFrame->GetFocusItemDataIndex());
        }
    }
}

void CDlgBluetoothDetail::AcceptMessageBox()
{
    if (m_bAsynchReq)
    {
        MessageBox_RemoveMessageBox(this);
#ifndef COMPLIE
        MsgBox_RemoveTopPopupBox(PopupBox_OK);
#endif
        m_bAsynchReq = false;
    }
}

void CDlgBluetoothDetail::OnETLMsg(int wParam, int lParam)
{
    //处理操作结果

    //连接操作
    if (wParam == OP_CONNECT_RESULT)
    {
        m_eOpCode = OP_NONE;

        //失败
        if (lParam == 0)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_FAILED)
                                        , MESSAGEBOX_Cancel, SHORT_TIME);
        }
    }
    else if (wParam == OP_DISCONNECT_RESULT)
    {
        m_eOpCode = OP_NONE;

        //失败
        if (lParam == 0)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_DISCONNECT_FAILED)
                                        , MESSAGEBOX_Cancel, SHORT_TIME);
        }
    }

    if (m_pGeneralFrame != NULL)
    {
        OnCurrentIndexChange(m_pGeneralFrame->GetFocusItemDataIndex());
    }
}

// 设置子页面的SoftKey数据
bool CDlgBluetoothDetail::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    SETTINGUI_INFO("CDlgBluetoothDetail::GetSoftkeyData");
    //本地详情界面不需要设置softkey
    if (m_eDetailType == BDT_PairedDevice)
    {
        //查看已连接设备：只设置第四个softkey，其它softkey使用默认值
        objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_CONNECT);
        objWndData[3].m_strSoftkeyAction = "Connect";
        objWndData[3].m_strIconPath = PIC_SOFTKEY_CONNECT;
        SETTINGUI_INFO("CDlgBluetoothDetail::GetSoftkeyData : BDT_PairedDevice");
    }

    return true;
}

//softkey按下后的响应
bool CDlgBluetoothDetail::OnAction(const QString & strAction)
{
    if (strAction == "Connect")
    {
        //连接当前设备
        OnConnect();
    }
    else if (strAction == "Disconnect")
    {
        OnDisconnect();
    }
    else if (strAction == "Back")
    {
        if (m_eDetailType == BDT_PairedDevice)
        {
            SavePageData();
        }

        return false;
    }
    else if (strAction == IME_ACTION)
    {
        OnBtnIMEClick();
    }
    else if (strAction == IME_DEL_ACTION)
    {
        OnBtnDeleteClick();
    }
    else if (strAction == "Save")
    {
        SavePageData();
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_DISCONNECTING)
                                    , MESSAGEBOX_NOTE, SHORT_TIME, "", "", PopupBoxID_Save);
    }
    else
    {
        //其它情况给父类处理
        return false;
    }

    return true;
}

//外部设置数据，用于设置一匹配设备的详情信息
void CDlgBluetoothDetail::SetData(void * pData)
{
    LoadPageData();

    m_SoftkeyData[0].m_strSoftkeyAction = "Back";
    m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_BACK);

    m_SoftkeyData[1].m_strSoftkeyAction = IME_ACTION;
    m_SoftkeyData[2].m_strSoftkeyAction = IME_DEL_ACTION;

    UpdateSoftkeyFN4();
}

void CDlgBluetoothDetail::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case PopupBoxID_CancelConnect:
        {
            if (MessageBox_Cancel == nResult)
            {
                m_eOpCode = OP_NONE;
                m_bAsynchReq = false;
            }
        }
        break;
    case PopupBoxID_CancelDisconnect:
        {
            if (MessageBox_Cancel == nResult)
            {
                m_bAsynchReq = false;
            }
        }
        break;
    case PopupBoxID_Save:
        {
            UIManager_PutPrivateDlg(this);
        }
        break;
    default:
        break;
    }
}
