#ifndef _SCAMENU_PROCESSOR_H
#define _SCAMENU_PROCESSOR_H


#include "common/common_data_define.h"
#include "talk/broadsoftsca/include/scamanager_def.h"
#include "talkuilogiccommon.h"

#include "ylvector.h"
#include <yllist.h>

//每一个SCAMenuItem 元素 数据
struct tagSCAMenuItemData
{
    yl::string strStatus;
    yl::string strDisplayName;

};

typedef struct tagSCAMenuItemData CSCAMenuItemData;

struct tagSCAMenuUIData
{
    //sca数据列表
    talkui_vector<CSCAMenuItemData> vectorSCAMenuItemData;


    tagSCAMenuUIData()
    {
        Reset();
    }

    void Reset()
    {
        vectorSCAMenuItemData.clear();
    }

};

typedef struct tagSCAMenuUIData CSCAMenuUIData;

//scamenu ui 的基础类
class CSCAMenuUIBase;

class CSCAMenuProcessor : public CMKProcessor
{
public:
    CSCAMenuProcessor();
    virtual ~CSCAMenuProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();

    //设置绑定的UI
    virtual void SetBindUI(CSCAMenuUIBase * pSCAMenuUI);
    //获取绑定的UI
    virtual CSCAMenuUIBase * GetBindUI();

    //获取 scamenu 数据
    CSCAMenuUIData * GetSCAMenuUIData();

    //设置数据
    virtual void SetData(void * pData = NULL);

public:
    //http://10.3.5.199/Bug.php?BugID=65420
    //实际上  数据是不可能为空的
    //获取Softkey数据
    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //获取title
    yl::string GetTitle();

public:



protected:
    // 获取当前焦点对应的账号分线信息
    SCA_Info * GetInfoByID(UINT uIndex);

    // 根据SCA数据，获取SCA显示内容
    // | refSCAList | [in] SCA菜单数据
    // 返回是否获取成功
    bool SetSCAMenuData(ListSCAInfo & refSCAList);

    // 根据SCA状态获取状态显示名
    yl::string GetNameByStatus(BLF_STATUS_CODE eStatus);
    // 获取SCA账号分线的显示名
    yl::string GetDisplayName(SCA_Info * pInfo);

    // 获取关联的账号序号
    int GetBindAccountID();

public:
    //处理硬按键按下事件
    bool OnKeyPress(int iKeyCode);
    //处理 触摸屏按下事件
    void OnMousePress();

    //处理new call事件
    void OnNewCallPress();
    //处理cancle事件
    void OnCancel();
    //处理Answer事件
    void OnAnswerPress();
    //处理Reject事件
    void OnRejectPress();
    //处理switch事件
    void OnSwitchPress();
    //处理bargein事件
    void OnBargeInPress();
    //处理resume事件
    void OnResumePress();
    //处理callpull事件
    void OnCallPullPress();
    //处理callpull事件
    void OnRetrievePress();
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    //处理silentbargin事件
    void OnSilentBarginPress();
#endif


public:
    //定时函数
    BOOL OnSCAMenuTimer(msgObject & msg);

protected:
    //启动定时器
    void StartTime();
    //停止定时器
    void StopTime();

protected:
    //保存的softkey
    YLVector<SoftKey_TYPE> m_vectorSoftkey;

protected:
    mkThreadTimer m_objSCAMenuTimer;        //定时器


protected:
    //保存的UI指针
    CSCAMenuUIBase * m_pSCAMenuUI;
    //保存的UI数据
    CSCAMenuUIData m_objSCAMenuUIData;

    //逻辑层上传的 数据指针， 后面再处理 softkey 的时候还会用到
    ScaMenuData    *    m_pScaMenuData;     // SCA状态的菜单数据


};


#endif
