#ifndef __BLUETOOTH_CONTACT_MANGE_H__
#define __BLUETOOTH_CONTACT_MANGE_H__

#include "basecontactmanager.h"

#ifdef IF_BT_SUPPORT_PHONE
class CBlueToothContactManager: public CBaseContactManger
{
public:
    static CBlueToothContactManager * GetInstance() DECLARE_ATTR;

    //重新加载蓝牙联系人
    bool ReLoadContact(const yl::string & strFile, bool suc);

    //处理蓝牙断开
    void ProcessDisconnect();

    //处理蓝牙连接上
    void ProcessConnected();

    //话机手动发起同步蓝牙联系人
    DIRECTORY_RET_CODE Update();

    //同步中取消同步
    void CancelUpdate();

    //蓝牙联系人保存到本地时，需调用此接口，触发后台下载图片
    bool LoadContactPhoto();

    void ProcessTimerOut(unsigned int uTimerID);

    FileDownloadStatus GetDownloadStatus();

    void InsertToNumMap(const yl::string & strNumber, const yl::string & strName);

    virtual bool GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync = true);

    const yl::string & GetContactNameByNumber(const yl::string & strNumber);

    const yl::string & GetContactImageByNumber(const yl::string & strNumber,
            const yl::string & strName = "");

protected:

    virtual bool IsNeedShowAllGroup();

private:
    CBlueToothContactManager();
    virtual ~CBlueToothContactManager();

private:
    int m_nTimeOut;

    MapNumberToName m_MapNumToName;
};

#define g_pBlueToothDirManager (CBlueToothContactManager::GetInstance())
#endif
#endif
