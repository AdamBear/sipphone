#ifndef _SETTINGUI_PAGING_LIST_DELEGATE_H_
#define _SETTINGUI_PAGING_LIST_DELEGATE_H_

#include "basesettinguidelegate.h"
#include "feature/include/featurecommon.h"
#include "messagebox/messageboxcallback.h"

class CSettinguiPagingListDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction
{
public:
    CSettinguiPagingListDelegate();
    virtual ~CSettinguiPagingListDelegate();

    static CSettingUIDelegateBase * CreatePagingListDelegate();
    static void OnPagingListCallBack();
public:
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
//  virtual void OnEnterPage(const yl::string& strPage, int iDataIndex);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & objSoftkey);
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);
private:
    void LoadPagingListItem(CSettingUIPageDataBase * pPageData);
    void AddPagingListItem(const yl::string & strName, CSettingUIPageDataBase * pPageData,
                           const yl::string strID);
private:
    void OnEditClicked();
    void OnDeleteClicked();
    void OnDeleteAllClicked();
    //paging到序号为index的组，序号从1开始
    bool OnPagingClicked(int iIndex);
private:
    void ShowNoteMessgaeBox(const yl::string & strNote);

protected:
    PagingListVec m_pagelist;
};

#endif //_SETTINGUI_PAGING_LIST_DELEGATE_H_
