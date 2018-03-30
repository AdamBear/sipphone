#ifndef __XSI_ACTION_H__
#define __XSI_ACTION_H__

#include <ylhashmap.h>
#include "xsicommon.h"
#include "threadtask/include/networkfileaction.h"

typedef YLHashMap<eXsiActionType, yl::string> XsiUrlMap;


class XsiAction : public CNetworkFileAction
{
public:
    // Override.
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    static void InsertMap(eXsiActionType eType, const yl::string & strAppUrl);

    int GetLineId()
    {
        return m_nLineID;
    }

    eXsiActionType GetXsiType()
    {
        return m_eType;
    }

    void SetChangeData(bool bChangeData);

    bool GetChangeData()
    {
        return m_bExeChangeData;
    }

    const yl::string & GetExtenUrl()
    {
        return m_strExtenUrl;
    }

    static yl::string GetRandomFile();

public:
    XsiAction(int nLineId, eXsiActionType eType, const yl::string & strExtUrl = "",
              const yl::string & strLocalFile = "");

protected:

private:

    bool SetMethed();

    static const yl::string & GetAppUrlByType(eXsiActionType eType);

private:

    static XsiUrlMap m_urlMap;

    bool m_bIsSipXsi;

    yl::string m_strUserId;
    yl::string m_strPassword;

    eXsiActionType m_eType;

    int m_nLineID;

    bool m_bExeChangeData;

    yl::string m_strExtenUrl;
};

#endif
