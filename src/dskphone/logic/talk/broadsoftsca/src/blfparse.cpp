#include "xmlparser/pugixml.hpp"
#include "scastatusmanager.h"
#include "commonunits/modcommonunits.h"
#include "talk/talklogic/include/commonfunctions.h"

BLF_STATUS_CODE MapBLFStatus(LPCSTR lpStatus, LPCSTR lpDirect, bool bRender/* = true*/)
{
    if (lpStatus == NULL || lpDirect == NULL)
    {
        return CN_BLF_STATUS_UNKOWN;
    }

    if (strcmp(lpStatus, "proceeding") == 0
            || strcmp(lpStatus, "early") == 0
            || strcmp(lpStatus, "trying") == 0)
    {
        if (strcmp(lpDirect, "initiator") == 0)
        {
            return CN_BLF_STATUS_CALLOUT;
        }

        return CN_BLF_STATUS_RINGING;
    }
    else if (strcmp(lpStatus, "confirmed") == 0)
    {
        if (bRender)
        {
            return CN_BLF_STATUS_TALKING;
        }

        return CN_BLF_STATUS_HOLD;
    }
    else if (strcmp(lpStatus, "terminated") == 0)
    {
        return CN_BLF_STATUS_IDLE;
    }

    return CN_BLF_STATUS_UNKOWN;
}

/* 将lpBlock数据分行打印到日志，一遍扫描且不需要额外空间，
   考虑到日志输出单条256Bit限制 */
void TraceBlockData(LPCSTR lpBlock, int iLenth)
{
    if (lpBlock == NULL)
    {
        return;
    }

#if 1
    int iIndex = 0;
    LPCSTR lpSub = lpBlock;

    while (iIndex < iLenth)
    {
        int iCount = 0;
        while (lpSub[iCount] != '\n' && iIndex + iCount < iLenth)
        {
            ++iCount;
            continue;
        }

        if (++iCount > 1)
        {
            int iNum = iCount - 1;
            // 支持"\r\n"换行方式
            if (lpSub[iNum - 1] == '\r')
            {
                --iNum;
            }

            if (iNum > 0)
            {
                APP_INFO("%.*s", iNum, lpSub);
            }
        }

        iIndex += iCount;
        lpSub += iCount;
    }
#else
    printf("%.*s", iLenth, lpBlock);
#endif
}

bool CBLFParse::Parse(LPCSTR lpNotify, int iLenth, CSCAStatusManager * pCallBack)
{
    if (lpNotify == NULL || pCallBack == NULL || iLenth <= 0)
    {
        return false;
    }

    TraceBlockData(lpNotify, iLenth);

    using namespace pugi;

    xml_document doc;
    if (!doc.load_buffer(lpNotify, iLenth))
    {
        return false;
    }

    xml_node noodRoot = doc.child("dialog-info");
    if (noodRoot)
    {
        int iVersion = noodRoot.attribute("version").as_int();
        bool bFull = strcmp(noodRoot.attribute("state").as_string(), "full") == 0;
        LPCSTR lpBlaNamber = noodRoot.attribute("entity").as_string();

        pCallBack->SetBlaNumer(lpBlaNamber);

        if (bFull)
        {
            pCallBack->Release();
            pCallBack->AllocArray();
        }
        // version 不是累加1的话，说明状态不同步了，TODO：重新订阅 以便服务器推送Full包
        else if (iVersion - m_iVersion != 1)
        {
            SCA_ERR("something may error for version [%d]->[%d]", m_iVersion, iVersion);
        }

        m_iVersion = iVersion;

        xml_node noodDialog = noodRoot.child("dialog");
        while (noodDialog)
        {
            SCA_Info sInfo;
            LPCSTR lpDialogID = noodDialog.attribute("id").as_string();
            LPCSTR lpCallID = noodDialog.attribute("call-id").as_string();
            LPCSTR lpDirect = noodDialog.attribute("direction").as_string();
            LPCSTR lpState = noodDialog.child_value("state");
            // Genband SCA appearance来自此节点
            sInfo.iIndex = atoi(noodDialog.child_value("sa:appearance"));
            bool bPrivate = noodDialog.attribute("x-private").as_bool();
            bool bRender = true;
            bool bByeLess = false;

            xml_node noodLocal = noodDialog.child("local");
            if (noodLocal)
            {
                xml_node noodTarget = noodLocal.child("target");
                if (noodTarget)
                {
                    xml_node noodParam = noodTarget.child("param");
                    while (noodParam)
                    {
                        LPCSTR lpParamName = noodParam.attribute("pname").as_string();

                        if (strstr(lpParamName, "sip.rendering") != NULL)
                        {
                            bRender = noodParam.attribute("pval").as_bool();
                        }
                        // 不需要主动发Bye，如被转接到了voicemail
                        else if (strstr(lpParamName, "sip.byeless") != NULL)
                        {
                            bByeLess = noodParam.attribute("pval").as_bool();
                        }
                        // Draft BLA appearance 来自此节点
                        else if (strcmp(lpParamName, "x-line-id") == 0)
                        {
                            sInfo.iIndex = noodParam.attribute("pval").as_int();
                        }

                        noodParam = noodParam.next_sibling("param");
                    }
                }
            }

            xml_node noodRemote = noodDialog.child("remote");
            if (noodRemote)
            {
                xml_node noodIdentity = noodRemote.child("identity");
                if (noodIdentity)
                {
                    yl::string strIdentity = noodIdentity.value();
                    sInfo.strDisplay = noodIdentity.attribute("display").as_string();

                    // display为空的情况，从ID里面取
                    if (sInfo.strDisplay.empty() && !strIdentity.empty())
                    {
                        yl::string strNum, strServer;
                        commonUnits_ParserDialURI(strIdentity, strNum, strServer);
                        sInfo.strDisplay = strNum;
                    }
                }

                xml_node noodTarget = noodRemote.child("target");
                if (noodTarget)
                {
                    sInfo.strURI = noodTarget.attribute("uri").as_string();
                }
            }

            if (pCallBack->GetBlaType() == BLA_DRAFT_BLA)
            {
                // Draft BLA appearance 从0开始，而逻辑层是从1开始
                ++sInfo.iIndex;
            }

            BLF_STATUS_CODE blfStatus = MapBLFStatus(lpState, lpDirect, bRender);
            sInfo.eStatus = Comm_MapBLF2BLAStatus(blfStatus);

            SCA_INFO("id=[%s] stats=[%s] dirct=[%s] prv=[%d] index=[%d] render=[%d] Bye=[%d]", lpDialogID,
                     lpState, lpDirect, bPrivate, sInfo.iIndex, bRender, bByeLess);

            pCallBack->AddSCAInfo(sInfo);

            noodDialog = noodDialog.next_sibling("dialog");
        }
    }

    return true;
}

CBLFParse::CBLFParse() : m_iVersion(0)
{
}
