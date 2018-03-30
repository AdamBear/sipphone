#ifndef __COMMON_UI_HELPER_H__
#define __COMMON_UI_HELPER_H__

#include "diruihelper.h"


class QString;

class CCommonUIHelper : public CDirUIHelper
{
public:
    explicit CCommonUIHelper(int nType);
    virtual ~CCommonUIHelper();

    static CCommonUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

    static void TransBsftFACText(yl::string & strValue, QString & strText);

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    void GetTextOfRecordToShow(yl::string & strText, const yl::string & strName,
                               const yl::string & strNumber,
                               const yl::string & strSipServer);
#endif

public:
    virtual int GetLisItemId(CListItem * pItem);
    virtual void OnAttachFrameList();

    bool FillData(ContactListData & listData);
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)     = 0;

    int GetAllContactAmount();
    virtual void RefreshUpdateTitle(int nContactAmount, bool bRefresh = false);

    void ExitBSFTPwdUI();

protected:
    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail)   = 0;

    bool RefreshGroupMenuItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              const QString & strName);
    bool RefreshCallLogItem(int nId, int nItemIndex, int nDataIndex, int nDataCount, int nCallType,
                            bool bHaveName, const QString & strName, const yl::string & strNumber,
                            const yl::string & strBornTick, const yl::string & strHead);

    // 更新标题(remote、bsft使用)
    bool IsShowUpateTitile();
    void InitUpdateTitle(bool bShowUpdateBtn = true);
    void SetUpdateTitleGroupName(const QString & strGroupName, bool bRefresh = false);
#if IF_FEATURE_GBBOOK
    void SetShowUpdate(bool bShow, bool bRefresh = false);
#endif

    virtual bool GetTitleColor(QColor & color, int nCallType);

    virtual void setCurrentPageForItem(CListItemPtr pItem);
};

#endif
