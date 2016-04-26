#include "idataanalysis.h"

IDataAnalysis::IDataAnalysis(QObject *parent)
    :
      QStandardItemModel(parent)
{

    QStandardItem* smoothingTitle = new QStandardItem("Сглаживание");
    smoothingTitle->setIcon(QIcon(":/splineandms.png"));
    QStandardItem* smoothingFactor = new QStandardItem("Сглаживающий параметр");
    QStandardItem* smoothingStd = new QStandardItem("Стандартное отклонение");
    m_smoothingVal = new QStandardItem("1000.0");
    m_smoothingVal->setData(1000.0,Qt::EditRole);
    m_stdVal = new QStandardItem("0.0");
    m_stdVal->setData(0.0,Qt::DisplayRole);
    m_stdVal->setEditable(false);
    setColumnCount(2);
    setHorizontalHeaderItem(0,new QStandardItem("Параметр"));
    setHorizontalHeaderItem(1,new QStandardItem("Величина"));
    setRowCount(1);
    setItem(0,0,smoothingTitle);
    smoothingTitle->setEditable(false);
    setItem(0,1,new QStandardItem);
    item(0,1)->setSelectable(false);
    item(0,1)->setEditable(false);

    QList<QStandardItem*> smoothingFactorList;
    smoothingFactorList.push_back(smoothingFactor);
    smoothingFactorList.push_back(m_smoothingVal);
    smoothingFactor->setEditable(false);
    smoothingFactor->setBackground(QBrush(Qt::green));
    m_smoothingVal->setBackground(QBrush(Qt::yellow));

    QList<QStandardItem*> stdList;
    stdList.push_back(smoothingStd);
    stdList.push_back(m_stdVal);
    smoothingStd->setEditable(false);
    smoothingStd->setBackground(QBrush(Qt::green));
    m_stdVal->setBackground(QBrush(Qt::yellow));

    smoothingTitle->appendRow(smoothingFactorList);
    smoothingTitle->appendRow(stdList);

    connect(this,SIGNAL(itemChanged(QStandardItem*)),
            this,SLOT(smoothing(QStandardItem*)));
}

IDataAnalysis::~IDataAnalysis()
{

}

void IDataAnalysis::smoothing(double smoothing)
{
    m_splineSmoothingFactor = smoothing;
    emit smoothingChanged(smoothing);
}

double IDataAnalysis::smoothing() const
{
    return m_splineSmoothingFactor;
}

void IDataAnalysis::splineStd(double splineStd)
{
    m_splineDeviation = splineStd;
    m_stdVal->setData(splineStd,Qt::DisplayRole);
    emit splineStdChanged(splineStd);
}

double IDataAnalysis::splineStd() const
{
    return m_splineDeviation;
}

void IDataAnalysis::smoothing(QStandardItem* item)
{
    if(item != m_smoothingVal) return;

    smoothing(item->data(Qt::EditRole).toDouble());
}

IDataAnalysisView::IDataAnalysisView(QWidget *parent)
    :
      QTreeView(parent),
      m_model(new IDataAnalysis(qobject_cast<QObject*>(parent)))
{
    setModel(m_model);
    connect(m_model,SIGNAL(smoothingChanged(double)),
            this,SIGNAL(smoothing(double)));
    connect(this,SIGNAL(splineStd(double)),
            m_model,SLOT(splineStd(double)));
}

IDataAnalysisView::~IDataAnalysisView()
{

}
