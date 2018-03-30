#ifndef _UILOGIC_DIAL_UI_BASE_H
#define _UILOGIC_DIAL_UI_BASE_H

#include "dialprocessor.h"
#include "uilogicbasetalkui.h"


class CUILogicDialUIBase : public CUILogicBaseTalkUI
{
public:
    CUILogicDialUIBase();
    virtual ~CUILogicDialUIBase();

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
    //从界面获取数据
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname) = 0;
    //从搜索框中获取选择数据
    virtual bool GetSearchTableContent(yl::string & strNum, yl::string & strDisplayname) = 0;
    //把选中的搜索结果赋值到编辑框中
    virtual void SetLineEditDataBySearchTable() = 0;
    //输入预编辑字符
    virtual void InputLineEditData() = 0;
    //是否有选中搜索结果
    virtual bool IsSearchTableSelectItem() = 0;
    //编辑框是否为空
    virtual bool IsEditLineEmpty() = 0;
    //获取穿梭数据callID
    virtual int GetShuttleCallID() = 0;
    //获取编辑框中的数据
    virtual yl::string GetLineEditData() = 0;
    //处理IME输入法
    virtual void DealIMEAction() = 0;
    //处理delete按键
    virtual void DealDeleteAction() = 0;
    //处理Pool的softkey
    virtual void DealPool() = 0;

    virtual void DealHistory() = 0;


protected:
    //保存的processor
    CDialProcessor * m_pProcessor;
};

#endif
