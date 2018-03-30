#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#include <process.h>
#ifndef __cplusplus
#include "winimp/linux.h"
#include "winimp/getopt.h"
#else
#include <ETLLib.hpp>
#include <string>
#include "winimp/template.hpp"
#include "winimp/linux.h"
#include "winimp/unicode.h"
#include "winimp/getopt.h"


// cfgserver 加密函数封装
/************************************* cfg encrypt wrap begin ************************************/
// 获取真实路径
bool path_is_encrypt(const std::string & path, std::string & out);
// 获取加密配置路径
std::string path_get_encrypt(const std::string & path);
// 在路径倒数第二级添加子目录
bool path_file_exist(const std::string & path);
// copy file
bool path_copy_encrypt_file(const std::string & src, const std::string & to);
// dump file
bool dump_encrypt_file(const std::string & path, std::string & data);
// write file
bool write_encrypt_file(const std::string & path, const std::string & data);
// 解密buf
bool decrypt_buf(const std::string & data, std::string & result);
// 加密buf
bool encrypt_buf(const std::string & data, std::string & result);

void setResPathMapFile(const std::string & path);

void setPhoneDeviceName(const char * deviceName);
/************************************* cfg encrypt wrap end ************************************/


// 常用的string操作算法
/************************************* string operator api begin ************************************/
// formate string
std::string formatString(const char * formate, ...);
// 字符分割
bool parseString(std::vector<std::string> & vec, const std::string & src,
                 const std::string & filter);
void trim_both(std::string & str, const std::string & trim_str = " \t");
void to_upper(std::string & str);
void to_lower(std::string & str);
void delete_char(std::string & str, char ch);
bool start_with(const std::string & str, const std::string & start);
bool end_with(const std::string & str, const std::string & end);
bool istart_with(const std::string & str, const std::string & start);
bool iend_with(const std::string & str, const std::string & end);
void replaceString(std::string & str, const std::string & from, const std::string & to);
/************************************* string operator api end ************************************/


// 常用的path操作接口
/************************************* path operator api begin ************************************/
// 清空文件和目录
bool delete_files_in_directory(LPCSTR path, bool recursion = false, bool remove_empty_dir = false);
// 清空文件和目录
bool delete_files_in_directory(LPCSTR path, LPCSTR filter, bool recursion = false);
// 清空空目录
bool remove_empty_directory(LPCSTR path);
// 统计文件个数（不递归）
size_t path_file_count(LPCSTR path, bool recursion = false);
// 拷贝目录
bool copy_directory(LPCSTR from, LPCSTR to, bool recursion = false, bool cover = false);
// 拷贝文件
bool copy_file(LPCSTR from, LPCSTR to);
// ls
size_t ls_path(LPCSTR path, std::vector<std::string> & vec, LPCSTR filter = "",
               bool recursion = false, bool full_path = true);
// 获取文件大小
long get_file_size(FILE * file);
long get_file_size(const std::string & path);
// 读文件(会去除文件头Bom等标志)
bool dump_file_imp(const std::string & fileName, std::string & data);
// 部分读文件(不会去除文件头Bom等标志)
size_t dump_file_imp(const std::string & fileName, std::string & data, size_t from, size_t size);
// 读uft16编码文件
bool dump_utf16_file_imp(const std::string & fileName, std::wstring & data);
// 读uft16编码文件
size_t strip_encode_bom(std::string & data);
// 写文件
bool write_file_imp(const std::string & fileName, const std::string & data);
// 字符替换
void path_replace_spec(std::string & str, const std::string & from, const std::string & to);
// 字符替换
bool path_match_spec(LPCSTR path, LPCSTR filter);
// 文件读写权限
bool path_rw_access(const std::string & path);
// 路径规范化
std::string path_canonicalize(const std::string & path);
// 在路径倒数第二级添加子目录
std::string path_append_sub(const std::string & path, const std::string & sub);
// 检测文件编码
UniMode DetermineCharEncoding(const char * buf, size_t len);
UniMode DetermineFileEncoding(const char * path);
// 字符长度
size_t UCS2LengthFromUTF8(const char * s, size_t len);
size_t UTF8LengthFromUCS2(const wchar_t * s, size_t len);
/************************************* path operator api end ************************************/


// 系统接口封装函数
/************************************* system api wrap begin ************************************/
// 获取system命令的输出
bool WAPIExecDosCmd(const std::string & cmd, bool bSync = true);
bool WAPIExecDosCmd(const std::string & cmd, std::string & out);
// Doc Path 2 Logic Path
std::string WAPIDosDevicePath2LogicalPath(LPCSTR path);

// 读注册表接口
std::string WAPIReadRegSzValue(LPCSTR lpSubkey, LPCSTR lpValue);
DWORD WAPIReadRegDwValue(LPCSTR lpSubkey, LPCSTR lpValue);

// 字符转换
std::wstring WAPIString2Wstring(const std::string & str);
std::string WAPIWstring2String(const std::wstring & wstr);

// 获取进程的绝对路径
bool WAPIGetProcessImagePath(const std::string & process, std::string & path);
bool WAPIGetProcessImagePath(DWORD process, std::string & path);

// 通过进程名查找进程ID
bool WAPIGetProcessID(const std::string & name, DWORD & process);
// 通过进程ID查找进程名
bool WAPIGetProcessName(DWORD process, std::string & name);

// 获取当前exe绝对路径
std::string WAPIGetCurrentDir();
// 获取当前exe名称
std::string WAPIGetCurrentFileName();
// 获取当前exe名称
std::string WAPIGetCurrentFileNameWithoutExtension();

// 调用系统的弹出框
void WAPICreateSystemMsgbox(int time, LPCSTR formate, ...);
// 通过端口获取占用端口的进程ID
bool WAPIGetProcessIDByLocalPort(DWORD port, DWORD & process);
bool WAPIGetProcessIDByTCPPort(DWORD port, DWORD & process);
bool WAPIGetProcessIDByUDPPort(DWORD port, DWORD & process);

// 杀进程
bool WAPICloseProcess(const std::string & process);
bool WAPICloseProcess(DWORD process);
/************************************* system api wrap end ************************************/


// windows 加解密相关接口
/*********************************** win32 encrypt api wrap begin ********************************/
// UUID
std::string WAPIGetUUID();
// hash 算法
int RSHash(const std::string & data);
int FNVHash1(const std::string & data);

// sha256 算法，返回16进制数组，大小64B
std::string WAPISha256Hex(const std::string & data);
std::string WAPISha256Imp(const std::string & data);
std::string WAPIHash2Hex(const std::string & data);
// 根据need length产生对应长度（8的倍数）的hash值返回
std::string WAPIHashGen(const std::string & data, int need_length);

// 为文件添加必要属性
int WAPIEncodeFileInfo(const std::string & data, const std::string & key, std::string & result);
int WAPIDecodeFileInfo(const std::string & data, std::string & key, size_t & size);

// windows api 实现的加密函数
bool WAPIDecryptFile(LPSTR szSource, LPSTR szDestination, LPSTR szPassword = NULL);
// windows api 实现的解密函数
bool WAPIEncryptFile(LPSTR szSource, LPSTR szDestination, LPSTR szPassword = NULL);

// windows api 实现的加密函数
bool WAPIDecryptBuffer(const std::string & data, std::string & result, LPSTR szPassword = NULL);
// windows api 实现的解密函数
bool WAPIEncryptBuffer(const std::string & data, std::string & result, LPSTR szPassword = NULL);
// 压缩(大于64B以上才有明显效果，否则添加hash值等反而造成文件变大)
bool WAPICompress(const std::string & data, std::string & result);
// 加压缩
bool WAPIUnCompress(const std::string & data, std::string & result);
// 压缩文件
bool WAPICompressFile(const std::string & from, const std::string & to);
// 加压缩文件
bool WAPIUnCompressFile(const std::string & from, const std::string & to);
// 文件真实大小（压缩前或加密前）
int WAPIRestoreBuferSize(const std::string & data);
/************************************* win32 encrypt api wrap end ************************************/


// 替换devicelib中libsystem部分接口
/************************************* devicelib wrap begin ************************************/
// 获取机型
int GetPhoneHwVersion();
int GetPhoneHwVersion(const std::string & hw);

// 获取机型
typedef int(*FuncGetPhoneType)();
typedef const char * (*FuncGetPhoneName)();

void SetGetPhoneTypeCallbackFunc(FuncGetPhoneType pFunc);
void SetGetPhoneNameCallbackFunc(FuncGetPhoneName pFunc);

// 设置机型
void SetPhoneType(int hardware);
// 获取软件版本号
const char * GetPhoneSwVersion();
/************************************* devicelib wrap end ************************************/


/************************************* win32 process begin ************************************/
// 创建进程
PROCESS_INFORMATION * WAPICreateProcess(LPCSTR szCmdline, LPCSTR szCrtDir = NULL,
                                        bool bShowWnd = false, HANDLE hStdoutHandle = INVALID_HANDLE_VALUE,
                                        HANDLE hStderrHandle = INVALID_HANDLE_VALUE);
// 关闭进程
bool WAPICloseProcessByInfo(PROCESS_INFORMATION * pi);
// 等待进程
void WAPIWaitProcess(PROCESS_INFORMATION * pi);
// 进程同步
void WAPIProcessSync(unsigned uTimer);
// 获取系统信息
std::string WAPIGetSystemInfo();
// CrashHandler
bool WAPIRunCrashHandler();
// ipv4的字符表示
std::string WAPIGetIPV4AsString(DWORD dwSocketIP);
// 通过cmd检测端口被哪个进程占用
bool WAPICheckPortByCMD(DWORD dwPort, std::string & strApplication, DWORD & dwProcessID);
// 通过TCPtable检测端口被哪个进程占用
bool WAPICheckPortByTCPTable(DWORD dwPort, std::string & strApplication, DWORD & dwProcessID);
// 可执行文件名是否一致
bool WAPIIsExeFileMatched(LPCTSTR strName, LPCTSTR strRefName);
// 主线程初始化前的一些操作
void WAPIMainProcessInit();
/************************************* win32 process end ************************************/

#endif //__cplusplus
#endif
