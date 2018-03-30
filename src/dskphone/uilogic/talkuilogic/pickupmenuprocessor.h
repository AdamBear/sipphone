#ifndef _PICKUPMENU_PROCESSOR_H
#define _PICKUPMENU_PROCESSOR_H


#include <ylstring.h>
#include "common/common_data_define.h"
#include "talkuilogiccommon.h"

#include <ylvector.h>
#include <yllist.h>

struct tagPickUPMenuData
{
    //pickup数据列表
    talkui_vector<yl::string> vectorMenuName;


    tagPickUPMenuData()
    {
        Reset();
    }

    void Reset()
    {
        vectorMenuName.clear();
    }

};

typedef struct tagPickUPMenuData CPickUPMenuData;

//pickupmenu ui 的基础类
class CPickUPMenuUIBase;

class CPickUPMenuProcessor
{
public:
    CPickUPMenuProcessor();
    virtual ~CPickUPMenuProcessor();

public:
    //构造完成后 绑定了UI后调用
    virtual void Init();
    //释放前解除UI绑定前调用
    virtual void Uninit();
    //设置绑定的UI
    virtual void SetBindUI(CPickUPMenuUIBase * pPickUPMenuUI);
    //获取绑定的UI
    virtual CPickUPMenuUIBase * GetBindUI();

    //获取 pickupmenu 数据
    CPickUPMenuData * GetPickUPMenuData();

public:
    //获取softkey数据信息
    virtual void GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey);

    //softKey 按键处理
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftkey);

    //获取title
    yl::string GetTitle();


public:
    //处理pickup事件
    void OnPickUpLine();
    //处理dial事件
    void OnDial();
    //处理new call事件
    void OnNewCall();
    //处理cancle事件
    void OnCancel();

protected:
    //更新pickupmenu 数据
    void UpdatePickUPMenuData();




protected:
    //保存的UI指针
    CPickUPMenuUIBase * m_pPickUPMenuUI;
    //保存的UI数据
    CPickUPMenuData m_objPickUPMenuData;


};


#endif
