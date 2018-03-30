#ifndef _DLG_CONFERENCE_UI_H_
#define _DLG_CONFERENCE_UI_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgconferencebase.h"
#include "commonwidget.h"
#include "uicommon/uikernel/qtimepiece.h"

//前向声明
class CContactInfoFrame;
class QTimePiece;
class CConferenceInfoFrame;


class CDlgConferenceUI: public CDlgConferenceBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgConferenceUI();
    }

public:
    CDlgConferenceUI();
    virtual ~CDlgConferenceUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // NetWork、Dtmf、音量条等Tip显示状态变化时的处理
    virtual void OnTipChange(TALK_TIP_TYPE eTipType);

#ifdef _T48
    // 调整控件布局
    virtual void AdjustLayout();
#endif

    //获取通话时间
    virtual QCustomTime GetTalkTime();

    //获取当前操作的CallID
    virtual int GetCurrentActionCallID();

#ifdef _T48


    //添加或删除图标
    virtual void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);

    //删除所有图标
    virtual void DeleteAllIcon();

    // 设置Tip信息
    void SetTipInfo(const QString & strTip, const QString & strPicPath = "");

    // 清空Tip信息
    void ClearTip();

#endif

public:
    //弹出框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    //构造函数调用初始化数据
    void InitData();

    // 刷新Hold、Mute的信息
    void UpdateHoldAndMuteInfo(bool bLocalHold, bool bMute);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    bool ShowErrorNote(DataLocalConf * pLocalConfData);
#endif


protected slots:
    void OnBtnClicked(CConferenceInfoFrame * pInfoFrame, const QString & strBtnAction);

protected:
    //当前操作的CallID
    int m_iCurrentActionCallID;

protected:
    std::vector<CConferenceInfoFrame *>  m_veCConInfoFrame; // 会议中其他方的信息
    qHoldFrame            m_HoldFrame;   // 显示HOLD信息的控件
    QTimePiece            m_qTimePiece; // 计时控件
};




#endif // _DLG_CONFERENCE_UI_H_
