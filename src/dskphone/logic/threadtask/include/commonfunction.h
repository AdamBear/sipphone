#ifndef TASKACTION_COMMONFUNCTION_H_
#define TASKACTION_COMMONFUNCTION_H_

//#include <string>
#include <ylstl/ylstring.h>
// 该文件定义一些公用接口.

namespace NS_TA
{

#define _MallocInit(x,x_type) x_type* x = new x_type;memset(x, 0, sizeof(x_type));
#define _strncpy(_dst,_src,_n) strncpy(_dst, _src, _n);_dst[_n-1]='\0';


#ifndef MyDelete
#define MyDelete(p) if (p != NULL) {delete (p); p = NULL;}
#endif


// 协议类型
enum NetworkProtocolType
{
    NETWORK_PROTOCOL_NONE = 0,  // 无协议类型.
    NETWORK_HTTP = 1,           // HTTP.
    NETWORK_HTTPS = 2,          // HTTPS.
    NETWORK_FTP = 3,            // FTP.
    NETWORK_TFTP = 4,           // TFTP.
    NETWORK_PROTOCOL_MAX
};

// 获取文件的协议类型.
// | strFilePath | 是文件路径
// 返回具体的协议类型, 如果路径不包含协议, 则返回NETWORK_PROTOCOL_NONE.
NetworkProtocolType GetNetworkFileProtocolType(const yl::string & strFilePath);

// 替换掉HTTP协议中的特殊字符.
// | strInput | 是输入字符串.
// 返回被替换的字符.
yl::string EncodeHttp(const yl::string & strInput);

// Translate all substring to the specified string.
// For example.
// Translate "D:\\Temp\\V40\\Doc\\" when strFrom if "\\" and strTo is "/"
// Then the string returned will be "D:/Temp/V40/Doc/"
// | strInput | is the input string.
// | strFrom | is the string to find in the input.
// | strTo | is the string replaced to.
// Return translated string if translated successfully.
yl::string translateString(const yl::string & strInput, const yl::string & strFrom,
                           const yl::string & strTo);

// 解析URL，解析出user，pwd，server，remoteFile，port
bool ParserURLPath(const yl::string & strSrcPath, yl::string & strUser, yl::string & strPwd
                   , yl::string & strServer, yl::string & strRemoteFile
                   , yl::string & strPort, yl::string & strDefaultPort);
}  // namespace NS_TA

#endif  // TASKACTION_COMMONFUNCTION_H_
