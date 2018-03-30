#include "imagefileinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include "dsklog/log.h"


CImageFileInfo::CImageFileInfo()
{
}

CImageFileInfo::~CImageFileInfo()
{
}

bool CImageFileInfo::IsImage(LPCSTR lpszFilePath)
{
    if (NULL == lpszFilePath)
    {
        return false;
    }

    // 取文件后缀名
    yl::string strFileType(lpszFilePath);
    yl::string::size_type pos = strFileType.rfind('.');
    if (pos != yl::string::npos)
    {
        strFileType = strFileType.substr(pos);
    }
    else
    {
        strFileType = "";
    }

    // 判断是否为图片资源
    return (strFileType == ".bmp"
            || strFileType == ".gif"
            || strFileType == ".jpg"
            || strFileType == ".jpeg"
            || strFileType == ".png"
            || strFileType == ".pbm"
            || strFileType == ".pgm"
            || strFileType == ".ppm"
            || strFileType == ".xbm"
            || strFileType == ".xpm"
            || strFileType.empty());// 没有后缀名的也可能是图片
}

//从文件头中读取相应字段以判断图片格式
int CImageFileInfo::getImageFormatInt(const yl::string& path)
{
    //BMP格式特征码
    unsigned char BMPHeader[] = {0x42, 0x4d};
    //JPG,JPEG格式特征码
    unsigned char JPGHeader[] = {0xff, 0xd8};
    //GIF格式特征码
    unsigned char GIFHeader1[] = {0x47, 0x49, 0x46, 0x38, 0x37, 0x61};
    unsigned char GIFHeader2[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};
    //PNG格式特征码
    unsigned char PNGHeader[] = {0x89, 0x50, 0x4E, 0x47};
    int count = 0;
    int step = 2;
    //以二进制方式打开文件并读取前几个字节
    unsigned char header[16];
    FILE *fid;
    if (NULL == (fid = fopen(path.c_str(), "rb")))
    {
        UIKERNEL_INFO("open file fail! error:%s", strerror(errno));
        return NVL_FORMAT;
    }
    //先读两个，判断是否BMP格式
    fread(&header[count], sizeof(char), 2, fid);
    count = count + step;
    if (memcmp(header, BMPHeader, count) == 0)
    {
#if IF_IMAGE_FILE_DEBUG
        UIKERNEL_INFO("File feature code:");
        for (int i = 0; i < count; i++)
        {
            printf("%0x\t", header[i]);
        }
        printf("\n");
        UIKERNEL_INFO("BMP File");
#endif
        fclose(fid);
        return BMP_FORMAT;
    }
    //再读两个，判断是否JPG格式、PNG格式
    fread(&header[count], sizeof(char), 2, fid);
    count = count + step;
    if (memcmp(header, JPGHeader, step) == 0)
    {
#if IF_IMAGE_FILE_DEBUG
        UIKERNEL_INFO("File feature code:");
        for (int i = 0; i < count; i++)
        {
            printf("%0x\t", header[i]);
        }
        printf("\n");
        UIKERNEL_INFO("JPG File");
#endif
        fclose(fid);
        return JPG_FORMAT;
    }
    else if (memcmp(header, PNGHeader, count) == 0)
    {
#if IF_IMAGE_FILE_DEBUG
        UIKERNEL_INFO("File feature code:");
        for (int i = 0; i < count; i++)
        {
            printf("%0x\t", header[i]);
        }
        printf("\n");
        UIKERNEL_INFO("PNG File");
#endif
        fclose(fid);
        return PNG_FORMAT;
    }
    //再读两个，判断是否GIF格式
    fread(&header[count], sizeof(char), 2, fid);
    count = count + step;
    if ((memcmp(header, GIFHeader1, count) == 0)
            || (memcmp(header, GIFHeader2, count) == 0))
    {
#if IF_IMAGE_FILE_DEBUG
        UIKERNEL_INFO("File feature code:");
        for (int i = 0; i < count; i++)
        {
            printf("%0x\t", header[i]);
        }
        printf("\n");
        UIKERNEL_INFO("GIF File");
#endif
        fclose(fid);
        return GIF_FORMAT;
    }
#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("File feature code:");
    for (int i = 0; i < count; i++)
    {
        printf("%0x\t", header[i]);
    }
    printf("\n");
    UIKERNEL_INFO("Unknown File");
#endif
    fclose(fid);
    return NVL_FORMAT;
}

yl::string CImageFileInfo::getImageFormat(const yl::string& path)
{
    yl::string strFormat = "NA";
    if (!path.empty())
    {
        if (pathFileExist(path.c_str()))
        {
            int iFormat = getImageFormatInt(path);
            switch (iFormat)
            {
            case BMP_FORMAT:
                strFormat = "BMP";
                break;
            case JPG_FORMAT:
                strFormat = "JPG";
                break;
            case GIF_FORMAT:
                strFormat = "GIF";
                break;
            case PNG_FORMAT:
                strFormat = "PNG";
                break;
            default:
                break;
            }
        }
    }
    return strFormat;
}

long CImageFileInfo::getImageSize(const yl::string& path)
{
    long nSize = 0;
    if (!path.empty())
    {
        if (pathFileExist(path.c_str()))
        {
            nSize = pathFileSize(path.c_str());
        }
    }

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("image size[%ld]", nSize);
#endif
    return nSize;
}

//BMP文件头的第2、3字为文件大小信息
long CImageFileInfo::getBMPSize(const yl::string& path)
{
    FILE *fid;
    long int size;
    if ((fid = fopen(path.c_str(), "rb")) == NULL)
    {
        UIKERNEL_INFO("open file fail! error:%s", strerror(errno));
        return 0;
    }
    //跳过图片特征码
    fseek(fid, 2, SEEK_SET);
    fread(&size, sizeof(long), 1, fid);
    fclose(fid);

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("BMP image size = %d\n", size);
#endif
    return size;
}

long CImageFileInfo::getGIFSize(const yl::string& path)
{
    return 0;
}

long CImageFileInfo::getPNGSize(const yl::string& path)
{
    return 0;
}

long CImageFileInfo::getJPGSize(const yl::string& path)
{
    FILE *fid;
    long int size;
    if ((fid = fopen(path.c_str(), "rb")) == NULL)
    {
        UIKERNEL_INFO("Open file fail!");
        return 0;
    }
    fseek(fid, 0, SEEK_END);
    size = ftell(fid);
    fclose(fid);
#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("JPG image size = %d\n", size);
#endif
    return size;
}

//BMP文件头的第10、11字为文件宽度信息
//BMP文件头的第12、13字为文件高度信息
chSize CImageFileInfo::getBMPDimension(const yl::string& path)
{
    FILE *fid;
    if ((fid = fopen(path.c_str(), "rb")) == NULL)
    {
        UIKERNEL_INFO("Open file fail!");
        return chSize(0, 0);
    }
    long int width;
    long int height;
    //读取宽度和高度
    fseek(fid, 18, SEEK_SET); //偏移18个字节
    fread(&width, sizeof(long), 1, fid);
    fread(&height, sizeof(long), 1, fid);
#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("BMP pixels size(%d, %d)", width, height);
#endif
    fclose(fid);
    return chSize(width, height);
}

chSize CImageFileInfo::getJPGDimension(const yl::string& path)
{
    FILE *fid;
    if ((fid = fopen(path.c_str(), "rb")) == NULL)
    {
        UIKERNEL_INFO("Open file fail!");
        return chSize(0, 0);
    }
    long int width = 0;
    long int height = 0;
    fseek(fid, 0, SEEK_END);
    long length = ftell(fid);
    unsigned char *buffer = new unsigned char[length];
    unsigned char *buffer_bakup = buffer;
    fseek(fid, 0, SEEK_SET);
    fread(buffer, length, 1, fid);
    fclose(fid);
    unsigned char *temp = buffer + length;
    unsigned char *temp_ori = buffer;
    unsigned char ff;
    unsigned char type = 0xff;
    int m_pos = 0;
    //跳过文件头中标志文件类型的两个字节
    for (int i = 0; i < 2; i++)
    {
        buffer++;
    }
    while ((temp > buffer) && (type != 0xDA))
    {
        do
        {
            ff = *buffer++;
        }
        while (ff != 0xff);
        do
        {
            type = *buffer++;
        }
        while (type == 0xff);

        switch (type)
        {
        case 0x00:
        case 0x01:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
            break;
        case 0xC0://SOF0段
            temp_ori = buffer;
            m_pos = (*buffer++) << 8;
            m_pos += *buffer++;
            buffer++; //舍弃精度值
            height = (*buffer++) << 8;
            height += *buffer++;
            width = (*buffer++) << 8;
            width += *buffer;
            break;
        case 0xE0: //APP0段
            UIKERNEL_INFO("APPO segment");
            temp_ori = buffer;
            m_pos = (*buffer++) << 8;
            m_pos += *buffer++;
            buffer = buffer + 12;
            //丢弃APP0标记(5bytes)、主版本号(1bytes)、次版本号(1bytes)、像素点单位(1bytes)、垂直像素点(2bytes)、 水平像素点(2bytes)
            break;
        default:
            temp_ori = buffer;
            m_pos = (*buffer++) << 8;
            m_pos += *buffer++;
            break;
        }
        buffer = temp_ori + m_pos;
    }

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("JPG pixels size(%d, %d)", width, height);
#endif
    //记得释放内存
    delete[] buffer_bakup;
    return chSize(width, height);
}

//PNG文件头的第9字为文件宽度信息
//PNG文件头的第10字为文件高度信息
chSize CImageFileInfo::getPNGDimension(const yl::string& path)
{
    FILE *fid = NULL;
    if ((fid = fopen(path.c_str(), "rb")) == NULL)
    {
        UIKERNEL_INFO("Open file fail!");
        return chSize(0, 0);
    }
    long int width;
    long int height;
    unsigned char wtmp[4] = {'0'}; //宽度
    unsigned char htmp[4] = {'0'}; //高度
    fseek(fid, 16, SEEK_SET);
    fread(wtmp, 4, 1, fid);         // example 00000080
    fread(htmp, 4, 1, fid);         // example 00000080
    fclose(fid);
    width = ((int)(unsigned char)wtmp[2]) * 256 + (int)(unsigned char)wtmp[3];
    height = ((int)(unsigned char)htmp[2]) * 256 + (int)(unsigned char)htmp[3];

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("PNG pixels size(%d, %d)", width, height);
#endif
    return chSize(width, height);
}

//GIF文件头的第4字为文件宽度信息
//GIF文件头的第5字为文件高度信息
chSize CImageFileInfo::getGIFDimension(const yl::string& path)
{
    FILE *fid;
    if (NULL == (fid = fopen(path.c_str(), "rb")))
    {
        UIKERNEL_INFO("Can not open this file.");
        return chSize(0, 0);
    }
    long int width;
    long int height;
    //读取宽度和高度
    char s1[2] = {0}, s2[2] = {0};
    fseek(fid, 6, SEEK_SET); //偏移6个字节
    fread(s1, sizeof(char), 2, fid);
    fread(s2, sizeof(char), 2, fid);
    width = (unsigned int)(s1[1]) << 8 | (unsigned int)(s1[0]);
    height = (unsigned int)(s2[1]) << 8 | (unsigned int)(s2[0]);
    fclose(fid);

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("GIF pixels size(%d, %d)", width, height);
#endif
    return chSize(width, height);
}

chSize CImageFileInfo::getImageDimension(const yl::string& path)
{
    chSize dimension;
    if (!path.empty())
    {
        if (pathFileExist(path.c_str()))
        {
            int iFormat = getImageFormatInt(path);
            switch (iFormat)
            {
            case BMP_FORMAT:
                dimension = getBMPDimension(path);
                break;
            case JPG_FORMAT:
                dimension = getJPGDimension(path);
                break;
            case GIF_FORMAT:
                dimension = getGIFDimension(path);
                break;
            case PNG_FORMAT:
                dimension = getPNGDimension(path);
                break;
            default:
                dimension = chSize(-1, -1);
                break;
            }
        }
    }

#if IF_IMAGE_FILE_DEBUG
    UIKERNEL_INFO("Image Size(%d, %d)", dimension.cx, dimension.cy);
#endif
    return dimension;
}
