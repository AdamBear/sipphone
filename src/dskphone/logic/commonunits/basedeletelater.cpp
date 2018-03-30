#include <ETLLib.hpp>
#include "basedeletelater.h"
#include "deletemanager.h"

#if 0
CBaseDeleteLater::CBaseDeleteLater()
{

}

CBaseDeleteLater::~CBaseDeleteLater()
{

}

// 延迟删除
void CBaseDeleteLater::deleteLater()
{
    CDeleteManager::deleteLater(this);
}
#endif
