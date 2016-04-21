/*!
  \author Alexey Chudinoff
  \date 15.04.2016
  \file cubicspline.h
  \brief Cubic spline implementation
  */

#ifndef CUBICSPLINE_H
#define CUBICSPLINE_H

#include <vector>

typedef std::vector<double> SplineCoefs;
typedef std::pair<double,double> FunVal;
typedef std::vector<FunVal> FunVals;

///\brief Implements cubic spline
///Spline is a function on x[1]...x[size()-1] poles, such as
/// for any x : x[i]<=x<=x[i+1] y-value can be calculated
/// : y = a[i] + b[i](x-x[i]) + c[i](x-x[i])^2 + d[i](x-x[i])^3
class CubicSpline
{

public:
    CubicSpline(const FunVals& data);
    ~CubicSpline();

    ///Estimates spline value at a current point
    /// \param [in] x x-value
    /// \returns y-value
    double operator()(double x) const;

private:
    int m_n;
    SplineCoefs m_x,m_a,m_b,m_c,m_d;
};

#endif // CUBICSPLINE_H
