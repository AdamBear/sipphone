#if FEATURE_BROADTOUCH_UC
#include "modbtucui.h"
#include "uimanager/moduimanager.h"
#include "btuccontactcontroler.h"
#include "dirctrlfactory.h"
#include "diruihelper.h"
#include "directorymgr.h"

bool BTUC_DialFavorite(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    if (UIManager_IsWndExist(eWT_Directory))
    {
        DIRUI_WARN("UIManager_IsWndExist(eWT_Directory) return true in DM_EnterDirectoryUI().");
        return false;
    }

    LIST_ATTRIBUTE lsNum;
    yl::string strName;

    if (!CBTUCContactControler::GetDialDetail(nContactId, strName, lsNum))
    {
        return false;
    }

    int nNumCount = lsNum.size();

    if (nNumCount <= 1)
    {
        yl::string strNumber = "";

        if (1 == nNumCount)
        {
            ST_CONTACT_ATTRIBUTE & stAttr = lsNum.front();

            strNumber = stAttr.m_strValue;
        }

        return CDirUIHelper::DialNumber(nAccountId, strName, strNumber,
                                        DIR_DIAL_ACTION_DIGIT_MAP_DIRECTORY);
    }

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_BTUC);
    CBTUCContactControler * pBTUCContact = CBTUCContactControler::GetControler(pControler);

    if (NULL == pBTUCContact)
    {
        return false;
    }

    pBTUCContact->SetContactId(nContactId);
    CDirUIHelper::SetDialAccountId(nAccountId);
    CDirUIHelper::ResetDialFocusNumber();

    CDirectoryControler::SetAttributeList(DIRECTORY_TYPE_BTUC, lsNum);

    g_DirectoryMgr.EnterNumberChoose(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);

    return true;
}

#endif

