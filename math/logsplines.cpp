#include "logsplines.h"
#include <algorithm>
#include <smoothing_splines.h>

LogSplines::LogSplines(const FunVals &data, const DataArray &w, double lambda)
{
    if(data.empty())
    {
        m_cubicSpline = nullptr;
        return;
    }
    DataArray
            x(data.size()),
            lgy(data.size()),
            lgw(data.size()),
            smoothedLgy(data.size());
    FunVals xLgy(data.size());
    m_yMin =
             std::min_element(
                data.begin(),
                data.end(),
                [](const FunVal& e1, const FunVal& e2)->bool
    {
        return e1.second < e2.second;
    })->second;

    int i = 0;
    std::for_each(
                data.begin(),
                data.end(),
                [&](const FunVal& txy)->void
    {
        double y = txy.second - m_yMin + 1.0;
        x[i]   = txy.first;
        lgw[i] = w[i]/(y*y);
        lgy[i++] = ::log( y );
    });

    math::third_order_smoothing_spline_eq
            (data.size(),
             lgy.data(),
             lgw.data(),
             lambda,
             smoothedLgy.data());

    i = 0;
    double x0   = 0.0; //average accumulator
    double x02  = 0.0; //average square accumulator
    double wnorm= 0.0; //sum of statistical weights
    for(FunVal& xy:xLgy)
    {
        double tx;
        xy.first = x[i];
        xy.second= smoothedLgy[i];
        tx = ((::exp(xy.second))+m_yMin-1.0) - data[i].second;
        x0 += w[i]*tx;
        x02+= w[i]*(tx*tx);
        wnorm+= w[i++];
    }

    m_std = ::sqrt((x02-(x0*x0)/wnorm)/wnorm
                   * static_cast<double>(xLgy.size())/static_cast<double>(xLgy.size()-1));

    m_cubicSpline = new CubicSpline(xLgy);
}

LogSplines::LogSplines(const LogSplines &spline)
    :
      m_std(spline.std()),
      m_yMin(spline.yMin())
{
    if(spline.cubicSpline())
        m_cubicSpline = new CubicSpline(*spline.cubicSpline());
    else
        m_cubicSpline = nullptr;
}

LogSplines& LogSplines::operator=(const LogSplines& spline)
{
    m_std = spline.std();
    m_yMin= spline.yMin();
    m_cubicSpline = new CubicSpline(*spline.cubicSpline());
    return *this;
}

LogSplines::~LogSplines()
{
    delete m_cubicSpline;
}

double LogSplines::operator ()(double x) const
{
    if(!m_cubicSpline) return 0.0;
    const CubicSpline& spline = *m_cubicSpline;
    return ::exp(spline(x)) + m_yMin - 1.0;
}

double LogSplines::std() const
{
    return m_std;
}

FunVals LogSplines::operator ()(double xmin, double xmax, int nBins) const
{
    if(!m_cubicSpline) return FunVals();
    const LogSplines& spline = *this;

    //error checking
    if(xmin == xmax)
    {
        FunVal splineVal;
        splineVal.first = xmin;
        splineVal.second= spline(xmin);
        return FunVals(1,splineVal);
    }
    if(xmax < xmin) std::swap(xmax,xmin);
    if(nBins <= 0) return FunVals();

    //calculate step
    double h = (xmax-xmin)/nBins;
    FunVals splineVals(nBins+1);
    std::for_each(splineVals.begin(),
                  splineVals.end(),
                  [h,&xmin,spline](FunVal& splineVal)->void
    {
       splineVal.first = xmin;
       splineVal.second= spline(xmin);
       xmin += h;
    });

    return splineVals;
}
