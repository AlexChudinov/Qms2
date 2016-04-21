#ifndef _calibration_
#define _calibration_
#include <iostream>
#include <matrix.hpp>
#include <cmath>
#include <functional>

using namespace math;

namespace cal{

class calibration{
    matrix mz_;          //m/z-values for calibration
    matrix times_;       //time values for calibration
    matrix w_;           //weighting coefficients it could be peak areas
    matrix coeffs_;      //calibration coefficients
    int nTerms_;         //number of terms in approximating polynomial
    int nPoints_;
    inline matrix build_F_matrix_(const matrix& t) const; //special matrix
public:
    friend std::ostream& operator<<(std::ostream&,const calibration&);

    inline double A() const {return coeffs_(0);}
    inline double B() const {return coeffs_(1);}

    calibration(const double* mz, const double* times, const double* w, int nPoints, int nTerms = 3)
        :
        mz_(mz,nPoints),
        times_(times,nPoints),
        w_(w,nPoints),
        coeffs_(nTerms),
        nTerms_(nTerms),
        nPoints_(nPoints)
/*
It is supposed that m/z = A * (t+t0)^2, where A and t0 is calibration parameters. In the simplest quadratic case
Hence, it can be seen that
B = 2*A*t0
C = A*t0^2
*/
    {
        matrix F = build_F_matrix_(times_);
        switch(nTerms_){
            case 1:{
                matrix a(times_.nrows());
                for (int i = 0; i < nPoints_; i++) a(i) = mz_(i)/times_(i)/times_(i);

                coeffs_(0) = mean(w_,a);
                break;
            }
            case 2:
            {
                std::function<double(double)> sqrt_ = ::sqrt;
                matrix sqrt_mz = map(sqrt_,mz_); //found sqrt of masses;
                coeffs_ = solve_overdetermined_system_with_weights(F,w_,sqrt_mz);
                break;
            }
            default:
                coeffs_ = solve_overdetermined_system_with_weights(F,w_,mz_);
        }
    }

    matrix estimated_mz(const matrix& t) const { //estimate m/z back to see difference between real values
        matrix F = build_F_matrix_(t);
        //std::cout<<t;
        switch(nTerms_){
            case 1:{
                matrix mz(t.nrows()); //create matrix for output mass values
                for (int i = 0; i < mz.nrows(); i++) mz(i) = coeffs_(0)*t(i)*t(i);

                return mz;
            }
            case 2:{
                matrix sqrt_mz = F*coeffs_;
                std::function<double(double)> sqr_ = sqr;
                return map(sqr_,sqrt_mz);
            }
            default:
                return F * coeffs_;
        }
    }

    inline double stderror() const { //calculate standart error
        matrix mz = estimated_mz(times_);
        double se = 0.0;

        if (nPoints_ == nTerms_){ //in the case of not overdetermined system
            //weights is the time data divergence for given point
            for (int i = 0; i < nPoints_; i++ ) se += w_(i)/times_(i)/times_(i);

            return se * mean(w_,times_);
        }

        se = var(w_,mz-mz_);

        return ::sqrt(se * nPoints_ / (nPoints_ - nTerms_));
    }

};

ostream& operator<<(ostream& os, const calibration& calib){
    switch(calib.nTerms_){
    case 1:
        os << "Calibration equation: a*t^2" << endl;
        break;
    case 2:
        os << "Calibration equation: a*(t+t0)^2" << endl;
        break;
    default:
        os << "Calibration equation: ";
        for(int i = 0; i < calib.nTerms_-1; i++) os << "a(" << i << ")*t^" << i << " + ";
        os << "a(" << calib.nTerms_-1 << ")*t^" << calib.nTerms_-1 << endl << endl;
    }
    os << "Backward estimation of masses used for calibration: " << endl;
    matrix mz = calib.estimated_mz(calib.times_);
    os << mz << endl;

    os << "Calibration error: " << calib.stderror() << endl << endl;

    return os;
}

inline matrix calibration::build_F_matrix_(const matrix& t) const {

    matrix F(t.nrows(),nTerms_,1.0);

    for (int i = 0; i < F.nrows(); i++)
    for (int j = 0; j < nTerms_;    j++)
    for (int k = 0; k < j;        k++){
        F(i,j) *= t(i,0);
    }

    return F;
}

}

#endif
