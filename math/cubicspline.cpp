#include <cubicspline.h>
#include <qcustomplot.h>
#include <Solvers.h>
#include <algorithm>

extern "C"
{
    void spline (double* x, double* y, double* b, double* c, double* d, int* n);
    double ispline(double* u, const double* x, const double* y,
                   const double* b, const double* c, const double* d, const int* n);
}

CubicSpline::CubicSpline(const FunVals &data)
    :
      m_n(data.size()),
      m_x(m_n),m_a(m_n),m_b(m_n),m_c(m_n),m_d(m_n)
{
    int i = 0;
    std::for_each(data.begin(),data.end(),
                  [this,&i](const FunVal& funVal)
    {
        m_x[i] = funVal.first;
        m_a[i] = funVal.second;
        ++i;
    });

    spline(m_x.data(),m_a.data(),m_b.data(),
           m_c.data(),m_d.data(),&m_n);
}

CubicSpline::~CubicSpline()
{

}

double CubicSpline::operator ()(double x) const
{
    return ispline(&x,m_x.data(),m_a.data(),
                   m_b.data(),m_c.data(),m_d.data(),&m_n);
}
