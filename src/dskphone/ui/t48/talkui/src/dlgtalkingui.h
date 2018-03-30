#ifndef _DLG_TALKING_UI_
#define _DLG_TALKING_UI_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgtalkingbase.h"
#include "commonwidget.h"

#include "uikernel/qtimepiece.h"
#include "uikernel/iframelistdelegate.h"

// 前向声明
class CContactInfoFrame;
class QTimePiece;
class CCallInfo;
class IconLabel;
class CFrameList;

struct NetworkConfMgrData
{
    NetworkConfMgrData()
    {
        Reset();
    }

    void Reset()
    {
        m_strName = "";
        m_strID = "";
        m_bRemove = false;
    }

    bool operator==(const NetworkConfMgrData& other)
    {
        return (this->m_strName == other.m_strName
                && this->m_strID == other.m_strID
                && this->m_bRemove == other.m_bRemove);
    }

    NetworkConfMgrData& operator=(const NetworkConfMgrData& other)
    {
        if (this == &other)
        {
            return *this;
        }

        m_strName = other.m_strName;
        m_strID = other.m_strID;
        m_bRemove = other.m_bRemove;

        return *this;
    }

    QString m_strName;
    yl::string  m_strID;
    bool    m_bRemove;
};

class INetworkManagerListDelegate : public IFrameListDelegate
{
public:
    INetworkManagerListDelegate() : IFrameListDelegate() {};
    virtual ~INetworkManagerListDelegate() {};

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
    virtual void OnCleanContent(CFrameListBase * pFrameList) {};

    virtual bool IsFrameListAttach(CFrameListBase * pFrameList)
    {
        return true;
    }

    virtual void OnFrameListDetach(CFrameListBase * pFrameList) {}

    virtual void OnFrameListAttach(CFrameListBase * pFrameList) {}

public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    void SetData(const std::vector<NetworkConfMgrData>&  vecData);

public:
    //重置序号
    void ResetIndex();
    //获取序号
    int GetCurrentIndex();

private:
    std::vector<NetworkConfMgrData> m_vecData;

    int m_iCurrentIndex;
};

class CDlgTalkingUI: public CDlgTalkingBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgTalkingUI();
    }

public:
    CDlgTalkingUI();
    virtual ~CDlgTalkingUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

    //设置区域
    virtual void AdjustLayout();

    //获取通话时间
    virtual QCustomTime GetTalkTime();

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);
#endif

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    bool eventFilter(QObject * pObject, QEvent * pEvent);

protected slots:
    void OnRemoveMember(QString strUserID, QString strName);

protected:
    //构造函数调用初始化数据
    void InitData();

    // 显示Park错误的信息
    void ShowFailInfo(bool bShow, const QString & strInfo = "");

    // 刷新联系人信息
    void UpdateContactInfo(CCallInfo * pCallInfo);

    //刷新错误提示信息
    void UpdateFialInfo();

    //优先级为 Hold、Held、Mute
    // 刷新Hold、Held、Mute的信息
    void UpdateHoldAndMuteInfo(bool bHold, bool bHeld, bool bMute);

private:
    //网络会议管理模式初始化
    void NetworkConfManagerInitData();
    //刷新成网络会议管理模式 返回值是否是会议管理模式
    bool UpdateConfManagerStyle(DataTalking2UI * pTalkingData);
    //隐藏所有控件
    void HideAllWidget();

private:
    QTimePiece          m_qTimePiece;    // 计时控件
    QLabel              m_qCoverLabel;   // 使联系人头像变成灰色的控件

    IconLabel          m_qFailedInfo;   //Transfer失败提示信息
    qHoldFrame          m_HoldFrame;     // 显示HOLD信息的控件

    //网络会议管理模式新增控件
    CFrameList*      m_pListFrame;
    INetworkManagerListDelegate m_listDeleget;
    bool                m_bBWConfMgr;       //是否显示BW网络会议管理模式
};
#endif // _DLG_TALKING_UI_
