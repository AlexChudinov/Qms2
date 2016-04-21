/*!
  \author Alexey Chudinov
  \date 16.04.2016
  \file ilogspline.h
  \brief Interface of logsplines into the Qt system
*/
#ifndef ILOGSPLINE_H
#define ILOGSPLINE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTreeView>

class LogSplines;


///\brief interface to a \class LogSpline
class ILogSpline : public QObject
{
    Q_OBJECT

public:
    ILogSpline(QObject* parent = 0);
    ~ILogSpline();


public slots:
    ///Shows spline on a some plotting staff
    /// \param plot Is a plot interface
    /// \param lambda Smoothing factor
    void showSpline(QObject* plot, double lambda);

    ///Releases spline data of the instance
    void releaseSpline();
signals:
    ///Sends the signal with the calculated spline pointer
    /// \param spline Pointer to a spline data
    void splineCreated(void* spline);

    ///Sends the signal about spline deletion
    void splineDeleted();

private:

    LogSplines* m_logSpline;
};

class LogSplineModel : public QStandardItemModel
{
    Q_OBJECT
public:
    LogSplineModel(QObject* parent = 0);
    ~LogSplineModel();

    QObject* getPlot() const;
    void setPlot(QObject* plot);

    void buildSmoothedLine();
public slots:
    void onItemChanged(QStandardItem* item);
    void onDeleteSpline();

signals:
    void smoothingChanged(QObject* plot,double lambda);
    void deleteSpline();

private:
    QObject* m_plot;

    ILogSpline* m_spline;
    QMap<QString,QStandardItem*> m_modelItems;
};

class LogSplinesView : public QTreeView
{
    Q_OBJECT

public:
    LogSplinesView(QWidget* parent = 0);
    ~LogSplinesView();

public slots:
    void setPlot(QObject* plot);

signals:
    void plotSmoothedLine(QObject* plot, double lambda);
    void deleteSpline();

private:
    LogSplineModel* m_splinesModel;
};
#endif // ILOGSPLINE_H
