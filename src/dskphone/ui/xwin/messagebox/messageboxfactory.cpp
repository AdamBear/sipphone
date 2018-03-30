#include "messageboxfactory.h"

#include "notemessagebox.h"
#include "commonmessagebox.h"
#include "passwordmessagebox.h"

CMessageboxFactory::CMessageboxFactory(MESSAGEBOX_TYPE eMsgType)
    : m_eMsgType(eMsgType)
{
    m_vecMsgBoxCacheInfo.clear();
}

CMessageBoxBase * CMessageboxFactory::GetMsgBox()
{
    int nMsgBoxCount = m_vecMsgBoxCacheInfo.size();
    if (0 == nMsgBoxCount)
    {
        return CreateMsgBoxAndAddToCache();
    }

    for (int i = 0; i < nMsgBoxCount; ++i)
    {
        MsgBoxCacheInfo & tempCacheInfo = m_vecMsgBoxCacheInfo[i];
        if (tempCacheInfo.m_bUsed)
        {
            continue;
        }
        else
        {
            if (NULL != tempCacheInfo.m_pMessageBox)
            {
                tempCacheInfo.m_bUsed = true;
                return tempCacheInfo.m_pMessageBox;
            }
            else
            {
                //
            }
        }
    }

    //如果没有找到未使用的弹框，则需要添加
    return CreateMsgBoxAndAddToCache();
}

CMessageBoxBase * CMessageboxFactory::CreateMsgBoxAndAddToCache()
{
    MsgBoxCacheInfo tempCacheInfo;
    tempCacheInfo.m_pMessageBox = GreateMsgBox(m_eMsgType);

    if (NULL != tempCacheInfo.m_pMessageBox)
    {
        tempCacheInfo.m_bUsed = true;
        m_vecMsgBoxCacheInfo.push_back(tempCacheInfo);
        return tempCacheInfo.m_pMessageBox;
    }

    return NULL;
}

void CMessageboxFactory::PutMsgBox(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    iterCacheInfo iter = m_vecMsgBoxCacheInfo.begin();
    for (; iter != m_vecMsgBoxCacheInfo.end(); ++iter)
    {
        MsgBoxCacheInfo & tempCacheInfo = (*iter);
        if (pMessageBox == tempCacheInfo.m_pMessageBox)
        {
            tempCacheInfo.m_bUsed = false;
            tempCacheInfo.m_pMessageBox->UninitMsgBox();
            return;
        }
    }

#ifndef _WIN32
#warning deletelater
#endif
    delete (pMessageBox);
}

CMessageBoxBase * CMessageboxFactory::GreateMsgBox(MESSAGEBOX_TYPE eMsgType)
{
    switch (eMsgType)
    {
    case MESSAGEBOX_NOTE:
        {
            return new CNoteMessageBox();
        }
        break;
    case MESSAGEBOX_Cancel:
    case MESSAGEBOX_OkCancel:
    case MESSAGEBOX_SPECIAL:
    case MESSAGEBOX_AUDIO_TEST:
        {
            return new CCommonMessageBox();
        }
        break;
    case MESSAGEBOX_PWD:
    case MESSAGEBOX_UNLOCK:
    case MESSAGEBOX_EDK:
        {
            return new CPasswordMessageBox();
        }
        break;
    default:
        {
            return NULL;
        }
        break;
    }

    return NULL;
}
