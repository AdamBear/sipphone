#ifndef _ZEROAUTOPPROCESSOR__H
#define _ZEROAUTOPPROCESSOR__H
#include "baseautopprocessor.h"
class CZeroProcessor : public CBaseAutopProcessor
{
public:
    CZeroProcessor();
#if IF_FEATURE_METASWITCH
    CZeroProcessor(AUTOP_UPDATE_TYPE eType);
#endif
    virtual ~CZeroProcessor();

    virtual bool ProcessRedirectMsg(msgObject & objMsg);

    virtual bool ProcessConnect();

#if IF_BUG_30885
    bool ProcessOption66Connect(bool bAddAuth = false);

    bool ProcessDMSConnect(bool bAddAuth = false);

    virtual bool OnTaskActionCallback(CNetworkFileAction * pTaskAction);
#endif //IF_BUG_30885

protected:
    virtual void ShowAuthWnd();
    virtual bool ReadyToConnect();

#if IF_BUG_30885
    bool GetAutoPWithOption66()
    {
        return m_bAutoPWithOption66;
    }
    yl::string GetDHCPOption66Url();

    // 获取更新的Mac.cfg
    yl::string GetConfigMacFile();

    // 组建url+cfg
    void PacketCfg(yl::string & strServerURL, const yl::string & strCfgFile);

    // 根据不同的机型,获取不同的配置文件名
    yl::string GetConfigFileByPhone();

    // 使用cfg进行连接
    bool ConnectWithCfg(const char * pURL);

    // 使用mac进行连接
    bool ConnectWithMac(const char * pURL);
    bool GetConnectWithCfg()
    {
        return m_bConnectWithCfg;
    }
    void SetConnectWithCfg(BOOL bConnectWithCfg)
    {
        m_bConnectWithCfg = bConnectWithCfg;
    }
    UINT GetConnectTimes()
    {
        return m_uConnectTimes;
    }
    void SetConnectTimes(UINT nConnectTimes)
    {
        m_uConnectTimes = nConnectTimes;
    }
private:
    bool        m_bAddAuthorized;     //是否有加了鉴权信息
    bool        m_bAutoPWithOption66; //是否下载Option66所指向的地址
    yl::string  m_strOption66URL;       // DHCP Option 66 所指定的url
    bool        m_bConnectWithCfg;
    UINT        m_uMaxConnectTimes;
    UINT        m_uConnectInterval;
    UINT        m_uConnectTimes;
    bool        m_bAddCfgFlag;
    static bool        m_bFirstStart;

#endif //IF_BUG_30885
};

#endif //_ZEROAUTOPPROCESSOR__H