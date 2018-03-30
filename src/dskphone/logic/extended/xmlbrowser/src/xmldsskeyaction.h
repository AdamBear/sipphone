#ifndef _XMLBROWSER_DSSKEY_ACTION_HEADER_
#define _XMLBROWSER_DSSKEY_ACTION_HEADER_
#include "dsskey/include/idsskeyimplaction.h"
class CDssKeyXMLBrowserAction: public IDsskeyImplAction
{
public:
    CDssKeyXMLBrowserAction(): IDsskeyImplAction(DT_XMLBROWSER)
    {

    }
    virtual ~CDssKeyXMLBrowserAction()
    {

    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        if (logicData.strValue.empty())
        {
            return false;
        }

        yl::string strUrl = logicData.strValue;

        if (dsskey_GetDsskeyStatus(dsskeyDataID) != DS_XMLBROSER_IDLE
                && dsskey_GetDsskeyStatus(dsskeyDataID) != DS_NON)
        {
            DSSKEY_INFO("dsskey[%d] Status is Loading!", dsskeyDataID);
            return false;
        }

        xmlbrowser_Enter(strUrl.c_str(), true, dsskeyDataID);
        return true;
    }

    virtual bool OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData & logicData,
                         void * pExtraData)
    {
        if (timerID == dsskey_GetRelatedTimerID(dsskeyDataID))
        {
            dsskey_KillTimer(dsskeyDataID);
            dsskey_SetDsskeyStatus(dsskeyDataID, DS_XMLBROSER_IDLE);
            dsskey_refreshDSSKeyUI(dsskeyDataID);
            return true;
        }
        return false;
    }
};
#endif
