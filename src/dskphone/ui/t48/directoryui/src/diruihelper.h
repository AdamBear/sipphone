#ifndef __DIR_UI_HELPER_H__
#define __DIR_UI_HELPER_H__

#include <ylsmartptr.h>
#include "directorycommon.h"
#include "directorycontroler.h"


// 定义联系人属性名
#define ATTR_NAME           "name"
#define ATTR_OFFICENUMBER   "officenumber"
#define ATTR_MOBILENUMBER   "mobilenumber"
#define ATTR_OTHERNUMBER    "othernumber"
#define ATTR_NUMBER         "number"
#define ATTR_LINE           "line"
#define ATTR_RING           "ring"
#define ATTR_GROUP          "group"
#define ATTR_FAVORITE_INDEX "favoriteindex"

// 定义broadsoft联系人属性名
#define ATTR_BSFT_LASTNAME  "lastname"
#define ATTR_BSFT_FIRSTNAME "firstname"
#define ATTR_BSFT_USERID    "userid"
#define ATTR_BSFT_GROUPID   "groupid"
#define ATTR_BSFT_EXTENSION "extension"
#define ATTR_BSFT_NUMBER    "number"
#define ATTR_BSFT_MOBILE    "mobile"
#define ATTR_BSFT_YAHOOID   "yahooid"
#define ATTR_BSFT_PAGER     "pager"
#define ATTR_BSFT_EMAIL     "email"
#define ATTR_BSFT_IMPID     "impid"
#define ATTR_BSFT_LOCATION  "location"
#define ATTR_BSFT_TITLE     "title"
#define ATTR_BSFT_ADDRESS1  "address1"
#define ATTR_BSFT_ADDRESS2  "address2"
#define ATTR_BSFT_CITY      "city"
#define ATTR_BSFT_STATE     "state"
#define ATTR_BSFT_COUNTRY   "country"
#define ATTR_BSFT_CODE      "code"
#define ATTR_BSFT_DEPARTMENT    "department"
#define ATTR_BSFT_PASSWORD  "password"
#define ATTR_BSFT_USERID    "userid"

#define ATTR_GENBAND_LASTNAME   "lastname"
#define ATTR_GENBAND_FIRSTNAME  "firstname"
#define ATTR_GENBAND_EMAIL      "email"
#define ATTR_MTSW_FIRST_NAME            "mtswfirstname"
#define ATTR_MTSW_LAST_NAME         "mtswlastname"
#define ATTR_MTSW_NICK_NAME         "mtswnickname"
#define ATTR_MTSW_JOBTITLE          "mtswjobtitle"
#define ATTR_MTSW_ORGANISATION          "mtsworganisation"
#define ATTR_MTSW_HOMENUMBER    "mtswhomenumber"
#define ATTR_MTSW_WORKNUMBER    "mtswworknumber"
#define ATTR_MTSW_MOBILENUMBER  "mtswmobilenumber"
#define ATTR_MTSW_FAXNUMBER "mtswfaxnumber"
#define ATTR_MTSW_OTHERNUMBER   "mtswothernumber"

#define ATTR_MTSW_DEPARTMENT    "mtswdepartment"
#define ATTR_MTSW_EXTENTION     "mtswextention"

#define ATTR_MTSW_NAME  "name"
#define ATTR_MTSW_NUMBER    "number"
#define ATTR_MTSW_EXTENSION "extension"

class CListItem;
class CFrameList;
class QString;


class CDirUIHelper
{
public:
    explicit CDirUIHelper(int nType);
    virtual ~CDirUIHelper();

public:
    void Init();
    void UnInit();

    int GetType() const;

public:
    // 退出当前界面
    static void ExitCurrentUI(int nUIState);
    // 退出某一类型联系人所有界面
    static void ExitAllUI(int nDirType);
    static void ReturnRootGroup(int nDirType);
    // 重调删除焦点后重刷界面
    static void RefreshUIAfterDeleted(bool bBackUI = false);
    // 重刷当前界面
    static void RefreshUI(bool bDataChange, bool bResetFocus);
    // 显示提示框
    static void ShowMsgBox(const char * pszMsg, const char * pszExtra = NULL,
                           int nAction = DIR_ACTION_NONE);
    // 弹出选择提示框(确定返回true，其他返回false)
    static bool PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId);

#if IF_FEATURE_GBBOOK
    static void ShowMsgBox(const char * pszMsg, int nTime, const char * pszExtra = NULL,
                           int nAction = DIR_ACTION_NONE);
#endif
    static bool PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId,
                              const char * pExtTitle, const char * pszExtra = NULL, bool bExistEdit = false);


#ifdef DIR_SUPPORT_BATCH_OPERATE
    static int PopSelectBoxAction();
    static bool EnterSelectGroup(int nDirType, int nExcludeGroupId, bool bRootInclude, int nAction);
    static bool GetSelectContactId(VEC_CONTACT_ID & vecContactId);
    static bool IsSelectAllContact();
#endif
    // 显示等待窗口
    static void ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction);
    // 关闭等待窗口
    static void EndWaiting();
    static void OnCancelWaiting();
    // 显示错误提示
    static void ShowErrorMsg(int nError);
    static void SetMsgBoxActionId(int nAction, int nId);
    static int PopMsgBoxActionId(int nAction);

    // 进入新界面
    static void EnterUI(int nDirType, int nUIState, int nAction, int nId);

    static void SetAttributeList(int nDirType, LIST_ATTRIBUTE & lsAtrr);
    static int GetAttributeType();
    static int GetAttributeSize();
    static bool GetAttributeByIndex(int nIndex, yl::string & strName, yl::string & strValue);
    static bool GetAttributeByName(int nIndex, const char * pszName, yl::string & strValue);

    // 拔号
    static bool Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                     CDirectoryControler * pContrl, BaseDirDataRecord * pRecord);
    static bool Dial(int nAccountId, const yl::string & strName, const yl::string & strNumber,
                     CDirectoryControler * pContrl, BaseDirDataRecord * pRecord);

    static bool DialCalllog(int nAccountId, LIST_ATTRIBUTE & lsAttr, CDirectoryControler * pContrl,
                            BaseDirDataRecord * pRecord);
    static bool DialCalllog(int nAccountId, const yl::string & strNumber, CDirectoryControler * pContrl,
                            BaseDirDataRecord * pRecord);

    static int GetDialAccountId();
    static void ResetDialAccountId();
#if IF_FEATURE_GBBOOK
    static void SetDialAccountId(int nAccountId);
#endif

public:
    virtual int GetLisItemId(CListItem * pItem);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle()              = 0;

    void SetFrameList(CFrameList * pFrameList);
    void SetFocusInvalidFrameList(CFrameList * pFrameList);

    int GetActionByItem(int nItemAction);
    bool IsShowSettingTitle();
    void RefreshSettingTitle(int nTotalCount, int nSelectCount);

    bool AdjustListData(int nTotalCount, int & nDataIndex, int & nDataCount);

#ifdef DIR_SUPPORT_BATCH_OPERATE
    void OnBatchCopyContactSuccess();
#endif

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);

    bool RefreshContactItem(int nItemIndex, int nDataIndex, int nDataCount, int nContactId,
                            const yl::string & strName, const yl::string & strInfo,
                            const yl::string & strHead
#if IF_FEATURE_FAVORITE_DIR
                            , int nFavoriteIndex = -1
#endif
                           );

    bool RefreshDetailItem(int nItemIndex, int nDataIndex, int nDataCount,
                           QString & strTitle, QString & strValue);

    virtual void RefreshSearchTitle(int nTotalCount);

public:
    typedef YLHashMap<int, int> MAP_MSG_BOX_ACTION_ID;
    static MAP_MSG_BOX_ACTION_ID s_mapActionId;

protected:
    int m_nType;
    CFrameList * m_pFrameList;
    static bool s_bShowWaiting;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    static int s_nSelectBoxAction;
#endif
    // 记录号码选择界面使用的AccountId;
    static int s_nDialAccountId;
    // 本地联系人拷贝时的数据
    static int s_nAttrType;
    static LIST_ATTRIBUTE s_listAtrr;

protected:
    CFrameList * m_pFocusInvalidFrameList;

};

#ifndef CDirUIHelperPtr
typedef YLSmartPtr<CDirUIHelper> CDirUIHelperPtr;
#endif

#endif
