#ifndef _matrix_
#define _matrix_

//Class contains main matrix operations
//Sometimes it could be very useful

#include <sstream>
#include <random>
#include <ctime>
#include <exception>
#include <cstdlib>
#include <fstream>
#include <iostream>

using std::istream;
using std::ostream;
using std::string;
using std::ostringstream;
using std::endl;

namespace math{ //math namespace

inline double sqr(double x) throw() { return x*x; }

class matrix{
   class exception : public std::exception{
        std::string msg_;
    public:
        exception (const char* msg) throw() : msg_(msg) {;}
        exception (const ostringstream& msg) throw() : msg_(msg.str().c_str()) {;}
        exception () throw() : msg_() {;}
        exception (const exception& ex) throw() : msg_(ex.what()) {;}
        exception& operator= (const exception& ex) throw() {
            msg_ = ex.what();
            return *this;
        }
        virtual ~exception() throw() {;}
        virtual const char* what() const throw() { return msg_.c_str(); }
    };

    class Random{ //to simulate matrix of random numbers
        bool active_;
        std::mt19937_64 gen_;
        std::uniform_real_distribution<> uniform_;
        std::normal_distribution<> norm_;
    public:
        explicit Random()
        :
        active_(true),
        gen_(::time(0)),
        uniform_(0.0,1.0),
        norm_(0.0,1.0)
        {;}
        double uniform() throw() { return uniform_(gen_); } //return uniform random number
        double normal() throw() { return norm_(gen_); } //return normal random number
        operator bool() const throw() { return active_; } //check if generator was created
    };

    double *a_; //matrix elements

    int nrows_; //number of rows

    int ncols_; //number of cols

    //true if matrix contains the element
    inline bool check_element_(int i, int j) const throw() { return (i<nrows_) & (j<ncols_); }
    inline void ispositive_(int i) const throw(); //number of rows and the cols should be positive
    inline void check_col_and_row_() const throw() { ispositive_(ncols_); ispositive_(nrows_); }

    //memory management
    void delete_points_() throw();
    void allocate_points_() throw(exception);
    void reallocate_points_() throw(exception);

public:
    //The classes can use matrix private data
    friend matrix operator*(const matrix&,const matrix&)throw(matrix::exception);
    friend matrix operator+(const matrix&,const matrix&)throw(matrix::exception);
    friend matrix operator-(const matrix&,const matrix&)throw(matrix::exception);

    //return matrix element, keep the elements columnwise
    //if second argument was not specified then return data from column
    inline double& operator()(int i, int j = 0) throw(exception);
    inline double  operator()(int i, int j = 0) const throw(exception);


    inline int nrows(void) const { return nrows_; }
    inline int ncols(void) const { return ncols_; }
    matrix& rand(double a, double b); //fill the matrix with random numbers uniformly distributed in the [a,b]
    matrix& randn(double t0, double s); //fill the matrix with normal random numbers with mean t0 and dispersion s

    inline void add_col(const matrix& col); //add column to the matrix
    inline matrix solve() const; //solve the system of equations where the last column of matrix is the free terms
    inline matrix transpose() const; //return transposed matrix
    inline void reset(); //set all elements in matrix to zero

//Constructors:
    matrix(const double* a, int _nrows, int _ncols = 1) throw(exception)
        : //create from c++ array
        nrows_(_nrows),
        ncols_(_ncols)
    {
        check_col_and_row_();
        allocate_points_();
        for (int i = 0; i < nrows_; i++)
            for (int j = 0; j < ncols_; j++)
                (*this)(i,j) = a[j*nrows_ + i];
    }

    matrix(int _nrows, int _ncols = 1, double val = 0.0) throw(exception)
        : //create matrix initialised with one value
        nrows_(_nrows),
        ncols_(_ncols)
    {
        check_col_and_row_();
        allocate_points_();
        for (int i = 0; i < nrows_; i++)
            for (int j = 0; j < ncols_; j++)
                (*this)(i,j) = val;
    }

    matrix(const matrix& m)
        : //copy matrix
        nrows_(m.nrows()),
        ncols_(m.ncols())
    {
        allocate_points_();
        for (int i = 0; i < nrows_; i++)
            for (int j = 0; j < ncols_; j++)
                (*this)(i,j) = m(i,j);
    }

    //assign one matrix to another
    void operator=(const matrix& m)
    {
        if (nrows()!=m.nrows() || ncols()!=m.ncols()){
            delete_points_();
            nrows_ = m.nrows();
            ncols_ = m.ncols();
            allocate_points_();
        }
        for (int i = 0; i < nrows_; i++)
            for (int j = 0; j < ncols_; j++)
                (*this)(i,j) = m(i,j);
    }

    virtual ~matrix() { delete_points_(); }
};

//Operations with elements:
double& matrix::operator() (int i, int j) throw(exception){
    //return matrix element, keep the elements columnwise
    if (!check_element_(i,j)){
        ostringstream os;
        os << "Matrix does not contain element with number ("
           << i << ", " << j <<") " << endl;
        exception ex(os);
        throw(ex);
    }
    return a_[j*nrows_ + i];
}

double matrix::operator() (int i, int j) const throw(exception) {
    //return matrix element, keep the elements columnwise
    if (!check_element_(i,j)){
        ostringstream os;
        os << "Matrix does not contain element with number ("
           << i << ", " << j <<") " << endl;
        exception ex(os);
        throw(ex);
    }
    return a_[j*nrows_ + i];
}

//Fill matrix with zeros
void matrix::reset(){
    matrix& m = *this;
    for(int i = 0; i < nrows_; i++)
    for(int j = 0; j < ncols_; j++){
        m(i,j) = 0.0;
    }
}

//**************************************************************

//Operation with memory:

void matrix::ispositive_(int i) const throw(){
    if (i <= 0){
        ostringstream os;
        os << "Negative or zero number is in matrix numeration." << endl;
        throw(exception(os));
    }
}

void matrix::allocate_points_() throw(exception){
    a_ = (double*)::malloc(nrows_*ncols_*sizeof(double));
    if (!a_){
        ostringstream os;
        os << "Malloc returned null pointer." << endl;
        exception ex(os);
        throw(ex);
    }
}

void matrix::delete_points_() throw() {
    ::free((void*)a_);
}

void matrix::reallocate_points_() throw(exception){
    a_ = (double*)::realloc((void*)a_,nrows_*ncols_*sizeof(double));
    if (!a_){
        ostringstream os;
        os << "Realloc returned null pointer." << endl;
        exception ex(os);
        throw(ex);
    }
}

//*************************************************************

//Out matrix to the string stream
ostream& operator<<(ostream& os, const matrix& mat){ //for debugging purposes
    for (int i = 0; i < mat.nrows(); i++){
    for (int j = 0; j < mat.ncols(); j++){
        os << mat(i,j) << "\t";
    }
        os << endl;
    }
    return os;
}

istream& operator>>(istream& in, matrix& m){
    for (int i = 0; i < m.nrows(); i++)
    for (int j = 0; j < m.ncols(); j++){
        if (in.eof()) break;
        in >> m(i,j);
    }
    return in;
}

//************************************************************

//Matrix algebra
template <class FcnClass> //apply functor to all matrix elements
matrix map(const FcnClass& fcn, const matrix& m){
    matrix result = m;
    for(int i = 0; i < m.nrows(); i++)
    for(int j = 0; j < m.ncols(); j++){
        result(i,j) = fcn(m(i,j));
    }
    return result;
}

matrix operator*(double a, const matrix& m) throw() {
    matrix result = m;
    for(int i = 0; i < m.nrows(); i++)
    for(int j = 0; j < m.ncols(); j++){
        result(i,j) = a*m(i,j);
    }
    return result;
}

matrix operator* (const matrix& m1, const matrix& m2) throw(matrix::exception) {
    if (m1.ncols() != m2.nrows()){
        ostringstream os;
        os << "Matrices mismatch for multiplication." << endl;
        matrix::exception ex(os);
        throw(ex);
    }

    matrix result (m1.nrows(),m2.ncols());
    for (int i = 0; i < result.nrows(); i++)
        for (int j = 0; j < result.ncols(); j++)
            for (int k = 0; k < m1.ncols(); k++)
                result(i,j) += m1(i,k)*m2(k,j);
    return result;
}

matrix operator- (const matrix& m1, const matrix& m2) throw(matrix::exception) {
    if (m1.ncols() != m2.ncols() || m1.nrows() != m2.nrows()){
        ostringstream os;
        os << "Matrices mismatch for subtraction." << endl;
        matrix::exception ex(os);
        throw(ex);
    }

    matrix result(m1.nrows(),m1.ncols());
    for (int i = 0; i < result.nrows(); i++)
        for (int j = 0; j < result.ncols(); j++)
            result(i,j) = m1(i,j) - m2(i,j);
    return result;
}

matrix operator+ (const matrix& m1, const matrix& m2) throw(matrix::exception) {
    if (m1.ncols() != m2.ncols() || m1.nrows() != m2.nrows()){
        ostringstream os;
        os << "Matrices mismatch for addition." << endl;
        matrix::exception ex(os);
        throw(ex);
    }

    matrix result(m1.nrows(),m1.ncols());
    for (int i = 0; i < result.nrows(); i++)
        for (int j = 0; j < result.ncols(); j++)
            result(i,j) = m1(i,j) + m2(i,j);
    return result;
}
//*************************************************************

//Matrix generators:
//creates the matrix of equally spaced points
inline matrix range(double start, double step, double n) throw() {
    matrix result(n);
    double current_value = start;
    for(int i = 0; i < n; i++){
        result(i) = current_value;
        current_value += step;
    }
    return result;
}
//creates matrix of uniformly distributed [a,b] values
matrix& matrix::rand(double a, double b){
    static Random rnd;

    if (!rnd){ //if rnd is not active then create new one
        rnd = Random();
    }

    matrix& m = *this;

    for(int i = 0; i < nrows_; i++)
    for(int j = 0; j < ncols_; j++){
        m(i,j) = (b-a) * rnd.uniform() + a;
    }

    return m;
}
//creates matrix of normally distributed (0,1) values
matrix& matrix::randn(double t0, double s){
    static Random rnd;

    if (!rnd){ //if rnd is not active then create new one
        rnd = Random();
    }

    matrix& m = *this;

    for(int i = 0; i < nrows_; i++)
    for(int j = 0; j < ncols_; j++){
        m(i,j) = s * rnd.normal() + t0;
    }

    return m;
}
//**********************************************************

//System of equations
matrix solve_overdetermined_system(const matrix& S, const matrix& b){
    /*Solves overdetermined system of linear equations Sx = b,
      where x are unknown values*/
    matrix ST = S.transpose();
    matrix S1 = ST*S;
    matrix b1 = ST*b;
    S1.add_col(b1);
    return S1.solve();
}

matrix solve_overdetermined_system_with_weights(const matrix& S, const matrix& w, const matrix& b){
    /*Solves overdetermined system of linear equations Sx = b,
      where x are unknown values and w is statistic weights*/
    matrix w_square(w.nrows(),w.nrows()); //create square matrix of weights
    for (int i = 0; i < w.nrows(); i++) w_square(i,i) = w(i);

    matrix ST = S.transpose();
    matrix S1 = ST*w_square*S;
    matrix b1 = ST*w_square*b;
    S1.add_col(b1);
    return S1.solve();
}
//Solve usual system of equations
matrix matrix::solve () const {

    matrix x (nrows_);
    matrix a = *this;

    for (int k = 1; k < nrows_; k++)
    for (int j = k; j < nrows_; j++){
        double m = a(j,k-1) / a(k-1,k-1);
    for (int i = 0; i < ncols_; i++)
        a(j,i) -= m*a(k-1,i);
    }

    for (int i = nrows_-1; i >= 0; i--) {
        for (int j = i+1; j < nrows_; j++) x(i,0) += a(i,j)*x(j,0);
        x(i,0) = (a(i,nrows_) - x(i,0))/a(i,i);
    }

    return x;
}

//Operations with matrix misc.
//transposition
matrix matrix::transpose() const {
    matrix a(ncols_,nrows_);
    const matrix& m = *this;
    for(int i = 0; i < nrows_; i++)
    for(int j = 0; j < ncols_; j++)
            a(j,i) = m(i,j);

    return a;
}
//add column to matrix
void matrix::add_col(const matrix& col) {
    ncols_++;
    reallocate_points_();
    matrix& m = *this;
    for (int i = 0; i < nrows_; i++) m(i,ncols_-1) = col(i); //copy new column
}
//estimate total square deviation between two matrices
double dev(const matrix& m1, const matrix& m2){//deviation between two matrices
    double result = 0.0;
    for(int i = 0; i < m1.nrows(); i++)
    for(int j = 0; j < m1.ncols(); j++){
        result += (m1(i,j) - m2(i,j)) * (m1(i,j) - m2(i,j));
    }
    return result;
}

//return mean of row vector t with weightings w
double mean(const matrix& w, const matrix& t){
    double s =  0.0;
    double t0 = 0.0;
    for(int i = 0; i < t.nrows(); i++){
        t0 += w(i) * t(i);
        s  += w(i);
    }
    return t0/s;
}

//return variance of vector t with weightings w
double var(const matrix& w, const matrix& t){
    double t0  = mean(w,t);
    double v   = 0.0;
    double s   = 0.0;
    for(int i = 0; i < t.nrows(); i++){
        s += w(i);
        v += w(i) * (t(i) - t0) * (t(i) - t0);
    }
    return v/s;
}

//return sum of vector elements
double sum(const matrix& w){
    double s = 0.0;
    for(int i = 0; i < w.nrows(); i++) s += w(i);
    return s;
}

}

#endif
