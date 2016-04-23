#include "idataanalysis.h"

IDataAnalysis::IDataAnalysis(QObject *parent)
    :
      QStandardItemModel(parent)
{
    QStandardItem* smoothingTitle = new QStandardItem("Сглаживание");
    QStandardItem* smoothingFactor = new QStandardItem("Сглаживающий параметр");
    QStandardItem* smoothingStd = new QStandardItem("Стандартное отклонение");

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
    emit splineStdChanged(splineStd);
}

double IDataAnalysis::splineStd() const
{
    return m_splineDeviation;
}
