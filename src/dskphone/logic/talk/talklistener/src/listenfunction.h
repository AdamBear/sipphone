#ifndef __LISTENER_FUNCTION_H__
#define __LISTENER_FUNCTION_H__

// 从联系人获取名称
yl::string GetNameFromContact(const yl::string& strNumber, CALL_TYPE eCallType);

// 格式化sip信息
yl::string FormatSIPNumber(const yl::string& strNum, const yl::string& strServer);


#endif
