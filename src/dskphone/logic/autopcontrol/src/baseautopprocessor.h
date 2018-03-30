#ifndef _BASEAUTOPPROCESSOR__H
#define _BASEAUTOPPROCESSOR__H
#include "ylstl/ylstring.h"
#include "threadtask/include/networkfileaction.h"
#include "../include/autop_common.h"
#include "autop_msg_mk.h"

class CBaseAutopProcessor
{
public:
    CBaseAutopProcessor(AUTOP_UPDATE_TYPE m_eAutopType);

    virtual ~CBaseAutopProcessor();

    virtual bool OnTaskActionCallback(CNetworkFileAction * pTaskAction)
    {
        return false;
    }

    virtual bool ProcessRedirectMsg(msgObject & objMsg)
    {
        return false;
    }

    virtual bool OnTimer(UINT uTimerID)
    {
        return false;
    }

    virtual bool ProcessConnect()
    {
        return false;
    }

    /**
     * @brief 是否处理END 消息
     *
     * @author Song
     * @date 2017/6/30
     *
     * @return True if process end message, false if not.
     */

    virtual bool IsProcessEndMsg()
    {
        return m_bProcessEnd;
    }

    /**
     * @brief 退出processor.
     *
     * @author Song
     * @date 2017/6/30
     *
     * @return True if it succeeds, false if it fails.
     */

    virtual bool ExitProcessor();

    /**
     * @brief 是否提示更新及更新结果.
     *
     * @author Song
     * @date 2017/6/30
     *
     * @return True if need show updating, false if not.
     */

    virtual bool IsNeedShowUpdating()
    {
        return true;
    }

    /**
     * @brief 设置是否处理End 消息
     *
     * @author Song
     * @date 2017/6/30
     *
     * @param bEnable True to enable, false to disable.
     */

    void SetProcessEndStatus(bool bEnable)
    {
        m_bProcessEnd = bEnable;
    }


    inline AUTOP_UPDATE_TYPE GetType()
    {
        return m_eAutopType;
    }

    inline void SetUrl(const yl::string & strUrl)
    {
        m_strUrl = strUrl;
    }

    inline yl::string GetUrl()
    {
        return m_strUrl;
    }

    inline void SetAuthInfo(const yl::string & strUser, const yl::string & strPasswd)
    {
        m_strPasswd = strPasswd;
        m_strUserName = strUser;
    }

    inline yl::string GetUserName()
    {
        return m_strUserName;
    }

    inline yl::string GetPasswd()
    {
        return m_strPasswd;
    }
    inline bool IsAuthSatus()
    {
        return m_bAuthStatus;
    }
    inline void SetAuthStatus(bool bStatus)
    {
        m_bAuthStatus = bStatus;
    }

protected:
    virtual bool ReadyToConnect()
    {
        return false;
    }

    virtual void ShowAuthWnd();

    virtual void ShowUpdatingWnd(ZeroSpDlgType eWndType);

    // 答复autop 鉴权信息
    bool ReplyAuthInfo();

    //启动autop
    bool SetUpAutop(const yl::string & strUrl);

    bool IsNeedShowAuth();

    // 增加鉴权信息
    yl::string AddAuthorizedInfo(const yl::string & strUrl, const yl::string & strUserName,
                                 const yl::string & strPassword);

    // 去除鉴权信息
    yl::string DeleteAuthInfoFromUrl(const yl::string & strOriginalUrl);

    //判断URL里面是否带有鉴权信息。
    bool IsIncludeAuthInfo(const yl::string & strUrl);

    bool ProcessAuth();

    // 取消更新超时
    void CancelUpdateTimeout();

    // 设置更新超时
    void SetUpdateTimeout();
protected:
    yl::string m_strUrl;
    yl::string m_strUserName;
    yl::string m_strPasswd;
    bool       m_bProcessEnd;
    bool       m_bAuthStatus;
    AUTOP_UPDATE_TYPE m_eAutopType;
    // 更新超时定时器
    int m_iUpdateTimeout;
};


class CNorMalAutopProcessor: public CBaseAutopProcessor
{
public:
    CNorMalAutopProcessor();
    virtual ~CNorMalAutopProcessor();

    virtual bool ProcessRedirectMsg(msgObject & objMsg);
    virtual bool ProcessConnect();
    /**
    * @brief  是否提示更新及更新结果
    */
    virtual bool IsNeedShowUpdating()
    {
        return m_bIsNeedShowUpdating;
    }
protected:
    void OnAutoProvision();
    virtual bool ReadyToConnect();
    virtual void ShowUpdatingWnd(ZeroSpDlgType eWndType);
    virtual void ShowAuthWnd();

private:
    bool       m_bIsNeedShowUpdating;
};


#endif //_BASEAUTOPPROCESSOR__H