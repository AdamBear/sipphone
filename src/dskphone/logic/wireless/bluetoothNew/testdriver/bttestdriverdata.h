#ifdef IF_BT_TEST_LOGIC
#ifndef __BTTESTDRIVERDATA_H__
#define __BTTESTDRIVERDATA_H__
#include "wireless/bluetoothNew/src/bluetooth_inc.h"

class CBTTestData
{
private:
    CBTTestData(void);
    ~CBTTestData(void);

public:
    static CBTTestData & GetInstance()
    {
        static CBTTestData sIns;
        return sIns;
    }

    //初始化数据
    void Init();

    //获取适配器属性
    bool GetDongleProperty(btkit_property_t * dongleProperty);

    //设置适配器名称
    bool SetDongleName(const char * szName);

    //获取设备类型
    btkit_dev_type_t GetDeviceType(btkit_dev_instance_t dev_hnd);

    //获取设备指针
    bttest_dev_t * GetDeviceInfo(btkit_dev_instance_t dev_hnd);

    bttest_dev_t * GetDeviceInfo(WIRELESS_DEVICE_STATE eState);

    //获取设备列表
    bool GetDeviceList(list_dev & listDevice);

    //获取手机设备指针
    bttest_mb_t * GetMbInfo(btkit_dev_instance_t dev_hnd);

    //获取保存的设备数量
    int GetDeviceSum();

    //获取设备句柄列表
    int GetDeviceHandleList(btkit_dev_instance_t * handleList);

    //下载联系人
    bool StartDownloadContact(btkit_dev_instance_t dev_hnd, btkit_pba_data_t * data,
                              yl::string & strPath);

    //取消下载
    bool StopDownloadContact(btkit_dev_instance_t dev_hnd);

    //添加一个通话
    int AddCall(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd, int iCallStatus,
                const yl::string & strNum);

    //更新一个通话状态
    bool UpdateCallStatus(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd, int iCallID,
                          bttest_callstatus_t & btCallStatus);

    //删除一路通话
    bool DeleteCall(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd, int iCallID);

    //删除所有通话
    void DeleteAllCall(list_callstatus & listStatus);

    //获取一路通话信息
    bttest_callstatus_t * GetCallInfo(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                                      int iCallID);

    //获取通话列表
    bool GetCallInfo(list_callstatus & listStatus, btkit_dev_instance_t dev_hnd,
                     list_callstatus & callList);

    bool GetCallInfoByStatus(list_callstatus & listStatus, int iStatus, list_callstatus & callList);

    //获取通话数量
    int GetCallCount(list_callstatus & listStatus,
                     btkit_dev_instance_t dev_hnd = BT_DEVICE_HANDLE_INVAILED);

    //随机获取一个设备句柄
    btkit_dev_instance_t GetRandDeviceHandle();

    static int GetRandNumber();

    int GetCallID();

private:
    list_dev          m_listDev ;

    list_mb           m_listMb ;

    list_headset      m_listHeadset ;

    btkit_property_t  m_dongleProperty ;
};

#define _BTTestData CBTTestData::GetInstance()
#endif //__BTTESTDRIVERDATA_H__
#endif // IF_BT_TEST_LOGIC
