#ifndef __TASKACTION_CALLBACKBASE_H__
#define __TASKACTION_CALLBACKBASE_H__
#include "common.h"
#include <ETLLib/ETLLib.hpp>

namespace NS_TA
{
class CTaskAction;

// 回调类, 继承此类, 实现业务异步执行后的回调
// TODO. Fix Me
class TASKACTION_DLL_DECLARRE CTaskActionCallbackBase : public chMessageHandler
{
public:
    CTaskActionCallbackBase();
    virtual ~CTaskActionCallbackBase();

    /***
     * @brief : Action执行结束的回调
     * @param : |pTaskAction| 执行结束的action
     */
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction) = 0;

    /***
     * @brief : Action执行结束的回调
     */
    bool SendMessageToCallingThread(CTaskAction* pTaskAction);

protected:
    /***
     * @brief: 处理线程消息
     */
    virtual BOOL TryThreadMessage(msgObject & objMessage);

private:
    DSK_TARGET_T m_idCallingThread;
};

}  // namespace NS_TA

#endif  // __TASKACTION_CALLBACKBASE_H__
