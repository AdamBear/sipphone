#ifndef _PARKNOTIFY_PROCESSOR_H
#define _PARKNOTIFY_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagParkNotifyUIData
{
    CContactInfo objContactInfo;        //联系人信息

    tagParkNotifyUIData()
    {
        Reset();
    }

    void Reset()
    {
        objContactInfo.Reset();
    }

};

typedef struct tagParkNotifyUIData CParkNotifyUIData;

class CUILogicParkNotifyUIBase;
class CParkNotifyProcessor : public CBaseTalkUIProcessor
{
public:
    CParkNotifyProcessor();
    ~CParkNotifyProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CUILogicBaseTalkUI * pTalkUI);
    //获取绑定的UI
    virtual CUILogicBaseTalkUI * GetBindUI();

    //获取数据
    CParkNotifyUIData * GetParkNotifyUIData();

private:
    //设置数据
    virtual void SetTalkData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

protected:
    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

private:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //设置title
    virtual void SetTitle(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //设置图标列表
    virtual void SetIcons(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //给子界面设置数据
    virtual void SetTalkUIData(UPDATE_REASON eReason = UPDATE_DEFAULT);

    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);

protected:
    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();


protected:
    //保存的UI指针
    CUILogicParkNotifyUIBase * m_pTalkUI;
    //保存的UI数据
    CParkNotifyUIData m_objParkNotifyUIData;


};


#endif
