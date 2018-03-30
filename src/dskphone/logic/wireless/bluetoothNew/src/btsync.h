#ifdef IF_SUPPORT_BLUETOOTH
///////////////////////////////////////////////////////////
//  CBTSync.h
//  Implementation of the Class CBTSync
//  Created on:      12-十二月-2014 19:07:34
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef __BTSYNC_H__
#define __BTSYNC_H__
#include "wireless/bluetoothNew/include/btdefines.h"
#include "vcard.h"

class CBTSync
{

public:

    static CBTSync & GetInstance()
    {
        static CBTSync sInstance;
        return sInstance;
    }

    //退出联动状态
    bool ExitSync(unsigned long ulDeviceHandle);

    //进入联动状态
    bool InitSync(unsigned long ulDeviceHandle);

    //处理联动消息
    LRESULT OnSyncMsg(msgObject & msg);

    //下载联系人
    bool StartDownloadDirectory(int iType = 0x05, bool bPhoto = false);

    //取消下载
    bool StopDownload(bool bCancel = true);

    //是否为蓝牙联动消息
    bool IsBtSyncMsg(unsigned long ulMsg);

    //是否已连接
    bool IsConnected()
    {
        return m_bConnected;
    }

    //获取蓝牙账号ID
    static int GetAccountID()
    {
        return BT_ACCOUNT_ID;
    }

    //获取手机信息
    const MobileProperty & GetMobileProperty()
    {
        return m_mProperty;
    }

    //定时器处理
    bool ProcessTimer(int iTimerID);

    //处理联系人失败
    void ProcessContactFailed(bool bNeedStop = false, const char * szErrorCode = NULL);

    ////处理解析成功
    void PraseContactSuccess(const yl::string & strFilePath);

    //是否在下载联系人
    bool IsInDownloadDir();

    //是否在解析联系人
    bool IsInParseDir();

    /**
    * 解析联系人
    * 输入为vcard 3.0 格式文件路径，
    * 输出本地联系人格式xml文件路径
    */
    static bool PraseDirectory(yl::string & strXmlPath, const char * pVcardFilePath);

    //设置dsskey
    bool SetDsskey(bool bEnable, btkit_dev_type_t dType = BT_UNKNOWN_TYPE);

    //获取联系人同步状态
    ContactSyncState GetContactSyncState()
    {
        return m_eSyncState;
    }

protected:
    ////设置联动状态
    void SetConnected(bool bConnected)
    {
        m_bConnected = bConnected;
    }

    //设置同步定时器
    void SetContactSyncTimer(bool bEnable);

    //更新手机信息
    void UpdateMobileProperty(const btkit_mb_cond_t * pProperty);

    //设置联系人同步状态
    void SetContactSyncState(ContactSyncState eState);

    static void FormatContact(LIST_MOBILE_CONTACT & listContact);
private:
    CBTSync();
    ~CBTSync();
    //是否已连接
    bool m_bConnected;
    //联系人同步状态
    ContactSyncState m_eSyncState;
    //联系人同步定时器
    int  m_iContactSyncTimer;
    //占用语音通道的设备id
    unsigned long  m_ulHandle;
    //蓝牙手机信息
    MobileProperty m_mProperty;
};

#define _BTSync CBTSync::GetInstance()
#endif // __BTSYNC_H__
#endif // IF_SUPPORT_BLUETOOTH
