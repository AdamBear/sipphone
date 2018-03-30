#ifdef IF_BT_TEST_LOGIC
#ifndef __BTTESTDRIVERTHREAD_H__
#define __BTTESTDRIVERTHREAD_H__
#include "../src/bluetooth_inc.h"

#define BLUETOOTH_MSG_TEST          BLUETOOTH_MESSAGE_END - 1
#define BLUETOOTH_MSG_TO_TESTDRIVER BLUETOOTH_MESSAGE_END - 2
#define VP_BLUETOOTH_TESTDRIVERTHREAD_NAME   "btTestThread"

/************************************************************************/
/*
* 1 answer call
  2 end call
  3 redial call
  4 dongle remove
  5 dongle add
  6 disconnect device
  7 property change
  8 disappear
  9 device power on
  10 device power off

  11 Mobile Disconnect
  12 Incomming call
  13 hang up call
  14 hold call
  15 call out
  16 channel switch


  100 move device test         wparam:handle,lparam:pos
  101 adjust device test       wparam:handle,lparam:1 up 0 down
*/
/************************************************************************/
enum BluetoothTestOption
{
    BTTEST_MSG_BEGIN = 0,

    BTTEST_ANSWERCALL = 1,           //lParam:call id
    BTTEST_ENDCALL = 2,              //lParam:call id
    BTTEST_REDIAL = 3,
    BTTEST_RMDONGLE = 4,
    BTTEST_ADDONGLE = 5,
    BTTEST_DISCONNECT = 6,
    BTTEST_PROPERTYCHANGE = 7,
    BTTEST_DISAPPEAR = 8,
    BTTEST_POWER_ON = 9,              //lParam 0:dongle power otherwise,device handle
    BTTEST_POWER_OFF = 10,            //
    BTTEST_MB_DISCONNECT = 11,        //
    BTTEST_INCOMMING_CALL = 12,       //
    BTTEST_HANGUP_CALL = 13,          //lParam:call id
    BTTEST_HOLD_CALL = 14,            //lParam:call id
    BTTEST_CALL_OUT = 15,             //
    BTTEST_CHANNEL_SWITCH = 16,
    BTTEST_UNHOLD_CALL = 17,
    BTTEST_ASKPAIR = 18,
    BTTEST_API_TEST = 100,         //接口测试  wParam:interface

    BTTEST_TALKANSWER = 200,
    BTTEST_TALKREJECT,
    BTTEST_TALKHANGUP,

    BTTEST_MSG_END,
};

class CBTTestDriverThread : public CMKThread
{
    BEGIN_MSG_MAP(CBTTestDriverThread)
    CHAIN_MSG_ON_METHOD(OnETLMessage)
    CHAIN_MSG_TO_BASE(CMKThread)
    END_MSG_MAP()

    DECLARE_SINGLETON_CLASS(CBTTestDriverThread)

    //父类继承消息
    virtual BOOL AfterThreadLaunched();
    virtual void BeforeThreadShutdown();

protected:
    //消息处理函数
    BOOL OnETLMessage(msgObject & objMessage);
};

#define _BTTestThread GET_SINGLETON_INSTANCE(CBTTestDriverThread)

#endif //__BTTESTDRIVERTHREAD_H__
#endif // IF_BT_TEST_LOGIC
