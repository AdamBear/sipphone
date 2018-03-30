#include "TransformateCode.h"
#include <winimp/winimp.h>

static void innerTrace(const std::string & str)
{
    std::cout << str;
    OutputDebugString(str.c_str());
};
#define LOG_TRACE(fmt, ...)  do { innerTrace(formatString(fmt "\n", ##__VA_ARGS__)); } while (0);


size_t getCheckBufSize(size_t fileSize)
{
    //return fileSize>CHECKBUFSIZE ? CHECKBUFSIZE : fileSize;
    return fileSize;
}

int writeDataToFile(const char * FileName, char * buf, size_t FileSize)
{
    FILE * fp;
    if ((fp = fopen(FileName, "wb+")) == NULL)
    {
        return -1;
    }
    fwrite(buf, 1, FileSize, fp);
    fclose(fp);
    return 0;
}

int dumpFromFile(const char * FileName, char * buf, size_t FileSize)
{
    FILE * fp;

    if ((fp = fopen(FileName, "rb")) == NULL)
    {
        return -1;
    }
    fread(buf, 1, FileSize, fp);

    fclose(fp);

    return 0;
}


int getFileSize(const char * FileName, size_t * FileSize)
{
    FILE * fp;

    if ((fp = fopen(FileName, "rb")) == NULL)
    {
        //perror(FileName);  //调试,显示具体出错信息
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    *FileSize = ftell(fp);

    fclose(fp);
    fp = NULL;

    return 0;
}

/*
    GB2312的编码范围是0xA1A1-0xFEFE
    GBK的整体编码范围是为0x8140-0xFEFE，不包括低字节是0×7F的组合。高字节范围是0×81-0xFE，低字节范围是0x40-7E和0x80-0xFE。
    GB18030的
        单字节编码范围是0x00-0x7F，完全等同与ASCII；
        双字节编码的范围和GBK相同，高字节是0x81-0xFE，低字节 的编码范围是0x40-0x7E和0x80-FE；
        四字节编码中第一、三字节的编码范围是0x81-0xFE，二、四字节是0x30-0x39。

    Big5是双字节编码，高字节编码范围是0x81-0xFE，低字节编码范围是0x40-0x7E和0xA1-0xFE。和GBK相比，少了低字节是0x80-0xA0的组合。0x8140-0xA0FE是保留区域，用于用户造字区。
*/
bool isGB18030Data(char * rawtext, size_t rawtextlen)
{
    int score = 0;
    int i = 0;
    int goodbytes = 0, asciibytes = 0;
    for (i = 0; i < rawtextlen; i++)
    {
        if ((rawtext[i] &  0x7F) == rawtext[i])
        {
            //最高位是0的ASCII字符
            //一位编码的情况
            asciibytes++;
        }
        else if (-127 <= rawtext[i] && rawtext[i] <= -2
                 && i + 1 < rawtextlen
                 && ((64 <= rawtext[i + 1] && rawtext[i + 1] <= 126) || (-128 <= rawtext[i + 1]
                         && rawtext[i + 1] <= -2)))
        {
            //两字节编码
            //高字节0×81-0xFE
            //低字节0x40-7E和0x80-0xFE
            goodbytes += 2;
            i++;
        }
        else if ((i + 3 < rawtextlen)
                 && -127 <= rawtext[i] && rawtext[i] <= -2
                 && -127 <= rawtext[i + 2] && rawtext[i + 2] <= -2
                 && 48 <= rawtext[i + 1] && rawtext[i + 1] <= 57
                 && 48 <= rawtext[i + 3] && rawtext[i + 3] <= 57)
        {
            goodbytes += 4;
            i += 3;
        }
    }

    if (asciibytes == rawtextlen)
    {
        return true;
    }
    score = 100 * goodbytes / (rawtextlen - asciibytes);
    //如果匹配率达到98%以上,则成功
    //允许一部分脏数据
    if (score > 98)
    {
        return true;
    }
    else if (score > 95 && goodbytes > 30)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
     Utf8编码规则
     0xxxxxxx (一位的情况,为ASCII)
     110xxxxx 10xxxxxx (110开头,代表两位)
     1110xxxx 10xxxxxx 10xxxxxx (1110开头代表三位)
     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (11110开头代表四位)
     111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (111110开头,代表五位)
     1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (1111110开头,代表六位)
*/

bool isUtf8Data(char * rawtext, size_t rawtextlen)
{
    int score = 0;
    int i = 0;
    int goodbytes = 0, asciibytes = 0;

    for (i = 0; i < rawtextlen; i++)
    {
        if ((rawtext[i] &  0x7F) == rawtext[i])
        {
            //最高位是0的ASCII字符
            //一位编码的情况
            asciibytes++;
        }
        else if (-64 <= rawtext[i] && rawtext[i] <= -33
                 //两位编码的情况,第一位11000000--11011111
                 //后一位跟10000000--10111111
                 && i + 1 < rawtextlen
                 && -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65)
        {
            goodbytes += 2;
            i++;
        }
        else if (-32 <= rawtext[i] && rawtext[i] <= -17
                 //三位编码的情况,第一位11100000--11101111
                 //后两位跟10000000--10111111
                 && i + 2 < rawtextlen
                 && -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65
                 && -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65)
        {
            goodbytes += 3;
            i += 2;
        }
        else if (-16 <= rawtext[i] && rawtext[i] <= -9
                 //四位编码的情况,第一位11110000--11110111
                 //后三位跟10000000--10111111
                 && i + 3 < rawtextlen
                 &&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65
                 && -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
                 && -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65)

        {
            goodbytes += 4;
            i += 3;
        }
        else if (-8 <= rawtext[i] && rawtext[i] <= -5
                 //五位编码的情况,第一位11111000--11111011
                 //后四位跟10000000--10111111
                 && i + 4 < rawtextlen
                 &&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65
                 && -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
                 && -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
                 && -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65)
        {
            goodbytes += 5;
            i += 4;
        }
        else if (-4 <= rawtext[i] && rawtext[i] <= -3
                 //六位编码的情况,第一位11111100--11111101
                 //后五位跟10000000--10111111
                 && i + 5 < rawtextlen
                 &&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65
                 && -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
                 && -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
                 && -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65
                 && -128 <= rawtext[i + 5] && rawtext[i + 5] <= -65)
        {
            goodbytes += 6;
            i += 5;
        }
    }
    if (asciibytes == rawtextlen)
    {
        return true;
    }
    score = 100 * goodbytes / (rawtextlen - asciibytes);
    //如果匹配率达到98%以上,则成功
    //允许一部分脏数据
    if (score > 98)
    {
        return true;
    }
    else if (score > 95 && goodbytes > 30)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkSupportCode(const char * codeStr)
{
    std::string supportCode = "UTF-8,GBK";
    if (NULL != strstr(supportCode.c_str(), codeStr))
    {
        return true;
    }
    return false;
}

CODE_TYPE checkCodeType(char * rawtext, size_t rawtextlen)
{
    if (isGB18030Data(rawtext, rawtextlen))
    {
        return ENUM_CODE_GBK;
    }
    else if (isUtf8Data(rawtext, rawtextlen))
    {
        return ENUM_CODE_UTF8;
    }
    else
    {
        return ENUM_CODE_UNKOWN;
    }
}


size_t convertUTF8_BOM(const char * fileName)
{
    //1、判断文件是否存在
    //2、判断来源编码是否支持
    //3、判断输出编码是否支持
    //
    size_t FileSize = 0;
    char * fileBuf = NULL;
    char * outpufileBuf = NULL;
    bool hasUtf8Bom = false;

    getFileSize(fileName, &FileSize);
    size_t checkBufSize = getCheckBufSize(FileSize);
    if (checkBufSize)
    {
        //检测文件大小
        fileBuf = new char[FileSize];
        dumpFromFile(fileName, fileBuf, FileSize);
        if (isUtf8Data(fileBuf, checkBufSize))
        {
            if (FileSize >= 3)
            {
                //检测是否带bom的UTF8  0xEF 0xBB 0xBF
                if (fileBuf[0] == -17
                        && fileBuf[1] == -69
                        && fileBuf[2] == -65)
                {
                    delete[]fileBuf;
                    LOG_TRACE("Already UTF-8 Bom: fileName: %s ", fileName);
                    return 0;
                }
                else
                {
                    outpufileBuf = new char[FileSize + 3];
                    outpufileBuf[0] = -17;
                    outpufileBuf[1] = -69;
                    outpufileBuf[2] = -65;
                    memcpy(&outpufileBuf[3], fileBuf, FileSize);

                    if (0 != writeDataToFile(fileName, outpufileBuf, FileSize + 3))
                    {
                        LOG_TRACE("APPEND BOM sucessed UTF-8 Bom: fileName: %s ", fileName);
                        delete []fileBuf;
                        delete []outpufileBuf;
                        return 0;
                    }
                    delete []fileBuf;
                    delete []outpufileBuf;
                    return -1;
                }
            }
            else
            {
                char buf[4] = {-17, -69, -65, 0};
                if (0 != writeDataToFile(fileName, buf, 3))
                {
                    LOG_TRACE("APPEND BOM sucessed UTF-8 Bom: fileName: %s ", fileName);
                    delete[]fileBuf;
                    return 0;
                }
                delete []outpufileBuf;
                return -1;

            }
        }
    }
    return 0;
}
/************************************************************************/
/* ret :0 sucess                                                                      */
/************************************************************************/
size_t convertCode_func(const char * fileName, const char * fromCode, const char * toCode)
{
    //1、判断文件是否存在
    //2、判断来源编码是否支持
    //3、判断输出编码是否支持
    //
    size_t FileSize = 0;
    char * fileBuf = NULL;
    char * outpufileBuf = NULL;
    bool hasUtf8Bom = false;

    if (checkSupportCode(fromCode) && checkSupportCode(toCode))
    {
        if (!strcmp(fromCode, toCode)) //输入输出码一致，直接跳过转码
        {
            LOG_TRACE("SKIP: Params fromCode and toCode is equal.");
            return -1;
        }

        getFileSize(fileName, &FileSize);
        size_t checkBufSize = getCheckBufSize(FileSize);
        if (checkBufSize)
        {
            //检测文件大小
            fileBuf = new char[FileSize];
            dumpFromFile(fileName, fileBuf, FileSize);

            //获取文件编码类型
            CODE_TYPE fileCodeType = checkCodeType(fileBuf, checkBufSize);
            switch (fileCodeType)
            {
            case ENUM_CODE_GBK:
                {
                    if (!strcmp("GBK", fromCode))
                    {
                    }
                    else
                    {
                        delete []fileBuf;
                        LOG_TRACE("Skip: file: %s Code is [GBK] but current fromCode:[%s] toCode:[%s]", fileName, fromCode,
                                  toCode);
                        return -1;
                    }
                }
                break;
            case ENUM_CODE_UTF8:
                {
                    if (!strcmp("UTF-8", fromCode))
                    {
                        if (FileSize >= 3)
                        {
                            //检测是否带bom的UTF8  0xEF 0xBB 0xBF
                            if (fileBuf[0] == -17
                                    && fileBuf[1] == -69
                                    && fileBuf[2] == -65)
                            {
                                hasUtf8Bom = true;
                                //去掉bom
                                if (0 != writeDataToFile(fileName, fileBuf + 3, FileSize - 3))
                                {
                                    delete[]fileBuf;
                                    LOG_TRACE("Fail When clearn UTF-8 Bom: fileName: %s ", fileName, fromCode);
                                    return -1;
                                }
                            }
                        }
                    }
                    else
                    {
                        delete []fileBuf;
                        LOG_TRACE("Skip: file: %s Code is [UTF-8] but current fromCode:[%s] toCode:[%s]", fileName,
                                  fromCode, toCode);
                        return -1;
                    }
                }
                break;
            case  ENUM_CODE_UNKOWN:
                {
                    delete[]fileBuf;
                    LOG_TRACE("Skip: file: %s  code is unkown.", fileName);
                    return -1;
                }
                break;
            default:
                break;
            }

            //开始转码准备
            outpufileBuf = new char[FileSize * 2];
            memset(outpufileBuf, 0, FileSize);
            iconv_t cd = iconv_open(toCode, fromCode);
            const char * srcStart = hasUtf8Bom ? fileBuf + 3 : fileBuf;
            size_t inselfSize = hasUtf8Bom ? FileSize - 3 : FileSize;
            size_t outselfSize = FileSize * 2;
            char * tempOutBuffer  = outpufileBuf;
            size_t ret = iconv(cd, (const char **)&srcStart, (size_t *)&inselfSize, &tempOutBuffer,
                               (size_t *)&outselfSize);
            if (ret != -1)
            {
                int len = FileSize * 2 - outselfSize;
                if (0 != writeDataToFile(fileName, outpufileBuf, FileSize * 2 - outselfSize))
                {
                    LOG_TRACE("Fail When ReWrite File: fileName: %s  formCode = %s, toCode = %s", fileName, fromCode,
                              toCode);
                }
                else
                {
                    LOG_TRACE("Success: fileName: %s  formCode = %s, toCode = %s", fileName, fromCode, toCode);
                }
            }
            iconv_close(cd);
            delete []outpufileBuf;
            delete []fileBuf;
            return ret;
        }
    }
    return 0;
}