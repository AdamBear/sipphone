#include "ETLLib/ETLLib.hpp"
#include "overlengtheffectfactory.h"


COverLengthEffectFactory::COverLengthEffectFactory()
{
}

COverLengthEffectFactory::~COverLengthEffectFactory()
{
    m_mapCreateOverLengthEffect.clear();
}

COverLengthEffectFactory& COverLengthEffectFactory::GetInstance()
{
#if IF_MULTI_WORKSTATION_ENBALE
    MULTI_KERNEL_INSTANCE(COverLengthEffectFactory)
#else
    SINGLE_KERNEL_INSTANCE(COverLengthEffectFactory)
#endif
}

void COverLengthEffectFactory::RegisterOverLengthEffect(int nOverLengthType, FUN_CREATE_OVERLENGTH_EFFECT pFunCreateOverLengthEffect)
{
    if (NULL == pFunCreateOverLengthEffect)
    {
        return;
    }

    chASSERTx(m_mapCreateOverLengthEffect.find(nOverLengthType) == m_mapCreateOverLengthEffect.end(), "repeat register same type over length style:%0x", nOverLengthType);

    m_mapCreateOverLengthEffect[nOverLengthType] = pFunCreateOverLengthEffect;
}

COverLengthEffectBase* COverLengthEffectFactory::ApplyOverLengthEffect(int nOverLengthEffectType)
{
    MAP_CREATE_OVERLENGTH_EFFECT_FUN::iterator iter = m_mapCreateOverLengthEffect.find(nOverLengthEffectType);
    FUN_CREATE_OVERLENGTH_EFFECT pFunCreateOverLengthEffect = NULL;

    if (iter != m_mapCreateOverLengthEffect.end())
    {
        pFunCreateOverLengthEffect = iter->second;
    }

    if (NULL == pFunCreateOverLengthEffect)
    {
        return NULL;
    }

    return (*pFunCreateOverLengthEffect)();
}
