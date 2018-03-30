#ifndef __I_LOCAL_CONTACT_UI_HELPER_H__
#define __I_LOCAL_CONTACT_UI_HELPER_H__

#include "directorycommon.h"
#include <diruilogic/include/diruilogicdefine.h>


class ILocalContactUIHelper
{
public:
    ILocalContactUIHelper() {}
    virtual ~ILocalContactUIHelper() {}

public:
    virtual bool GetLocalGroupData(ContactGroupData & groupData)                         = 0;
    virtual bool GetLocalContactData(int & nGroupId, ContactGeneralItemData & itemData)   = 0;
    virtual void SetInputFocus(const char * pszAttr) = 0;
#if IF_FEATURE_FAVORITE_DIR
    virtual bool GetLocalContactFavorite(yl::string &strFavoriteIndex) = 0;
    virtual void PopSureEditMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId) = 0;
#endif
#ifdef DIR_SUPPORT_EXIST_SAVE
    virtual bool IsSameContactRing(yl::string & strRing, yl::string & strContactRing)     = 0;
#ifdef PHOTO_SHOW
    virtual bool IsSameContactPhoto(yl::string & strPhoto, yl::string & strContactPhoto)  = 0;
#endif
#endif
};

#endif
