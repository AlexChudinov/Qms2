#ifndef IDATAANALYSIS_H
#define IDATAANALYSIS_H

#include <QStandardItemModel>
#include <QTreeView>

class IDataAnalysis : public QStandardItemModel
{
    Q_OBJECT
public:
    IDataAnalysis(QObject* parent = 0);
    ~IDataAnalysis();

    double smoothing() const;
    double splineStd() const;

signals:
    void smoothingChanged(double smoothing);
    void splineStdChanged(double splineStd);

public slots:
    void splineStd(double splineStd);
    void smoothing(double smoothing);
    void smoothing(QStandardItem* item);

private:
    double m_splineSmoothingFactor;
    double m_splineDeviation;

    QStandardItem* m_smoothingVal;
    QStandardItem* m_stdVal;
};

class IDataAnalysisView : public QTreeView
{
    Q_OBJECT
public:
    IDataAnalysisView(QWidget* parent);
    ~IDataAnalysisView();

signals:
    void smoothing(double smoothingPar);
    void splineStd(double std_);

public slots:

private:
    IDataAnalysis* m_model;
};

#endif // IDATAANALYSIS_H
