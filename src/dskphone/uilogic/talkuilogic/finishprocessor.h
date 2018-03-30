#ifndef _FINISH_PROCESSOR_H
#define _FINISH_PROCESSOR_H

#include "basetalkuiprocessor.h"


struct tagFinishUIData
{
    CContactInfo objContactInfo;            //联系人信息

    bool bError;                            //是否通话错误
    bool bInvalidAccount;                   //是否是账号错误

    bool bMaxSessionError;                  //最大session错误
    bool bAllowNewCall;                     //是否允许创建新话路


    yl::string
    strInfo;                     //提示信息  如果是错误的话 保存的是错误信息  如果是正常结束保存的是时间


    tagFinishUIData()
    {
        Reset();
    }

    void Reset()
    {
        objContactInfo.Reset();         //联系人信息重置

        bError = false;                         //是否通话错误
        bInvalidAccount = false;                    //是否是账号错误
        bMaxSessionError = false;                   //最大session错误
        bAllowNewCall = false;                      //是否允许创建新话路

        strInfo = "";                       //提示信息  如果是错误的话 保存的是错误信息  如果是正常结束保存的是事件
    }

};

typedef struct tagFinishUIData CFinishUIData;

class CUILogicFinishUIBase;
class CFinishProcessor : public CBaseTalkUIProcessor
{
public:
    CFinishProcessor();
    ~CFinishProcessor();

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
    CFinishUIData * GetFinishUIData();

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

    //获取蓝牙softkey数据信息
    void GetBluetoothSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    // 是否允许自定义softkey
    virtual bool IsAllowDefineSoftkey();
    // softkey处理(自定义类型)
    virtual void GetSoftkey(yl::string & strFile, yl::string & strStatus);


protected:
    //保存的UI指针
    CUILogicFinishUIBase * m_pTalkUI;
    //保存的UI数据
    CFinishUIData m_objFinishUIData;


};


#endif
