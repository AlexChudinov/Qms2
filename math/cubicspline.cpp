#include <cubicspline.h>
#include <qcustomplot.h>
#include <Solvers.h>
#include <algorithm>
#include <peacewisepolynomial.h>

extern "C"
{
    void spline (double* x, double* y, double* b, double* c, double* d, int* n);
    double ispline(double* u, const double* x, const double* y,
                   const double* b, const double* c, const double* d, const int* n);
}

CubicSpline::CubicSpline(const FunVals &data)
{
    int i = 0;
    std::valarray<double> a(data.size()),
            b(data.size()),
            c(data.size()),
            d(data.size()),
            polyCoefs(data.size()*4),
            xvals(data.size());

    std::for_each(data.begin(),data.end(),
                  [&](const FunVal& funVal)
    {
        xvals[i] = funVal.first;
        a[i++] = funVal.second;
    });

    i = data.size();

    spline(&xvals[0],std::begin(a),std::begin(b),
           std::begin(c),std::begin(d),&i);

    i = 0;
    for(std::size_t j = 0; j < a.size(); ++j)
    {
        polyCoefs[i++] = d[j];
        polyCoefs[i++] = c[j];
        polyCoefs[i++] = b[j];
        polyCoefs[i++] = a[j];
    }

    m_polynomial = new PeacewisePolynomial(std::begin(xvals),
                                           std::begin(polyCoefs),
                                           xvals.size(),
                                           4);
}

CubicSpline::~CubicSpline()
{

}

double CubicSpline::operator ()(double x) const
{
    return (*m_polynomial)(x);
}

double CubicSpline::fRightMax(double xstart) const
{
    if(xstart >= m_polynomial->rBound()) //calculation goes out of array
        return xstart;

    //Look for zero of derivative
    PeacewisePolynomial dp = m_polynomial->diff();
    double xzero = dp.fRightZero(xstart);

    int i = m_polynomial->idxOfInterval(xzero);
    double fZ = (*this)(xzero);

    if (fZ >= (*this)(i) && fZ >= (*this)(i+1)) //this is the max - return
        return xzero;
    else
        return fRightMax(i+1); // this is not the max - go further
}

const PeacewisePolynomial &CubicSpline::polynomial() const
{
    return *m_polynomial;
}
