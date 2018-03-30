#ifndef _OVER_LENGTH_STYLE_FACTORY_H
#define _OVER_LENGTH_STYLE_FACTORY_H

#include "overlengtheffectbase.h"
#include <ylhashmap.h>

typedef COverLengthEffectBase* (*FUN_CREATE_OVERLENGTH_EFFECT)();
typedef YLHashMap<int, FUN_CREATE_OVERLENGTH_EFFECT> MAP_CREATE_OVERLENGTH_EFFECT_FUN;

class COverLengthEffectFactory
{
public:
    COverLengthEffectFactory();
    ~COverLengthEffectFactory();

    static COverLengthEffectFactory& GetInstance();

public:
    void RegisterOverLengthEffect(int nOverLengthType, FUN_CREATE_OVERLENGTH_EFFECT pFunCreateOverLengthEffect);

    COverLengthEffectBase* ApplyOverLengthEffect(int nOverLengthEffectType);

protected:
    MAP_CREATE_OVERLENGTH_EFFECT_FUN m_mapCreateOverLengthEffect;
};

#define g_OverLengthEffectFactory COverLengthEffectFactory::GetInstance()

#endif //_OVER_LENGTH_STYLE_FACTORY_H
