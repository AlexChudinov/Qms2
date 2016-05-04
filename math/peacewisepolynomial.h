/*!
  \author Alexey Chudinov
  \file peacewisepolynomial.h
  \date 28.04.2016
  \brief Peacewise polynomial implementation
*/
#ifndef PEACEWISEPOLYNOMIAL_H
#define PEACEWISEPOLYNOMIAL_H

#include <valarray>

///Peacewise polynomial implementation
class PeacewisePolynomial
{
    typedef std::valarray<double> DArray;
public:

    ///State of a current instance
    enum ErrorState
    {
        NoError = 0x00, ///<Everything is OK
        Empty   = 0x01, ///<Initialized with empty arrays
        BadOrder = 0x02 ///<Order that is lower than 1 was set
    };

    ///Creates peacewise polynomial
    /// It is supposed that input x array is already sorted,
    /// N >= 0 and M >= 1
    /// \param x x-values
    /// \param polyCoefs Peacewise polynomial coefficients
    /// \param N Number of points
    /// \param M Order of polynomials
    PeacewisePolynomial(const double* x,
                        const double* polyCoefs,
                        int N, int M);
    ~PeacewisePolynomial();

    ///\return True if instance is in error state
    operator bool() const;

    ///\return Error state type
    ErrorState errorState() const;
    ///\return Error state string
    std::string errorStateStr() const;

    ///Calculates polynomial value
    double operator()(double x) const;

    ///Calculates polynomial derivative
    PeacewisePolynomial diff() const;

    ///\return x-value of closest polynomial zeros at the right side from xstart
    /// it is supposed that polynomial has continuous first derivative
    double fRightZero(double xstart) const;

    ///Returns left-most point of peacewisepolynomial
    inline double lBound() const { return *std::begin(m_x); }
    ///Returns right-most point of a peacewisepolynomial
    inline double rBound() const { return *(std::end(m_x)-1);}

    ///\return Interval index to witch x belongs to
    std::size_t idxOfInterval(double x) const;

private:
    DArray m_x;
    DArray m_polyCoefs;

    int m_polyOrder;

    ErrorState m_state;

    const char* m_emptyError;
    const char* m_noError;
    const char* m_badOrder;

    const char* m_className;
};

#endif // PEACEWISEPOLYNOMIAL_H
