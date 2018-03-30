#if IF_FEATURE_GENBAND_E911
#ifndef __E911_ADDRESS_MANAGER_H__
#define __E911_ADDRESS_MANAGER_H__

#include <singletonclassdefine.h>
#include "xmlparser.hpp"
#include "../include/e911address_def.h"
#include "taskaction/taskactioncallbackbase.h"

using namespace NS_TA;

class CE911AddressManager : public CTaskActionCallbackBase
{
    DECLARE_SINGLETON_CLASS(CE911AddressManager)
public:
    // 消息处理
    static LRESULT OnMessage(msgObject&  objMessage);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction);

    // 初始化
    void Init();

    //请求E911 地址列表
    bool ReqE911AddrestList(yl::string strUrl = "");

    bool GetE911AddressListData(E911AddressListData *pData, int nParentID);

    bool GetE911AddressList(YLVector<int> &listout, int nParentID = -1);

    E911AddressData* GetAddressDetailByID(int nID);

    int GetParentIDByID(int nID);

    int GetIDByIndex(int nParentID, int nIndex);

    const yl::string& GetDisplayNameByID(int nID);

    bool GetFullLocationAddr(int& nID, yl::string& strFullAddr);

    bool SetLocationID(int& nID, yl::string& strFullAddr);

    int GetAddressListCount(int nID);

    bool CancelDownload();

    void ClearE911Address();

    bool IsGenbandE911On();

    void OnConfigChange(msgObject& objMessage);

protected:
    bool ParseE911AddressList(const yl::string& strFile);

    bool ProcessE911ChildList();

    bool AppandReqE911ListNote(yl::string& strFile);

    bool AppendDocument(xml_document& doc, xml_node& root);
    bool AppendHeaderNode(xml_node& root);
    bool AppendBodyNode(xml_node& root);
    void AppendStringTypeNode(xml_node& node, const yl::string& strName, const yl::string& strValue);

private:
    MapE911Address m_mapAddress; // 地址列表

    YLVector<int> m_vAddressList; //顶层地址id列表

    yl::string m_strServerUrl;

    E911DownLoadStatus m_eStatus;

};

// 实例
#define _E911AddressMgr GET_SINGLETON_INSTANCE(CE911AddressManager)
#endif // __E911_ADDRESS_MANAGER_H__
#endif
