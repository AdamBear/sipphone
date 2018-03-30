#ifndef SCREENSAVERPAINTER_H
#define SCREENSAVERPAINTER_H

#include "screensavercontroller.h"

enum SCREENSAVER_CHANGE_TYPE
{
    //屏保切换背景时使用的切换绘制方法
    SCREENSAVER_CHANGE_TYPE_ERROR = 0,
    //透明度变化
    SCREENSAVER_CHANGE_TYPE_OPACITY,
#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    //百叶窗变化
    SCREENSAVER_CHANGE_TYPE_BLINDS,
    //中央形状放大变化
    SCREENSAVER_CHANGE_TYPE_DIAMOND_SCALE,
    SCREENSAVER_CHANGE_TYPE_RECTANGLE_SCALE,
    SCREENSAVER_CHANGE_TYPE_EllIPSE_SCALE,

    //扇形变化
    SCREENSAVER_CHANGE_TYPE_PIE,

    //带渐变的线性变化
    SCREENSAVER_CHANGE_TYPE_GRAD_LINE,

    //带渐变的圆形变化
    SCREENSAVER_CHANGE_TYPE_GRADIENT,

    //窗格式
    SCREENSAVER_CHANGE_TYPE_PANE,
#endif
    //最大值
    SCREENSAVER_CHANGE_TYPE_MAX
};

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
#define SCREENSAVER_PANE_LINE_COUNT                             5
#define SCREENSAVER_PANE_ROW_COUNT                              8
#endif

class ScreenSaverPainter : public QObject
{
    Q_OBJECT

public:
    ScreenSaverPainter();
    ~ScreenSaverPainter();

    static ScreenSaverPainter * GetInstance();
    static void ReleaseInstance();

public:
    void StartChanging();
    void ResetChanging();

    inline void SetBGChangeType(SCREENSAVER_CHANGE_TYPE eType)
    {
        m_eChangeType = eType;
    }
    inline bool IsBeChanging()
    {
        return m_bIsChanging;
    }
    inline void SetToEntering(bool bEntering)
    {
        m_bEntering = bEntering;
    }
    inline bool IsBeEntering()
    {
        return m_bEntering;
    }

signals:
    //实际上真正替换背景的信号
    void ScreenSaverReallyChange();
    //变化结束的信号
    void ScreenSaverChangeEnd();

public:
    //总入口
    void PaintScreenSaverChanging(QPainter & dlgpainter);

private:
    void GetChangeType();

    //处理变化量的变化，返回值为false则表示不再进行绘制
    bool ProcessChangePercent(int nStep);

    //绘制透明度变化
    void PaintOpacityChange(QPainter & dlgpainter);

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    void GetRandomChangeType();

    //绘制百叶窗变化
    void PaintBlindsChange(QPainter & dlgpainter);
    //绘制中央形状放大变化
    void PaintDiamondScaleChange(QPainter & dlgpainter);
    void PaintRectangleScaleChange(QPainter & dlgpainter);
    void PaintEllipseScaleChange(QPainter & dlgpainter);
    void PaintPieChange(QPainter & dlgpainter);

    void PaintGradienLineChange(QPainter & dlgpainter);

    void PaintGradienEllipseChange(QPainter & dlgpainter);

    //窗格状态置满/置空
    void SetPaneStatus(bool bSetStatus);
    //窗格增/减
    void PaneCountChange();
    void PaintGradienPaneChange(QPainter & dlgpainter);

    void PrintPaneMap();
#endif

private:
    static ScreenSaverPainter * s_pInstance;

private:
    SCREENSAVER_CHANGE_TYPE     m_eChangeType;

    //是否处于变化中，如果是则需要刷背景
    bool                    m_bIsChanging;
    //是否正在“进入”变化
    bool                    m_bEntering;
    //变化的比例
    int                 m_nScalePercent;
    //当前正在使用的颜色
    QColor              m_colorBG;

#if SCREEN_SAVE_FEATURE_SPECTIAL_EFFTICS_CHANGE
    //百叶窗的方向
    bool                    m_bIsBlindsVertical;
    //使用的百叶窗的条数
    int                 m_nBlindsCount;
    //百叶窗每一条的宽度
    int                 m_nBlindsWidth;

    //时钟旋转的方向
    bool                    m_bIsPieClockWise;

    //线性渐变方向
    bool                    m_bIsLineGradientV;
    bool                    m_bIsLineGradientH;

    //每个窗格是否显示颜色
    int                 m_arrnPaneMap[SCREENSAVER_PANE_LINE_COUNT][SCREENSAVER_PANE_ROW_COUNT];
    //每行窗格显示颜色的个数
    int                 m_arrnPaneLineCount[SCREENSAVER_PANE_LINE_COUNT];
    //已经满了的行数
    int                 m_nPaneFullLineCount;
    //窗格是否已经填满
    bool                    m_bPaneFull;
    //每次变化的窗格个数
    int                 m_nPaneChangeCount;
#endif
};

#define g_pScreenSaverPainter (ScreenSaverPainter::GetInstance())

#endif // SCREENSAVERPAINTER_H
