/*!
  \author Alexey Chudinov
  \file smlogsplines.h
  \date 16.04.2016
  \brief Smoothing logarithmic splines implementation
*/

#ifndef SMLOGSPLINES_H
#define SMLOGSPLINES_H

#include <cubicspline.h>

typedef std::vector<double> DataArray;

///\brief Implements logarithmic splines
///If S(x) is an usual spline function then logarithm spline function
/// can be expressed as exp(S)
class LogSplines
{
public:
    ///Creates smoothed logarithm spline
    /// \param data Function values
    /// \param w Statistical weigthings
    /// \param lambda Smoothing factor
    LogSplines(const FunVals& data, const DataArray& w, double lambda);
    LogSplines(const LogSplines& spline);
    LogSplines& operator=(const LogSplines& spline);

    ~LogSplines();

    ///Returns spline Y-value for a given x value
    /// \param x X-value
    /// \returns Y-value
    double operator()(double x) const;

    ///Returns vector of spline values on choosen interval
    /// \param xmin Lower interval bound
    /// \param xmax Upper interval bound
    /// \param nBins Number of bins in interval
    /// \returns Std vector of calculated spline values
    FunVals operator()(double xmin, double xmax, int nBins) const;

    ///Returns smoothed spline standard deviation from the original raw data points
    /// \returns Standart deviation
    double std() const;

    inline double yMin() const { return m_yMin; }
    ///\returns Inner cubic spline pointer
    CubicSpline* cubicSpline() const { return m_cubicSpline; }


private:
    double m_std; //standard spline deviation
    double m_yMin;
    CubicSpline* m_cubicSpline;
};

#endif // SMLOGSPLINES_H
