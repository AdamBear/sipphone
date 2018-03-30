#ifndef __X_GRAPHIC_PROCESS_H__
#define __X_GRAPHIC_PROCESS_H__

#include "xGraphic.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// alphaBlend
#if IF_PIXEL_ALPHA
// 预乘混合
#define ALPHA_BLEND_PRE_MULT(dst, src) \
    dst.r = src.r + dst.r * (255 - src.a) / 255; \
    dst.g = src.g + dst.g * (255 - src.a) / 255; \
    dst.b = src.b + dst.b * (255 - src.a) / 255;
#endif

#if !IF_ONLY_USE_GRADATION
typedef void (*FUN_PIX_OPT)(BYTE* pSrc, BYTE* pDst);
typedef void(*FUN_GRAPHIC_OPERATE)(const xGraphic& src, const chRect& rcSrc, xGraphic& dst, const chRect& rcDst, FUN_PIX_OPT pFunPixOpt);
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicOperate
#if IF_PIXEL_ALPHA
#define PIXEL_PRE_MULT(src, dst) \
    dst.r = src.r * src.a / 255; \
    dst.g = src.g * src.a / 255; \
    dst.b = src.b * src.a / 255;
#endif

class xGraphicOperate
{
public:
    xGraphicOperate();
    ~xGraphicOperate();
public:
    static void tranGraphic(const xGraphic& src, const chRect& rcSrc,
                            xGraphic& dst, const chRect& rcDst
#if !IF_ONLY_USE_GRADATION
                            , FUN_PIX_OPT pFunPixOpt
#endif
                           );

    static void fillColor(BYTE* pColor, xGraphic& dst, const chRect& rcDst
#if !IF_ONLY_USE_GRADATION
                          , FUN_PIX_OPT pFunPixOpt
#endif
                         );

    static void fillClipRect(xGraphic& dst, const chRect& rcDst, const xRegion& rgnClip, BYTE* pColor
#if !IF_ONLY_USE_GRADATION
                             , FUN_PIX_OPT pFunPixOpt
#endif
                            );

    static void drawClipGraphic(const xGraphic& src, const chRect& rcSrc,
                                xGraphic& dst, const chRect& rcDst,
                                const xRegion& rgnClip
#if !IF_ONLY_USE_GRADATION
                                , FUN_PIX_OPT pFunPixOpt
#endif
                               );

#if IF_USE_FREETYPE_LIB
    static void fillPolygon(BYTE* pAlpha, const chPoint& ptOffset, int nLineLength,
                            xGraphic& dst, const chRect& rcDst, const xColor& color);
#endif

public:
#if IF_ONLY_USE_GRADATION
    //////////////////////////////////////////////////////////////////////////////////
    /*==================================黑白屏======================================*/
    static void tranGadation8B_2_1B(const xGraphic& src, xGraphic& dst);
#if IF_INVERT_EFFECT
    static void invertRect(xGraphic& dst, const chRect& rcDst);
#endif
    /*------------------------------------------------------------------------------*/
#else
    //////////////////////////////////////////////////////////////////////////////////
    /*====================================彩屏======================================*/
    static int getDataLength(int nFormat, int nWidth);

#if IF_PIXEL_ALPHA
    static void alphaBlend(const xPixel* src, xPixel* dst);
#endif

#if IF_XWINDOW_STATION_ALPHA
    static void blendAlphaBGR16(xGraphic& dst, const chRect& rcDst, int nAlpha);
#endif

    // rcSrc在src的有效范围内，函数内不再做有效性判断
    static xGraphicPtr convertGraphic(const xGraphic& src, const chRect& rcSrc, int nDstFormat);

#if IF_GRAPHIC_ROTATE
    static void tranGraphic90(const xGraphic& src, const chRect& rcSrc,
                              xGraphic& dst, const chRect& rcDst,
                              FUN_PIX_OPT pFunPixOpt);

    static void tranGraphic180(const xGraphic& src, const chRect& rcSrc,
                               xGraphic& dst, const chRect& rcDst,
                               FUN_PIX_OPT pFunPixOpt);
    static void tranGraphic270(const xGraphic& src, const chRect& rcSrc,
                               xGraphic& dst, const chRect& rcDst,
                               FUN_PIX_OPT pFunPixOpt);
#endif

#if IF_GRAPHIC_ZOOM
    static void tileDrawClipGraphic(const xGraphic& src, const chRect& rcSrc,
                                    xGraphic& dst, const chRect& rcDst,
                                    const xRegion& rgnClip);

    // rcSrc在src的有效范围内，函数内不再做有效性判断
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    static xGraphicPtr zoomGraphic(const xGraphic& src, const chRect& rcSrc, int nDstWidth, int nDstHeight);
#endif

    static void zoomDrawClipGraphic(const xGraphic& src, const chRect& rcSrc,
                                    xGraphic& dst, const chRect& rcDst,
                                    const xRegion& rgnClip);
#endif

#if IF_GRAPHIC_ROTATE
    static void rotateGraphic(xGraphic& src);
#endif
    // 格式和大小要一致，外部需要保证，内部不校验
    static void quickCopy(const xGraphic& src, xGraphic& dst, const chRect& rcSrc, int nDataLen);
    /*------------------------------------------------------------------------------*/
#endif
};

#if IF_ONLY_USE_GRADATION
#define INVERT_GRADATION(byGray) byGray ^= 0xFF;
#else
//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicConverter
#define GET_BGR565(r, g, b) (((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) >> 3) & 0x1f)
#define GET_BGR565_R(u16Color) (((u16Color) & 0xf800) >> 8)
#define GET_BGR565_G(u16Color) (((u16Color) & 0x07e0) >> 3)
#define GET_BGR565_B(u16Color) (((u16Color) & 0x1f) << 3)

class xGraphicConverter
{
    friend class xGraphicOperate;

public:
    xGraphicConverter();
    ~xGraphicConverter();

#if IF_GRAPHIC_ROTATE
public:
    static void SetRotateAngle(int nAngle);
    static int GetRotateAngle();

    // 以直角的倍数进行旋转(源坐标系按逆时针旋转，目标坐标系按顺时针旋转)
    static void RotateRightAngle(int& nWidth, int& nHeight);
    // 以直角的倍数进行旋转(源坐标系按逆时针旋转，目标坐标系按顺时针旋转)
    static void RotateRightAngle(chPoint& ptRotate, int nTarWidth, int nTarHeight);
    // 以直角的倍数进行旋转(源坐标系按逆时针旋转，目标坐标系按顺时针旋转)
    static void RotateRightAngle(chRect& rcRotate, int nTarWidth, int nTarHeight);

    static FUN_GRAPHIC_OPERATE GetFunGraphicOperate();
#endif

#if !IF_ONLY_USE_GRADATION
    static void Copy_GBRA32_Or_RGBA32(BYTE* pSrc, BYTE* pDst);

    static void Tran_GBRA32_2_RGBA32(BYTE* pSrc, BYTE* pDst);
    static void Tran_GBRA32_2_PreMult(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
    static void Tran_GBRA32_2_BGR24(BYTE* pSrc, BYTE* pDst);
#endif
    static void Tran_GBRA32_2_RGB24(BYTE* pSrc, BYTE* pDst);
    static void Tran_GBRA32_2_BGR565(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
    static void Tran_GBRA32_2_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst);
#endif

    static void Blend_GBRA32_PreMult_2_GBRA32(BYTE* pSrc, BYTE* pDst);

    static void Blend_GBRA32_PreMult_2_BGR565(BYTE* pSrc, BYTE* pDst);
    static void Blend_GBRA32_2_BGR565(BYTE* pSrc, BYTE* pDst);

#if IF_GRAPHIC_SUPPORT_RGBA_PAINT
    static void Tran_RGBA32_2_PreMult(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
    static void Tran_RGBA32_2_BGR24(BYTE* pSrc, BYTE* pDst);
#endif
    static void Tran_RGBA32_2_RGB24(BYTE* pSrc, BYTE* pDst);
    static void Tran_RGBA32_2_BGR565(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
    static void Tran_RGBA32_2_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst);
#endif

    static void Blend_RGBA32_PreMult_2_BGR565(BYTE* pSrc, BYTE* pDst);
    static void Blend_RGBA32_2_BGR565(BYTE* pSrc, BYTE* pDst);
#endif

#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
    static void Tran_BGR24_2_GBRA32(BYTE* pSrc, BYTE* pDst);
    static void Tran_BGR24_2_BGR565(BYTE* pSrc, BYTE* pDst);
#endif
    static void Tran_RGB24_2_GBRA32(BYTE* pSrc, BYTE* pDst);
    static void Tran_RGB24_2_BGR565(BYTE* pSrc, BYTE* pDst);

    static void Copy_BGR565(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
    static void Tran_BGR565_ColorKey_2_BGRA32(BYTE* pSrc, BYTE* pDst);
    static void Blend_BGR565_ColorKey(BYTE* pSrc, BYTE* pDst);
#endif

    static void Tran_BGR565_2_GBRA32(BYTE* pSrc, BYTE* pDst);
#if IF_GRAPHIC_SUPPORT_BGR24_PAINT
    static void Tran_BGR565_2_BGR24(BYTE* pSrc, BYTE* pDst);
#endif
    static void Tran_BGR565_2_RGB24(BYTE* pSrc, BYTE* pDst);

    static FUN_PIX_OPT GetFunTranPixOpt(int nSrcFormat, int nDstFormat);
    static FUN_PIX_OPT GetFunBlendPixOpt(int nSrcFormat, int nDstFormat);
#endif

#if IF_GRAPHIC_ROTATE
protected:
    // 设备相对于正常坐标系统按顺时针旋转
    static int s_nRotateAngle;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// xGraphicScale
#if IF_GRAPHIC_ZOOM
class xGraphicScaleInfo;
class xGraphicScale
{
public:
    xGraphicScale();
    ~xGraphicScale();

public:
    static xGraphicPtr ScaleGraphic(const xGraphic& src, const chRect& rcSrc,
                                    int nWidth, int nHeight
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
                                    , bool bAlpha
#endif
                                   );
    static bool ScaleGraphic(const xGraphic& src, const chRect& rcSrc, xGraphic& dst
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
                             , bool bAlpha
#endif
                            );

protected:
    static xGraphicScaleInfo* FreeScaleInfo(xGraphicScaleInfo* pInfo);
    static xGraphicScaleInfo* CalcScaleInfo(const xGraphic& objGraphic, const chRect& rcSrc,
                                            int nDstWidth, int nDstHeight
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
                                            , bool bAlpha
#endif
                                           );
    static void ScaleAARGBA(xGraphicScaleInfo* pScaleInfo, xPixel* dest, int dxx, int dyy,
                            int dx, int dy, int dw, int dh, int dow, int sow);
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
    static void ScaleAARGB(xGraphicScaleInfo* pScaleInfo, xPixel* dest, int dxx, int dyy,
                           int dx, int dy, int dw, int dh, int dow, int sow);
#endif
};
#endif
#endif

#endif
