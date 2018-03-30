#ifndef _C_VVM_FILE_PLAYER_CONTROLLER_H
#define _C_VVM_FILE_PLAYER_CONTROLLER_H

#include "fileplayerdefine.h"
#include "fileplayercontroller.h"
#include "messagebox/modmessagebox.h"

#ifdef _T48
#include "vvmoptionselectdelegate.h"
#endif

enum VVM_DIALOG_TYPE
{
    VVM_DIALOG_TYPE_ERROR = 0,
    VVM_DIALOG_TYPE_LIST,
    VVM_DIALOG_TYPE_DETAIL,
    VVM_DIALOG_TYPE_OPTION,
    VVM_DIALOG_TYPE_PLAY,
    VVM_DIALOG_TYPE_SET,
    VVM_DIALOG_TYPE_END
};

struct VVM_ITEM_PAIR
{
    QString         m_strAction;
    QString         m_strTitle1;
    QString         m_strTitle2;
};

typedef QVector<VVM_ITEM_PAIR>      vecVVMItemPair;

class CVVMFilePlayerController : public CFilePlayerController, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    static CVVMFilePlayerController & GetInstance();

private:
    CVVMFilePlayerController(QObject * pParent = NULL);
    virtual ~CVVMFilePlayerController();

public:
    static void OnPlayVVMTopWndChange(CBaseDialog * pDlgPreTop, CBaseDialog * pDlgNewTop);
    void OnTopWndChange();

    LRESULT OnMyVoiceMailMsg(msgObject & objMessage);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    void SetListDialogExist(bool bExist);
    void SetDetailDialogExist(bool bExist);
    void SetPlayDialogExist(bool bExist);

    void SetCurrentAccount(void * pData);
    inline int GetCurrentAccount()
    {
        return m_iAccountId;
    }

    void OnVVMDialogShow();

    inline int GetCurrentFocusFileIndex()
    {
        return m_nCurrentIndex;
    }

    const vecVVMItemPair & GetOptionData(bool & bResult);
    const vecVVMItemPair & GetDetailData(bool & bResult);
    const vecVVMItemPair & GetPlayInfo(bool & bResult, int nIndex = -1);
    vecVVMItemPair GetPlaySoftkey(bool & bResult);
    vecVVMItemPair GetPlayButtons(bool & bResult, int nDataIndex);

private:
    void InitOptionData();
    void InitDetailData();
    void InitPlayInfo();
    void InitPlaySoftkey();
    void InitPlayButtons();

    void LoadPlayDataToPlayer();

protected:
    virtual void ShowVirtualVoiceMailNote(CMsgBoxCallBackAction * pAction,
                                          const QString & strTitle,
                                          const QString & strNote,
                                          MESSAGEBOX_TYPE eType,
                                          int nMsTime = -1,
                                          const QString & strCancelBtnTitle = "",
                                          const QString & strConfirmBtnTitle = "",
                                          int iID = -1,
                                          const QVariant & extraData = QVariant());

    // 发送处理
    virtual bool ProcessCallBackKey();

    // 处理密码弹窗
    virtual bool ProcessMsgBoxPassword(int nLineId);

    bool VoiceMailCallOut(int nAccountId);
    void VoiceMailItemClicked(int nAccountId, bool bIsSetVoiceMailNumber);

    void EnterPlayDialog();

    virtual void JudgeAndCutTime(const time_t & tTime, QString & strTime1, QString & strTime2);

signals:
    void ListDialogOpenPage(VVM_DIALOG_TYPE eDialogPage);
    void DetailDialogOpenPage(VVM_DIALOG_TYPE eDialogPage);

    void DialogUpdate(int nIndex);
    void DialogListExit();
    void DialogRefreshSoftkey();
    void DialogRefreshPlayInfo();
    void DialogPlayExit();

    void FIlePlayStart();
    void FIlePlayStop();
    void FilePlayFinish();

public Q_SLOTS:
    virtual void OnPlayStateChange();

    virtual bool OnPlayerAction(QString strAction);
    virtual bool OnPlayerAction(QString strAction, QVariant var);

    // 密码框提交响应函数
    virtual void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    virtual void OnCancelInputXSIPassword();

    virtual void OnFilePlayFinish();

protected:
    //列表界面是否存在
    bool                            m_bIsListDialogExist;
    //详细信息界面是否存在
    bool                            m_bIsDetailDialogExist;
    //播放界面是否存在
    bool                            m_bIsPlayDialogExist;

    //是否正在加载
    bool                            m_bIsLoading;
    //是否已经加载完成
    bool                            m_bListLoaded;

    //当前账号
    int                         m_iAccountId;
    //当前正在播放的index
    int                         m_nCurrentIndex;

    vecVVMItemPair      m_vecOptionData;
    vecVVMItemPair      m_vecDetailData;
    vecVVMItemPair      m_vecPlayData;
    vecVVMItemPair      m_vecPlaySoftkey;

#ifdef _T48
    CVVMOptionSelectDelegate    *   m_pOptionSelectDelegate;
#endif
};

#define g_VVMController (CVVMFilePlayerController::GetInstance())

#endif //_C_VVM_FILE_PLAYER_CONTROLLER_H