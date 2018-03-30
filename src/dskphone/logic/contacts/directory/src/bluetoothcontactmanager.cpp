#ifdef IF_BT_SUPPORT_PHONE
#include "bluetoothcontactmanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "directorymessagedefine.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "basemethod.h"
#include "commonunits/modcommonunits.h"

// 定时器
LRESULT BlueToothTimeOut(msgObject & refObj)
{
    g_pBlueToothDirManager->ProcessTimerOut(refObj.wParam);

    return 0;
}


CBlueToothContactManager * CBlueToothContactManager::GetInstance()
{
    static CBlueToothContactManager instance;
    return &instance;
}

CBlueToothContactManager::CBlueToothContactManager()
{
    m_eDirType = BLUETOOTH_DIR;

    //暂定超时时间15s
    m_nTimeOut = 15 * 1000;

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, BlueToothTimeOut);
}

CBlueToothContactManager::~CBlueToothContactManager()
{

}

void CBlueToothContactManager::ProcessTimerOut(unsigned int uTimerID)
{
    if (uTimerID == (UINT)&m_nTimeOut)
    {
        timerKillThreadTimer((UINT)&m_nTimeOut);

        etl_NotifyApp(false, BLUETOOTH_CONTACT_DOWNLOAD_RESULT, 0, 0);

        DIR_WARN("BlueTooth Contact Update timeout!");
    }
}

bool CBlueToothContactManager::IsNeedShowAllGroup()
{
    return false;
}

bool CBlueToothContactManager::ReLoadContact(const yl::string & strFile, bool suc)
{
    DIR_INFO("CBlueToothContactManager::ReLoadContact file[%s]", strFile.c_str());

    if (!suc)
    {
        return false;
    }

    MyFinal();

    MyInit();

    m_MapNumToName.clear();

    xml_document doc;
    doc.load_file(strFile.c_str());

    xml_node rootNode = doc.child(kszXMLRootNode);
    if (!rootNode)
    {
        remove(strFile.c_str());
        return false;
    }

    xml_node groupNode = rootNode.child(kszGXmlNodeGroup);

    if (groupNode.empty())
    {
        remove(strFile.c_str());
        return false;
    }

    LocalDirGroupRecord * pNewGroup = new LocalDirGroupRecord();

    if (pNewGroup != NULL)
    {
        pNewGroup->ParseFromXMLNode(groupNode);

        InsertMap(pNewGroup, kszBluethoothGroupId);

        AddIdToMenuShowList(kszBluethoothGroupId);

        for (xml_node nodeContact = groupNode.child(kszCXmlNodeContact);
                !nodeContact.empty(); nodeContact = nodeContact.next_sibling(kszCXmlNodeContact))
        {
            BlueToothDirContactRecord * pNew = new BlueToothDirContactRecord();

            if (pNew == NULL)
            {
                DIR_INFO("Error new LocalDirContactRecord");
                remove(strFile.c_str());
                return false;
            }
            //赋值父ID
            pNew->m_nParentId = kszBluethoothGroupId;

            pNew->ParseFromXMLNode(nodeContact);

            AddContact(pNew, kszBluethoothGroupId, configParser_GetConfigInt(kszNotLocalDirSortEnable) == 1);
        }
    }

    remove(strFile.c_str());
    DIR_INFO("CBlueToothContactManager::ReLoadContact,Send Reload mesg to UI");

    return true;
}

void CBlueToothContactManager::ProcessDisconnect()
{
    m_MapNumToName.clear();
    MyFinal();
    etl_NotifyApp(false, BLUETOOTH_CONTACT_RELOAD, 0, 0);
}

void CBlueToothContactManager::ProcessConnected()
{
    MyInit();

    AddIdToMenuShowList(kszBluethoothGroupId);

    etl_NotifyApp(false, BLUETOOTH_CONTACT_RELOAD, 1, 0);
}

DIRECTORY_RET_CODE CBlueToothContactManager::Update()
{
    //开启超时计时器
    //timerSetThreadTimer((UINT)&m_nTimeOut, m_nTimeOut);
    //调用蓝牙接口
    return Bluetooth_StartDownloadMobileContact() ? DIR_RET_SUCC : DIR_RET_FAIL;
}

void CBlueToothContactManager::CancelUpdate()
{
    //调用蓝牙接口
    Bluetooth_StopDownloadMobileContact();
}

//蓝牙联系人保存到本地时，需调用此接口，触发后台下载图片
bool CBlueToothContactManager::LoadContactPhoto()
{
    //to do
    //调用蓝牙接口

    return true;
}

FileDownloadStatus CBlueToothContactManager::GetDownloadStatus()
{
    FileDownloadStatus eStatus = FILE_DOWNLOAD_STATUS_NONE;

    ContactSyncState eState = BT_CONTACT_UNINIT;
    Bluetooth_GetMobileContactDownloadStatus(eState);

    if (eState == BT_CONTACT_DOWNLOADING
            || eState == BT_CONTACT_PRASEING)
    {
        eStatus = FILE_DOWNLOADING;
    }
    else if (eState == BT_CONTACT_DOWNLOAD_SUCCESS)
    {
        eStatus = FILE_DOWNLOADED_OK;
    }
    else if (eState == BT_CONTACT_DOWNLOAD_FAILED)
    {
        eStatus = FILE_DOWNLOAD_FAIL;
    }
    else if (eState == BT_CONTACT_DOWNLOAD_CANCEL)
    {
        eStatus = FILE_DOWNLOAD_CANCELED;
    }
    else if (eState == BT_CONTACT_UNINIT)
    {
        eStatus = FILE_DOWNLOAD_READY;
    }

    return eStatus;
}

void CBlueToothContactManager::InsertToNumMap(const yl::string & strNumber,
        const yl::string & strName)
{
    if (strNumber.empty())
    {
        return;
    }
    m_MapNumToName.put(strNumber, strName);
}

bool CBlueToothContactManager::GetContactNameByNumber(const yl::string & strNumber,
        yl::string & strName, bool bSync /* = true*/)
{
#if IF_BUG_38979
    if (!bSync)
    {
        return CBaseContactManger::GetContactNameByNumber(strNumber, strName, bSync);
    }
#endif
    MapNumberToName::iterator iter = m_MapNumToName.find(strNumber);

    if (iter != m_MapNumToName.end())
    {
        strName = iter->second;
        return true;
    }

    return false;
}

const yl::string & CBlueToothContactManager::GetContactNameByNumber(const yl::string & strNumber)
{
    MapNumberToName::iterator iter = m_MapNumToName.find(strNumber);

    if (iter != m_MapNumToName.end())
    {
        return iter->second;
    }

    return kszEmptyString;
}

const yl::string & CBlueToothContactManager::GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName/* = ""*/)
{
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {

        const LocalDirContactRecord * pContact = Base2LocalContact(GetBaseContactByNumber(strNumber, strName));

        if (pContact != NULL)
        {
            DIR_INFO("pContact->m_strDefaultPhoto[%s]", pContact->m_strDefaultPhoto.c_str());
            return pContact->m_strDefaultPhoto;
        }

    }

    return kszEmptyString;
}
#endif
