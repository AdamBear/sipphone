#if IS_COVER_TEST
#include "bluetooth_inc.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <ETLLib.hpp>
#include <netmsgtypedefine.h>
#include <configiddefine.h>
#include <service_config.h>

/*************************************************
gcov 在程序退出时生成数据文件
进入msgkit系统的程序无法正常退出，怎么办？
gcov使用__gcov_flush函数刷新.gcna文件，程序中直接调用。
在源代码中直接声明该函数，C++代码中注意extern。
extern "C" void __gcov_flush(void);
在需要的地方调用。
***************************************************/
#if IS_COVER_TEST
extern "C" void __gcov_flush(void);
#endif

class EndExit : public testing::Environment
{
public:
    //该函数在所有用例执行前执行
    virtual void SetUp()
    {
    }
    //该函数在所有用例执行完后执行
    virtual void TearDown()
    {
#if IS_COVER_TEST
        //刷新gcov，生成gcna
        __gcov_flush();
        //删除所有设备
        Bluetooth_DeleteDevice();
#endif
    }
};


int main(int argc, char * argv[])
{
    //设置gtest环境
    //testing::GTEST_FLAG(output)= "xml:";
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new EndExit);
    ////进入msgkit消息系统
    //mkit_handle_t bluetoothTest = mkRegistThread(BT_TEST_THREAD, bluetooth_test_msg);
    //mkRunThread(bluetoothTest);
    //等待启动完成
    Bluetooth_Init();
    //驱动有延迟驱动时间
    sleep(5);
    return RUN_ALL_TESTS();

    return 0;
}

//清空设备信息
void EmptyList()
{
    _BTData.ClearDeviceList();
    _BTData.ClearScanDeviceList();
}

//构造设备信息
void ConstructDeviceInfo(unsigned long ulDeviceHandle, bool bAutoConnect,
                         WIRELESS_DEVICE_STATE eState, btkit_dev_type_t eType, DeviceInfo & devInfo)
{
    devInfo.devHandle = ulDeviceHandle;
    devInfo.strMac = commonAPI_FormatString("%d", ulDeviceHandle);
    devInfo.eState = eState;
    devInfo.eType = eType;
    devInfo.bAutoConnect = bAutoConnect;
}

//初始化一些设备
void InitSomeDevice(btkit_dev_type_t eType)
{
    for (int i = 0; i < 5; i++)
    {
        DeviceInfo devInfo;
        devInfo.devHandle = i + 10086;
        devInfo.strMac = commonAPI_FormatString("%d", devInfo.devHandle);
        ConstructDeviceInfo(devInfo.devHandle, true, WIRE_DEVICE_PAIRED_STATE, eType, devInfo);
        _BTData.AddDevice(devInfo);
    }
}

//获取设备位置
int GetDevicePos(yl::string strMac, UI_LIST_BT_DEVICE & listDevice)
{
    int i = 1;
    for (UI_LIST_BT_DEVICE::iterator it = listDevice.begin(); it != listDevice.end(); it++, i++)
    {
        if (strMac == (*it).strMac)
        {
            return i;
        }
    }
    return -1;
}

////只对接口做测试
//无需启动dsongkPhone.exx
//// 设置蓝牙是否可用
//void Bluetooth_SetEnable(bool bEnable = true);
TEST(BTTEST, SetEnable)
{
    Bluetooth_SetEnable(true);
    sleep(2);
    Bluetooth_SetEnable(false);
    sleep(3);
    EXPECT_FALSE(Bluetooth_IsEnable());

    Bluetooth_SetEnable(true);
    sleep(3);
    EXPECT_TRUE(Bluetooth_IsEnable());
}
// 扫描设备 (独占，Scan之后不能再做其他事情)
//bool Bluetooth_StartScanDevice();

TEST(BTTEST, StartScanDevice)
{
    Bluetooth_SetEnable(true);
    sleep(2);
    Bluetooth_StartScanDevice();
    sleep(2);
    EXPECT_EQ(DS_SCAN, Bluetooth_GetDeviceState());

    Bluetooth_SetEnable(false);
    sleep(2);
    Bluetooth_StartScanDevice();
    sleep(2);
    EXPECT_NE(DS_SCAN, Bluetooth_GetDeviceState());
}
//// 停止扫描设备
//bool Bluetooth_StopScanDevice();
TEST(BTTEST, StopScanDevice)
{
    Bluetooth_SetEnable(true);
    sleep(2);
    Bluetooth_StartScanDevice();
    sleep(2);
    Bluetooth_StopScanDevice();
    sleep(2);
    EXPECT_NE(DS_SCAN, Bluetooth_GetDeviceState());
    Bluetooth_SetEnable(false);
}
//// 连接指定序号的蓝牙
//bool Bluetooth_ConnectDevice(const char* pMac);
TEST(BTTEST, ConnectDevice)
{
    Bluetooth_SetEnable(true);

    EmptyList();
    DeviceInfo devInfo;
    unsigned long ulDeviceHandle = 10088;
    ConstructDeviceInfo(ulDeviceHandle, true, WIRE_DEVICE_PAIRED_STATE, BT_HEADSET, devInfo);
    _BTData.AddDevice(devInfo);

    //匹配
    unsigned long ulDeviceHandle1 = 10089;
    ConstructDeviceInfo(ulDeviceHandle1, false, WIRE_DEVICE_INIT_STATE, BT_PHONE, devInfo);
    _BTData.AddDevice(devInfo);

    Bluetooth_ConnectDevice("10089");
    CSessionBase * pNewSession = _BTManager.GetSessionByHandle(ulDeviceHandle1);
    ASSERT_TRUE(pNewSession);
    EXPECT_EQ(SESSION_CONNECTED_STATE, pNewSession->GetSessionState());

    //连接
    Bluetooth_ConnectDevice(devInfo.strMac.c_str());
    CSessionBase * pSession = _BTManager.GetSessionByHandle(ulDeviceHandle);
    ASSERT_TRUE(pSession);
    EXPECT_EQ(SESSION_CONNECTED_STATE, pSession->GetSessionState());


    Bluetooth_SetEnable(false);
}
//// 配对时回复pin
//bool Bluetooth_AnswerPin(const char* pMac, const char* pPin);
TEST(BTTEST, AnswerPin)
{
    Bluetooth_SetEnable(true);
    sleep(2);
    EmptyList();
    //添加一些设备
    InitSomeDevice(BT_HEADSET);

    Bluetooth_ConnectDevice("10086");
    sleep(2);
    Bluetooth_AnswerPin("10086", "0010");
    sleep(2);
    DeviceInfo devInfo1;
    Bluetooth_GetBluetoothInfoByMac("10086", devInfo1);
    EXPECT_EQ(WIRE_DEVICE_PAIRED_STATE, devInfo1.eState);

    Bluetooth_SetEnable(false);
}
//// 断开连接
//bool Bluetooth_DisconnectDevice(const char* pMac);
TEST(BTTEST, DisconnectDevice)
{
    Bluetooth_SetEnable(true);
    EmptyList();
    //添加两个设备

    InitSomeDevice(BT_HEADSET);

    Bluetooth_ConnectDevice("10086");
    sleep(2);
    Bluetooth_AnswerPin("10086", "0010");

    Bluetooth_DisconnectDevice("10086");


    CSessionBase * pSession = _BTManager.GetSessionByHandle(10086);

    EXPECT_TRUE(NULL == pSession);

    Bluetooth_SetEnable(false);
}
//// 设置设备名
//bool Bluetooth_SetDeviceName(const char* pName);
TEST(BTTEST, SetDeviceName)
{
    Bluetooth_SetEnable(true);

    const char * strName = "test_btname";
    Bluetooth_SetDeviceName(strName);
    DeviceInfo devInfo;
    Bluetooth_GetDeviceInfo(devInfo);
    EXPECT_STREQ("test_btname", devInfo.strName.c_str());

    //可以为空字符串
    const char * strName1 = "";
    Bluetooth_SetDeviceName(strName);
    Bluetooth_GetDeviceInfo(devInfo);
    EXPECT_STREQ("", devInfo.strName.c_str());

    //不能超长
    Bluetooth_SetDeviceName(strName);
    const char * strName2 =
        "821938182391823819283918239819238918239182938129381092381982391823981290381023809182398120938";
    Bluetooth_SetDeviceName(strName2);
    Bluetooth_GetDeviceInfo(devInfo);
    EXPECT_STRNE(strName2, devInfo.strName.c_str());
    EXPECT_STREQ("", devInfo.strName.c_str());
}

//// 上下移动设备位置
//bool Bluetooth_AdjustDevicePos(const char* pMac, bool bUp);
TEST(BTTEST, AdjustDevicePos)
{
    Bluetooth_SetEnable(true);
    EmptyList();
    InitSomeDevice(BT_PHONE);
    UI_LIST_BT_DEVICE listDevice;

    //最顶部的上移
    yl::string strMac = "10086";
    Bluetooth_AdjustDevicePos(strMac.c_str(), true);
    Bluetooth_GetDeviceList(listDevice);
    int pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(1, pos);

    //最顶部下移
    Bluetooth_AdjustDevicePos(strMac.c_str(), false);
    sleep(2);
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(2, pos);

    //最底部下移
    EmptyList();
    InitSomeDevice(BT_PHONE);
    strMac = "10090";
    Bluetooth_AdjustDevicePos(strMac.c_str(), false);
    sleep(2);
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(listDevice.size(), pos);

    //最底部上移
    EmptyList();
    InitSomeDevice(BT_PHONE);
    strMac = "10090";
    Bluetooth_AdjustDevicePos(strMac.c_str(), true);
    sleep(2);
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    sleep(2);
    pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(listDevice.size() - 1, pos);

    //中间上移
    EmptyList();
    InitSomeDevice(BT_PHONE);
    strMac = "10087";
    Bluetooth_AdjustDevicePos(strMac.c_str(), true);
    sleep(2);
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(1, pos);

    //中间下移
    EmptyList();
    InitSomeDevice(BT_PHONE);
    strMac = "10087";
    Bluetooth_AdjustDevicePos(strMac.c_str(), false);
    sleep(2);
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    pos = GetDevicePos(strMac, listDevice);
    EXPECT_EQ(3, pos);
    EmptyList();
}

//// 设置是否自动连接
//void Bluetooth_SetAutoConnect(const char* pMac, bool bAutoConnect);
TEST(BTTEST, SetAutoConnect)
{
    Bluetooth_SetEnable(true);
    DeviceInfo devInfo;
    ConstructDeviceInfo(10086, false, WIRE_DEVICE_PAIRED_STATE, BT_HEADSET, devInfo);
    _BTData.AddDevice(devInfo);

    Bluetooth_SetAutoConnect("10086", true);
    DeviceInfo newDevInfo;
    Bluetooth_GetBluetoothInfoByMac("10086", newDevInfo);
    EXPECT_TRUE(newDevInfo.bAutoConnect);

    Bluetooth_SetAutoConnect("10086", false);
    Bluetooth_GetBluetoothInfoByMac("10086", newDevInfo);
    EXPECT_FALSE(newDevInfo.bAutoConnect);
}

TEST(BTTEST, DeleteDevice)
{
    Bluetooth_SetEnable(true);
    EmptyList();
    InitSomeDevice(BT_PHONE);

    yl::string strMac = "10086";
    Bluetooth_DeleteDevice(strMac.c_str());
    UI_LIST_BT_DEVICE listDevice;
    Bluetooth_GetDeviceList(listDevice);
    EXPECT_EQ(4, listDevice.size());

    strMac = "10087";
    Bluetooth_DeleteDevice(strMac.c_str());
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    EXPECT_EQ(3, listDevice.size());

    strMac = "10087";
    Bluetooth_DeleteDevice(strMac.c_str());
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    EXPECT_EQ(3, listDevice.size());

    //NULL 时，删掉所有
    EmptyList();
    InitSomeDevice(BT_PHONE);
    Bluetooth_DeleteDevice();
    listDevice.clear();
    Bluetooth_GetDeviceList(listDevice);
    EXPECT_EQ(0, listDevice.size());
}
#endif
