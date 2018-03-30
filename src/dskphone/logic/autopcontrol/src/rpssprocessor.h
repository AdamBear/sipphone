#ifndef _RPSAUTOPPROCESSOR__H
#define _RPSAUTOPPROCESSOR__H
#include "baseautopprocessor.h"
class CRpsProcessor : public CBaseAutopProcessor
{
public:
    CRpsProcessor();

    virtual ~CRpsProcessor();

    virtual bool OnTaskActionCallback(CNetworkFileAction * pTaskAction);

    virtual bool ProcessRedirectMsg(msgObject & objMsg);

    virtual bool ReadyToConnect();

    virtual bool ProcessConnect();

    virtual bool ExitProcessor();

    virtual bool OnTimer(UINT uTimerID);
protected:

    //virtual void ShowAuthWnd();
    void InitConfig();
    bool ParseUserName(const yl::string & strOriginalUrl, yl::string & strUrl, yl::string & strUserName,
                       yl::string & strPasswd);
    UINT GetConnectTimes()
    {
        return m_uConnectTimes;
    }
    void SetConnectTimes(UINT nConnectTimes)
    {
        m_uConnectTimes = nConnectTimes;
    }
private:
    UINT m_uConnectTimes;
    UINT m_uMaxConnectTimes;
    yl::string m_strOldUrl;

};

#endif //_RPSAUTOPPROCESSOR__H