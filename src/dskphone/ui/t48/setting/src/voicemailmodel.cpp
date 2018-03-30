#include "voicemailmodel.h"

#include "account/include/modaccount.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "setting/include/common.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CVoiceMailModel.

bool CVoiceMailModel::GetConfigList(VoiceMailConfigListData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 获取各个账号中设置的语音信箱列表.
    VoiceMailConfigData stVMCD;
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        stVMCD.Reset();
        GetMailNumberByAccountId(&stVMCD, i);

        pData->m_listCfg.push_back(stVMCD);
    }  // for.

    return true;
}

void CVoiceMailModel::SetConfigList(VoiceMailConfigListData & refData)
{
    // 设置各个账号的语音信箱.
    int i = 0;
    YLList<VoiceMailConfigData>::iterator citer = refData.m_listCfg.begin();
    for (; citer != refData.m_listCfg.end() && i < acc_AccountNum(); ++citer, ++i)
    {
        const VoiceMailConfigData & refVMCD = *citer;
        configParser_SetCfgItemStringValue(kszAccountVoiceMailNumber, i, refVMCD.m_strMailNumber.c_str(),
                                           CONFIG_LEVEL_PHONE);
    }  // for.
}

bool CVoiceMailModel::GetInfoList(VoiceMailInfoListData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 获取各个账号当前拥有的Voice Mail.
    VoiceMailInfoData stVMID;
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        stVMID.Reset();
        GetInfoByAccountId(&stVMID, i);
        pData->m_listInfo.push_back(stVMID);
    }  // for.

    return true;
}

bool CVoiceMailModel::GetInfoByAccountId(VoiceMailInfoData * pData,
        int nAccountId) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 获取某个下标对应的Voice Mail信息.
    if (acc_IsAccountIDValid(nAccountId))
    {
        pData->m_strMailNumber = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, nAccountId);
        pData->m_nAccountStaus = (int)acc_GetState(nAccountId);
        //label > displayname > registerName
        yl::string strName = acc_GetAccountShowText(nAccountId);

        pData->m_strAccountDisplayName = strName;
        pData->m_nReadCount = (int)acc_GetReadedVoiceMailNum(nAccountId);
        pData->m_nUnReadCount = (int)acc_GetUnReadVoiceMailNum(nAccountId);
        return true;

    }

    return false;
}

bool CVoiceMailModel::GetMailNumberByAccountId(VoiceMailConfigData * pData,
        int nAccountId) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 获取账号的语音信箱.
    pData->m_strMailNumber = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, nAccountId);
    return true;
}

bool CVoiceMailModel::HasUnRead()
{
    bool bResult = (acc_GetUnReadVoiceMailNum() > 0);
    return bResult;
}

CVoiceMailModel::CVoiceMailModel()
{

}

CVoiceMailModel::~CVoiceMailModel()
{

}

}  // namespace NS_SETTING
