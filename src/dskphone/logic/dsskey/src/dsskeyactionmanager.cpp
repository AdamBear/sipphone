#include "dsskeyactionmanager.h"
#include "dsskeycommonfunc.h"
#include "dsskeyaction.h"

void CDSSKeyActionManger::Init()
{

}

CDSSKeyActionManger * CDSSKeyActionManger::GetInstance()
{
    static CDSSKeyActionManger instance;
    return &instance;
}

CDSSKeyActionManger::CDSSKeyActionManger()
{
    Init();
}

CDSSKeyActionManger::~CDSSKeyActionManger()
{
    typeDsskeyactionMap::iterator iter = m_mapDsskeyaction.begin();
    for (; iter != m_mapDsskeyaction.end(); iter++)
    {
        if (iter->second)
        {
            delete iter->second;
        }
    }
    m_mapDsskeyaction.clear();
}

CDSSKeyAction * CDSSKeyActionManger::GetDSSKeyAction(DssKey_Type dsskeyType)
{
    // 校验合法区间
    if (dsskeyType < DT_NA || dsskeyType > DT_LAST)
    {
        return NULL;
    }
    typeDsskeyactionMap::iterator iter = m_mapDsskeyaction.find(dsskeyType);

    if (iter != m_mapDsskeyaction.end())
    {
        return iter->second;
    }

    CDSSKeyAction * pAction = new CDSSKeyAction();

    pAction->SetDsskeyType((DssKey_Type)dsskeyType);

    m_mapDsskeyaction[dsskeyType] = pAction;

    return pAction;
}
