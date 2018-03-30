#ifndef _DLGRINGINGUI_H_
#define _DLGRINGINGUI_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgringingbase.h"

//前向声明
class qMultiPicAnimation;

class CDlgRingingUI: public CDlgRingingBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgRingingUI();
    }

public:
    CDlgRingingUI();
    virtual ~CDlgRingingUI(void);

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

#if IF_FEATURE_NFON_RTP
    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);
#endif

    //由于联系人头像显示的变化导致的布局变化
    virtual void ResetWidgetByContactPhotoVisibleChange();

protected:
    //构造函数调用初始化数据
    void InitData();

private:
    //清除TargetInf
    void ClearTargetInfo();

    //显示 TargetInfo
    void ShowTargetInfo(yl::string strTargetName, yl::string strTargetNumber);

    // 刷新联系人信息
    void UpdateContactInfo(yl::string strName, yl::string strNumber, yl::string strFullContactInfo,
                           yl::string strContactImage, bool bIsRecall, yl::string strViaInfo);

    //组装via信息
    yl::string MakeViaInfo(const yl::string & strViaName, const yl::string & strViaNumber);

private:
    //动画控件
    qMultiPicAnimation * m_pPicAnimation;

    // 来电小图标(无头像使用)
    QLabel              m_qImageComingCall;
    // 被呼叫者名字(无头像使用)
    qScrollTextLabel   m_qLocalName;
    // 被呼叫者号码(无头像使用)
    qScrollTextLabel   m_qLocalNumber;

    bool                m_bIsShowTargetInfo;            //是否显示targetinfo

};

#endif
