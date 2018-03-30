#ifdef IF_BT_TEST_LOGIC
#include "bttestdriverdata.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include <time.h>
#include <stdlib.h>

CBTTestData::CBTTestData(void)
{
}

CBTTestData::~CBTTestData(void)
{
}

//初始化数据
void CBTTestData::Init()
{
    //配种
    srand((unsigned)time(NULL) * 10) ;

    //初始化适配器信息
    strcpy(m_dongleProperty.bt_addr, "DD:DD:DD:DD:DD:DD");
    m_dongleProperty.discoverable = 1;
    m_dongleProperty.discovering = 0;
    strcpy(m_dongleProperty.name, "TestDongleName");
    m_dongleProperty.powered = 0;

    bttest_dev_t dev;
    bttest_headset_t headset;
    bttest_mb_t mb;

    //初始化BTTEST_MAX_DEVICE个设备
    yl::string strMac;
    for (int i = 0; i < BTTEST_MAX_DEVICE; ++i)
    {
        dev.dev_hnd = i + 10086;

        dev.state = WIRE_DEVICE_INIT_STATE;

        strMac = commonAPI_FormatString("%d", dev.dev_hnd);
        strncpy(dev.dev_property.bt_dev_addr, strMac.c_str(), BT_ADDR_STR_LEN);
        dev.dev_property.bt_dev_addr[BT_ADDR_STR_LEN - 1] = '\0';

        dev.dev_property.bt_class = 0;
        dev.dev_property.paired = 0;
        dev.dev_property.trusted = 1;
        dev.dev_property.blocked = 0;
        dev.dev_property.connected = 0;
        dev.bPower = 1;

        if (i <= 10
                || (i < 50 && i > 25))
        {
            sprintf(dev.dev_property.name, "PhoneDeviceName%d", i);
            sprintf(dev.dev_property.alias, "PhoneDeviceAlias%d", i);

            dev.dev_type = (i <= 10) ? BT_PHONE : BT_COMPUTER;

            mb.dev_hnd = dev.dev_hnd;
            sprintf(mb.mb_property.local_phonenum,
                    "%d%d%d%d%d%d%d%d%d%d%d",
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10,
                    GetRandNumber() % 10);

            mb.mb_property.call = 0;
            mb.mb_property.call_hold = 0;
            mb.mb_property.cond.battery_level = 3;
            mb.mb_property.cond.in_roam = 0;
            mb.mb_property.cond.in_service = 1;
            mb.mb_property.cond.signal_level = 4;

            m_listMb.push_back(mb);
        }
        else if (i < 25
                 && i > 10)
        {
            sprintf(dev.dev_property.name, "HeadSetDeviceName%d", i);
            sprintf(dev.dev_property.alias, "HeadSetDeviceAlias%d", i);
            dev.dev_type = BT_HEADSET;

            headset.dev_hnd = dev.dev_hnd;
            headset.headset_property.connected = 0;
            headset.headset_property.mic_gain = 5;
            headset.headset_property.playing = 0;
            headset.headset_property.spk_gain = 5;

            m_listHeadset.push_back(headset);
        }
        else
        {
            sprintf(dev.dev_property.name, "UnknowDeviceName%d", i);
            sprintf(dev.dev_property.alias, "UnknowDeviceAlias%d", i);
            dev.dev_type = BT_UNKNOWN_TYPE;
        }

        m_listDev.push_back(dev);
    }
}

//获取适配器属性
bool CBTTestData::GetDongleProperty(btkit_property_t * dongleProperty)
{
    if (NULL == dongleProperty)
    {
        return false;
    }

    memcpy(dongleProperty, &m_dongleProperty, sizeof(btkit_property_t));
    return true;
}

//设置适配器名称
bool CBTTestData::SetDongleName(const char * szName)
{
    if (NULL == szName
            || strlen(szName) > BT_MAX_NAME_LEN)
    {
        return false;
    }

    strcpy(m_dongleProperty.name, szName);
    return true;
}

//获取设备类型
btkit_dev_type_t CBTTestData::GetDeviceType(btkit_dev_instance_t dev_hnd)
{
    bttest_dev_t * pdev = GetDeviceInfo(dev_hnd);
    if (NULL == pdev)
    {
        return BT_UNKNOWN_TYPE;
    }

    return pdev->dev_type;
}

//获取设备信息
bttest_dev_t * CBTTestData::GetDeviceInfo(btkit_dev_instance_t dev_hnd)
{
    for (list_dev::iterator itBeg = m_listDev.begin(); itBeg != m_listDev.end(); ++itBeg)
    {
        if (itBeg->dev_hnd == dev_hnd)
        {
            return &(*itBeg);
        }
    }

    return NULL;
}

bttest_dev_t * CBTTestData::GetDeviceInfo(WIRELESS_DEVICE_STATE eState)
{
    for (list_dev::iterator itBeg = m_listDev.begin(); itBeg != m_listDev.end(); ++itBeg)
    {
        if (itBeg->state == eState)
        {
            return &(*itBeg);
        }
    }

    return NULL;
}

bool CBTTestData::GetDeviceList(list_dev & listDevice)
{
    listDevice = m_listDev;
    return true;
}

//获取手机设备指针
bttest_mb_t * CBTTestData::GetMbInfo(btkit_dev_instance_t dev_hnd)
{
    for (list_mb::iterator itBeg = m_listMb.begin(); itBeg != m_listMb.end(); ++itBeg)
    {
        bttest_mb_t & mbProperty = (*itBeg);

        if (mbProperty.dev_hnd == dev_hnd)
        {
            return &(*itBeg);
        }
    }

    return NULL;
}

//获取保存的设备数量
int CBTTestData::GetDeviceSum()
{
    int iCount = 0;
    for (list_dev::iterator itBeg = m_listDev.begin(); itBeg != m_listDev.end(); ++itBeg)
    {
        bttest_dev_t & devProperty = (*itBeg);

        if (devProperty.dev_property.paired != 0
                || devProperty.dev_property.connected != 0)
        {
            iCount ++;
        }
    }

    return iCount;
}

//获取设备句柄列表
int CBTTestData::GetDeviceHandleList(btkit_dev_instance_t * handleList)
{
    if (handleList == NULL)
    {
        return -1;
    }

    int iCount = 0;
    for (list_dev::iterator itBeg = m_listDev.begin(); itBeg != m_listDev.end(); ++itBeg)
    {
        bttest_dev_t & devProperty = (*itBeg);

        if (devProperty.dev_property.paired != 0
                || devProperty.dev_property.connected != 0)
        {
            handleList[iCount] = itBeg->dev_hnd;
            iCount ++;
        }
    }

    return iCount;
}

//下载联系人
bool CBTTestData::StartDownloadContact(btkit_dev_instance_t dev_hnd, btkit_pba_data_t * data,
                                       yl::string & strPath)
{
    bttest_dev_t * pDevice = GetDeviceInfo(dev_hnd);
    if (NULL == pDevice
            || !_ISBTPHONE(pDevice->dev_type))
    {
        return false;
    }

    //TODO 根据 data进行策略选择

    YLList<yl::string> listFile;

    commonAPI_getFilesInDirectoryByExt(&listFile, "/tmp/testvcf/", ".vcf", true);

    if (listFile.size() == 0)
    {
        return false;
    }

    int iIndex = GetRandNumber() % listFile.size();

    int i = 0;
    for (YLList<yl::string>::iterator itBeg = listFile.begin(); itBeg != listFile.end(); ++itBeg, ++i)
    {
        if (iIndex == i)
        {
            strPath = (*itBeg);
            return true;
        }
    }

    return false;
}

//添加一路通话
int CBTTestData::AddCall(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                         int iCallStatus, const yl::string & strNum)
{
    bttest_callstatus_t callStatus;
    callStatus.dev_hnd = dev_hnd;
    callStatus.callstatus.status = iCallStatus;
    strncpy(callStatus.callstatus.number, strNum.c_str(), BT_MAX_CALL_NUMBER_LEN);
    callStatus.callstatus.callid = GetCallID();

    listStatus.push_back(callStatus);
    return callStatus.callstatus.callid;
}

//更新一路通话
bool CBTTestData::UpdateCallStatus(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                                   int iCallID, bttest_callstatus_t & btCallStatus)
{
    bttest_callstatus_t * pCallStatus = GetCallInfo(listStatus, dev_hnd, iCallID);
    if (NULL == pCallStatus)
    {
        return false;
    }

    pCallStatus->callstatus = btCallStatus.callstatus;
    return true;
}

//删除一路通话信息
bool CBTTestData::DeleteCall(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                             int iCallID)
{
    for (list_callstatus::iterator itBeg = listStatus.begin(); itBeg != listStatus.end(); ++itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);
        if (callStatus.dev_hnd == dev_hnd
                && iCallID == callStatus.callstatus.callid)
        {
            itBeg = listStatus.erase(itBeg);
            return true;
        }
    }

    return false;
}

//删除所有通话
void CBTTestData::DeleteAllCall(list_callstatus & listStatus)
{
    listStatus.clear();
}

//获取一路通话信息
bttest_callstatus_t * CBTTestData::GetCallInfo(list_callstatus & listStatus,
        btkit_dev_instance_t dev_hnd, int iCallID)
{
    for (list_callstatus::iterator itBeg = listStatus.begin(); itBeg != listStatus.end(); ++itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);

        if (callStatus.dev_hnd == dev_hnd
                && callStatus.callstatus.callid == iCallID)
        {
            return &(*itBeg);
        }
    }

    return NULL;
}

bool CBTTestData::GetCallInfoByStatus(list_callstatus & listStatus, int iStatus,
                                      list_callstatus & callList)
{
    for (list_callstatus::iterator itBeg = listStatus.begin(); itBeg != listStatus.end(); ++itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);

        if (callStatus.callstatus.status == iStatus)
        {
            callList.push_back(*itBeg);
        }
    }

    return callList.size() != 0;
}

//获取通话列表
bool CBTTestData::GetCallInfo(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                              list_callstatus & callList)
{
    for (list_callstatus::iterator itBeg = listStatus.begin(); itBeg != listStatus.end(); ++itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);

        if (callStatus.dev_hnd == dev_hnd)
        {
            callList.push_back(*itBeg);
        }
    }

    return callList.size();
}

//获取通话数量
int CBTTestData::GetCallCount(list_callstatus & listStatus,
                              btkit_dev_instance_t dev_hnd /* = BT_DEVICE_HANDLE_INVAILED */)
{
    int iCount = 0;
    for (list_callstatus::iterator itBeg = listStatus.begin(); itBeg != listStatus.end(); ++itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);

        if (dev_hnd == callStatus.dev_hnd
                || BT_DEVICE_HANDLE_INVAILED == dev_hnd)
        {
            iCount ++;
        }
    }

    return iCount;
}

//随机获取一个设备句柄
btkit_dev_instance_t CBTTestData::GetRandDeviceHandle()
{
    int iIndex = GetRandNumber() % BTTEST_MAX_DEVICE;

    return m_listDev.at(iIndex).dev_hnd;
}

//随机数生成
int CBTTestData::GetRandNumber()
{
    return rand();
}

int CBTTestData::GetCallID()
{
    static int iCallID = 0;

    if (iCallID >= 30000)
    {
        iCallID = 0;
    }

    return ++iCallID;
}
#endif // IF_BT_TEST_LOGIC
