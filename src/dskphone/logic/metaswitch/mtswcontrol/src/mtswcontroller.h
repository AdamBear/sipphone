#if IF_FEATURE_METASWITCH
#ifndef MTSW_CONTROLLER_H_
#define MTSW_CONTROLLER_H_
#include "taskaction/taskactioncallbackbase.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "metaswitch/mtswcontrol/include/mtswaction.h"
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"

using NS_TA::CTaskAction;
using NS_TA::CTaskActionCallbackBase;

enum LoadStatus
{
    FILE_LOADING_READY = 0,  // 准备好.
    FILE_LOADING = 1,
    FILE_LOADING_SUC = 2,  //
    FILE_LOADING_FAIL = 3,  //

};

class CMTSWController : public CTaskActionCallbackBase, public CMTSWActionCmdBase
{

protected:
    CMTSWController();
    ~CMTSWController();

public:
    // 获取单键实例.
    static CMTSWController & GetInstance();

    // Override.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId);

    virtual bool OnLoadSucProcess(CMTSWAction * pAction);

    //初始化
    void Init();

    //获取SessionID
    yl::string GetSessionId()
    {
        return m_strSessionID;
    }

    int Login();
    int Logout();

    bool OnTimer(UINT uTimerID);

    void SetKeepAliveTimer();

    bool ExecPost(CMTSWActionCmdBase * pCallback, yl::string strURL, yl::string strData,
                  MtswActionType nType);
    bool ExecDownLoad(CMTSWActionCmdBase * pCallback, yl::string strURL, MtswActionType nType,
                      yl::string strFilePath = "", int nExParm = -1);

    void LoadMaxDownloadSize();

protected:
    int KeepAlive();

    bool SaveDataToFile(yl::string strData, yl::string strPath);

    void CleanAllData();

    void RegisterMsg();
    void UnRegisterMsg();

    void FormatString(yl::string & strString);

private:
    bool MyExecPostTaskAction(CMTSWActionCmdBase * pCallback, yl::string strURL, yl::string strHead,
                              MtswActionType nType, yl::string strFilePath);
    bool MyExecDownLoadTaskAction(CMTSWActionCmdBase * pCallback, yl::string strURL,
                                  MtswActionType nType, yl::string strFilePath, int nExParm = -1);

private:
    //KeepAlive查询定时器
    int m_iTimerKeepAlive;

    static int m_iFileIndex;

    yl::string m_strCommprotalURL;
    yl::string m_strSessionID;

    LoadStatus m_eDownLoading;

    int m_NetTimer;

    int m_iTimerLoginFail;
    int m_iTimerLoginTimeOut; //登录超时

    int m_nMaxDownloadSize;
};

#define _MTSWController CMTSWController::GetInstance()

#endif //MTSW_CONTROLLER_H_
#endif


