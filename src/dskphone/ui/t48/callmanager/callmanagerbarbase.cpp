#include "callmanagerbarbase.h"

CCallManagerBarBase::CCallManagerBarBase(QWidget * parent /* = 0 */)
    : QWidget(parent)
{
}

CCallManagerBarBase::~CCallManagerBarBase(void)
{
}

//设置所有 item 信息
void CCallManagerBarBase::SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo)
{

}

//设置焦点 item
void CCallManagerBarBase::SetFocusMGRInfo(const yl::string & strUIName)
{

}

