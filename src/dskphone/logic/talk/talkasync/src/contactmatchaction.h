#ifndef __CONTACT_MATCH_ACTION_H__
#define __CONTACT_MATCH_ACTION_H__

#if IF_BUG_38979
#include "baselibs/include/taskaction/taskaction.h"
#include "contacts/directory/include/moddirinterface.h"

namespace NS_TA
{

class CContactMatchAction : public CTaskAction
{
public:

    CContactMatchAction();
    virtual ~CContactMatchAction();

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

    void SetMatchNum(const yl::string & strNum);

    const yl::string & GetTargetName();

private:

    yl::string m_strMatchNum;

    yl::string m_strTarget;
};
}//NS_TA
#endif //IF_BUG_38979
#endif
