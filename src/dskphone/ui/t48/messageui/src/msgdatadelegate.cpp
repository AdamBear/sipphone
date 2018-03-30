#include "msgdatadelegate.h"
#include "cmsguilogic.h"
#include "setting/include/common.h"
#include "settingui/src/settingitembase.h"
#include "cdlgmessageframe.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"

MsgDataDelegate::MsgDataDelegate()
{
    //
}

MsgDataDelegate::~MsgDataDelegate()
{
    //
}

bool MsgDataDelegate::CheckItemCanReused()
{
    return true;
}

bool MsgDataDelegate::GetData(int iPage, int & iTotData, QWPVector * pVecWP /*= NULL*/)
{
    if (NULL == pVecWP || iPage < 0)
    {
        return false;
    }

    TextMsgSimpListData sMsgList;
    if (!g_pMsgUILogic->GetMsgList(iPage, &sMsgList))
    {
        return false;
    }

    //QWPVector m_vecWidgetPair;
    YLList<TextMsgItemSimpData>::ListIterator iter = sMsgList.m_listMessage.begin();
    YLList<TextMsgItemSimpData>::ListIterator listEnd  = sMsgList.m_listMessage.end();

    for (int i = 0; iter != listEnd; ++iter, ++i)
    {
        QWidgetPair qPair;
        qPair.eType = MESSAGE_LIST_ITEM;

        qPair.bAddIndex = false;
        qPair.first = (*iter).m_strName.c_str();
        qPair.bAddIcon = true;
        qPair.strIcon = (*iter).m_strIconPath;
        //qPair.iIndex
        QLabel * pLalTime = new QLabel(NULL);
        pLalTime->setText(Timet2String((*iter).m_tTime).c_str());
        qPair.second = pLalTime;

        pVecWP->push_back(qPair);
    }

    iTotData = sMsgList.m_nTotalCount;
    return true;
}

bool MsgDataDelegate::SetData(void * pData /*= NULL*/)
{
    return true;
}
