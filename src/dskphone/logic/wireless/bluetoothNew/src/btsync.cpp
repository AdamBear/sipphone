#ifdef IF_SUPPORT_BLUETOOTH
///////////////////////////////////////////////////////////
//  CBTSync.cpp
//  Implementation of the Class CBTSync
//  Created on:      12-十二月-2014 19:07:34
//  Original author: songk
///////////////////////////////////////////////////////////
#include "bluetooth_inc.h"
#include "xmlparser.hpp"
#include "commonapi/stringhelperapi.h"

#define  BT_DOWNLOAD_TIMER  1*90*1000 //下载超时暂定为1分钟

CBTSync::CBTSync()
    : m_ulHandle(BT_DEVICE_HANDLE_INVAILED)
{
    m_bConnected = false;
    m_eSyncState = BT_CONTACT_UNINIT;
    m_iContactSyncTimer = (UINT)&m_iContactSyncTimer;
}

CBTSync::~CBTSync()
{
}

//退出联动状态
bool CBTSync::ExitSync(unsigned long ulDeviceHandle)
{
    if (m_ulHandle != ulDeviceHandle)
    {
        WIRE_INFO("[%d] exit sync fail, current sync dev[%d]", ulDeviceHandle, m_ulHandle);
        return false;
    }
    if (!IsConnected())
    {
        return true;
    }

    WIRE_INFO("Exit Sync Mode");

    m_ulHandle = BT_DEVICE_HANDLE_INVAILED;
    //退出所有话路
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_MOBILE_STATECHANGE, 0, 0);
    /*_BTMobileManager.ExitMobileSync();*/

    if (IsInDownloadDir()
            || IsInParseDir())
    {
        StopDownload();
        //需要通知UI弹出提示
        etl_NotifyApp(FALSE, BLUETOOTH_MSG_DIR_DONE, OP_DOWNLOAD_RESULT, 2);
    }

    //通知联系人删除手机联系人分组
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_DIR_DONE, OP_DISCONNECT_RESULT, true);

    SetConnected(false);     //设置连接状态为FALSE

    return true;
}

//进入联动状态
bool CBTSync::InitSync(unsigned long ulDeviceHandle)
{
    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    // 更新mobile开关状态
    // 所有profile开关都为关, 底层默认打开开关, 并连接 --- 需求
    _BTData.UpdateProfilesActivity(ulDeviceHandle, PROFILE_MOBILE);
    if (!_BTData.GetProfileEnable(ulDeviceHandle, PROFILE_MOBILE))
    {
        WIRE_WARN("device mobile disabled");
        return false;
    }

    _BTManager.ResetAutoConnectIndex();
    _BTManager.ResetAutoConnectCount(BT_AUTOCONEECT_COUNT);
    //蓝牙手机才进入sync 模式
    if (!_ISBTPHONE(pDevice->eType))
    {
        SetDsskey(false, pDevice->eType);
        return false;
    }

    //初始化联动
    if (IsConnected())
    {
        return true;
    }

    WIRE_INFO("Init Sync Mode");

    m_ulHandle = ulDeviceHandle;

    _BTData.UpdateAutoConnectDevice(ulDeviceHandle);
    if (NULL != pDevice)
    {
        _BTDriver.GetMbProperty(pDevice->devHandle, m_mProperty);

        // 支持通话功能才有 mobile dsskey
        if (_BTManager.IsSupportFeature(BT_FEATURE_AUDIO))
        {
            SetDsskey(true, pDevice->eType);
        }

        //临时同步联系人
        if (pDevice->bSyncContact
                && _BTManager.IsSupportFeature(BT_FEATURE_CONTACT))
        {
            etl_NotifyApp(FALSE, BLUETOOTH_MSG_DIR_DONE, OP_CONNECT_RESULT, true);
            StartDownloadDirectory();
        }
    }

    SetConnected(true); //设置连接状态为TRUE
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_MOBILE_STATECHANGE, 0, 1);
    return true;
}

//是否为蓝牙联动消息
bool CBTSync::IsBtSyncMsg(unsigned long ulMsg)
{
    if (ulMsg == BT_AG_SCO_PLAY_STOP
            || ulMsg == BT_AG_SCO_PLAY_START
            || ulMsg == BT_AG_CALL_STATUS_CHANGED
            || ulMsg == BT_AG_INDICATOR_CHANGED
            || ulMsg == BT_PBA_DOWNLOAD_COMPLETE
            || ulMsg == BT_PBA_DOWNLOAD_FAIL)
    {
        return true;
    }
    return false;
}

//是否在下载联系人
bool CBTSync::IsInDownloadDir()
{
    return GetContactSyncState() == BT_CONTACT_DOWNLOADING;
}

//是否在解析联系人
bool CBTSync::IsInParseDir()
{
    return GetContactSyncState() == BT_CONTACT_PRASEING;
}

//处理联动消息
LRESULT CBTSync::OnSyncMsg(msgObject & msg)
{
    const btkit_msg_t * pMsg = (const btkit_msg_t *)msg.GetExtraData();
    if (NULL == pMsg)
    {
        return false;
    }

    bool bHandle = true;
    if (BT_DEV_PROFILE_CONNECTED == pMsg->bt_event)
    {
        InitSync(pMsg->bt_callback_param.pf_info.dev_hnd);
    }
    else if ((BT_DEV_PROFILE_CONNECT_FAIL == pMsg->bt_event
              || BT_DEV_PROFILE_DISCONNECTED == pMsg->bt_event))
    {
        ExitSync(pMsg->bt_callback_param.pf_info.dev_hnd);
    }
    else if (BT_AG_CALL_STATUS_CHANGED == pMsg->bt_event
             || BT_AG_SCO_PLAY_START == pMsg->bt_event
             || BT_AG_SCO_PLAY_STOP == pMsg->bt_event)
    {
        if (_BTManager.IsSupportFeature(BT_FEATURE_AUDIO))
        {
            //通话状态改变
            DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.dev_hnd);
            if (pDevice && pDevice->bMobileEnable)
            {
                etl_NotifyAppEx(FALSE, BLUETOOTH_MSG_MOBILE_STATECHANGE, 1, 1, sizeof(btkit_msg_t), pMsg);
            }
        }
    }
    else if (BT_AG_INDICATOR_CHANGED == pMsg->bt_event)
    {
        //手机信息改变
        UpdateMobileProperty(&pMsg->bt_callback_param.mb_cond);
    }
    else if (BT_PBA_DOWNLOAD_COMPLETE == pMsg->bt_event)
    {
        if (IsInDownloadDir())
        {
            unsigned long ulDevHandle = pMsg->bt_callback_param.pb_file.dev_hnd;

            //下载成功后需要停止下载
            WLAsyncHelper_BtAsync(BT_TASK_DOWNLOAD_CONTACT_STOP, ulDevHandle);

            // 设置状态为解析
            SetContactSyncState(BT_CONTACT_PRASEING);

            // 开始异步解析
            BTTaskParam sParam;
            sParam.strParam = pMsg->bt_callback_param.pb_file.file;
            WLAsyncHelper_BtAsync(BT_TASK_PRASECONTACT, ulDevHandle, &sParam);
        }
    }
    else if (BT_PBA_DOWNLOAD_FAIL == pMsg->bt_event)
    {
        ProcessContactFailed(true, pMsg->bt_callback_param.pb_err.code);
    }
    else
    {
        bHandle = false;
    }

    return bHandle;
}

//更新手机信息
void CBTSync::UpdateMobileProperty(const btkit_mb_cond_t * pProperty)
{
    if (NULL == pProperty)
    {
        return;
    }

    WIRE_INFO("before updatemobileproperty");

    _BTDriver.GetMbProperty(pProperty->dev_hnd, m_mProperty);
    _BTDriver.GetDeviceMac(pProperty->dev_hnd, m_mProperty.strMac);

    WIRE_INFO("after updatemobileproperty");

    m_mProperty.bRoam = (pProperty->in_roam == 1);
    m_mProperty.bService = (pProperty->in_service == 1);
    m_mProperty.iBattery = pProperty->battery_level;
    m_mProperty.iSignal = pProperty->signal_level;

    WIRE_INFO("Mobile[%d][%s], roam[%d], service[%d], battery[%d], signal[%d],number[%s]",
              pProperty->dev_hnd
              , m_mProperty.strMac.c_str()
              , m_mProperty.bRoam
              , m_mProperty.bService
              , m_mProperty.iBattery
              , m_mProperty.iSignal
              , m_mProperty.strNum.c_str());
}

//处理解析成功
void CBTSync::PraseContactSuccess(const yl::string & strFilePath)
{
    if (!IsInParseDir())
    {
        return;
    }

    SetContactSyncTimer(false);

    //处理完成
    if (strFilePath.empty())
    {
        //通知下载失败
        etl_NotifyApp(false, BLUETOOTH_MSG_DIR_DONE, OP_DOWNLOAD_RESULT, false);
        SetContactSyncState(BT_CONTACT_DOWNLOAD_FAILED);
        return;
    }

    WIRE_INFO("prase contact success, output[%s]", strFilePath.c_str());
    SetContactSyncState(BT_CONTACT_DOWNLOAD_SUCCESS);
    etl_NotifyAppEx(false, BLUETOOTH_MSG_DIR_DONE, OP_DOWNLOAD_RESULT, true, strFilePath.length() + 1,
                    (LPCVOID)strFilePath.c_str());
}

//处理联系人失败
void CBTSync::ProcessContactFailed(bool bNeedStop/* = false*/, const char * szErrorCode/* = NULL*/)
{
    if (!IsInDownloadDir()
            && !IsInParseDir())
    {
        return;
    }

    if (bNeedStop)
    {
        DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
        if (NULL != pDevice)
        {
            WLAsyncHelper_BtAsync(BT_TASK_DOWNLOAD_CONTACT_STOP, pDevice->devHandle);
        }
    }

    SetContactSyncTimer(false);
    SetContactSyncState(BT_CONTACT_DOWNLOAD_FAILED);
    etl_NotifyApp(false, BLUETOOTH_MSG_DIR_DONE, OP_DOWNLOAD_RESULT, false);
}

//处理定时器消息
bool CBTSync::ProcessTimer(int iTimerID)
{
    if (iTimerID == m_iContactSyncTimer)
    {
        //超时当做下载失败处理
        WIRE_INFO("download failed, overtime");
        ProcessContactFailed(true);
        return true;
    }

    return false;
}

//设置超时定时器
void CBTSync::SetContactSyncTimer(bool bEnable)
{
    WIRE_INFO("set contact sync timer[%s]", bEnable ? "Enable" : "Disable");

    if (bEnable)
    {
        timerSetThreadTimer(m_iContactSyncTimer, BT_DOWNLOAD_TIMER);
    }
    else
    {
        timerKillThreadTimer(m_iContactSyncTimer);
    }
}

/*
* 通知到Autoset模块
* AutoSet处理具体逻辑
*/
bool CBTSync::SetDsskey(bool bEnable, btkit_dev_type_t dType/* = BT_UNKNOWN_TYPE*/)
{
    static btkit_dev_type_t devOldType = BT_UNKNOWN_TYPE;
    bool bNeedAutoSet = true;

    //再次连接上手机时，不需要重新设置
    if (bEnable)
    {
        if (_ISBTPHONE(dType)
                && _ISBTPHONE(devOldType))
        {
            bNeedAutoSet = false;
        }
    }

    etl_NotifyApp(false, BLUETOOTH_MSG_SETDSSKEY, bEnable, bNeedAutoSet);
    WIRE_INFO("set bt dsskey[%d][%d][%d]", bEnable, dType, devOldType);

    devOldType = dType;
    return true;
}

//下载联系人
bool CBTSync::StartDownloadDirectory(int iType/* = 0x05*/, bool bPhoto/* = false*/)
{
    // 存在通话则不下载联系人
    // 配合Net修改：T5XS芯片较差, 两者同时存在容易产生问题
    if (talklogic_SessionExist())
    {
        WIRE_WARN("Talk exist, do not download directory.");
        return false;
    }

    //正在同步时，不再重复下载
    if (IsInDownloadDir()
            || IsInParseDir())
    {
        return true;
    }

    WIRE_INFO("start download contact,type[%d],photo[%d]", iType, bPhoto);

    //直接读取固定路径的文件，解析功能的测试
    if (configParser_GetConfigInt(kszBluetoothVCardTestEnable) == 1)
    {
        SetContactSyncState(BT_CONTACT_PRASEING);

        BTTaskParam sParam;
        sParam.strParam = configParser_GetConfigString(kszBluetoothVCardFileDir);
        WLAsyncHelper_BtAsync(BT_TASK_PRASECONTACT, 0, &sParam);
    }
    else
    {
        DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
        if (NULL == pDevice)
        {
            WIRE_ERR("start download mobile contact error[NULL == pDevice]");
            return false;
        }

        // 开始异步下载
        BTTaskParam sParam(iType, bPhoto);
        WLAsyncHelper_BtAsync(BT_TASK_DOWNLOAD_CONTACT_START, pDevice->devHandle, &sParam);

        SetContactSyncTimer(true);
        SetContactSyncState(BT_CONTACT_DOWNLOADING);
    }

    return true;
}

//取消下载联系人
bool CBTSync::StopDownload(bool bCancel /*= true*/)
{
    if (!IsInParseDir()
            && !IsInDownloadDir())
    {
        return true;
    }

    //下载过程中才需要通知手机取消
    if (IsInDownloadDir())
    {
        DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
        if (NULL == pDevice)
        {
            return false;
        }

        WIRE_INFO("stop download contact");
        WLAsyncHelper_BtAsync(BT_TASK_DOWNLOAD_CONTACT_STOP, pDevice->devHandle);
    }

    SetContactSyncTimer(false);
    SetContactSyncState(bCancel ? BT_CONTACT_DOWNLOAD_CANCEL : BT_CONTACT_UNINIT);
    return true;
}

void CBTSync::SetContactSyncState(ContactSyncState eState)
{
    WIRE_INFO("set contact sync state[%d] to [%d]", GetContactSyncState(), eState);

    m_eSyncState = eState;
}

bool CompareByDisplayName(const MobileContactInfo & lParam, const MobileContactInfo & rParam)
{
    return lParam.strContentName > rParam.strContentName;
}
//解析联系人
/************************************************************************/
/* 只会在子线程中调用，不要操作到主线程中的数据
* 输入 VCard3.0 文件路径
* 输出 XML 文件路径
*/
/************************************************************************/
bool CBTSync::PraseDirectory(yl::string & strXmlPath, const char * pVcardFilePath)
{
    if (NULL == pVcardFilePath)
    {
        return false;
    }

    WIRE_INFO("prase contact[%s]--beg", pVcardFilePath);

    LIST_MOBILE_CONTACT listCard;
    if (!VCard::exportToVCardlist(listCard, pVcardFilePath))
    {
        return false;
    }
    else if (listCard.size() == 0)
    {
        // 联系人空默认成功
        // http://10.2.1.199/Bug.php?BugID=110718
        WIRE_INFO("csv is null");
        return true;
    }

    WIRE_INFO("prase contact[%s] num[%d]--end", pVcardFilePath, listCard.size());

    FormatContact(listCard);
    /************************************************************************/
    /*
    <?xml version="1.0" encoding="utf-8"?>
    <root>
        <group displayname="Mobile Contact" ring="">
            <contact display_name=" " office_number="" mobile_number="" other_number="" line="" ring="" default_photo=" " selected_photo="0"/>
        </group>
    </root>
    */
    /************************************************************************/
    xml_document doc;
    xml_node root = doc.append_child("root");
    xml_node group = root.append_child("group");
    group.append_attribute("displayname", "Mobile Contact");
    group.append_attribute("ring", "");

    int iMaxCount = 3000;
#ifndef _T49
    iMaxCount = configParser_GetConfigInt(kszBluetoothMaxContactNum);
    iMaxCount = iMaxCount < 0 || iMaxCount > 1500 ? 1500 : iMaxCount;
#endif
    int iCount = 0;
    //加入调试手段
    if (configParser_GetConfigInt(kszBluetoothVCardTestEnable) == 1)
    {
        iMaxCount = configParser_GetConfigInt("BluetoothContactMaxNum");
    }

    WIRE_INFO("fill data-->beg size = [%d]", listCard.size());
    for (IT_MOBILE_CONTACT it = listCard.begin(); it != listCard.end(); ++it)
    {
        if (iCount >= iMaxCount)
        {
            break;
        }

        MobileContactInfo & itMobileContactInfo = (*it);

        //不允许空姓名联系人
        if (itMobileContactInfo.strContentName == "")
        {
            continue;
        }

        // 添加display name
        xml_node contact = group.append_child("contact");
        contact.append_attribute("display_name", itMobileContactInfo.strContentName.c_str());

        // 添加office_number、mobile_number、other_number
        YLVector<yl::string> vecNumberBuffer;
        for (MAP_NUMBER_TYPE_ITER iter = itMobileContactInfo.mapNumber.begin();
                iter != itMobileContactInfo.mapNumber.end();
                iter ++)
        {

            const typeNumType & eNumType = (typeNumType)iter->second;
            if (NUM_TYPE_OFFICE == eNumType
                    && strcmp("", contact.attribute("office_number").value()) == 0)
            {
                contact.append_attribute("office_number", iter->first.c_str());
            }
            else if (NUM_TYPE_PHONE == eNumType
                     && strcmp("", contact.attribute("mobile_number").value()) == 0)
            {
                contact.append_attribute("mobile_number", iter->first.c_str());
            }
            else if (NUM_TYPE_OTHER == eNumType && vecNumberBuffer.size() <= 3)
            {
                vecNumberBuffer.push_back(iter->first);
            }
        }

        while (vecNumberBuffer.size() != 0)
        {
            if (strcmp("", contact.attribute("other_number").value()) == 0)
            {
                contact.append_attribute("other_number", vecNumberBuffer[vecNumberBuffer.size() - 1].c_str());
            }
            else if (strcmp("", contact.attribute("office_number").value()) == 0)
            {
                contact.append_attribute("office_number", vecNumberBuffer[vecNumberBuffer.size() - 1].c_str());
            }
            else if (strcmp("", contact.attribute("mobile_number").value()) == 0)
            {
                contact.append_attribute("mobile_number", vecNumberBuffer[vecNumberBuffer.size() - 1].c_str());
            }
            vecNumberBuffer.removeAt(vecNumberBuffer.size() - 1);
        }

        //这里LINE写成auto,其他为默认
        contact.append_attribute("line", GetAccountID());
        contact.append_attribute("ring", "");
        contact.append_attribute("default_photo", "");
        contact.append_attribute("selected_photo", "");
        iCount ++;
    }

    WIRE_INFO("fill data-->end  icount = [%d]", iCount);

    //文件名
    strXmlPath = pVcardFilePath;
    int iPos = strXmlPath.find_last_of('.');
    if (iPos != yl::string::npos)
    {
        strXmlPath = strXmlPath.substr(0, iPos);
    }

    //重命名
    //规则：filename(nCount).xml
    int nCount = 1;
    while (pathFileExist((strXmlPath + ".xml").c_str()))
    {
        iPos = strXmlPath.find_last_of('(');
        if (iPos != yl::string::npos)
        {
            strXmlPath = strXmlPath.substr(0, iPos);
        }
        strXmlPath = commonAPI_FormatString("%s(%d)", strXmlPath.c_str(), nCount);

        nCount++;
    }

//#warning 测试过程中，保留源文件以供分析，稳定后需要去除

#if 0
    //删除VCard文件
    remove(pVcardFilePath);
#endif // 0

    strXmlPath += ".xml";
    WIRE_INFO("transfer to xml[%s]", strXmlPath.c_str());
    return doc.save_file(strXmlPath.c_str());
}

void CBTSync::FormatContact(LIST_MOBILE_CONTACT & listContact)
{
    WIRE_INFO("sort list--beg");
    Sort(listContact, CompareByDisplayName);
    WIRE_INFO("sort list--end");

    WIRE_INFO("ConvertListData --->beg");
    IT_MOBILE_CONTACT itBeg = listContact.begin();
    yl::string strName = "";
    int iCount = 1;
    for (; itBeg != listContact.end();
            ++ itBeg)
    {
        MobileContactInfo & mContact = (*itBeg);

        //去掉号码中的横线和空格
        for (MAP_NUMBER_TYPE_ITER iter = mContact.mapNumber.begin();
                iter != mContact.mapNumber.end();
                iter ++)
        {
            yl::string & strNumber = iter->first;
            strNumber = commonAPI_replaceSubStringByTag(strNumber, " ", "");
            strNumber = commonAPI_replaceSubStringByTag(strNumber, "-", "");
        }

        //重命名，已经排好序了，名字相同的都会在附近
        if (mContact.strContentName == strName)
        {
            mContact.strContentName = commonAPI_FormatString("%s(%d)", mContact.strContentName.c_str(), iCount);
            iCount ++;
        }
        else
        {
            strName = mContact.strContentName;
            iCount = 1;
        }
    }
    WIRE_INFO("ConvertListData --->end");
}

#endif // IF_SUPPORT_BLUETOOTH

