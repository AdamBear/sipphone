#ifndef __BSFT_PWD_MGR_H__
#define __BSFT_PWD_MGR_H__

#include "include/features-define.h"
#include <singletonclassdefine.h>
#include "idiruihelper.h"


class CBSFTPwdMgr
{
    DECLARE_SINGLETON_CLASS(CBSFTPwdMgr)

public:
    void SetDirType(int nDirType);
    void SetPassword(const yl::string & strPwd);

    int GetDirType();
    int GetGroupId();

    int CheckBSFTPassword(IDirUIHelper * pUIHelper, int nGroupId);
    void EnterBSFTPwdUI();
    void ExitBSFTPwdUI();
    bool RetryBSFTPwd(IDirUIHelper * pUIHelper);

protected:
    int m_nDirType;
    int m_nGroupId;
    yl::string m_strPwd;
};
#define _BsftPwdMgr GET_SINGLETON_INSTANCE(CBSFTPwdMgr)

#endif
