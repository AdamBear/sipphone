/************************************************************************
 * FileName  : CAnywhereUILogic.h (header file)
 * Purpose   :
 * Date      : 2012/08/11 11:37:41
 ************************************************************************/

#ifndef _CANYWHEREUILOGIC_H_
#define _CANYWHEREUILOGIC_H_

#include <ylstring.h>
#include <vector>
#include <QtCore/QString>
#include <QtCore/QObject>
#include "broadsoft/anywhere/include/anywherecommon.h"
#include "messagebox/messageboxcallback.h"

#define EDIT_MAX_LENGTH_ANYWHERE_NAME       25
#define EDIT_MAX_LENGTH_ANYWHERE_NUMBER     19
#define EDIT_MAX_LENGTH_ANYWHERE_ALTNUMBER  30

struct SAYWLocationArray;
struct SAYWLocation;

class CDlgAnyWhereLocationNew;
class CDlgAnyWhereLocationList;
class CDlgAnyWhereLocationEdit;
class CMessageBoxBase;


class CAnywhereUILogic : public QObject, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    static CAnywhereUILogic * GetInstance();
    static void DestoryInstance();

protected:
    CAnywhereUILogic();

    virtual ~CAnywhereUILogic();

public:
    void Exit();
    bool IsAnywhereActive();

    bool IsLocationListPageActive();
    bool IsNewLocationPageActive();
    bool IsLocationEditPageActive();

public:
    QString FormatContryCodePhoneNumber(const yl::string & strCountryCode,
                                        const yl::string & strPhoneNum);

    bool SplitCountryCodePhoneNumber(const QString & strCountPhone, yl::string & strCountryCode,
                                     yl::string & strPhoneNum);

public Q_SLOTS:
    bool RequestLocationList(bool bShowMsgBox = true);

public:
    bool RequestAddLocation(const SAYWLocation & sLoct, bool bShowMsgBox = true);

    bool RequestModifyLocation(const yl::string & strOriginalPhoneNumber,
                               const SAYWLocation & sLoct, bool bShowMsgBox = true);

    bool RequestDeleteLocation(const yl::string & strPhoneNumber, bool bShowMsgBox = true);

    bool RequestDeleteAllLocation(bool bShowMsgBox = true);

    bool RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList,
                                     SAYWLocationArray & arry, bool bShowMsgBox = true);

    bool RequestLocationDetail(const yl::string & strPhoneNumber, bool bShowMsgBox = true);

    bool RequestQueryLocationExist(const yl::string & strPhoneNumber, bool bShowMsgBox = true);

    bool RequestModifyLocationGlobalAttribute(const bool * pbClick2DialCall, const bool * pbPagingCall);

public:
    void ResponseDeleteLocation();

    void ResponseEditLocation();

    void ResponseQueryLocationExist(bool bExist);

    void ResponseGetLocationDetail();

    void ResponseActionError(int iLineId, int errCode);

    void ResponseLocationListDownload();

public Q_SLOTS:
    void OnSubmitXSIPassword(const yl::string & strPswd);
    void OnCancelInputXSIPassword();

public:
    int GetCachedLocationCount() const;

    bool IsEnableAddLocation() const;

    //设置当前LineID
    void ChangeDefaultAccountID(int nLineID);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

Q_SIGNALS:
    void requestLocationList(bool bShowMsgBox);

private:
    CDlgAnyWhereLocationList * getDlgLocationListPage();
    CDlgAnyWhereLocationEdit * getDlgLocationEditPage();
    CDlgAnyWhereLocationNew * getDlgLocationNewPage();
    bool ProcessMsgBoxPassword(int nLineId);
    bool BackToPrePage();
private:
    static CAnywhereUILogic * s_pInst;
    yl::string m_strDelNum;
    yl::string m_strModifyPhoneNum;
};

#define g_pAnywhereUILogic (CAnywhereUILogic::GetInstance())

#endif
