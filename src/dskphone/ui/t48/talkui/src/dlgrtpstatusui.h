/************************************************************************
 * FileName  : CDlgRTPStatusUI.h (header file)
 * Purpose   :
 * Date      : 2012/12/24 9:50:39
 ************************************************************************/
#ifndef _RTP_STATUS_UI_H_
#define _RTP_STATUS_UI_H_

//mod-Talk
#include "talk/talklogic/src/talklogic_inc.h"
#include "talk/talklogic/include/talklogic_common.h"

//mod-this(TalkUI)
#include "dlgbasetalkui.h"
#include "uikernel/iframelistdelegate.h"

class CFrameList;

class IRTPListDelegate : public IFrameListDelegate
{
public:
    IRTPListDelegate() : IFrameListDelegate() {};
    virtual ~IRTPListDelegate() {};

public:
    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext()
    {
        return false;
    };
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious()
    {
        return false;
    };
    // 数据清空
    virtual void OnCleanContent(CFrameListBase * pFrameList)     {};

    virtual bool IsFrameListAttach(CFrameListBase * pFrameList)
    {
        return true;
    }

    virtual void OnFrameListDetach(CFrameListBase * pFrameList) {}

    virtual void OnFrameListAttach(CFrameListBase * pFrameList) {}

public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    bool LoadData(SVQReportData * pSVPRData);

public:
    //重置序号
    void ResetIndex();
    //获取序号
    int GetCurrentIndex();

private:
    vector<pair<QString, QString> > m_vecText;

    int m_iCurrentIndex;
};

class CDlgRTPStatusUI : public CDlgBaseTalkUI
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgRTPStatusUI();
    }

public:
    CDlgRTPStatusUI();

    virtual ~CDlgRTPStatusUI();

public:
    //初始化数据
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    virtual bool OnUIAction(const QString & strAction);

    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //talking状态通道键处理
    virtual bool OnTalkingChannelKeyPress(int iKey);

    //会议状态 通道键处理
    virtual bool OnConferenceChannelKeyPress(int iKey);

    // 是否允许切换Session
    virtual bool IsChangeSession()
    {
        return false;
    }

    virtual void AdjustLayout();

    void UpdateWnd();

    void resizeEvent(QResizeEvent * pEvent);

    //http://10.2.1.199/Bug.php?BugID=53414 这是由CCStatusLabel把下拉按钮挡住了 所以这边让I说ShowTip返回true 从而不显示CCStatusLabel
    // 是否有显示提示信息
    virtual bool IsShowTip();

public:
    //处理RTPStatus消息
    void OnRTPStatusMessage(msgObject & refMessage);
    //刷新数据
    void UpdateData();

protected:
    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL);

    void InitData();

    void AdjustList();

private:
    CFrameList * m_pListFrame;
    IRTPListDelegate m_listDeleget;

    int m_iCurrentIndex;    //当前选中
    bool m_bShowSwitch;     //是否显示switch按钮，用在会议中
};

////rtp 信息改变时 回调函数
//void s_FunRTPStatusUpdate(ERTPStatusToUI eRTPStatus);

#endif  //_RTP_STATUS_UI_H_
