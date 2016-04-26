/*!
  \author Alexey Chudinoff
  \date 26.04.2016
  \file idataanalisis.h
  \brief Data analysis interface
*/
#ifndef IDATAANALYSIS_H
#define IDATAANALYSIS_H

#include <QStandardItemModel>
#include <QTreeView>

///Data analysis interface
class IDataAnalysis : public QStandardItemModel
{
    Q_OBJECT
public:
    IDataAnalysis(QObject* parent = 0);
    ~IDataAnalysis();

    ///Spline smoothing parameter
    /// \returns The value of spline smoothing parameter
    double smoothing() const;
    ///Standard deviation of data from the spline
    /// \returns The value of standard deviation
    double splineStd() const;

signals:
    ///
    void smoothingChanged(double smoothing);
    ///
    void splineStdChanged(double splineStd);

public slots:
    ///Set standard deviation value
    /// This interface for n inner code because this deviation is always
    /// calculated
    /// \param slineStd Spline standard deviation
    void splineStd(double splineStd);
    ///Set spline smoothing parameter
    /// \param smoothing Spline smoothing parameter
    void smoothing(double smoothing);
    ///Get spline smoothing parameter from the controlling item
    /// \param item The item which controls the spline smoothing value
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
