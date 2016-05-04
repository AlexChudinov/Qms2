#ifndef MASSSPECTRUMBASE_H
#define MASSSPECTRUMBASE_H

#include <QObject>
#include <logsplines.h>
#include <QVector>

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
