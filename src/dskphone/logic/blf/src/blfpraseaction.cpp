#include "blfpraseaction.h"
#include "ylstringalgorithm.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

using namespace NS_TA;

CBLFPraseAction::CBLFPraseAction(char * strStr, bool IsPath)
    : m_bIsFull(false)
{
    m_strContent = IsPath ? yl::dump_file(strStr) : strStr;
    m_eTARequestType = TA_REQUEST_XML_BROWSER;
}

CBLFPraseAction::~CBLFPraseAction()
{

}

bool CBLFPraseAction::Exec()
{
    printf("---------------CBLFPraseAction::Exec() begin----------------\n");
    xml_document doc;
    bool suc = false;

    int nIdex1 = m_strContent.find("<list");
    //第一个xml结束位置
    int nIndex2 = m_strContent.find("</list>");

    if (nIdex1 != m_strContent.npos && nIndex2 != m_strContent.npos)
    {
        int nlengh = nIndex2 - nIdex1 + 7;
        char * pStrSub = &m_strContent.at(nIdex1);
        suc = PraseList(pStrSub, nlengh);
    }
    else
    {
        APP_ERR("not find list Node!");
    }

    //第一个xml的开始位置
    nIdex1 = m_strContent.find("<dialog-info", 0);
    //第一个xml结束位置
    nIndex2 = m_strContent.find("</dialog-info>", 0);

    while (nIdex1 != m_strContent.npos && nIndex2 != m_strContent.npos)
    {
        int nlengh = nIndex2 - nIdex1 + 14;
        char * pStrSub = &m_strContent.at(nIdex1);

        //第一个xml的开始位置
        nIdex1 = m_strContent.find("<dialog-info", nIndex2 + 14);
        //第一个xml结束位置
        nIndex2 = m_strContent.find("</dialog-info>", nIndex2 + 14);

        doc.reset();

        doc.load_buffer(pStrSub, nlengh);

        suc |= PraseDialogInfo(doc);
    }

    m_mapDisplayName.clear();

    if (suc)
    {
        SetTaskActionErrorType(NS_TA::TA_NO_ERROR);
    }

    printf("---------------CBLFPraseAction::Exec() end----------------\n");

    return suc;
}

bool CBLFPraseAction::PraseList(char * pStr, int nSize)
{
    xml_document doc;

    doc.load_buffer(pStr, nSize);

    xml_node list = doc.child("list");

    if (list.empty())
    {
        return false;
    }

    xml_attribute status = list.attribute("fullState");

    if (!status.empty())
    {
        m_bIsFull = strcmp("true", status.value()) == 0;
    }

    xml_node nodeChild = list.child("resource");

    while (nodeChild)
    {
        yl::string strUri = nodeChild.attribute("uri").as_string();

        if (strUri.empty())
        {
            nodeChild = nodeChild.next_sibling();
            continue;
        }

        char lpszUri[128] = {0};

        if (sscanf(strUri.c_str(), "sip:%[^:]", lpszUri) == 1)
        {
            strUri = lpszUri;

            memset(lpszUri, 0, 128);
        }

        xml_node nodeDisplayName = nodeChild.child("name");

        yl::string strDisplayName = nodeDisplayName.text().as_string();

        m_mapDisplayName.insert(strUri, strDisplayName);

        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

bool CBLFPraseAction::PraseDialogInfo(xml_document & doc)
{
    xml_node nodeDialogInfo = doc.child("dialog-info");

    if (nodeDialogInfo.empty())
    {
        APP_WARN("CBLFPraseAction::PraseDialogInfo root node not find!");
        return false;
    }

    BLFBaseData * pData = new BLFBaseData();

    pData->m_strKey = nodeDialogInfo.attribute("entity").as_string();

    char lpszUri[128] = {0};

    if (sscanf(pData->m_strKey.c_str(), "sip:%[^:]", lpszUri) == 1)
    {
        pData->m_strKey = lpszUri;

        memset(lpszUri, 0, 128);
    }

    APP_INFO("CBLFPraseAction::PraseDialogInfo key[%s]", pData->m_strKey.c_str());

    //获取远程号码
    if (!pData->m_strKey.empty()
            && sscanf(pData->m_strKey.c_str(), "%[^@]", lpszUri) == 1)
    {
        pData->m_strNumber = lpszUri;
    }
    else
    {
        pData->m_strNumber = pData->m_strKey;
    }

    xml_attribute statusAtt = nodeDialogInfo.attribute("state");

    if (!statusAtt.empty())
    {
        pData->m_bIsFullStatus = strcmp(statusAtt.value(), "full") == 0;
    }

    // http://192.168.1.99/Bug.php?BugID=20717
    if (1 == configParser_GetConfigInt(kszBLFListVersionActive))
    {
        xml_attribute versionAtt = nodeDialogInfo.attribute("version");

        if (!versionAtt.empty())
        {
            pData->m_nVersion = atoi(versionAtt.value());
        }
    }

    for (xml_node nodeDialog = nodeDialogInfo.child("dialog"); !nodeDialog.empty();
            nodeDialog = nodeDialog.next_sibling("dialog"))
    {
        yl::string strDirection;

        yl::string strStatus;

        char * lpValue = NULL;

        xml_attribute attDirection = nodeDialog.attribute("direction");
        if (!attDirection.empty())
        {
            strDirection = attDirection.value();
        }

        xml_node nodeStatus = nodeDialog.child("state");
        if (!nodeStatus)
        {
            continue;
        }

        DialgInfo * pDialog = new DialgInfo;

        pDialog->m_strDialgId = nodeDialog.attribute("id").as_string();

        strStatus = nodeStatus.child_value();

        // 获取状态
        pDialog->m_eStatus = GetStatus(strStatus, strDirection);
#if IF_FEATURE_SHOW_BLF_CALLID
        if (pDialog->m_eStatus == DS_BLF_STATUS_CALLOUT)
        {
            pDialog->m_bCallIn = false;
        }
#endif
        // CallPark
        xml_node parkNode = nodeDialog.child("bw:callpark");

        if (!parkNode.empty())
        {
            pData->AddDialog(pDialog);
            if (pDialog->m_eStatus == DS_BLF_STATUS_TALKING)
            {
                pDialog->m_eStatus = DS_BLF_STATUS_PARK;
            }
            continue;
        }

        // mode 指定特殊的亮灯方式
        xml_node nodeMode = nodeDialog.child("mode");
        if (nodeMode)
        {
            pDialog->m_nMode = atoi(nodeMode.child_value());
        }

        if (pData->m_strDisplayName.empty())
        {
            xml_node nodeLocal = nodeDialog.child("local");
            if (nodeLocal)
            {
                //解析是否是hold状态
                if (pDialog->m_eStatus == DS_BLF_STATUS_TALKING)
                {
                    xml_node nodeTarget = nodeLocal.child("target");
                    if (nodeTarget)
                    {
                        nodeTarget = nodeTarget.child("param");
                        if (nodeTarget)
                        {
                            yl::string strValue = nodeTarget.attribute("pval").value();
                            if (!strValue.empty() && strValue.compare("no") == 0)
                            {
                                pDialog->m_eStatus = DS_BLF_STATUS_HOLD;
                            }
                            else
                            {
                                strValue = nodeTarget.attribute("pvalue").value();
                                if (!strValue.empty() && strValue.compare("no") == 0)
                                {
                                    pDialog->m_eStatus = DS_BLF_STATUS_HOLD;
                                }
                            }
                        }
                    }
                }

                xml_node identity = nodeLocal.child("identity");

                if (!identity.empty())
                {
                    xml_attribute name = identity.attribute("display");

                    pData->m_strDisplayName = name.value();

                    identity = identity.next_sibling("identity");

                    yl::string strValue = identity.child_value();
                    if (!strValue.empty())
                    {
                        int index = strValue.find("ext=");
                        if (index != strValue.npos)
                        {
                            pData->m_strExten = strValue.substr(index + 4);
                        }
                    }
                }
            }
            else
            {
                nodeLocal = nodeDialog.child("local-uri");

                if (!nodeLocal.empty())
                {
                    pData->m_strDisplayName = nodeLocal.attribute("display").value();
                }
            }
        }

        xml_node nodeRemote = nodeDialog.child("remote");
        if (!nodeRemote.empty())
        {
            nodeRemote = nodeRemote.child("identity");
            if (!nodeRemote.empty())
            {
                xml_attribute attName = nodeRemote.attribute("display");

                if (!attName.empty())
                {
                    pDialog->m_strDisplayName = attName.value();
                }
                yl::string strValue = nodeRemote.child_value();

                char lpszUri[128] = {0};

                //获取远程号码
                if (!strValue.empty()
                        && sscanf(strValue.c_str(), "sip:%[^@]", lpszUri) == 1)
                {
                    pDialog->m_strNumber = lpszUri;
                }
            }
        }
        else
        {
            nodeRemote = nodeDialog.child("remote-uri");

            if (!nodeRemote.empty())
            {
                pDialog->m_strDisplayName = nodeRemote.attribute("display-name").value();

                yl::string strValue = nodeRemote.child_value();

                char lpszUri[128] = {0};

                //获取远程号码
                if (!strValue.empty()
                        && sscanf(strValue.c_str(), "sip:%[^@]", lpszUri) == 1)
                {
                    pDialog->m_strNumber = lpszUri;
                }
            }
        }

        pData->AddDialog(pDialog);
    }

    //显示名为空则去list里面获取
    if (pData->m_strDisplayName.empty())
    {
        YLHashMap<yl::string, yl::string>::iterator it = m_mapDisplayName.find(pData->m_strKey);
        if (it != m_mapDisplayName.end())
        {
            pData->m_strDisplayName = it->second;
        }
    }

    m_listData.push_back(pData);

    APP_INFO("Add Monitor name[%s],m_strNumber[%s],m_strKey[%s],strExten[%s]",
             pData->m_strDisplayName.c_str(), pData->m_strNumber.c_str(), pData->m_strKey.c_str(),
             pData->m_strNumber.c_str());

    return true;
}

Dsskey_Status CBLFPraseAction::GetStatus(const yl::string & strStatus,
        const yl::string & strDirection/* = "recipient"*/)
{
    DSSKEY_INFO("CBLFPraseAction: status:%s,strDirection[%s]", strStatus.c_str(), strDirection.c_str());
    // "trying"状态的处理参考网络部对BLF的处理,跟Talk状态一样
    if (strStatus.compare("confirmed") == 0
            || strStatus.compare("trying") == 0
            || strStatus.compare("onthephone") == 0
            || strStatus.compare("Established") == 0)
    {
        return DS_BLF_STATUS_TALKING;
    }
    else if (strStatus.compare("terminated") == 0
             || strStatus.compare("online") == 0
             || strStatus.compare("Open") == 0)
    {
        return DS_BLF_STATUS_IDLE;
    }
    else if (strStatus.compare("proceeding") == 0
             || strStatus.compare("early") == 0
             || strStatus.compare("offline") == 0
             || strStatus.compare("Ringing") == 0)
    {
        if (strDirection.compare("initiator") == 0)
        {
            return DS_BLF_STATUS_CALLOUT;
        }
        else
        {
            return DS_BLF_STATUS_RINGING;
        }
    }
    else if (strStatus.compare("Alerting") == 0)
    {
        return DS_BLF_STATUS_CALLOUT;
    }
    else if (strStatus.compare("parked") == 0)
    {
        return DS_BLF_STATUS_PARK;
    }
    else if (strStatus.compare("hold") == 0)
    {
        return DS_BLF_STATUS_HOLD;
    }
    else if (strStatus.compare("donotdisturb") == 0)
    {
        return DS_BLF_STATUS_DND;
    }
    else
    {
        DSSKEY_INFO("CBLFPraseAction:Unkown status");
        return DS_BLF_STATUS_UNKOWN;
    }
}

void CBLFPraseAction::ClearListInfo()
{
    for (YLList<BLFBaseData *>::ListIterator iter = m_listData.begin(); iter != m_listData.end();
            iter++)
    {
        BLFBaseData * pData = *iter;

        if (pData != NULL)
        {
            delete pData;
        }
    }

    m_listData.clear();
}
