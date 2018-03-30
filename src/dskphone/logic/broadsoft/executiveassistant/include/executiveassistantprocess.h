//////////////////////////////////////////////////////////////////////////
// executive assistant processor.h
// By liany, 2015/7/15

#ifndef __EXECUTIVE_ASSISTANT_PROCESSOR_H__
#define __EXECUTIVE_ASSISTANT_PROCESSOR_H__

#include <ETLLib.hpp>
#include <ylstring.h>
#include <ylhashmap.h>
#include <ylsmartptr.h>
#include "execommon.h"
#include "executiveassistantdata.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
// 该类用于实现XSI\SIP接口, 用于实现从XSI接口继承过来的接口

class CExecutiveAssistantProcessor : public XsiBaseProcesser, public CExecutiveAssistantData
{
public:
    static CExecutiveAssistantProcessor & GetInstance() DECLARE_ATTR;

    // 操作映射函数
    static ExeAssis_Action MapXSIAction2LocalAction(eXsiActionType eXsiType);
    static eXsiActionType MapLocalAction2XSIAction(ExeAssis_Action eActionType, bool bGet);

public:
    // 查询对应节点的数据
    bool QueryByType(ExeAssis_Action eAction, int iAccount);

    // call push
    bool CallPush(int iCallId, int iAccount);
    // call intiation
    bool CallIntiation(const yl::string & strTarget, int iAccount);

    bool IsXsiImp()
    {
        return m_bXSIImp;
    }

    // 对基类ExeXsiAction函数的进一步封装
    XsiAction * CreateExeXsiAction(int iAccount, ExeAssis_Action eActionType, bool bGet,
                                   const yl::string & strExtUrl = "", const yl::string & strFile = "", int iIndex = -1);
private:
    // 继承基类的接口
    virtual bool ProcessByType(XsiAction * pAction);

    //操作失败处理
    virtual void FailProcess(XsiAction * pAction);

private:
    // 隐藏的接口
    CExecutiveAssistantProcessor();
    virtual ~CExecutiveAssistantProcessor() {}

private:
    // 是否通过XSI接口实现
    bool                                m_bXSIImp;
};

#define g_refExeAssisProcessor (CExecutiveAssistantProcessor::GetInstance())
#endif
#endif // __EXECUTIVE_ASSISTANT_PROCESSOR_H__

