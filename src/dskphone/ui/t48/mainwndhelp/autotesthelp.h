#ifndef _AUTO_TEST_HELP_H_
#define _AUTO_TEST_HELP_H_

class CBaseDialog;

////----需要在init的时候注册
// 主要用于配合AutoTest提高AutoTest随机点击屏幕的准确率
void AutoTestHelp(const CBaseDialog * pDialog);

#endif
