#ifndef QIMAGE_FUNC
#define QIMAGE_FUNC

#include "ylstl/ylstring.h"

/************************************************************************/
/* 接口  ： Image_Scaled(const QString& strSrcFile, const QString& strDstFile, Image_Type eImagetype, int nScaleX = 0, int nScaleY = 0)*/
/* 功能  ： 图片压缩                                                 */
/* 参数说明 ：const std::string& strSrcFile原始图片路径                  */
/*            const std::string& strDstFile图片保存的路径                */
/* 返回值 ： bool 是否压缩成功                                            */
/************************************************************************/
int Image_Scaled(const yl::string & strSrcFile, const yl::string & strDstFile, int eImagetype,
                 int nScaleXDef = 0, int nScaleYDef = 0);

#endif // !QIMAGE_FUNC
