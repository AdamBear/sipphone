#include "screensaverpainter.h"
#include "uimanager/rectdefine.h"

#include "uimanager/moduimanager.h"

namespace
{
#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_OPACITY             1

#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_BLINDS              3
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_DIAMOND             4
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_RECTANGLE           5
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_ELLIPSE             6
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_PIE                 8
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_LINE_GRAND          9
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_GRADIENT            10
#define SCREENSAVER_CHANGE_TYPE_BOUNDRY_PANE                11

#define SCREENSAVER_CHANGE_TYPE_TOTAL_COUNT                 7
#endif

//变换透明度的单步变化量
#define SCREENSAVER_OPACITY_CHANGE_STEP                     5

//透明度边界值
#define SCREENSAVER_MAX_OPACITY                             255
#define SCREENSAVER_MIN_OPACITY                             0

//变化值的最大值
#define SCREENSAVER_SCALE_MAX_PERCENT                       100

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
//变化值的判断值（形状恰好内嵌于屏幕的值）
#define SCREENSAVER_SCALE_JUDGE_HALF_PERCENT        (SCREENSAVER_SCALE_MAX_PERCENT / 2)
#define SCREENSAVER_SCALE_JUDGE_DOUBLE_PERCENT      (SCREENSAVER_SCALE_MAX_PERCENT * 2)
#endif

//变化值的最小值
#define SCREENSAVER_SCALE_MIN_PERCENT                       0

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
//scale变化值的单步变化量
#define SCREENSAVER_SCALE_STEP                              2

//百叶窗默认条数
#define SCREENSAVER_BLINDS_COUNT                            8
//百叶窗单步变化量
#define SCREENSAVER_BLINDS_CHANGE_STEP                      3
//允许的最小值
#define SCREENSAVER_BLINDS_MIN_WIDTH                        0

//旋转的最大值、最小值
#define SCREENSAVER_MAX_ROTATE                              360
#define SCREENSAVER_MIN_ROTATE                              0

#define SCREENSAVER_MAX_HALF_ROTATE                         180

#define SCREENSAVER_SEPERATE_ANGLE                          90
#define SCREENSAVER_ROTATE_TIMES                            9

//屏幕对角线长度的一半
#define SCREENSAVER_DIAGONAL_HALF_LENGTH                    755
//2的开方近似
#define SQUARE_2                                            1.4

#define SCREENSAVER_PIE_START_ANGLE                         90
#define SCREENSAVER_PIE_ANGLE_RATE                          16

#define SCREENSAVER_SCALE_TRIANGLE_POLY                     3
#define SCREENSAVER_SCALE_DIAMOND_POLY                      4
#define SCREENSAVER_SCALE_RECTANGLE_POLY                    4

#define SCREENSAVER_GRADIENT_STEP                           10

#define SCREENSAVER_GRADIENT_MAX_PERCENT                    1.0
#define SCREENSAVER_GRADIENT_DIFFER_PERCENT                 0.2

#define SCREENSAVER_PANE_CHANGE_COUNT                       ((SCREENSAVER_PANE_LINE_COUNT * SCREENSAVER_PANE_ROW_COUNT) / SCREENSAVER_SCALE_MAX_PERCENT + 1)
#define SCREENSAVER_PANE_WIDTH                              (PHONE_WIDTH / SCREENSAVER_PANE_ROW_COUNT)
#define SCREENSAVER_PANE_HEIGHT                             (PHONE_HEIGHT / SCREENSAVER_PANE_LINE_COUNT)
//窗格变化值的单步变化量
#define SCREENSAVER_PANE_STEP                               1

#define SCREENSAVER_PANE_LEVEL_MIN                          0
#define SCREENSAVER_PANE_LEVEL_MAX                          10
#endif
}

ScreenSaverPainter * ScreenSaverPainter::s_pInstance = NULL;

ScreenSaverPainter * ScreenSaverPainter::GetInstance()
{
    if (NULL == s_pInstance)
    {
        s_pInstance = new ScreenSaverPainter();
    }

    return s_pInstance;
}

void ScreenSaverPainter::ReleaseInstance()
{
    if (s_pInstance != NULL)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

ScreenSaverPainter::ScreenSaverPainter()
    : QObject(NULL)
    , m_eChangeType(SCREENSAVER_CHANGE_TYPE_ERROR)
    , m_bIsChanging(false)
    , m_bEntering(true)
    , m_nScalePercent(SCREENSAVER_SCALE_MIN_PERCENT)
    , m_colorBG(Qt::transparent)
#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    , m_bIsBlindsVertical(true)
    , m_nBlindsCount(SCREENSAVER_BLINDS_COUNT)
    , m_nBlindsWidth(SCREENSAVER_BLINDS_MIN_WIDTH)
    , m_bIsPieClockWise(true)
    , m_bIsLineGradientV(true)
    , m_bIsLineGradientH(true)
    , m_nPaneFullLineCount(0)
    , m_bPaneFull(false)
    , m_nPaneChangeCount(SCREENSAVER_PANE_CHANGE_COUNT)
#endif
{

}

ScreenSaverPainter::~ScreenSaverPainter()
{

}

void ScreenSaverPainter::StartChanging()
{
    //获取一个颜色
    m_colorBG = g_pScreenSaverController->GetNextScreenSaverColor();

    //获取切换方式
    GetChangeType();

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    //百叶窗的初始化处理
    if (SCREENSAVER_CHANGE_TYPE_BLINDS == m_eChangeType)
    {
        //随机决定百叶窗方向
        int nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
        if (0 == nRandomBool)
        {
            m_bIsBlindsVertical = true;
        }
        else
        {
            m_bIsBlindsVertical = false;
        }

        if (m_bIsBlindsVertical)
        {
            m_nBlindsWidth = PHONE_WIDTH / m_nBlindsCount;
        }
        else
        {
            m_nBlindsWidth = PHONE_HEIGHT / m_nBlindsCount;
        }
    }
    else if (SCREENSAVER_CHANGE_TYPE_PIE == m_eChangeType)
    {
        //随机决定时钟变化旋转方向
        int nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
        if (0 == nRandomBool)
        {
            m_bIsPieClockWise = true;
        }
        else
        {
            m_bIsPieClockWise = false;
        }
    }
    else if (SCREENSAVER_CHANGE_TYPE_GRAD_LINE == m_eChangeType)
    {
        //随机决定渐变颜色方向
        int nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
        if (0 == nRandomBool)
        {
            m_bIsLineGradientV = true;
        }
        else
        {
            m_bIsLineGradientV = false;
        }
        nRandomBool = g_pScreenSaverController->GetRandomNumber(SCREENSAVER_BOOL_BOUNDRY);
        if (0 == nRandomBool)
        {
            m_bIsLineGradientH = true;
        }
        else
        {
            m_bIsLineGradientH = false;
        }
    }
    else if (SCREENSAVER_CHANGE_TYPE_PANE == m_eChangeType)
    {
        SetPaneStatus(false);
    }
#endif

    //总标志置位，开始变化，并且处于进入状态
    m_bIsChanging = true;
    m_bEntering = true;

    m_nScalePercent = SCREENSAVER_SCALE_MIN_PERCENT;
}

void ScreenSaverPainter::ResetChanging()
{
    //如果在变化过程中界面退出了，则需要调用该接口来清空标志
    m_bIsChanging = false;
    m_bEntering = true;

    m_nScalePercent = SCREENSAVER_SCALE_MIN_PERCENT;
}

void ScreenSaverPainter::PaintScreenSaverChanging(QPainter & dlgpainter)
{
    //只有处于变化状态时才要填充颜色
    if (!m_bIsChanging)
    {
        return;
    }

    switch (m_eChangeType)
    {
    case SCREENSAVER_CHANGE_TYPE_OPACITY:
        {
            PaintOpacityChange(dlgpainter);
        }
        break;
#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    case SCREENSAVER_CHANGE_TYPE_BLINDS:
        {
            PaintBlindsChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_DIAMOND_SCALE:
        {
            PaintDiamondScaleChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_RECTANGLE_SCALE:
        {
            PaintRectangleScaleChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_EllIPSE_SCALE:
        {
            PaintEllipseScaleChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_PIE:
        {
            PaintPieChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_GRAD_LINE:
        {
            PaintGradienLineChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_GRADIENT:
        {
            PaintGradienEllipseChange(dlgpainter);
        }
        break;
    case SCREENSAVER_CHANGE_TYPE_PANE:
        {
            PaintGradienPaneChange(dlgpainter);
        }
        break;
#endif
    default:
        {
        }
        break;
    }
}

void ScreenSaverPainter::GetChangeType()
{
#if !SCREEN_SAVE_FEATURE_TEST

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    GetRandomChangeType();
#else
    // 根据产品部和交互的意见，只使用透明度的方式
    m_eChangeType = SCREENSAVER_CHANGE_TYPE_OPACITY;
#endif // #if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE

#else
    m_eChangeType = SCREENSAVER_CHANGE_TYPE_PANE;
#endif // #if !SCREEN_SAVE_FEATURE_TEST
}

bool ScreenSaverPainter::ProcessChangePercent(int nStep)
{
    bool bResult = false;

    //增加变化量
    m_nScalePercent += nStep;
    if (m_nScalePercent >= SCREENSAVER_SCALE_MAX_PERCENT)
    {
        //当变化量增加到最大值时，又从头开始变化
        m_nScalePercent = SCREENSAVER_SCALE_MIN_PERCENT;

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
        if (SCREENSAVER_CHANGE_TYPE_PANE == m_eChangeType)
        {
            SetPaneStatus(false);
        }
#endif

        //如果是进入则切换背景
        if (m_bEntering)
        {
            //进入状态置为反
            m_bEntering = false;

            //更换一种切换方式
            GetChangeType();

            //切换背景
            emit ScreenSaverReallyChange();
            //需要继续绘制
            bResult = true;
        }
        //否则停止计时器，各标志置位
        else
        {
            m_bEntering = true;
            m_bIsChanging = false;
            //停止绘制计时
            emit ScreenSaverChangeEnd();
            //不需要绘制了
            bResult = false;
        }
    }
    //如果变化量还没有到达最大值，则不需要其他处理，继续绘制
    else
    {
        bResult = true;
    }

    return bResult;
}

void ScreenSaverPainter::PaintOpacityChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_OPACITY_CHANGE_STEP))
    {
        return;
    }

    int nColorAlpha = SCREENSAVER_MIN_OPACITY;
    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;

    if (m_bEntering)
    {
        nColorAlpha = fCurrentPercent * SCREENSAVER_MAX_OPACITY;
    }
    else
    {
        nColorAlpha = (1 - fCurrentPercent) * SCREENSAVER_MAX_OPACITY;
    }

    m_colorBG.setAlpha(nColorAlpha);
    dlgpainter.fillRect(RECT_FULL_SCREEN, m_colorBG);
}

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
void ScreenSaverPainter::GetRandomChangeType()
{
    int nRandomChangeType = g_pScreenSaverController->GetRandomNumber(
                                SCREENSAVER_CHANGE_TYPE_TOTAL_COUNT);

    if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_OPACITY)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_OPACITY;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_BLINDS)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_BLINDS;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_DIAMOND)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_DIAMOND_SCALE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_RECTANGLE)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_RECTANGLE_SCALE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_ELLIPSE)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_EllIPSE_SCALE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_PIE)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_PIE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_LINE_GRAND)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_GRAD_LINE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_GRADIENT)
    {
        //有效率问题所以不使用
        //  m_eChangeType = SCREENSAVER_CHANGE_TYPE_GRADIENT;
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_PIE;
    }
    else if (nRandomChangeType < SCREENSAVER_CHANGE_TYPE_BOUNDRY_PANE)
    {
        m_eChangeType = SCREENSAVER_CHANGE_TYPE_PANE;
    }
}

void ScreenSaverPainter::PaintBlindsChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_BLINDS_CHANGE_STEP))
    {
        return;
    }

    int nBlindsWidth = SCREENSAVER_BLINDS_MIN_WIDTH;
    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;

    if (m_bEntering)
    {
        nBlindsWidth = fCurrentPercent * m_nBlindsWidth;
    }
    else
    {
        nBlindsWidth = (1 - fCurrentPercent) * m_nBlindsWidth;
    }

    //根据是否是垂直百叶窗进行绘制
    if (m_bIsBlindsVertical)
    {
        //每一条的尺寸是一样的，只有位置不一样
        QRect rctPainted;
        rctPainted.setWidth(nBlindsWidth);
        rctPainted.setHeight(PHONE_HEIGHT);

        if (m_bEntering)
        {
            for (int i = 0; i < m_nBlindsCount; ++i)
            {
                //设置不同的位置并填充颜色
                rctPainted.moveTo((i * m_nBlindsWidth), 0);
                dlgpainter.fillRect(rctPainted, m_colorBG);
            }
        }
        else
        {
            for (int i = 0; i < m_nBlindsCount; ++i)
            {
                //如果是退出状态，则以右边不动为基准
                rctPainted.moveTo(((i + 1) * m_nBlindsWidth - nBlindsWidth), 0);
                dlgpainter.fillRect(rctPainted, m_colorBG);
            }
        }
    }
    else
    {
        //水平方向的情况
        QRect rctPainted;
        rctPainted.setWidth(PHONE_WIDTH);
        rctPainted.setHeight(nBlindsWidth);

        if (m_bEntering)
        {
            for (int i = 0; i < m_nBlindsCount; ++i)
            {
                rctPainted.moveTo(0, (i * m_nBlindsWidth));
                dlgpainter.fillRect(rctPainted, m_colorBG);
            }
        }
        else
        {
            for (int i = 0; i < m_nBlindsCount; ++i)
            {
                rctPainted.moveTo(0, ((i + 1) * m_nBlindsWidth - nBlindsWidth));
                dlgpainter.fillRect(rctPainted, m_colorBG);
            }
        }
    }
}

void ScreenSaverPainter::PaintDiamondScaleChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_SCALE_STEP))
    {
        return;
    }

    dlgpainter.setPen(m_colorBG);
    dlgpainter.setBrush(m_colorBG);

    if (m_bEntering)
    {
        float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_JUDGE_HALF_PERCENT;

        int nHalfWidth = PHONE_WIDTH / 2;
        int nHalfHeight = PHONE_HEIGHT / 2;

        QPointF points[SCREENSAVER_SCALE_DIAMOND_POLY] =
        {
            QPointF(nHalfWidth, nHalfHeight * (1 - fCurrentPercent)),
            QPointF(nHalfWidth * (1 + fCurrentPercent), nHalfHeight),
            QPointF(nHalfWidth, nHalfHeight * (1 + fCurrentPercent)),
            QPointF(nHalfWidth * (1 - fCurrentPercent), nHalfHeight),
        };
        //直接填充颜色即可
        dlgpainter.drawPolygon(points, SCREENSAVER_SCALE_DIAMOND_POLY);
    }
    else
    {
        //由于使用了openGL导致clear会清掉全部的内容变成后黑所以不能用这种方法
//      //先用颜色填充整个屏幕
//      dlgpainter.fillRect(RECT_FULL_SCREEN, m_colorBG);
//      //再用擦除方式去中空
//      dlgpainter.setCompositionMode(QPainter::CompositionMode_Clear);
//      dlgpainter.drawPolygon(points, SCREENSAVER_SCALE_DIAMOND_POLY);

        float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_JUDGE_DOUBLE_PERCENT;

        int nHalfWidth = PHONE_WIDTH;
        int nHalfHeight = PHONE_HEIGHT;

        //左上角
        QPointF points1[SCREENSAVER_SCALE_TRIANGLE_POLY] =
        {
            QPointF(nHalfWidth * (-fCurrentPercent), nHalfHeight * (-fCurrentPercent)),
            QPointF(nHalfWidth * (1 - fCurrentPercent), nHalfHeight * (-fCurrentPercent)),
            QPointF(nHalfWidth * (-fCurrentPercent), nHalfHeight * (1 - fCurrentPercent)),
        };

        //左下角
        QPointF points2[SCREENSAVER_SCALE_TRIANGLE_POLY] =
        {
            QPointF(nHalfWidth * (-fCurrentPercent), nHalfHeight * (fCurrentPercent)),
            QPointF(nHalfWidth * (-fCurrentPercent), nHalfHeight * (1 + fCurrentPercent)),
            QPointF(nHalfWidth * (1 - fCurrentPercent), nHalfHeight * (1 + fCurrentPercent)),
        };

        //右上角
        QPointF points3[SCREENSAVER_SCALE_TRIANGLE_POLY] =
        {
            QPointF(nHalfWidth * (fCurrentPercent), nHalfHeight * (-fCurrentPercent)),
            QPointF(nHalfWidth * (1 + fCurrentPercent), nHalfHeight * (-fCurrentPercent)),
            QPointF(nHalfWidth * (1 + fCurrentPercent), nHalfHeight * (1 - fCurrentPercent)),
        };

        //右下角
        QPointF points4[SCREENSAVER_SCALE_TRIANGLE_POLY] =
        {
            QPointF(nHalfWidth * (fCurrentPercent), nHalfHeight * (1 + fCurrentPercent)),
            QPointF(nHalfWidth * (1 + fCurrentPercent), nHalfHeight * (fCurrentPercent)),
            QPointF(nHalfWidth * (1 + fCurrentPercent), nHalfHeight * (1 + fCurrentPercent)),
        };

        //分别填充三角形颜色
        dlgpainter.drawPolygon(points1, SCREENSAVER_SCALE_TRIANGLE_POLY);
        dlgpainter.drawPolygon(points2, SCREENSAVER_SCALE_TRIANGLE_POLY);
        dlgpainter.drawPolygon(points3, SCREENSAVER_SCALE_TRIANGLE_POLY);
        dlgpainter.drawPolygon(points4, SCREENSAVER_SCALE_TRIANGLE_POLY);
    }
}

void ScreenSaverPainter::PaintRectangleScaleChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_SCALE_STEP))
    {
        return;
    }

    dlgpainter.setPen(m_colorBG);
    dlgpainter.setBrush(m_colorBG);

    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;

    int nHalfWidth = PHONE_WIDTH / 2;
    int nHalfHeight = PHONE_HEIGHT / 2;

    if (m_bEntering)
    {
        QPointF points[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-nHalfWidth * fCurrentPercent, -nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth * fCurrentPercent, -nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth * fCurrentPercent, nHalfHeight * fCurrentPercent),
            QPointF(-nHalfWidth * fCurrentPercent, nHalfHeight * fCurrentPercent),
        };

        dlgpainter.translate(nHalfWidth, nHalfHeight);
        dlgpainter.rotate(fCurrentPercent * SCREENSAVER_MAX_HALF_ROTATE);

        //直接填充颜色即可
        dlgpainter.drawPolygon(points, SCREENSAVER_SCALE_DIAMOND_POLY);
    }
    else
    {
        //上
        QPointF points1[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-nHalfWidth, -nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth, -nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth, -nHalfHeight * (2 + fCurrentPercent)),
            QPointF(-nHalfWidth, -nHalfHeight * (2 + fCurrentPercent)),
        };

        //下
        QPointF points2[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-nHalfWidth, nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth, nHalfHeight * fCurrentPercent),
            QPointF(nHalfWidth, nHalfHeight * (2 + fCurrentPercent)),
            QPointF(-nHalfWidth, nHalfHeight * (2 + fCurrentPercent)),
        };

        //左
        QPointF points3[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-nHalfWidth * fCurrentPercent, -nHalfHeight),
            QPointF(-nHalfWidth * fCurrentPercent, nHalfHeight),
            QPointF(-nHalfWidth * (2 + fCurrentPercent), nHalfHeight),
            QPointF(-nHalfWidth * (2 + fCurrentPercent), -nHalfHeight),
        };

        //右
        QPointF points4[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(nHalfWidth * fCurrentPercent, -nHalfHeight),
            QPointF(nHalfWidth * fCurrentPercent, nHalfHeight),
            QPointF(nHalfWidth * (2 + fCurrentPercent), nHalfHeight),
            QPointF(nHalfWidth * (2 + fCurrentPercent), -nHalfHeight),
        };

        dlgpainter.translate(nHalfWidth, nHalfHeight);
        dlgpainter.rotate(fCurrentPercent * SCREENSAVER_MAX_HALF_ROTATE);

        //分别填充矩形颜色
        dlgpainter.drawPolygon(points1, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points2, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points3, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points4, SCREENSAVER_SCALE_RECTANGLE_POLY);
    }
}

void ScreenSaverPainter::PaintEllipseScaleChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_SCALE_STEP))
    {
        return;
    }

    dlgpainter.setPen(m_colorBG);
    dlgpainter.setBrush(m_colorBG);

    int nHalfWidth = PHONE_WIDTH / 2;
    int nHalfHeight = PHONE_HEIGHT / 2;

    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;
    int nRadius = SCREENSAVER_DIAGONAL_HALF_LENGTH * fCurrentPercent;

    if (m_bEntering)
    {
        QPoint pointCenter(nHalfWidth, nHalfHeight);

        //直接填充颜色即可
        dlgpainter.drawEllipse(pointCenter, nRadius, nRadius);
    }
    else
    {
        //上
        QPointF points1[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (-1 - fCurrentPercent)),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (-1 - fCurrentPercent)),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (-fCurrentPercent)),
            QPointF(-SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (-fCurrentPercent)),
        };

        //下
        QPointF points2[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(-SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (1 + fCurrentPercent)),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (1 + fCurrentPercent)),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (fCurrentPercent)),
            QPointF(-SCREENSAVER_DIAGONAL_HALF_LENGTH, SCREENSAVER_DIAGONAL_HALF_LENGTH * (fCurrentPercent)),
        };

        //左
        QPointF points3[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (-1 - fCurrentPercent), -SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (-1 - fCurrentPercent), SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (-fCurrentPercent), SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (-fCurrentPercent), -SCREENSAVER_DIAGONAL_HALF_LENGTH),
        };

        //右
        QPointF points4[SCREENSAVER_SCALE_RECTANGLE_POLY] =
        {
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (1 + fCurrentPercent), -SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (1 + fCurrentPercent), SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (fCurrentPercent), SCREENSAVER_DIAGONAL_HALF_LENGTH),
            QPointF(SCREENSAVER_DIAGONAL_HALF_LENGTH * (fCurrentPercent), -SCREENSAVER_DIAGONAL_HALF_LENGTH),
        };

        dlgpainter.translate(nHalfWidth, nHalfHeight);
        dlgpainter.rotate(fCurrentPercent * SCREENSAVER_MAX_ROTATE);

        //分别填充矩形颜色
        dlgpainter.drawPolygon(points1, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points2, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points3, SCREENSAVER_SCALE_RECTANGLE_POLY);
        dlgpainter.drawPolygon(points4, SCREENSAVER_SCALE_RECTANGLE_POLY);

        //有点效率问题
        for (int i = 0; i < SCREENSAVER_ROTATE_TIMES; ++i)
        {
            //再旋转SCREENSAVER_SEPERATE_ANGLE / SCREENSAVER_ROTATE_TIMES度画一次
            dlgpainter.rotate(SCREENSAVER_SEPERATE_ANGLE / SCREENSAVER_ROTATE_TIMES);
            //分别填充矩形颜色
            dlgpainter.drawPolygon(points1, SCREENSAVER_SCALE_RECTANGLE_POLY);
            dlgpainter.drawPolygon(points2, SCREENSAVER_SCALE_RECTANGLE_POLY);
            dlgpainter.drawPolygon(points3, SCREENSAVER_SCALE_RECTANGLE_POLY);
            dlgpainter.drawPolygon(points4, SCREENSAVER_SCALE_RECTANGLE_POLY);
        }
    }
}

void ScreenSaverPainter::PaintPieChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_SCALE_STEP))
    {
        return;
    }

    dlgpainter.setPen(m_colorBG);
    dlgpainter.setBrush(m_colorBG);

    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;

    int nHalfWidth = PHONE_WIDTH / 2;
    int nHalfHeight = PHONE_HEIGHT / 2;

    QRect rctPie(nHalfWidth - SCREENSAVER_DIAGONAL_HALF_LENGTH
                 , nHalfHeight - SCREENSAVER_DIAGONAL_HALF_LENGTH
                 , SCREENSAVER_DIAGONAL_HALF_LENGTH * 2
                 , SCREENSAVER_DIAGONAL_HALF_LENGTH * 2);

    int nStartAngle = 0;
    int nPieAngle = 0;

    if (m_bIsPieClockWise)
    {
        if (m_bEntering)
        {
            nStartAngle = SCREENSAVER_PIE_START_ANGLE;
            nPieAngle = fCurrentPercent * SCREENSAVER_MAX_ROTATE;
            nPieAngle = -nPieAngle;

            nStartAngle *= SCREENSAVER_PIE_ANGLE_RATE;
            nPieAngle *= SCREENSAVER_PIE_ANGLE_RATE;
        }
        else
        {
            nStartAngle = SCREENSAVER_PIE_START_ANGLE - fCurrentPercent * SCREENSAVER_MAX_ROTATE;
            nPieAngle = SCREENSAVER_MAX_ROTATE;

            if (nStartAngle >= 0)
            {
                nPieAngle = nPieAngle - SCREENSAVER_PIE_START_ANGLE + nStartAngle;
                nPieAngle = -nPieAngle;
            }
            else
            {
                nStartAngle += SCREENSAVER_MAX_ROTATE;
                nPieAngle = nStartAngle - SCREENSAVER_PIE_START_ANGLE;
                nPieAngle = -nPieAngle;
            }

            nStartAngle *= SCREENSAVER_PIE_ANGLE_RATE;
            nPieAngle *= SCREENSAVER_PIE_ANGLE_RATE;
        }
    }
    else
    {
        if (m_bEntering)
        {
            nStartAngle = SCREENSAVER_PIE_START_ANGLE;
            nPieAngle = fCurrentPercent * SCREENSAVER_MAX_ROTATE;

            nStartAngle *= SCREENSAVER_PIE_ANGLE_RATE;
            nPieAngle *= SCREENSAVER_PIE_ANGLE_RATE;
        }
        else
        {
            nStartAngle = SCREENSAVER_PIE_START_ANGLE + fCurrentPercent * SCREENSAVER_MAX_ROTATE;
            nPieAngle = SCREENSAVER_MAX_ROTATE;

            if (nStartAngle < SCREENSAVER_MAX_ROTATE)
            {
                nPieAngle = nPieAngle + SCREENSAVER_PIE_START_ANGLE - nStartAngle;
            }
            else
            {
                nStartAngle -= SCREENSAVER_MAX_ROTATE;
                nPieAngle = SCREENSAVER_PIE_START_ANGLE - nStartAngle;
            }

            nStartAngle *= SCREENSAVER_PIE_ANGLE_RATE;
            nPieAngle *= SCREENSAVER_PIE_ANGLE_RATE;
        }
    }

    //直接填充颜色即可
    dlgpainter.drawPie(rctPie, nStartAngle, nPieAngle);
}

void ScreenSaverPainter::PaintGradienLineChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_SCALE_STEP))
    {
        return;
    }

    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;

    int nHalfWidth = PHONE_WIDTH / SQUARE_2;
    int nHalfHeight = PHONE_HEIGHT / SQUARE_2;

    int nWidth = nHalfWidth * 2;
    int nHeight = nHalfHeight * 2;

    QRect rctOverScreen(-(nHalfWidth - PHONE_WIDTH / 2)
                        , -(nHalfHeight - PHONE_HEIGHT / 2)
                        , nWidth
                        , nHeight);
    QLinearGradient lineGradient;
    if (m_bIsLineGradientV)
    {
        if (m_bIsLineGradientH)
        {
            lineGradient.setStart(0, 0);
            lineGradient.setFinalStop(nWidth, nHeight);
        }
        else
        {
            lineGradient.setStart(nWidth, 0);
            lineGradient.setFinalStop(0, nHeight);
        }
    }
    else
    {
        if (m_bIsLineGradientH)
        {
            lineGradient.setStart(0, nHeight);
            lineGradient.setFinalStop(nWidth, 0);
        }
        else
        {
            lineGradient.setStart(nWidth, nHeight);
            lineGradient.setFinalStop(0, 0);
        }
    }

    if (m_bEntering)
    {
        lineGradient.setColorAt(fCurrentPercent, m_colorBG);
        fCurrentPercent += SCREENSAVER_GRADIENT_DIFFER_PERCENT;
        fCurrentPercent = fCurrentPercent > SCREENSAVER_GRADIENT_MAX_PERCENT ?
                          SCREENSAVER_GRADIENT_MAX_PERCENT : fCurrentPercent;
        lineGradient.setColorAt(fCurrentPercent, Qt::transparent);
        lineGradient.setColorAt(1, Qt::transparent);
    }
    else
    {
        lineGradient.setColorAt(fCurrentPercent, Qt::transparent);
        fCurrentPercent += SCREENSAVER_GRADIENT_DIFFER_PERCENT;
        fCurrentPercent = fCurrentPercent > SCREENSAVER_GRADIENT_MAX_PERCENT ?
                          SCREENSAVER_GRADIENT_MAX_PERCENT : fCurrentPercent;
        lineGradient.setColorAt(fCurrentPercent, m_colorBG);
        lineGradient.setColorAt(1, m_colorBG);
    }

    //直接填充颜色即可
    dlgpainter.fillRect(rctOverScreen, lineGradient);
}

void ScreenSaverPainter::PaintGradienEllipseChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_GRADIENT_STEP))
    {
        return;
    }

    dlgpainter.setPen(Qt::transparent);

    int nHalfWidth = PHONE_WIDTH / 2;
    int nHalfHeight = PHONE_HEIGHT / 2;

    float fCurrentPercent = float(m_nScalePercent) / SCREENSAVER_SCALE_MAX_PERCENT;
    int nRadius = SCREENSAVER_DIAGONAL_HALF_LENGTH * fCurrentPercent;
    QPoint pointCenter(nHalfWidth, nHalfHeight);
    QRadialGradient radialGradient(nHalfWidth, nHalfHeight, SCREENSAVER_DIAGONAL_HALF_LENGTH,
                                   nHalfWidth, nHalfHeight);

    if (m_bEntering)
    {
        radialGradient.setColorAt(fCurrentPercent, m_colorBG);
        radialGradient.setColorAt(fCurrentPercent + SCREENSAVER_GRADIENT_DIFFER_PERCENT, Qt::transparent);
        radialGradient.setColorAt(1, Qt::transparent);
    }
    else
    {
        radialGradient.setColorAt(fCurrentPercent, Qt::transparent);
        radialGradient.setColorAt(fCurrentPercent + SCREENSAVER_GRADIENT_DIFFER_PERCENT, m_colorBG);
        radialGradient.setColorAt(1, m_colorBG);
    }

    dlgpainter.setBrush(radialGradient);

    //直接填充颜色即可
    dlgpainter.drawEllipse(pointCenter, SCREENSAVER_DIAGONAL_HALF_LENGTH,
                           SCREENSAVER_DIAGONAL_HALF_LENGTH);

}

void ScreenSaverPainter::SetPaneStatus(bool bSetStatus)
{
    m_nPaneFullLineCount = bSetStatus ? SCREENSAVER_PANE_LINE_COUNT : 0;
    m_bPaneFull = bSetStatus;

    for (int i = 0; i < SCREENSAVER_PANE_LINE_COUNT; ++i)
    {
        for (int j = 0; j < SCREENSAVER_PANE_ROW_COUNT; ++j)
        {
            if (bSetStatus)
            {
                m_arrnPaneMap[i][j] = SCREENSAVER_PANE_LEVEL_MAX;
            }
            else
            {
                m_arrnPaneMap[i][j] = SCREENSAVER_PANE_LEVEL_MIN;
            }
        }
        m_arrnPaneLineCount[i] = bSetStatus ? SCREENSAVER_PANE_ROW_COUNT : 0;
    }
}

void ScreenSaverPainter::PaneCountChange()
{
    //如果所有窗格都已经填满，则直接返回
    if (m_bPaneFull)
    {
        return;
    }

    for (int nTimes = 0; nTimes < m_nPaneChangeCount; ++ nTimes)
    {
        //从剩下未改变的行中获取一行
        int nLine = -1;
        int nLeftCount = 0;
        int nRandomLine = 1 + g_pScreenSaverController->GetRandomNumber(SCREENSAVER_PANE_LINE_COUNT -
                          m_nPaneFullLineCount);
        while (nLeftCount < nRandomLine)
        {
            ++nLine;
            //如果该行未满，则记录为下一行
            if (SCREENSAVER_PANE_ROW_COUNT != m_arrnPaneLineCount[nLine])
            {
                ++nLeftCount;
            }
        }

        //从剩下未改变的窗格中获取一个
        int nRow = -1;
        nLeftCount = 0;
        int nRandomRow = 1 + g_pScreenSaverController->GetRandomNumber(SCREENSAVER_PANE_ROW_COUNT -
                         m_arrnPaneLineCount[nLine]);
        while (nLeftCount < nRandomRow)
        {
            ++nRow;
            //如果该窗格未改变，则记录为下一个
            if (m_arrnPaneMap[nLine][nRow] == SCREENSAVER_PANE_LEVEL_MIN)
            {
                ++nLeftCount;
            }
        }

        m_arrnPaneMap[nLine][nRow] = SCREENSAVER_PANE_LEVEL_MIN + 1;
        ++m_arrnPaneLineCount[nLine];

        if (SCREENSAVER_PANE_ROW_COUNT == m_arrnPaneLineCount[nLine])
        {
            ++m_nPaneFullLineCount;
        }

        if (SCREENSAVER_PANE_LINE_COUNT == m_nPaneFullLineCount)
        {
            m_bPaneFull = true;
            return;
        }
    }
}

void ScreenSaverPainter::PaintGradienPaneChange(QPainter & dlgpainter)
{
    if (!ProcessChangePercent(SCREENSAVER_PANE_STEP))
    {
        return;
    }

    PaneCountChange();

    QRect rctPainted;

    for (int i = 0; i < SCREENSAVER_PANE_LINE_COUNT; ++i)
    {
        for (int j = 0; j < SCREENSAVER_PANE_ROW_COUNT; ++j)
        {
            m_colorBG.setAlpha(SCREENSAVER_MAX_OPACITY);

            if (m_bEntering)
            {
                if (m_arrnPaneMap[i][j] >= SCREENSAVER_PANE_LEVEL_MAX)
                {
                    rctPainted.setWidth(SCREENSAVER_PANE_WIDTH);
                    rctPainted.setHeight(SCREENSAVER_PANE_HEIGHT);

                    rctPainted.moveTo((j * SCREENSAVER_PANE_WIDTH), (i * SCREENSAVER_PANE_HEIGHT));
                    dlgpainter.fillRect(rctPainted, m_colorBG);
                }
                else if (m_arrnPaneMap[i][j] > SCREENSAVER_PANE_LEVEL_MIN)
                {
                    int nWidth = SCREENSAVER_PANE_WIDTH * m_arrnPaneMap[i][j] / SCREENSAVER_PANE_LEVEL_MAX;
                    int nHeight = SCREENSAVER_PANE_HEIGHT * m_arrnPaneMap[i][j] / SCREENSAVER_PANE_LEVEL_MAX;
                    rctPainted.setWidth(nWidth);
                    rctPainted.setHeight(nHeight);

                    rctPainted.moveTo((j * SCREENSAVER_PANE_WIDTH + (SCREENSAVER_PANE_WIDTH - nWidth) / 2),
                                      (i * SCREENSAVER_PANE_HEIGHT + (SCREENSAVER_PANE_HEIGHT - nHeight) / 2));
                    dlgpainter.fillRect(rctPainted, m_colorBG);

                    ++m_arrnPaneMap[i][j];
                }
                else
                {
                    //
                }
            }
            else
            {
                if (m_arrnPaneMap[i][j] >= SCREENSAVER_PANE_LEVEL_MAX)
                {
                    //
                }
                else if (m_arrnPaneMap[i][j] > SCREENSAVER_PANE_LEVEL_MIN)
                {
                    rctPainted.setWidth(SCREENSAVER_PANE_WIDTH);
                    rctPainted.setHeight(SCREENSAVER_PANE_HEIGHT * (SCREENSAVER_PANE_LEVEL_MAX - m_arrnPaneMap[i][j]) /
                                         SCREENSAVER_PANE_LEVEL_MAX);

                    rctPainted.moveTo((j * SCREENSAVER_PANE_WIDTH),
                                      (i * SCREENSAVER_PANE_HEIGHT + SCREENSAVER_PANE_HEIGHT * m_arrnPaneMap[i][j] /
                                       SCREENSAVER_PANE_LEVEL_MAX * 1.5));
                    m_colorBG.setAlpha(SCREENSAVER_MAX_OPACITY * (SCREENSAVER_PANE_LEVEL_MAX - m_arrnPaneMap[i][j]) /
                                       SCREENSAVER_PANE_LEVEL_MAX);

                    dlgpainter.fillRect(rctPainted, m_colorBG);
                    ++m_arrnPaneMap[i][j];
                }
                else
                {
                    rctPainted.setWidth(SCREENSAVER_PANE_WIDTH);
                    rctPainted.setHeight(SCREENSAVER_PANE_HEIGHT);

                    rctPainted.moveTo((j * SCREENSAVER_PANE_WIDTH), (i * SCREENSAVER_PANE_HEIGHT));
                    dlgpainter.fillRect(rctPainted, m_colorBG);
                }
            }
        }
    }
}

void ScreenSaverPainter::PrintPaneMap()
{
    printf("\n");
    for (int i = 0; i < SCREENSAVER_PANE_LINE_COUNT; ++i)
    {
        for (int j = 0; j < SCREENSAVER_PANE_ROW_COUNT; ++j)
        {
            printf("%d", m_arrnPaneMap[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < SCREENSAVER_PANE_LINE_COUNT; ++i)
    {
        printf("%d,", m_arrnPaneLineCount[i]);
    }
    printf("\n");
    printf("\n");
}
#endif
