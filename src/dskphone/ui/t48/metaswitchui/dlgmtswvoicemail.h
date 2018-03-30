#ifndef _DLG_MTSW_VOICEMAIL_H_
#define _DLG_MTSW_VOICEMAIL_H_

#include "config.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include <QDialog>
#include "mtswvoicemaillistdelegate.h"


class CDlgMTSWVoicemail : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    explicit CDlgMTSWVoicemail(QWidget * parent = 0);
    ~CDlgMTSWVoicemail();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgMTSWVoicemail();
    }

public:
    void InitData();
    // 读取页面数据
    virtual bool LoadPageData();
    // 查看页面状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    // 保存页面数据
    virtual bool SavePageData()
    {
        return true;
    }
    // 事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    // 执行动作
    virtual bool OnAction(const QString & strAction);

    // 弹框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // 收到logic层返回的消息
    void OnReceiveMsg(const msgObject & objMessage);
    // 加载Voicemail列表
    bool LoadVoicemailList(int nDataIndex, int nDataCount);

    virtual void ExtraDialogInitData();

    //virtual bool IsRejectReturnToIdle(bool bClickToIdle = false) { return m_bRejectToIdle;    }
	//是否显示音量条
	virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
		QRect & rtVolume, yl::string & strBGPath) const;
private:
    // 发送处理
    bool ProcessCallBackKey();
    // Play键处理
    bool ProcessPlayKey();
    // 添加到联系人
    bool AddToContacts(bool bIsLocal);
    // 标记读取状态
    bool MarkReadStatus(ReadStatus readStatus);
    // 显示more选项
    void ShowMore(int iCurrentIndex);
    // 处理more选项
    void DealWithMore(const int iResult);
    // 播放VoiceMail
    void PlayVoiceMail(int iCurrentIndex);
    // 暂停VoiceMail
    void PauseVoiceMail(int iCurrentIndex);
    // 继续VoiceMail
    void ResumeVoiceMail(int iCurrentIndex);
    // 停止VoiceMail
    void StopVoiceMail(int iCurrentIndex, bool bOpenTimer = true, bool bResetChannel = true);

    QWidgetPair * FindWidgetPair(int iCurrentIndex);

public Q_SLOTS:
    void OnShow();

    void OnItemMoreAction(CListItem * pItem);

    void OnItemStopAction(CListItem * pItem);

    void OnItemFinishAction(CListItem * pItem);

protected:
    virtual void showEvent(QShowEvent * e);

private:
    MTSWVoicemailListDelegate * m_pMTSWListDelegate;

    bool m_bAddNewItem;//是否添加新Item

    int m_iPlayIndex;//当前播放文件的下标

    //bool m_bRejectToIdle;
};

#endif
#endif
