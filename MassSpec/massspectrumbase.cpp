#include "massspectrumbase.h"
#include <calibration.hpp>
#include <QMessageBox>
#include <peacewisepolynomial.h>

MassSpectrumBase::MassSpectrumBase(QObject *parent)
    :
      QObject(parent),
      m_k(2.9),m_t0(-0.07), //default calibration parameters taken from the most of our data
      m_idx(-1),
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
    deleteSpline();
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
    if(idx == m_idx) return;
    m_idx = idx;
    if(m_spline) createSpline();
    recalculateMassSpec();
    emit massSpecIdxChanged();
}

int MassSpectrumBase::getMassSpecIdx() const
{
    return m_idx;
}

//Show total mass spectrum on a graph
void MassSpectrumBase::setShowTotal(bool showTotal)
{
    if(showTotal == m_showTotal) return;
    m_showTotal = showTotal;
    if(m_spline) createSpline();
    recalculateMassSpec();
    emit showTotalChanged();
    //if(m_spline) createSpline();
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

const LogSplines& MassSpectrumBase::getSplineLine(double lambda)
{
    if(m_spline && m_lambda == lambda) return *m_spline;

    m_lambda = lambda;
    createSpline();

    return *m_spline;
}

void MassSpectrumBase::deleteSpline()
{
    if(m_spline)
    {
        delete m_spline;
        m_spline = nullptr;
    }
}

void MassSpectrumBase::createSpline()
{
    deleteSpline();

    FunVals xy(m_t.size());
    DataArray w(m_t.size(),1.0);

    int i = 0;
    std::for_each(xy.begin(),
                  xy.end(),
                  [this,&i](FunVal& txy)
    {
        txy.first = m_t[i];
        txy.second= m_I[i++];
    });

    m_spline = new LogSplines(xy,w,m_lambda);
}

PeakParams MassSpectrumBase::getPeak(double xstart, double ymin)
{
    double x0,I0,xl,xr;
    x0 = m_spline->cubicSpline()->fRightMax(xstart);
    I0 = (*m_spline)(x0);
    if(I0 < ymin)
    {
        int idx = m_spline->cubicSpline()->polynomial().idxOfInterval(x0);
        return getPeak(idx+1,ymin);
    }
    xl = m_spline->cubicSpline()->polynomial().diff().fLeftZero(x0);
    xr = m_spline->cubicSpline()->polynomial().diff().fRightZero(x0);
    return PeakParams(x0,I0,xr-xl);
}
