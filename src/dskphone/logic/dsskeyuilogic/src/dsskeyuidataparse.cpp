#include "dsskey_inc.h"

void CDsskeyUIDataParse::Init()
{
    // 解析出厂文件
    ParseFile(XML_FILE_DSSKEY, DSSKEY_ROAD_MOD_FACTORY);
    // 解析自定义文件
    ParseFile(XML_FILE_DSSKEY_CUSTOM, DSSKEY_ROAD_MOD_CUSTOM);

    // 创建静态函数结果
    m_FunLogic.CreateFuctionResult();

    m_FunLogic.SaveRegexp(DSS_TEMP_REGEXP_FILE);

    LogTrace();
}

void CDsskeyUIDataParse::UnInit()
{
    /* 暂时不需要这个函数
    m_FunLogic.UnInit();
    m_vecMsgHook.clear();
    m_vecSoftkey.clear();

    // m_vecStringPool 必须和m_mapIncAttr一同被清空
    m_vecStringPool.clear();
    for (mapDsskeyIncAttr::iterator it = m_mapIncAttr.begin();
        it != m_mapIncAttr.end(); ++it)
    {
        delete it->second;
    }

    for (mapDsskeyList::iterator it = m_mapSet.begin();
        it != m_mapSet.end(); ++it)
    {
        delete it->second;
    }

    m_mapIncAttr.clear();
    m_mapSet.clear();*/
}

bool CDsskeyUIDataParse::ParseFile(LPCSTR lpFileName, DSSKEY_RELOAD_MODE mode)
{
    IF_TimeTrace;
    if (textEMPTY(lpFileName))
    {
        return false;
    }

    xml_document doc;
    xml_parse_result xParseResult = doc.load_file(lpFileName);
    if (xParseResult.status != status_ok)
    {
        DSSKEY_ERR("file=[%s] status=[%d] encode=[%d]", lpFileName, xParseResult.status,
                   xParseResult.encoding);
    }

    xml_node nodeRoot = doc.child(XML_ROOT_NODE_NAME);
    if (nodeRoot.empty())
    {
        return false;
    }

    if (mode < DSSKEY_ROAD_MOD_RECOVER)
    {
        ParseTips(nodeRoot.child(XML_NODE_TIPS));

        ParseFunction(nodeRoot.child(XML_NODE_FORMATE));
        ParseFunction(nodeRoot.child(XML_NODE_FUNCTION));

        ParseDefaultDsskey(nodeRoot.child(XML_NODE_DEFAULT));
        ParseDsskey(nodeRoot.child(XML_NODE_ENUMERATION));
        ParseAuthority(nodeRoot.child(XML_NODE_AUTHORITY));
    }

    PasrseSequence(nodeRoot.child(XML_NODE_SEQUENCE), mode);

    return true;
}

bool CDsskeyUIDataParse::ParseTips(const xml_node& nodeRoot)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeItem = nodeRoot.first_child();
    while (!nodeItem.empty())
    {
        // ID
        int iTipsID = nodeItem.attribute(XML_ATTR_ID).as_int();
        // Tips
        yl::string strTips = nodeItem.attribute(XML_ATTR_VALUE).as_string();

        m_FunLogic.CreateTips(iTipsID, strTips);
        nodeItem = nodeItem.next_sibling();
    }

    return true;
}

bool CDsskeyUIDataParse::ParseFunction(const xml_node& nodeRoot)
{
    IF_TimeTrace;
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeItem = nodeRoot.first_child();
    while (!nodeItem.empty())
    {
        // ID
        yl::string strID = nodeItem.attribute(XML_ATTR_ID).as_string();
        // body
        yl::string strValue = nodeItem.attribute(XML_ATTR_VALUE).as_string();
        // type
        LPCSTR lpType = nodeItem.attribute(XML_ATTR_TYPE).as_string();
        // Tips
        int iTipsID = nodeItem.attribute(XML_ATTR_TIPS).as_int();

        trim_both(strID);
        trim_both(strValue);

        if (m_FunLogic.CreateFuncData(strID, lpType, strValue, iTipsID))
        {
            int iMsgID = nodeItem.attribute(XML_ATTR_MSG_ID).as_int();
            if (iMsgID != 0 && !Find(m_vecMsgHook, iMsgID))
            {
                m_vecMsgHook.push_back(iMsgID);
            }
        }

        nodeItem = nodeItem.next_sibling();
    }

    return true;
}

bool CDsskeyUIDataParse::ParseAuthority(const xml_node& nodeRoot)
{
    IF_TimeTrace;
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeItem = nodeRoot.first_child();
    while (!nodeItem.empty())
    {
        int iDsskeyType = nodeItem.attribute(XML_ATTR_ID).as_int();
        mapDsskeyIncAttr::iterator it = m_mapIncAttr.find(iDsskeyType);

        if (it != m_mapIncAttr.end())
        {
            LPCSTR lpAuth = nodeItem.attribute(XML_ATTR_VALUE).as_string();
            if (!textEMPTY(lpAuth))
            {
                CDsskeyInrInfo& IncInfo = *(it->second);

                // For LockDown
                vecString vecList;
                if (::ParseString(vecList, lpAuth, "| "))
                {
                    size_t ilevel = GetLockDownLevel();
                    const yl::string& strAuth =
                        vecList.size() > ilevel ? vecList[ilevel] : vecList[vecList.size() - 1];
                    strncpy(IncInfo.chAuth, strAuth.c_str(), MAX_MODE_LENGTH);
                }

                IF_LOG_TRACE("type=[%d] value=[%s]", iDsskeyType, lpAuth);
            }
        }

        nodeItem = nodeItem.next_sibling();
    }

    return true;
}

bool CDsskeyUIDataParse::PasrseSequence(const xml_node& nodeRoot, DSSKEY_RELOAD_MODE mode)
{
    IF_TimeTrace;
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeItem = nodeRoot.first_child();
    while (!nodeItem.empty())
    {
        DSSKEY_UI_TYPE eMoudle = GetMoudleIndex(nodeItem.name());
        if (eMoudle != DSSKEY_UI_NONE)
        {
            DsskeyMoudleSet* pMoudleSet = NULL;
            mapDsskeyList::iterator it = m_mapSet.find(eMoudle);
            if (it == m_mapSet.end())
            {
                pMoudleSet = new DsskeyMoudleSet;
                m_mapSet[eMoudle] = pMoudleSet;
            }
            else
            {
                pMoudleSet = it->second;
            }

            // list
            LPCSTR lpKeyList = nodeItem.attribute(XML_ITEM_KEY_LIST).as_string();
            FillListData(lpKeyList, pMoudleSet->vecDsskeyList, pMoudleSet->vecDsskeyEvent, DT_NA);

#ifdef IF_DEBUG
            yl::string strOut;
            for (vecDsskeySet::iterator itx = pMoudleSet->vecDsskeyList.begin();
                itx != pMoudleSet->vecDsskeyList.end(); ++itx)
            {
                strOut += commonAPI_FormatString("%d,", *itx);
            }

            TALK_WARN("list mode[%d] module[%d] str[%s] out[%s]", mode, eMoudle, lpKeyList, strOut.c_str());
#endif
            // key_event
            LPCSTR lpKeyEvent = nodeItem.attribute(XML_ITEM_KEY_EVENT).as_string();
            FillListData(lpKeyEvent, pMoudleSet->vecDsskeyEvent, pMoudleSet->vecDsskeyList, DT_KEYEVENT);

#ifdef IF_DEBUG
            strOut.clear();
            for (vecDsskeySet::iterator itx = pMoudleSet->vecDsskeyEvent.begin();
                itx != pMoudleSet->vecDsskeyEvent.end(); ++itx)
            {
                strOut += commonAPI_FormatString("%d,", *itx);
            }

            TALK_WARN("event mode[%d] module[%d] str[%s] out[%s]", mode, eMoudle, lpKeyEvent, strOut.c_str());
#endif
        }

        nodeItem = nodeItem.next_sibling();
    }

    return true;
}

bool CDsskeyUIDataParse::ParseDsskey(const xml_node& nodeRoot)
{
    IF_TimeTrace;
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node nodeItem = nodeRoot.first_child();
    while (!nodeItem.empty())
    {
        xml_node nodeTmp = nodeItem;
        nodeItem = nodeItem.next_sibling();

        int iDsskeyType = nodeTmp.attribute(XML_ATTR_ID).as_int();

#ifndef IF_BT_SUPPORT_PHONE
        if (iDsskeyType == DT_BTLINE)
        {
            continue;
        }
#endif

        CDsskeyInrInfo* pIncInfo = NULL;
        mapDsskeyIncAttr::iterator it = m_mapIncAttr.find(iDsskeyType);
        if (it != m_mapIncAttr.end())
        {
            pIncInfo = it->second;
        }
        else
        {
            pIncInfo = new CDsskeyInrInfo;
            m_mapIncAttr[iDsskeyType] = pIncInfo;
        }

        // 解析单个dsskey
        if (!ParseDsskeyAttr(nodeTmp, *pIncInfo))
        {
            IF_LOG_TRACE("dsskey type [%d][%s] invalid skiped", iDsskeyType, pIncInfo->strDisplayName.c_str());
            delete pIncInfo;
            pIncInfo = NULL;
            m_mapIncAttr.erase(iDsskeyType);
            continue;
        }
    }

    return true;
}

bool CDsskeyUIDataParse::ParseDefaultDsskey(const xml_node& nodeRoot)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    // 解析默认数据
    return ParseDsskeyAttr(nodeRoot.child(XML_SUB_NODE_ITEM), m_DefaultData);
}

bool CDsskeyUIDataParse::ParseDsskeyAttr(const xml_node& nodeRoot,
        CDsskeyInrInfo& dsskeyIncAttrData)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    if (!IsDefaultData(dsskeyIncAttrData))
    {
        // 显示名不能为空
        if (!GetXmlAttrValue(nodeRoot, XML_ATTR_DISPLAY_NAME, dsskeyIncAttrData.strDisplayName)
                || dsskeyIncAttrData.strDisplayName.empty())
        {
            return false;
        }
    }

    // function id
    GetXmlAttrValue(nodeRoot, XML_ATTR_FUNCTION, dsskeyIncAttrData.strFunID);
    // uri
    GetXmlAttrValue(nodeRoot, XML_ATTR_URI, dsskeyIncAttrData.strURI);
    // redirect
    dsskeyIncAttrData.iRedirectType = nodeRoot.attribute(XML_ATTR_REDIRECT).as_int();

    // contract type
    LPCSTR lpClassName = nodeRoot.attribute(XML_ATTR_CLASS).as_string();
    dsskeyIncAttrData.iClassType = GetDirType(lpClassName);

    IF_LOG_TRACE("uri=[%s] display=[%s] fun=[%s] class=[%s][%d]",
                 dsskeyIncAttrData.strURI.c_str(), dsskeyIncAttrData.strDisplayName.c_str(),
                 dsskeyIncAttrData.strFunID.c_str(), lpClassName, dsskeyIncAttrData.iClassType);

    xml_node nodeAttr = nodeRoot.first_child();
    while (!nodeAttr.empty())
    {
        // 解析单条属性
        ParseDsskeySingleAttr(nodeAttr, dsskeyIncAttrData);
        nodeAttr = nodeAttr.next_sibling();
    }

    return true;
}

bool CDsskeyUIDataParse::ParseDsskeySingleAttr(const xml_node& nodeRoot,
        CDsskeyInrInfo& dsskeyIncAttrData)
{
    if (nodeRoot.empty())
    {
        return false;
    }

    LPCSTR lpNodeName = nodeRoot.name();
    int iAttrType = GetAttrType(lpNodeName);
    // 非法节点不继续解析
    if (DSSKEY_ATTR_ITEM_NONE == iAttrType)
    {
        DSSKEY_WARN("unkown item name=[%s]", lpNodeName);
        return false;
    }

    CDsskeyItemAttr* pAttr = dsskeyIncAttrData.Find(iAttrType);
    if (pAttr == NULL)
    {
        pAttr = dsskeyIncAttrData.CreateItem(iAttrType);
        if (pAttr == NULL)
        {
            return false;
        }

        if (!IsDefaultData(dsskeyIncAttrData))
        {
            CDsskeyItemAttr* pDefaaultAttr = m_DefaultData.Find(iAttrType);
            if (pDefaaultAttr != NULL)
            {
                // 先从Default节点加载默认值
                (*pAttr) = (*pDefaaultAttr);
            }
        }
    }

    // visibled readonly 有可能是函数控制、即使隐藏不可见也需要继续加载
    yl::string strTmp;
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_VISIBLED, strTmp))
    {
        pAttr->SetVisible(CreatPool(strTmp));
    }

    // 只读属性
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_READONLY, strTmp))
    {
        pAttr->SetReadOnly(CreatPool(strTmp));
    }

    // 输入法
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_IME, strTmp))
    {
        pAttr->SetIme(CreatPool(strTmp));
    }

    // 显示为
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_NAME, strTmp))
    {
        pAttr->SetName(CreatPool(strTmp));
    }

    // 正则表达式校验值
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_FORMATE, strTmp))
    {
        pAttr->SetRegexpID(CreatPool(strTmp));
    }

    // 正则表达式校验值
    if (GetXmlAttrValue(nodeRoot, XML_ATTR_DEFAULT, strTmp))
    {
        pAttr->SetDefault(CreatPool(strTmp));
    }

    xml_attribute xmlAttr = nodeRoot.attribute(XML_ATTR_PASSWD);
    if (!xmlAttr.empty())
    {
        pAttr->bPasswd = xmlAttr.as_bool();
    }

    //显示联系人关联按钮的方式
    xmlAttr = nodeRoot.attribute(XML_ATTR_BUTTON_TYPE);
    if (!xmlAttr.empty())
    {
        pAttr->SetButtonType(DSSKEY_BUTTON_TYPE(xmlAttr.as_int()));
    }

    IF_LOG_TRACE("item name=[%s] ime=[%s] name=[%s] exp=[%s]",
                 nodeRoot.name(), pAttr->GetIme().c_str(), pAttr->GetName().c_str(), pAttr->GetRegexp().c_str());

    return true;
}

bool CDsskeyUIDataParse::FillListData(LPCSTR lpListData, vecDsskeySet& vecSet,
                                      vecDsskeySet& vecSetRef, int iSkipType)
{
    if (lpListData == NULL)
    {
        return false;
    }

    // 跳过空格
    while (*lpListData == ' ')
    {
        ++lpListData;
    }

    // 是否剔除模式
    bool bRemoveMask = false;
    bool bAppendMask = false;

    if (*lpListData == '-')
    {
        bRemoveMask = true;
        ++lpListData;
    }
    else if (*lpListData == '+')
    {
        bAppendMask = true;
        ++lpListData;
    }

    YLVector<int> vecList;
    if (!ParseString(vecList, lpListData, ", "))
    {
        return false;
    }

    bool bSkip = true;
    for (YLVector<int>::iterator it = vecList.begin(); it != vecList.end(); ++it)
    {
        if (*it != iSkipType)
        {
            bSkip = false;
            break;
        }
    }

    if (bSkip)
    {
        return false;
    }

    //DSSKEY_INFO("FillListData iSkipType base=[%d] type=[%s]", iSkipType, lpListData);

    // 覆盖模式
    if (!bRemoveMask) // && !bAppendMask)
    {
        vecSet.clear();
        //! 作为兼容，NA类型一定是存在的，且为第一个
        if (iSkipType == DT_NA)
        {
            vecSet.push_back(iSkipType);
        }
    }

    for (YLVector<int>::iterator it = vecList.begin(); it != vecList.end(); ++it)
    {
        int iType = *it;

        //! NA不可以剔除和添加
        if (iType <= 0)
        {
            continue;
        }

        if (bRemoveMask)
        {
            // 负数执行编辑模式剔除之
            vecDsskeySet::iterator iter = yl::find(vecSet.begin(), vecSet.end(), iType);
            if (iter != vecSet.end())
            {
                vecSet.erase(iter);
            }

            // 剔除list项目的时候，event列表也需要做同步删除
            if (iSkipType == DT_NA)
            {
                vecDsskeySet::iterator iter = yl::find(vecSetRef.begin(), vecSetRef.end(), iType);
                if (iter != vecSetRef.end())
                {
                    vecSetRef.erase(iter);
                }
            }
        }
        else
        {
            // 存在对应的配置，且没有重复
            if (iType == iSkipType
                    || (iType != DT_KEYEVENT && m_mapIncAttr.count(iType) <= 0)
                    || Find(vecSet, iType)
                    || (iSkipType != DT_NA && Find(vecSetRef, iType)))
            {
                //DSSKEY_INFO("FillListData skip base=[%d] type=[%d]", iSkipType, iType);
                continue;
            }

            vecSet.push_back(iType);
        }
    }

    return vecSet.size() > 0;
}

bool CDsskeyUIDataParse::GetXmlAttrValue(const xml_node& nodeRoot, LPCSTR lpAttr,
        yl::string& strValue)
{
    if (nodeRoot.empty() || textEMPTY(lpAttr))
    {
        return false;
    }

    // as string
    xml_attribute xmlAttr = nodeRoot.attribute(lpAttr);
    if (!xmlAttr.empty())
    {
        strValue = xmlAttr.as_string();
        return true;
    }

    return false;
}

DSSKEY_UI_TYPE CDsskeyUIDataParse::GetMoudleIndex(LPCSTR lpName)
{
    mapItem s_moudle_info_arr[] =
    {
        { DSSKEY_UI_NONE,     NULL                      },
        { DSSKEY_UI_SOFTKEY,  DSSKEY_MOUDLE_PROGRAMKEY  },
        { DSSKEY_UI_LINEKEY,  DSSKEY_MOUDLE_LINEKEY     },
        { DSSKEY_UI_EXPKEY,   DSSKEY_MOUDLE_EXPKEY      },
    };

    return (DSSKEY_UI_TYPE)StrMap2KeyEx(lpName, s_moudle_info_arr);
}

DIR_TYPE CDsskeyUIDataParse::GetDirType(LPCSTR lpName)
{
    mapItem s_class_info_arr[] =
    {
        { BASE_DIR,         NULL                    },
        { REMOTE_DIR,       XML_ATTR_REMOTE_DIR     },
        { LOCAL_DIR,        XML_ATTR_LOCAL_DIR      },
        { BSFT_DIR,         XML_ATTR_BSFT_DIR       },
        { LOCAL_LOG,        XML_ATTR_LOCAL_LOG      },
        { BLUETOOTH_DIR,    XML_ATTR_BLUETOOTH_DIR  },
        { PLCM_DIR,         XML_ATTR_PLCM_DIR       },
        { LDAP_DIR,         XML_ATTR_LDAP_DIR       },
        { UC_DIR,           XML_ATTR_UC_DIR         },
        { CLOUD_DIR,        XML_ATTR_CLOUD_DIR      },
        { GB_DIR,           XML_ATTR_GB_DIR         },
        { MTSW_DIR,         XML_ATTR_MTSW_DIR       },
        { MTSW_LOG,         XML_ATTR_MTSW_LOG       },
        { BSFT_LOG,         XML_ATTR_BSFT_LOG       },
    };

    return (DIR_TYPE)StrMap2KeyEx(lpName, s_class_info_arr);
}

DSSKEY_ATTR_ITEM CDsskeyUIDataParse::GetAttrType(LPCSTR lpName)
{
    mapItem s_attr_info_arr[] =
    {
        { DSSKEY_ATTR_ITEM_NONE,        NULL                    },
        { DSSKEY_ATTR_ITEM_LABEL,       XML_NODE_NAME_LABEL     },
        { DSSKEY_ATTR_ITEM_VALUE,       XML_NODE_NAME_VALUE     },
        { DSSKEY_ATTR_ITEM_EXT,         XML_NODE_NAME_EXTENSION },
        { DSSKEY_ATTR_ITEM_LINE,        XML_NODE_NAME_LINE      },
        { DSSKEY_ATTR_ITEM_INDEX,       XML_NODE_NAME_INDEX     },
        { DSSKEY_ATTR_ITEM_LINE_VALUE,  XML_NODE_NAME_LINEVALUE },
    };

    return (DSSKEY_ATTR_ITEM)StrMap2KeyEx(lpName, s_attr_info_arr);
}

int CDsskeyUIDataParse::GetLockDownLevel()
{
    int iLevel = configParser_GetConfigInt(kszLockLevel);
    return iLevel > 0 ? iLevel : 0;
}

CDsskeyInrInfo& CDsskeyUIDataParse::GetDsskeyXmlData(int iDsskeyType)
{
    mapDsskeyIncAttr::iterator it = m_mapIncAttr.find(iDsskeyType);
    if (it != m_mapIncAttr.end())
    {
        return *(it->second);
    }

    // return default data
    return m_DefaultData;
}

bool CDsskeyUIDataParse::IsDefaultData(const CDsskeyInrInfo& dsskeyInfo)
{
    return &dsskeyInfo == &m_DefaultData;
}

bool CDsskeyUIDataParse::Find(vecDsskeySet& vecSet, int iValue)
{
    return yl::find(vecSet.begin(), vecSet.end(), iValue) != vecSet.end();
}

bool CDsskeyUIDataParse::GetFunctionResult(yl::string& strID)
{
    return m_FunLogic.OnLoadAction(strID);
}

yl::string* CDsskeyUIDataParse::CreatPool(const yl::string& strValue)
{
    // 缓存池找到了，直接返回
    YLList<yl::string>::iterator it = yl::find(m_listString.begin(), m_listString.end(), strValue);
    if (it != m_listString.end())
    {
        return &(*it);
    }

    // 添加到缓存池
    m_listString.push_back(strValue);
    return &(m_listString.back());
}

void CDsskeyUIDataParse::Reload()
{
    IF_TimeTrace;
    // 恢复模式执行factory
    ParseFile(XML_FILE_DSSKEY, DSSKEY_ROAD_MOD_RECOVER);

    // 解析自定义文件
    ParseFile(XML_FILE_DSSKEY_CUSTOM, DSSKEY_ROAD_MOD_CUSTOM);
}

bool CDsskeyUIDataParse::ParseString(YLVector<int> & veclist, const yl::string & strSrc, const yl::string & filterStr)
{
    if (!strSrc.empty())
    {
        size_t begpos = 0;
        size_t found = strSrc.find_first_of(filterStr, begpos);
        while (found != yl::string::npos)
        {
            if (begpos < found)
            {
                yl::string str = strSrc.substr(begpos, found - begpos);
                if (!str.empty() && str[0] != '+' && str[0] != '-')
                {
                    veclist.push_back(atoi(str.c_str()));
                }
            }

            begpos = found + 1;
            found = strSrc.find_first_of(filterStr, begpos);
        }

        if (begpos < strSrc.length())
        {
            // 剩余字符串
            yl::string str = strSrc.substr(begpos);
            if (!str.empty() && str[0] != '+' && str[0] != '-')
            {
                veclist.push_back(atoi(str.c_str()));
            }
        }
    }

    return veclist.size() > 0;
}

void CDsskeyUIDataParse::LogTrace()
{
#ifdef IF_DEBUG
    IF_TimeTrace;
    int iIndex = 1;
    fprintf(stderr, "---------------paris pool begin---------------\n");
    for (vecString::iterator iter = m_vecStringPool.begin();
            iter != m_vecStringPool.end(); ++iIndex, ++iter)
    {
        fprintf(stderr, " pairs=[%02d][%s]\n", iIndex, iter->c_str());
    }
    fprintf(stderr, "---------------paris pool end---------------\n");

    iIndex = 1;
    fprintf(stderr, "---------------paris msg list begin---------------\n");
    for (vecDsskeySet::iterator iter = m_vecMsgHook.begin();
            iter != m_vecMsgHook.end(); ++iIndex, ++iter)
    {
        fprintf(stderr, " pairs=[%02d][%d]\n", iIndex, *iter);
    }
    fprintf(stderr, "---------------paris msg list end---------------\n");

    iIndex = 1;
    fprintf(stderr, "---------------paris key list begin---------------\n");
    for (mapDsskeyList::iterator iter = m_mapSet.begin();
            iter != m_mapSet.end(); ++iIndex, ++iter)
    {
        DsskeyMoudleSet& set = *(iter->second);
        for (vecDsskeySet::iterator it = set.vecDsskeyList.begin();
                it != set.vecDsskeyList.end(); ++iIndex, ++it)
        {
            fprintf(stderr, " pairs=[%02d][%d]\n", iIndex, *it);
        }
        fprintf(stderr, "    -----------middle line----------    \n");
        for (vecDsskeySet::iterator it = set.vecDsskeyEvent.begin();
                it != set.vecDsskeyEvent.end(); ++iIndex, ++it)
        {
            fprintf(stderr, " pairs=[%02d][%d]\n", iIndex, *it);
        }
        fprintf(stderr, "    -----------single node finished----------    \n");
    }
    fprintf(stderr, "---------------paris key list end---------------\n");

    iIndex = 1;
    fprintf(stderr, "---------------paris key map begin---------------\n");
    for (mapDsskeyIncAttr::iterator iter = m_mapIncAttr.begin();
            iter != m_mapIncAttr.end(); ++iIndex, ++iter)
    {
        CDsskeyInrInfo& info = *(iter->second);

        char buf[4] = {0};
        memcpy(buf, info.chAuth, 3);
        fprintf(stderr, " key=[%02d] author=[%3s] class=[%d] uri=[%s] dsp=[%s] fun=[%s]\n",
                iIndex, buf, info.iClassType,
                info.strURI.c_str(), info.strDisplayName.c_str(), info.strFunID.c_str());

        int iSecIndex = 1;
        VecAttrItem& item = info.vecInrAttr;
        for (VecAttrItem::iterator it = item.begin();
                it != item.end(); ++iSecIndex, ++it)
        {
            CDsskeyItemAttr& attr = *(*it);
            fprintf(stderr,
                    "  attr=[%02d] type=[%d] read=[%d] write=[%d] passwd=[%d] ime=[%s] name=[%s] reg=[%s]\n",
                    iSecIndex, attr.iAttrType, attr.IsVisible(), attr.IsReadOnly(), attr.bPasswd,
                    attr.GetIme().c_str(), attr.GetName().c_str(), attr.GetRegexp().c_str());
        }
        fprintf(stderr, "    -----------single node finished----------    \n");
    }
    fprintf(stderr, "---------------paris key map end---------------\n\n");
#endif
}
