#include "mockmsgobject.h"
MockMsgObject * g_pMockMsgObj;

namespace ETL_MsgQueueHelper
{
class msgObject
{
public:
    msgObject(unsigned int m = 0, unsigned int w = 0, int l = 0);
    ~msgObject();

};


msgObject::msgObject(unsigned int m /*= 0*/, unsigned int w /*=0*/, int l /*=0*/)
{

}

msgObject::~msgObject()
{

}
}
//void msgObject::ReplyMessage(LRESULT result)
//{
//  g_pMockMsgObj->RMessage(result);
//}
//
//
//LPVOID  msgObject::GetExtraData()
//{
//  return  g_pMockMsgObj->GetExtraData();
//}