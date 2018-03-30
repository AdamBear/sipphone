#pragma once
#ifndef _TransformateCode_
#define _TransformateCode_
#include "iconv/iconv.h"
#include <stdio.h>
#include <stdlib.h>  // exit()
#include <direct.h>  // _access() detect a file's existence
#include <iostream>  // true, false defination

#define CHECKBUFSIZE 512

typedef enum
{
    ENUM_CODE_UNKOWN = -1,
    ENUM_CODE_GBK,
    ENUM_CODE_UTF8,
} CODE_TYPE;

/************************************************************************/
/* 获取校验的数据大小。最大为 CHECKBUFSIZE                              */
/************************************************************************/
size_t getCheckBufSize(size_t fileSize);
//写文件将buf内容以二进制方式进行写文件
int writeDataToFile(const char * FileName, char * buf, size_t FileSize);
//从文件中获取指定大小的BUF
int dumpFromFile(const char * FileName, char * buf, size_t FileSize);
//获取文件大小
int getFileSize(const char * FileName, size_t * FileSize);
//判断Buf数据是否是GB18030
bool isGB18030Data(char * rawtext, size_t rawtextlen);
//判断Buf数据是否是UTF-8
bool isUtf8Data(char * rawtext, size_t rawtextlen);
//检测是否是支持的编码code
bool checkSupportCode(const char * codeStr);

CODE_TYPE checkCodeType(char * rawtext, size_t rawtextlen);
//为UTF -8 文件增加BOM前缀
size_t convertUTF8_BOM(const char * fileName);
//转化编码的入口函数
size_t convertCode_func(const char * fileName, const char * fromCode, const char * toCode);
bool test_trans(std::string & path);
#endif
