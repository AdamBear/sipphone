#ifndef _EXP_EXP_GTEST_SCANNER_TEST_H__
#define  _EXP_EXP_GTEST_SCANNER_TEST_H__

#include "expgtestinc.h"

using testing::NotNull;
using testing::_;

#define  FAKE_CALL_MSG(msg,w,l) do{ msgObject msgRjExpIn ;  \
msgRjExpIn.message = msg;       \
msgRjExpIn.wParam = w;          \
msgRjExpIn.lParam = l;        \
TriggerEtlMsgCallback(msgRjExpIn );} \
                    while (0)






#endif // !_EXP_EXP_GTEST_SCANNER_TEST_H__






