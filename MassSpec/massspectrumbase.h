#ifndef MASSSPECTRUMBASE_H
#define MASSSPECTRUMBASE_H

#include <QObject>
#include <logsplines.h>
#include <QVector>

///Peal parameters
class PeakParams
{
public:
    inline PeakParams(double x0_ = 0.0,
                      double I0_ = 0.0,
                      double w_ = 0.0)
        :
          m_x0(x0_),m_I0(I0_),m_w(w_){;}

    inline double x0() const { return m_x0; }
    inline double I0() const { return m_I0; }
    inline double w()  const { return m_w; }

    inline void x0(double x0_) { m_x0 = x0_; }
    inline void I0(double I0_) { m_I0 = I0_; }
    inline void w(double w_)   { m_w  = w_; }
private:
    double m_x0; //peak position
    double m_I0; //peak intensity
    double m_w;  //peak width (diffference between two first derivative extrema)
};

class MassSpectrumBase : public QObject
{
    Q_OBJECT
public:
    MassSpectrumBase(QObject* parent = 0);
    virtual ~MassSpectrumBase();

    inline QVector<double> MassScale()   const{ return m_mz; }
    inline QVector<double> TimeScale()   const{ return m_t; }
    inline QVector<double> Intensities() const{ return m_I; }
    inline QVector<double> Chromatogram()const{ return m_tic;}
    virtual size_t getMassSpecNum() const = 0; //returns number of mass spectrums in the data struct

    ///Calculates spline line
    /// \param lambda Smoothing factor
    /// \return Spline for given mass spectrum state
    const LogSplines &getSplineLine(double lambda);

    double massToTime(double mass) const; //Translates mass to time value
    double timeToMass(double time) const; //Translates time to m/z value
    int getMassSpecIdx() const; //Returns current mass spectrum idx
    bool isShowTotal() const;
    bool isNeedToSave() const; //
    void needToSave(bool save = true);
    bool isError() const;

    ///Returns parameters of a peak that is higher than ymin
    /// to the right from the xstart
    /// \param xstart Start point
    /// \param ymin
    PeakParams getPeak(double xstart, double ymin);
signals:
    void calibrationChanged();
    void massSpecIdxChanged();
    void showTotalChanged();
    void massSpecViewChanged(); //connects all signals in one

public slots:
    void calibrate(const QVector<double>& masses, const QVector<double>& times);
    void calibrate(const QVector<double>& masses, const QVector<double>& times, const QVector<double>& intensities);
    void setCalibration(double k, double t0);

    void setMassSpecIdx(int idx);

    void setShowTotal(bool showTotal);

protected:
    double m_k, m_t0; //callibration coeffs

    int m_idx; //mass spectrum index

    bool m_showTotal; //if true then estimates sum over all mass spectrums to show
    bool m_isNeedToSave; //data needs to be saved

    bool m_isError; //True if some errors occur in the instance

    LogSplines* m_spline;
    double m_lambda;

    void deleteSpline();

    ///Data bufferization
    virtual void recalculateMassSpec() = 0;
    QVector<double> m_mz;
    QVector<double> m_I;
    QVector<double> m_t;
    QVector<double> m_tic;

protected slots:
    void createSpline();
};

#endif // MASSSPECTRUMBASE_H
