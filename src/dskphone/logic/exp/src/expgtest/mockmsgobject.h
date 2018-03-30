#ifndef _MOCK_MSG_OBECJT_H__
#define  _MOCK_MSG_OBECJT_H__

#include <gmock/gmock.h>
//#include <ETLLib.hpp>
//#include <ETLLib/ETLMsgSystem.h>
typedef void * LPVOID;
//typedef long int LRESULT;

class MockMsgObject
{
public:
    /*msgObject(UINT uMsgID = 0, WPARAM wPar = 0, LPARAM lPar = 0);
    ~msgObject();*/
    void ReplyMessage(long result);

    MOCK_METHOD1(RMessage, void(long res));
    MOCK_METHOD0(GetExtraData, void * ());

private:

};


extern MockMsgObject * g_pMockMsgObj;

#endif