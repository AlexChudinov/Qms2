#ifndef IDATAANALYSIS_H
#define IDATAANALYSIS_H

#include <QStandardItemModel>

class IDataAnalysis : public QStandardItemModel
{
    Q_OBJECT
public:
    IDataAnalysis(QObject* parent = 0);
    ~IDataAnalysis();

    void smoothing(double smoothing);
    double smoothing() const;

    void splineStd(double splineStd);
    double splineStd() const;
signals:
    void smoothingChanged(double smoothing);
    void splineStdChanged(double splineStd);

public slots:


private:
    double m_splineSmoothingFactor;
    double m_splineDeviation;
};

#endif // IDATAANALYSIS_H
