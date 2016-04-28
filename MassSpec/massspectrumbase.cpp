#include "massspectrumbase.h"
#include <calibration.hpp>
#include <QMessageBox>

MassSpectrumBase::MassSpectrumBase(QObject *parent)
    :
      QObject(parent),
      m_k(2.9),m_t0(-0.07), //default calibration parameters taken from the most of our data
      m_showTotal(false),
      m_isNeedToSave(false),
      m_isError(false),
      m_spline(nullptr),
      m_lambda(0.0)
{
    connect(this,SIGNAL(massSpecIdxChanged()),
            this,SIGNAL(massSpecViewChanged()));
    connect(this,SIGNAL(showTotalChanged()),
            this,SIGNAL(massSpecViewChanged()));
    connect(this,SIGNAL(calibrationChanged()),
            this,SIGNAL(massSpecViewChanged()));
}

MassSpectrumBase::~MassSpectrumBase()
{

}

//Functions to estimate calibration coefficients
void MassSpectrumBase::calibrate(const QVector<double> &masses, const QVector<double> &times)
{
    //Do simple argument checking
    int N = masses.size() > times.size() ? times.size() : masses.size();

    if (N < 2)
    {
        QMessageBox::warning
                (
                    0,
                    QObject::tr("Calibration warning"),
                    QObject::tr("Number of points %1 is too small for a callibration.").arg(N),
                    QMessageBox::Ok, QMessageBox::Ok);
        m_k = 1.0;
        m_t0= 0.0;
        return;
    }

    QVector<double> w(N,1.0);

    cal::calibration callibr(masses.data(),times.data(),w.data(),N,2);
    m_k = 1.0 / callibr.A();
    m_t0= -m_k * callibr.B();

    emit calibrationChanged();
}

void MassSpectrumBase::calibrate(const QVector<double> &masses, const QVector<double> &times, const QVector<double> &intensities)
{
    //Do simple argument checking
    int N = masses.size() > times.size() ? times.size() : masses.size();
    N = masses.size() > intensities.size() ? intensities.size() : N;

    if (N < 2)
    {
        QMessageBox::warning
                (
                    0,
                    QObject::tr("Calibration warning"),
                    QObject::tr("Number of points %1 is too small for a callibration.").arg(N),
                    QMessageBox::Ok, QMessageBox::Ok);
        m_k = 1.0;
        m_t0= 0.0;
        return;
    }

    cal::calibration callibr(masses.data(),times.data(),intensities.data(),N,2);
    m_k = 1.0 / callibr.A();
    m_t0= -m_k * callibr.B();

    emit calibrationChanged();
}

void MassSpectrumBase::setCalibration(double k, double t0)
{
    m_k = k;
    m_t0= t0;
    emit calibrationChanged();
}

//Set the mass spectrum idx to show
void MassSpectrumBase::setMassSpecIdx(int idx)
{
    m_idx = idx;
    emit massSpecIdxChanged();
}

int MassSpectrumBase::getMassSpecIdx() const
{
    return m_idx;
}

//Show total mass spectrum on a graph
void MassSpectrumBase::setShowTotal(bool showTotal)
{
    m_showTotal = showTotal;
    emit showTotalChanged();
}

bool MassSpectrumBase::isShowTotal() const
{
    return m_showTotal;
}

double MassSpectrumBase::massToTime(double mass) const
{
    return ::sqrt(mass)*m_k + m_t0;
}

double MassSpectrumBase::timeToMass(double time) const
{
    return ::sqr((time-m_t0)/m_k);
}

bool MassSpectrumBase::isNeedToSave() const
{
    return m_isNeedToSave;
}

void MassSpectrumBase::needToSave(bool save)
{
    m_isNeedToSave = save;
}

bool MassSpectrumBase::isError() const
{
    return m_isError;
}

LogSplines MassSpectrumBase::getSplineLine(double lambda) const
{
    QVector<double> mz  = Intensities();
    QVector<double> time= TimeScale();
    FunVals xy(time.size());
    DataArray w(time.size(),1.0);

    for(size_t i = 0; i < xy.size(); i++)
    {
        xy[i].first = time[i];
        xy[i].second= mz[i];
    }

    return LogSplines(xy,w,lambda);
}
