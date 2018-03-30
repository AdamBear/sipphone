#ifndef __ANYEHERE_PROCESS_H__
#define __ANYEHERE_PROCESS_H__

#include "broadsoft/anywhere/include/anywherecommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CAnywherePrcocesser : public XsiBaseProcesser
{
public:

    static CAnywherePrcocesser * GetInstance();

    //Request
    bool RequestLocationList();

    bool RequestDeleteLocation(const yl::string & strPhoneNumber);

    bool RequestAddLocation(const SAYWLocation & sLoct, bool bCreatorbyModify = false);

    bool RequestModifyLocation(const yl::string & strOriginalPhoneNumber, const SAYWLocation & sLoct,
                               eXsiActionType eType = XSI_ANYWHERE_MODIFY_LOCATION);

    bool RequestLocationDetail(const yl::string & strPhoneNumber);

    bool RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList
                                     , SAYWLocationArray & arry);

    bool RequestDeleteAllLocation();

    int GetCachedLocationCount();

public:

    void GetAllLocationsQueryResultList(SAYWLocationArray & arry);

    bool GetLocationDetailQueryResult(SAYWLocation & sLoct) const;

    bool CheckNumberExit(const yl::string & szNumber);

private:
    CAnywherePrcocesser();

    virtual ~CAnywherePrcocesser();

private:
    bool PraseLocationList(const yl::string & strFile);

    bool PraseLocationDetail(const yl::string & strFile);

    virtual bool ProcessByType(XsiAction * pAction);

    bool SaveDataToFile(const SAYWLocation & sLoct, eXsiActionType eType,
                        const yl::string & strFileName, bool bCreatorbyModify = false);

    SAYWLocation * GetLocationByNumber(const yl::string & strNumber);

protected:

private:
    SAYWLocationArray m_loactArry;

    SAYWLocation m_locatDetail;


};

#define g_pAnywhereProcesser (CAnywherePrcocesser::GetInstance())

#endif

