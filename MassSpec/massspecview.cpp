#include <massspecview.h>
#include <massspectrumbase.h>
#include <cmath>
#include <tdcdatastorage.h>

//MassSpecView functions
//==============================================================
MassSpecView::MassSpecView(QWidget *parent)
    :
      QMainWindow(parent),
      m_massSpecPlot(new MassSpecPlot(this)),
      m_toolBar(new MassSpecToolBar(this)),
      m_ionCurrentWnd(new QDockWidget("Полный ионный ток",this)),
      m_ionCurrentPlot(new IonCurrentPlot(this))
{

    //Set up ion current window
    m_ionCurrentWnd->setWidget(m_ionCurrentPlot);
    m_ionCurrentWnd->setMinimumSize(QSize(200,200));
    addDockWidget(Qt::BottomDockWidgetArea,m_ionCurrentWnd);
    QAction* toggleIonCurrent = m_ionCurrentWnd->toggleViewAction();
    toggleIonCurrent->setIcon(QIcon(":/tic.png"));
    toggleIonCurrent->setToolTip("Показать полный ионный ток");
    m_toolBar->addAction(toggleIonCurrent);

    connect(m_toolBar,SIGNAL(hZoom(bool)),m_ionCurrentPlot,SLOT(setHZoom(bool)));
    connect(m_toolBar,SIGNAL(vZoom(bool)),m_ionCurrentPlot,SLOT(setVZoom(bool)));
    connect(m_toolBar,SIGNAL(zoomOut()),m_ionCurrentPlot,SLOT(setZoomOut()));
    connect(m_massSpecPlot,SIGNAL(zoomOutChanged(bool)),m_ionCurrentPlot,SLOT(setZoomOut(bool)));
    connect(m_ionCurrentPlot,SIGNAL(zoomOutChanged(bool)),m_massSpecPlot,SLOT(setZoomOut(bool)));
    connect(m_ionCurrentPlot,SIGNAL(hZoomChanged(bool)),m_toolBar,SLOT(setHZoom(bool)));
    connect(m_ionCurrentPlot,SIGNAL(vZoomChanged(bool)),m_toolBar,SLOT(setVZoom(bool)));

    connect(this,SIGNAL(showMassSpec(QObject*)),
            m_ionCurrentPlot,SLOT(setMassSpec(QObject*)));

    //Set up mass spec window
    connect(m_toolBar,SIGNAL(switchScale()),m_massSpecPlot,SLOT(switchScale()));
    connect(m_toolBar,SIGNAL(hZoom(bool)),m_massSpecPlot,SLOT(setHZoom(bool)));
    connect(m_toolBar,SIGNAL(vZoom(bool)),m_massSpecPlot,SLOT(setVZoom(bool)));
    connect(m_toolBar,SIGNAL(zoomOut()),m_massSpecPlot,SLOT(setZoomOut()));
    connect(m_massSpecPlot,SIGNAL(hZoomChanged(bool)),m_toolBar,SLOT(setHZoom(bool)));
    connect(m_massSpecPlot,SIGNAL(vZoomChanged(bool)),m_toolBar,SLOT(setVZoom(bool)));
    connect(m_massSpecPlot,SIGNAL(switchScaleNotify(bool)),
            m_toolBar,SLOT(changeSwitchScalesActionIcon(bool)));
    connect(m_toolBar,SIGNAL(showSplineTriggered()),
            this,SLOT(onShowSplineTriggered()));

    addToolBar(Qt::TopToolBarArea,m_toolBar);
    setCentralWidget(m_massSpecPlot);

    connect(this,SIGNAL(showMassSpec(QObject*)),m_massSpecPlot,SLOT(setMassSpec(QObject*)));
    connect(m_toolBar,SIGNAL(showTotal(bool)),m_massSpecPlot,SIGNAL(showTotal(bool)));
    connect(m_massSpecPlot,SIGNAL(massSpecLoaded()),m_toolBar,SLOT(isShowTotal()));
    connect(m_massSpecPlot,SIGNAL(splineShowChanged(MassSpecPlot::MassSpecSplineShow)),
            m_toolBar,SLOT(setShowSplineIcon(MassSpecPlot::MassSpecSplineShow)));
    m_toolBar->setShowSplineIcon(m_massSpecPlot->splineShow());
    connect(m_massSpecPlot,SIGNAL(splineStd(double)),
            this,SIGNAL(splineStd(double)));
}

MassSpecView::~MassSpecView()
{

}

void MassSpecView::onShowSplineTriggered()
{
    switch(m_massSpecPlot->splineShow())
    {
    case MassSpecPlot::OnlySpline:
        m_massSpecPlot->splineShow(MassSpecPlot::OnlyMassSpec);
        break;
    case MassSpecPlot::OnlyMassSpec:
        m_massSpecPlot->splineShow(MassSpecPlot::MassSpecAndSpline);
        break;
    case MassSpecPlot::MassSpecAndSpline:
        m_massSpecPlot->splineShow(MassSpecPlot::OnlySpline);
        break;
    }
}

void MassSpecView::smoothing(double lambda)
{
    m_massSpecPlot->lambda(lambda);
}

//================================================================

//MassSpecToolBar methods
//================================================================

MassSpecToolBar::MassSpecToolBar(QWidget *parent)
    :
      ZoomPlotToolBar(parent),
      m_switchScales(new QAction(QIcon(":/balance.png"),"Переключение между шкалой масс и времени",this)),
      m_showTotal(new QAction(QIcon(":/sum.png"),"Показать суммарный спектр / Показать единичный спектр", this)),
      m_showSpline(new QAction(QIcon(":/smooth.png"),"Показать линию сплайна", this))
{
    addAction(m_switchScales);
    connect(m_switchScales,SIGNAL(triggered()),this,SIGNAL(switchScale()));

    m_showTotal->setCheckable(true);
    addAction(m_showTotal);
    connect(m_showTotal,SIGNAL(toggled(bool)),this,SIGNAL(showTotal(bool)));

    addAction(m_showSpline);
    connect(m_showSpline,SIGNAL(triggered()),this,SIGNAL(showSplineTriggered()));
}

MassSpecToolBar::~MassSpecToolBar()
{

}

void MassSpecToolBar::isShowTotal()
{
    emit showTotal(m_showTotal->isChecked());
}

void MassSpecToolBar::changeSwitchScalesActionIcon(bool isTimeScale)
{
    if(isTimeScale)
        m_switchScales->setIcon(QIcon(":/clock.png"));
    else
        m_switchScales->setIcon(QIcon(":/balance.png"));
}

void MassSpecToolBar::setShowSplineIcon(MassSpecPlot::MassSpecSplineShow type)
{
    switch(type)
    {
    case MassSpecPlot::OnlyMassSpec:
        m_showSpline->setIcon(QIcon(":/onlyms.png"));
        break;
    case MassSpecPlot::OnlySpline:
        m_showSpline->setIcon(QIcon(":/spline.png"));
        break;
    case MassSpecPlot::MassSpecAndSpline:
        m_showSpline->setIcon(QIcon(":/splineandms.png"));
        break;
    }
}

//================================================================

//MassSpecPlot methods
//================================================================
MassSpecPlot::MassSpecPlot(QWidget *parent)
    :
      ZoomPlot(parent),
      m_msDataStruct(nullptr),
      m_isTimeScale(false)
{
    yAxis->setLabel(QObject::tr("Число ионов"));
    xAxis->setLabel("m/z, Да");
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0,0,new QCPPlotTitle(this,QObject::tr("Масс-спектр")));

    splineBins(1000);
    splineShow(OnlyMassSpec);
    lambda(1000);

    connect(this,SIGNAL(lambdaChanged(double)),this,SLOT(showMassSpec()));
    connect(this,SIGNAL(splineShowChanged(MassSpecPlot::MassSpecSplineShow)),
            this,SLOT(showMassSpec()));
    connect(xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(showMassSpec()));
}

MassSpecPlot::~MassSpecPlot()
{

}

void MassSpecPlot::setMassSpec(QObject *massSpec)
{
    m_msDataStruct = qobject_cast<MassSpectrumBase*>(massSpec);
    connect(m_msDataStruct,SIGNAL(massSpecViewChanged()),this,SLOT(showMassSpec()));
    connect(this,SIGNAL(showTotal(bool)),m_msDataStruct,SLOT(setShowTotal(bool)));

    showMassSpec();

    emit massSpecLoaded();
}

void MassSpecPlot::switchScale()
{
    if(!m_msDataStruct) return;
    if(m_isTimeScale)
    {
        m_isTimeScale = false;
        xAxis->setLabel("m/z, Да");
        xAxis->setRange
                (
                    m_msDataStruct->timeToMass(xAxis->range().lower),
                    m_msDataStruct->timeToMass(xAxis->range().upper)
                );
    }
    else
    {
        m_isTimeScale = true;
        xAxis->setLabel("Время, мксек");
        xAxis->setRange
                (
                    m_msDataStruct->massToTime(xAxis->range().lower),
                    m_msDataStruct->massToTime(xAxis->range().upper)
                );
    }
    emit switchScaleNotify(m_isTimeScale);
    showMassSpec();
}

void MassSpecPlot::showMassSpec()
{
    if(graphCount() != 0)
        clearGraphs(); //remove all graphs if there is any
    if(!m_msDataStruct)
        return; //if m_msDataStuct was not created

    switch(m_splineShow)
    {
    case OnlyMassSpec:
        addMassSpec();
        break;
    case OnlySpline:
        addSpline();
        break;
    case MassSpecAndSpline:
        addMassSpec();
        addSpline();
        break;
    }

    replot();
}

void MassSpecPlot::addSpline()
{
    QVector<double> xvalues, yvalues;
    LogSplines spline = m_msDataStruct->getSplineLine(m_lambda);
    emit splineStd(spline.std());
    if(rescalable())
    {
        if(m_isTimeScale) //show mass spectrum in a time scale
            xvalues = m_msDataStruct->TimeScale(); //set time scale
        else
            xvalues = m_msDataStruct->MassScale(); //set mass scale

        yvalues.resize(xvalues.size());
        int i = 0;

        //Note, spline is build upon the time scale, because it is native
        std::for_each(xvalues.begin(),
                  xvalues.end(),
                  [spline,&yvalues,&i,this](double x)->void

        {
            if(m_isTimeScale)
            {
                yvalues[i++] = spline(x);
            }
            else
                yvalues[i++] = spline(m_msDataStruct->massToTime(x));
        });

        QCPGraph* massSpec = addGraph();
        massSpec->addData(xvalues,yvalues);
        massSpec->setPen(QPen(Qt::red));
        if(rescalable()) massSpec->rescaleAxes();
    }
    else //if axis is not rescalable than build spline using bins
    {
        xvalues.resize(m_nSplineBins+1);
        yvalues.resize(m_nSplineBins+1);
        double xmin = m_isTimeScale ? xAxis->range().lower
                                    : m_msDataStruct->massToTime(xAxis->range().lower);
        double xmax = m_isTimeScale ? xAxis->range().upper
                                    : m_msDataStruct->massToTime(xAxis->range().upper);
        FunVals xy = spline(xmin,xmax,m_nSplineBins);

        int i = 0;
        std::for_each(xy.begin(),
                      xy.end(),
                      [&xvalues,&yvalues,&i,this](FunVal txy)->void
        {
            xvalues[i] = m_isTimeScale ? txy.first : m_msDataStruct->timeToMass(txy.first);
            yvalues[i++] = txy.second;
        });

        QCPGraph* massSpec = addGraph();
        massSpec->addData(xvalues,yvalues);
        massSpec->setPen(QPen(Qt::red));
    }
}

void MassSpecPlot::addMassSpec()
{
    QVector<double> xvalues, yvalues;

    if(m_isTimeScale) //show mass spectrum in a time scale
        xvalues = m_msDataStruct->TimeScale(); //set time scale
    else
        xvalues = m_msDataStruct->MassScale(); //set mass scale

    yvalues = m_msDataStruct->Intensities();

    QCPGraph* massSpec = addGraph();
    massSpec->addData(xvalues,yvalues);

    if(rescalable()) massSpec->rescaleAxes();
}

MassSpecPlot::MassSpecSplineShow MassSpecPlot::splineShow() const
{
    return m_splineShow;
}

void MassSpecPlot::splineShow(MassSpecPlot::MassSpecSplineShow splineShow)
{
    switch(splineShow)
    {
    case MassSpecAndSpline:
    case OnlySpline:
    case OnlyMassSpec:
        m_splineShow = splineShow;
        emit splineShowChanged(splineShow);
        break;
    default:
        QMessageBox::warning(this,
                             "Ошибка при переключении сплайна",
                             "Недопустимое значение параметра");
    }
}

void MassSpecPlot::splineBins(int nBins)
{
    if(nBins <= 0)
    {
        QMessageBox::warning(this,
                             "Ошибка при установке разбиения сплайна",
                             "Допустимы только неотрицательные и ненулевые значения");
        return;
    }

    m_nSplineBins = nBins;

    emit splineBinsChanged(nBins);
}

int MassSpecPlot::splineBins() const
{
    return m_nSplineBins;
}

void MassSpecPlot::lambda(double lambda_)
{
    m_lambda = lambda_;

    emit lambdaChanged(m_lambda);
}

//===========================================================================================

//IonCurrentPlot methods
//================================================================
IonCurrentPlot::IonCurrentPlot(QWidget *parent)
    :
      ZoomPlot(parent),
      m_msDataStruct(nullptr),
      m_massSpecIdxPos(new QCPItemLine(this))
{
    yAxis->setLabel(QObject::tr("Число ионов"));
    xAxis->setLabel("Номер масс-спектра");

    m_massSpecIdxPos->setPen(QPen(Qt::white)); //hide
    this->addItem(m_massSpecIdxPos);
}

IonCurrentPlot::~IonCurrentPlot()
{

}

void IonCurrentPlot::attachCurve(const QVector<double> &x, const QVector<double> &y)
{
    if(graphCount() == 0) //Only one mass spectrum per plot
    {
        addGraph();
    }
    else
    {
        clearGraphs();
        addGraph();
    }
    graph(0)->addData(x,y);

    if(rescalable()) graph(0)->rescaleAxes();

    replot();
}

void IonCurrentPlot::setMassSpec(QObject *massSpec)
{
    m_msDataStruct = qobject_cast<MassSpectrumBase*>(massSpec);

    connect(m_msDataStruct,SIGNAL(massSpecIdxChanged()),
            this,SLOT(showMassSpec()));

    m_massSpecIdxPos->setPen(QPen(Qt::green,2)); //show

    showMassSpec();
}

void IonCurrentPlot::showMassSpec()
{
    QVector<double> massSpecIdxs(m_msDataStruct->getMassSpecNum());
    int i = 0;
    std::for_each
    (
        massSpecIdxs.begin(),
        massSpecIdxs.end(),
        [&i](double& num){ num = static_cast<double>(i++); }
    );

    drawMassSpecIdxPos();

    attachCurve(massSpecIdxs,m_msDataStruct->Chromatogram());
}

void IonCurrentPlot::mousePressEvent(QMouseEvent *event)
{
    if(isZoom()) zoomCheck(event);
    else
        if(isZoomOut()) unzoom();
        else
        {
            int massSpecIdx = static_cast<int>(::round(xAxis->pixelToCoord(event->pos().x())));
            m_msDataStruct->setMassSpecIdx(massSpecIdx);
        }
}

void IonCurrentPlot::drawMassSpecIdxPos()
{
    int idx = m_msDataStruct->getMassSpecIdx();
    if(idx == -1) return; //not accumulated yet

    double nMassSpec = static_cast<double>(idx);
    double I0        = (m_msDataStruct->Chromatogram())[idx];
    m_massSpecIdxPos->start->setCoords(nMassSpec,xAxis->range().lower);
    m_massSpecIdxPos->end->setCoords(nMassSpec,I0);
}

//================================================================
