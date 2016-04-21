#include <ilogspline.h>
#include <logsplines.h>
#include <zoomplot.h>
#include <delegates.h>

ILogSpline::ILogSpline(QObject *parent)
    :
      QObject(parent),
      m_logSpline(nullptr)
{

}

ILogSpline::~ILogSpline()
{
    releaseSpline();
}

void ILogSpline::releaseSpline()
{
    if(m_logSpline) delete m_logSpline;
    m_logSpline = nullptr;
    emit splineDeleted();
}

void ILogSpline::showSpline(QObject *plot, double lambda)
{
    ZoomPlot* zoomPlot = qobject_cast<ZoomPlot*>(plot);
    if (zoomPlot->graphCount() == 0) return; //no graphs on a plot

    //Do for a only one graph with zero index
    const QCPDataMap& data = *zoomPlot->graph(0)->data();
    //Second one graph is always a spline line
    if(zoomPlot->graphCount() > 1)
    {
        zoomPlot->removeGraph(1);
        zoomPlot->addGraph();
    }
    else
    {
        zoomPlot->addGraph();
    }

    FunVals splineData(data.size());
    DataArray w(data.size(),1.0);

    int i = 0;
    for(const QCPData& txy:data)
    {
        splineData[i].first = txy.key;
        splineData[i++].second= txy.value;
    }

    //Build spline with coefs
    m_logSpline = new LogSplines(splineData,w,lambda);
    emit splineCreated(reinterpret_cast<void*>(m_logSpline));

    zoomPlot->graph(1)->setPen(QPen(Qt::red));

    QVector<double> x(data.size()),y(data.size()); //spline data
    i = 0;
    const LogSplines& spline = *m_logSpline;
    for(const QCPData& txy:data)
    {
        x[i] = txy.key;
        y[i++]= spline(txy.key);
    }
    zoomPlot->graph(1)->addData(x,y);

    zoomPlot->replot();
}

//LogSplineModel:
LogSplineModel::LogSplineModel(QObject *parent)
    :
      QStandardItemModel(1,2,parent),
      m_plot(nullptr),
      m_spline(new ILogSpline)
{
    setHorizontalHeaderItem(0,new QStandardItem("Свойство"));
    setHorizontalHeaderItem(1,new QStandardItem("Величина"));

    m_modelItems["Сглаживание"] = new QStandardItem("Сглаживание");

    m_modelItems["Сглаживание"]->setEditable(false);
    setItem(0,m_modelItems["Сглаживание"]);
    QStandardItem* emptyItem = new QStandardItem();
    setItem(0,1,emptyItem);
    emptyItem->setEditable(false);

    QList<QStandardItem*> smoothingFactor;
    m_modelItems["Параметр сглаживания"] = new QStandardItem("Параметр сглаживания");
    m_modelItems["Величина параметра сглаживания"] = new QStandardItem("1000");

    smoothingFactor.push_back(m_modelItems["Параметр сглаживания"]);
    smoothingFactor.push_back(m_modelItems["Величина параметра сглаживания"]);
    m_modelItems["Параметр сглаживания"]->setEditable(false);

    m_modelItems["Сглаживание"]->insertRow(0,smoothingFactor);

    connect(this,SIGNAL(smoothingChanged(QObject*,double)),
            m_spline,SLOT(showSpline(QObject*,double)));
    connect(this,SIGNAL(deleteSpline()),m_spline,SLOT(releaseSpline()));
    connect(this,SIGNAL(itemChanged(QStandardItem*)),
            this,SLOT(onItemChanged(QStandardItem*)));
}

LogSplineModel::~LogSplineModel()
{

}

void LogSplineModel::buildSmoothedLine()
{
    double lambda = static_cast<double>
        (m_modelItems["Величина параметра сглаживания"]->data(Qt::EditRole).toInt());
    if(m_plot)
    {
        emit smoothingChanged(m_plot,lambda);
    }
}

void LogSplineModel::setPlot(QObject *plot)
{
    m_plot = plot;
    buildSmoothedLine();
}

QObject* LogSplineModel::getPlot() const
{
    return m_plot;
}

void LogSplineModel::onItemChanged(QStandardItem *item)
{
    if (item == m_modelItems["Величина параметра сглаживания"])
    {
        buildSmoothedLine();
    }
}

void LogSplineModel::onDeleteSpline()
{
    ZoomPlot* plot = qobject_cast<ZoomPlot*>(m_plot);
    plot->removeGraph(1);
    plot->replot();
    emit deleteSpline();
}

//Log splines view
LogSplinesView::LogSplinesView(QWidget *parent)
    :
      QTreeView(parent),
      m_splinesModel(new LogSplineModel(parent))
{
    setModel(m_splinesModel);
    setItemDelegateForRow(0,new SmoothingDelegate(this));

    connect(this,SIGNAL(deleteSpline()),m_splinesModel,SLOT(onDeleteSpline()));
}

LogSplinesView::~LogSplinesView()
{

}

void LogSplinesView::setPlot(QObject *plot)
{
    m_splinesModel->setPlot(plot);
}
