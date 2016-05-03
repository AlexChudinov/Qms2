#include "peacewisepolynomial.h"
#include <Solvers.h>

#define RU_LOCALE //defines russian locale for error string

PeacewisePolynomial::PeacewisePolynomial(const double *x,
                                         const double *polyCoefs,
                                         int N,
                                         int M)
    :
      m_x(x,N),
      m_polyCoefs(polyCoefs,N*M),
      m_polyOrder(M)
{
    int state = 0;
    if ( N <= 0 ) state |= static_cast<int>(Empty);
    if ( M < 1)   state |= static_cast<int>(BadOrder);
    m_state = static_cast<ErrorState>(state);

    m_className = "PeacewisePolynomial";

#if defined(RU_LOCALE)
    m_noError = "Нет ошибок";
    m_emptyError = "Заданы пустые массивы";
    m_badOrder = "Задано недопустимое значение степени полинома";
#elif defined(ENG_LOCALE)
    m_noError = "No errors";
    m_emptyError = "Empty arrays";
    m_badOrder = "Invalid polynom order value";
#endif
}

PeacewisePolynomial::~PeacewisePolynomial()
{

}

PeacewisePolynomial::operator bool() const
{
    return static_cast<bool>(m_state);
}

PeacewisePolynomial::ErrorState PeacewisePolynomial::errorState() const
{
    return m_state;
}

std::string PeacewisePolynomial::errorStateStr() const
{
    switch(m_state)
    {
    case NoError:
        return std::string(m_noError);
    case Empty:
        return std::string(m_emptyError);
    case BadOrder:
        return std::string(m_badOrder);
    default:
        return "Unexpected state.";
    }
}

std::size_t PeacewisePolynomial::idxOfInterval(double x) const
{
    std::size_t start = 0;
    std::size_t end = m_x.size() - 1;

    if(x < m_x[start]) return start;
    if(x >= m_x[end]) return end;

    while(end-start != 1)
    {
        std::size_t m = (end+start)/2;
        if(x >= m_x[m]) start = m;
        else end = m;
    }

    return start;
}

double PeacewisePolynomial::operator ()(double x) const
{
    if(m_state) return 0.0;

    int idx = idxOfInterval(x);
    DArray coefs = m_polyCoefs[std::slice(idx*m_polyOrder,m_polyOrder,1)];

    double y = 0.0;
    double h = x-m_x[idx];
    std::for_each(std::begin(coefs),
                  std::end(coefs),
                  [&](double a){ y = y*h+a;});

    return y;

}

PeacewisePolynomial PeacewisePolynomial::diff() const
{
    if(m_polyOrder == 1)
    {
        return PeacewisePolynomial(std::begin(m_x),
                                   std::begin(std::valarray<double>(0.0,m_x.size())),
                                   m_x.size(),
                                   1);
    }
    else
    {
        int n = m_polyOrder - 1;
        std::valarray<double> polyCoefs(m_x.size()*n);
        for(int i = 0; i < n; ++i)
        {
            polyCoefs[std::slice(i,m_x.size(),n)]
                    = (n-i)*m_polyCoefs[std::slice(i,m_x.size(),m_polyOrder)];
        }

        return PeacewisePolynomial(std::begin(m_x),
                                   std::begin(polyCoefs),
                                   m_x.size(),
                                   n);
    }
}

double PeacewisePolynomial::fRightZero(double xstart) const
{
    unsigned int i = idxOfInterval(xstart);

    while(i != m_x.size()-1 && (*this)(i)*(*this)(++i) > 0);

    return math::fZero(*this,
                       static_cast<double>(i-1),
                       static_cast<double>(i),
                       1e-10);
}
