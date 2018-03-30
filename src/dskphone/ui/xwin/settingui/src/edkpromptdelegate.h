#ifndef EDK_PROMPT_DELEGATE_H_
#define EDK_PROMPT_DELEGATE_H_

#include "settingui/src/settinguidatadelegatebase.h"
#include "edkui/include/edkui_def.h"

class CEdkPromptDelegate : public CSettinguiDataDelegateBase
{
public:
    CEdkPromptDelegate();
    ~CEdkPromptDelegate();

    static CSettingUIDelegateBase * CreateEdkPromptDelegate();
public:
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    // 点击Back事件
    virtual bool OnBackAction();
    virtual bool OnAction(int eActionType, int iDataIndex);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
private:
    // 反馈数据给逻辑层
    void FeedBack(bool bOk = false);
private:
    ST_EDK_DATA     m_stEdkData;
};

#endif //EDK_PROMPT_DELEGATE_H_
