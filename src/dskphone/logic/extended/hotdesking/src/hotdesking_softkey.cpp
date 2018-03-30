#include "hotdesking_inc.h"

#ifdef IF_FEATURE_HOTDESKING
#ifdef IF_ADVANCE_HOTDESKING

IMPLEMENT_GETINSTANCE(HotDesking_SoftKey)

HotDesking_SoftKey::HotDesking_SoftKey()
{
    m_iSoftKeyNum = 0;
    m_iFirstKeyIndex = 0;
    for (int i = 0; i < MAXKEYNUM + 1; i++)
    {
        m_iKeyIndex[i] = -1;
        memset(&m_pDeskKey[i], 0, sizeof(DeskKey_Info));
    }
    m_iMaxKeyNum = 4;
    m_bIsLoadAllCfg = FALSE;
}

HotDesking_SoftKey::~HotDesking_SoftKey()
{
    for (int i = 0; i < MAXKEYNUM + 1; i++)
    {
        FreeString(m_pDeskKey[i]);
    }
}

VOID HotDesking_SoftKey::LoadSoftKey()
{
    if (!pathFileExist(szDeskingKeyFile))
    {
        HOTDESKING_WARN("Softkey of Hot Desking does not exist!");
        return;
    }

    xml_document doc;

    m_bIsLoadAllCfg = FALSE;
    if (doc.load_file(szDeskingKeyFile))
    {
        xml_node nodeXmlNode = doc.child("HotDeskingKey");
        if (!nodeXmlNode.empty())
        {
            int iIndex = 0;
            const char * pstr = NULL;
            nodeXmlNode = nodeXmlNode.first_child();
            while (!nodeXmlNode.empty())
            {
                pstr = nodeXmlNode.name();
                if (strncmp(pstr, "softkey", 7) == 0)
                {
                    iIndex = atoi(pstr + 7);
                    AddAllKeyInfo(iIndex - 1, nodeXmlNode);
                    if (iIndex == MAXKEYNUM)
                    {
                        m_bIsLoadAllCfg = TRUE;
                    }
                }
                nodeXmlNode = nodeXmlNode.next_sibling();
            }
        }
    }
    ArrangeSoftKey();
}

VOID HotDesking_SoftKey::ArrangeSoftKey()
{
    m_iSoftKeyNum = 0;
    for (int i = 0; i < MAXKEYNUM; i++)
    {
        if (m_pDeskKey[i].pLabel != NULL)
        {
            if (*m_pDeskKey[i].pLabel != '\0')
            {
                m_iKeyIndex[m_iSoftKeyNum] = i;
                m_iSoftKeyNum++;
            }
        }
    }
    // 如果SoftKey有值,则增加Menu按键
    if (m_iSoftKeyNum > 0)
    {
        AddMenuKey();
    }
    m_iFirstKeyIndex = 0;
}

// 获取Softkey文本
VOID HotDesking_SoftKey::GetSoftKeyTextByIndex(int iIndex, yl::string & strText)
{
    if (iIndex < 0 || iIndex >= m_iMaxKeyNum)
    {
        return;
    }

    if (m_iFirstKeyIndex + iIndex < MAXKEYNUM + 1)
    {
        int itmp = m_iKeyIndex[m_iFirstKeyIndex + iIndex];
        if (itmp >= 0 && itmp < MAXKEYNUM + 1)
        {
            if (m_pDeskKey[itmp].pLabel != NULL)
            {
                strText = m_pDeskKey[itmp].pLabel;
                return;
            }
        }
    }

    strText = "";
}

VOID HotDesking_SoftKey::On_FuncKeyPress(int iKey)
{
    if (iKey == PHONE_KEY_FN4 && m_iSoftKeyNum > m_iMaxKeyNum)
    {
        ProcessSoftKeyMore();
        return;
    }
    int iIndex = iKey - PHONE_KEY_FN1;

    if (iIndex < 0 || iIndex >= m_iMaxKeyNum)
    {
        return;
    }
    if (m_iFirstKeyIndex + iIndex >= MAXKEYNUM + 1)
    {
        return;
    }
    int itmp = m_iKeyIndex[m_iFirstKeyIndex + iIndex];
    if (itmp < 0 || itmp >= MAXKEYNUM + 1)
    {
        return;
    }
    if (m_pDeskKey[itmp].pValue == NULL)
    {
        return;
    }
    ProcessURI(m_pDeskKey[itmp].pValue);
}

VOID HotDesking_SoftKey::ProcessURI(LPCSTR pstr)
{
    if (NULL == pstr
            || *pstr == '\0')
    {
        return;
    }

    if (strncmp(pstr, "http:", 5) == 0
            || strncmp(pstr, "https:", 6) == 0)
    {
        yl::string str = pstr;
        ReplaceURL(str);
        msgPostMsgToThreadEx(msgGetThreadByName(VP_LOAD_THREAD_NAME), XMLBROWSER_MSG_LOAD_XML, 0, 0,
                             str.length() + 1, str.size());
    }
    else if (strcmp(pstr, "Menu") == 0)
    {
        UIManager_OnFunckeyPress(PHONE_KEY_MENU, FALSE);
    }
}

VOID HotDesking_SoftKey::ProcessSoftKeyMore()
{
    if (m_iSoftKeyNum > m_iMaxKeyNum)
    {
        m_iFirstKeyIndex += m_iMaxKeyNum - 1;
        if (m_iFirstKeyIndex >= m_iSoftKeyNum)
        {
            m_iFirstKeyIndex = 0;
        }
    }

    UIManager_UpdateWnd();
    //SetSoftKey();
}

VOID HotDesking_SoftKey::ReplaceURL(yl::string & str)
{
    if (str.empty())
    {
        return;
    }
    char pBuffer[100] = "";
    strcpy(pBuffer, acc_GetRegisterName(0));
    string_replace(str, "$$SIPUSERNAME$$", pBuffer);
}

VOID HotDesking_SoftKey::AddAllKeyInfo(int iIndex, xml_node nodeXmlNode)
{
    if (iIndex < 0 || iIndex >= MAXKEYNUM || nodeXmlNode.empty())
    {
        return;
    }

    yl::string str[3];
    GetValue(nodeXmlNode.child("type"), str[0]);
    GetValue(nodeXmlNode.child("label"), str[1]);
    GetValue(nodeXmlNode.child("value"), str[2]);

    AddKeyInfo(m_pDeskKey[iIndex], str[0], str[1], str[2]);
}

VOID HotDesking_SoftKey::AddKeyInfo(DeskKey_Info & ToKey, const yl::string & pType,
                                    const yl::string & pLabel, const yl::string & pValue)
{
    FreeString(ToKey, !pType.empty(), !pLabel.empty(), !pValue.empty());

    if (!pType.empty())
    {
        ToKey.pType = (char *)malloc(pType.size() + 1);
        strcpy(ToKey.pType, pType.c_str());
    }

    if (!pLabel.empty())
    {
        ToKey.pLabel = (char *)malloc(pLabel.size() + 1);
        strcpy(ToKey.pLabel, pLabel.c_str());
    }

    if (!pValue.empty())
    {
        ToKey.pValue = (char *)malloc(pValue.size() + 1);
        strcpy(ToKey.pValue, pValue.c_str());
    }
}

BOOL HotDesking_SoftKey::GetValue(xml_node nodeXmlNode, yl::string & str)
{
    str = "";
    if (!nodeXmlNode.empty())
    {
        str = nodeXmlNode.text().get();
        return TRUE;
    }
    return FALSE;
}

VOID HotDesking_SoftKey::FreeString(DeskKey_Info & ToKey, BOOL bFreeType, BOOL bFreeLabel,
                                    BOOL bFreeValue)
{
    if (bFreeType && ToKey.pType != NULL)
    {
        free(ToKey.pType);
        ToKey.pType = NULL;
    }
    if (bFreeLabel && ToKey.pLabel != NULL)
    {
        free(ToKey.pLabel);
        ToKey.pLabel = NULL;
    }
    if (bFreeValue && ToKey.pValue != NULL)
    {
        free(ToKey.pValue);
        ToKey.pValue = NULL;
    }
}

VOID HotDesking_SoftKey::ClearAllKeyInfo()
{
    m_iSoftKeyNum = 0;
    m_iFirstKeyIndex = 0;
    for (int i = 0; i < MAXKEYNUM + 1; i++)
    {
        m_iKeyIndex[i] = -1;
        FreeString(m_pDeskKey[i]);
    }
    ClearFileInfo();
}

VOID HotDesking_SoftKey::ClearFileInfo()
{
    // 清空文件
    xml_document doc;
    doc.save_file(szDeskingKeyFile);
}

VOID HotDesking_SoftKey::AddMenuKey()
{
    AddKeyInfo(m_pDeskKey[MAXKEYNUM], "Idle", "Menu", "Menu");
    m_iKeyIndex[m_iSoftKeyNum] = MAXKEYNUM;
    m_iSoftKeyNum++;
}

BOOL HotDesking_SoftKey::OnSIPMessage(msgObject & objMessage)
{
    if (objMessage.message == SIP_SUB_HOT_DESKING_NOTIFY
            && objMessage.GetExtraData() == NULL) // 如果ExtraData不为空,则是pushxml消息
    {
        HOTDESKING_INFO("HotDesking_SoftKey ProcessNotify");
        return ProcessNotify(objMessage.wParam);
    }
    return 0;
}

BOOL HotDesking_SoftKey::ProcessNotify(int iLineId)
{

    if (acc_IsAccountIDValid(iLineId))
    {
        ProcessURI(configParser_GetCfgItemStringValue(kszAccountHotDeskingSipNotifyURL, iLineId));
        return 0;
    }
    return -1;
}

#endif  //IF_ADVANCE_HOTDESKING
#endif // #ifdef IF_FEATURE_HOTDESKING
