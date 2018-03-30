#ifndef __MTSW_ACD_ACTION_CMD_H__
#define __MTSW_ACD_ACTION_CMD_H__

#if IF_FEATURE_METASWITCH_ACD
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"
#include "metaswitch/mtswcontrol/include/mtswaction.h"

class CMTSWACDActionCmd :
    public CMTSWActionCmdBase
{
public:
    CMTSWACDActionCmd(void);
    ~CMTSWACDActionCmd(void);

    //在这个函数里需要做的是根据错误码得出错误信息，把出错信息传递给上一层进行处理
    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId);

    //strFilePath是保存服务器返回的数据的文件路径。在此需要解析数据，通知上一层进行处理
    virtual bool OnLoadSucProcess(CMTSWAction * pAction);
};
#endif

#endif //__MTSW_ACD_ACTION_CMD_H__