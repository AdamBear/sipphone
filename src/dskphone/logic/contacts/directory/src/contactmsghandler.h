// 此文件处理联系人模块的消息.

#ifndef DIRECTORY_MSGHANDLER_H_
#define DIRECTORY_MSGHANDLER_H_

#include "ETLLib.hpp"

namespace NS_DIRECTORY
{
// 处理联系人消息.
// | refObj | 是联系人模块的消息.
// 返回LRESULT.
LRESULT OnDirectoryMessage(msgObject & refObj);

}  // namespace NS_DIRECTORY

#endif  // DIRECTORY_MSGHANDLER_H_
