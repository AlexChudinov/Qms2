#ifndef _SOLVERS_
#define _SOLVERS_

namespace math
{
    /*System of equations with symmetric diagonal matrix*/
    template<class DataType> int tridiagonalsolve //tridiagonal bound matrix
        (
            int n,          //number of equations
            DataType* a,
            DataType* b,    //main diagonal
            DataType* c,
            DataType* r,    //right-hand part
            DataType* x     //solution
        ) throw();

    template<class DataType> int fivediagonalsolve //five diagonal bound matrix
        (
            int n,          //number of equations
            DataType* a,
            DataType* b,
            DataType* c,    //main diagonal
            DataType* d,
            DataType* e,
            DataType* r,    //right-hand part
            DataType* x     //solution
        ) throw();

    template <class DataType>
    int tridiagonalsolve
        (
            int n, //number of equations
            DataType* a,  //down diagonal
            DataType* b,  //main diagonal
            DataType* c,  //upper diagonal
            DataType* r,  //right-hand part
            DataType* x   //solution
        ) throw()
    //solve Ax=b where A is a tridiagonal matrix, returns 0 if it is ok
    //It changes inserted data array b and r. It was taken from
    //https://vk.com/pages?oid=-30481242&p=%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D0%BF%D1%80%D0%BE%D0%B3%D0%BE%D0%BD%D0%BA%D0%B8
    {
        for (int i = 0; i < n-1; i++)
        {
            DataType m = a[i] / b[i];
            b[i+1] = b[i+1] - m * c[i];
            r[i+1] = r[i+1] - m * r[i];
        }
        x[n - 1] = r[n - 1] / b[n - 1];
        for (int i = n - 2; i >= 0; i--)
                x[i] = (r[i] - c[i] * x[i + 1]) / b[i];
        return 0;
    }

    template<class DataType> int fivediagonalsolve
        (
            int n,          //number of equations
            DataType* a,
            DataType* b,
            DataType* c,    //main diagonal
            DataType* d,
            DataType* e,
            DataType* r,    //right-hand part
            DataType* x     //solution
        ) throw()
    {
        for(int i = 0; i < n-2; i++)
        {
            DataType m1 = b[i]/c[i];
            DataType m2 = a[i]/c[i];
            c[i+1] = c[i+1] - m1*d[i];
            d[i+1] = d[i+1] - m1*e[i];
            b[i+1] = b[i+1] - m2*d[i];
            c[i+2] = c[i+2] - m2*e[i];
            r[i+1] = r[i+1] - m1*r[i];
            r[i+2] = r[i+2] - m2*r[i];
        }
        DataType m3 = b[n-2]/c[n-2];
        c[n-1] = c[n-1] - m3*d[n-2];
        r[n-1] = r[n-1] - m3*r[n-2];
        x[n-1] = r[n-1] / c[n-1];
        x[n-2] = (r[n-2] - d[n-2]*x[n-1]) / c[n-2];

        for(int i = n-3; i >= 0; i--)
            x[i] = (r[i] - d[i]*x[i+1] - e[i]*x[i+2]) / c[i];

        return 0;
    }

    ///Solves equation fun(x) = 0 (abs(fun(x))<eps) on interval [a,b]
    /// \param fun Function
    /// \param a Left interval boundary
    /// \param b Right interval bounadry
    /// \param eps Small value
    template<class FunObject, class DataType>
    DataType fZero(FunObject fun, DataType a, DataType b, DataType eps) throw()
    {
        class Abs
        {
        public:
            DataType operator()(DataType x) const
            {
                return x >= static_cast<DataType>(0.0) ? x : -x;
            }
        } abs;

        if(fun(a)*fun(b) > static_cast<DataType>(0.0)
                || (abs(fun(a)) < eps && abs(fun(b)) < eps))
            return fun(a) <= fun(b) ? a : b;

        if(fun(a) < fun(b)) std::swap(a,b);

        while(true)
        {
            DataType c = .5*(a+b), fc = fun(c);
            if (abs(fc)<eps) return c;
            if (fc < 0) b = c;
            if (fc > 0) a = c;
        }

    }
}

#endif // _SOLVERS_
