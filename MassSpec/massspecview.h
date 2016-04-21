/*!
  \file massspecview.h
  \brief Tools for show mass spectrums in the window
  \author Alexey Chudinov
  \version 1.0
  \date 07.04.2016
*/
#ifndef MASSSPECVIEW_H
#define MASSSPECVIEW_H

#include <QMainWindow>
#include <zoomplot.h>
#include <zoomplottoolbar.h>
#include <imassspectrum.h>

class MassSpectrumBase;

/*!
  Draws mass spectrum
*/
class MassSpecPlot : public ZoomPlot
{
    Q_OBJECT

public:
    MassSpecPlot(QWidget* parent = 0);
    ~MassSpecPlot();

    ///Possible values of spline show state
    enum MassSpecSplineShow
    {
        OnlyMassSpec, ///< Only mass spectrum will be shown
        MassSpecAndSpline, ///< Show mass spec and the spline
        OnlySpline ///< Only spline will be shown
    };

    ///Answers a question is there some need to show mass spec or spline or both
    /// \return Enum value MassSpecPlot::MassSpecSplineShow
    MassSpecSplineShow splineShow() const;
    ///Set the tipe of a plot appearance
    /// \param Enum value MassSpecPlot::MassSpecSplineShow
    void splineShow(MassSpecSplineShow splineShow);

    int splineBins() const;
    void splineBins(int nBins);

    inline double lambda() const { return m_lambda; }
    void lambda(double lambda_);
signals:
    void switchScaleNotify(bool isTimeScale);
    void showTotal(bool);
    void massSpecLoaded();

    ///Signalizes that spline show state was changed
    /// \param splineShow Current spline show state
    void splineShowChanged(MassSpecPlot::MassSpecSplineShow splineShow);

    ///Signalizes that number of spline bins was changed
    /// \param nBins New number of spline bins
    void splineBinsChanged(int nBins);

    ///Signalizes that smoothing degree was changed
    void lambdaChanged(double lambda);
public slots:
    ///Sets mass spectrum data pointer to an instance
    /// \param[in] massSpec Pointer to mass spectrum data wrapper
    void setMassSpec(QObject* massSpec,IMassSpectrum::MassSpecType type);

    ///Shows mass spectrum on a graph
    void showMassSpec();

    ///Switchs between time and m/z scales
    void switchScale();

private:
    MassSpectrumBase* m_msDataStruct;

    bool m_isTimeScale;

    MassSpecSplineShow m_splineShow;

    int m_nSplineBins;

    double m_lambda; //spline smoothing param

    void addSpline();
    void addMassSpec();
};

/*!
  ToolBar for mass spectrum plot
*/
class MassSpecToolBar : public ZoomPlotToolBar
{
    Q_OBJECT
public:
    MassSpecToolBar(QWidget* parent = 0);
    ~MassSpecToolBar();

signals:
    void switchScale();
    void showTotal(bool);
    void showSpline(bool);
    ///Sets the spline button position
    void setShowSpline(bool show);
    ///Trigger the show spline button
    void showSplineTriggered();

public slots:
    ///Changes the displayed icon on the button
    /// \param[in] isTimeScale If TRUE than set clock icon. Set balance icon otherwise.
    void changeSwitchScalesActionIcon(bool isTimeScale);

    ///Shows the state of show total button, returns the value emitting a signal
    void isShowTotal();
private:
    QAction* m_switchScales;
    QAction* m_showTotal;
    QAction* m_showSpline;
};

/*!
  Total ion current plot
*/
class IonCurrentPlot : public ZoomPlot
{
    Q_OBJECT
public:
    IonCurrentPlot(QWidget* parent = 0);
    ~IonCurrentPlot();

protected:
    ///Zoom in, zoom out, choose mass spectrum index
    void mousePressEvent(QMouseEvent* event);

signals:

public slots:
    ///Sets mass spectrum data pointer to an instance
    /// \param[in] massSpec Pointer to mass spectrum data wrapper
    void setMassSpec(QObject* massSpec,IMassSpectrum::MassSpecType type);

    ///Create new graph on a plot
    /// \param[in] x X-axis values
    /// \param[in] y Y-axis values
    void attachCurve(const QVector<double>& x, const QVector<double>& y);

    ///Shows total ion current if the number of mass spectrums was changed
    void showMassSpec();

    ///Draws mass spectrum position
    void drawMassSpecIdxPos();
private:
    MassSpectrumBase* m_msDataStruct;

    QCPItemLine* m_massSpecIdxPos;
};

/*!
  Creates window to display both mass spectrum and total ion current
*/
class MassSpecView : public QMainWindow
{
    Q_OBJECT
public:
    MassSpecView(QWidget *parent = 0);
    ~MassSpecView();

    bool getShowMSSpline() const;
signals:
    void showMassSpec(QObject* massSpec, IMassSpectrum::MassSpecType);
    void showMSSplineNotify(bool show);
    void showSpline(QObject* plot);
    void deleteSpline();

public slots:
    void setShowMSSpline(bool show);
    void onShowSplineTriggered();
    void emitShowSpline();

private:
    MassSpecPlot* m_massSpecPlot;
    MassSpecToolBar* m_toolBar;
    bool m_isShowMassSpecSpline;

    QDockWidget* m_ionCurrentWnd;
    IonCurrentPlot* m_ionCurrentPlot;
};

#endif // MASSSPECVIEW_H
