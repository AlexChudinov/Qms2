#ifndef TDCVIEW_H
#define TDCVIEW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMap>
#include <QPair>

class QTableView;
class TdcDataStorage;
class TdcReadThread;

/*! \class TdcParams
 * TdcParams hold the TDC parameters string and TDC default parameters
 * that will be displayed in TDC model view. All members are const.
 */
class TdcParams
{
public:
    const static int m_itemsNum = 4;

    static const char* m_paramsStr[m_itemsNum];

    static const QVariant m_paramsVal[m_itemsNum];
};

/*! \class TdcModel
 * It is intended to drive events flow from the TDC
 *
 */
class TdcModel : public QStandardItemModel
{
    Q_OBJECT
public:
    TdcModel(QObject *parent = 0);
    ~TdcModel();

signals:
    void activeTimeChanged(float activeTime);
    void accumNumberChanged(int accumNumber);
    void showMassSpec(QObject* massSpec);
    void tdcStopped(bool isStopped = false);

public slots:
    void setRun();
    void setStop();
    void setErrorState(const char* errorStateStr);
    void emitModelChanged(QStandardItem* item);
    void setStopIcon();
    void setOverflowsNum(int overflowsNum);

private:
    TdcReadThread* m_tdc;
    TdcDataStorage* m_tdcDataStream;
    QMap<QString,QPair<QStandardItem*,QStandardItem*> > m_tdcModelItems;
};

class TdcView : public QMainWindow
{
    Q_OBJECT
public:
    TdcView(QWidget *parent = 0);
    ~TdcView();

signals:
    showMassSpec(QObject* massSpec);

public slots:

private:
    QAction*  m_actionRun;
    ///QAction*  m_actionPause;
    ///TO DO: the pause supposedly required
    /// but this conception needs to be investigated further
    QAction*  m_actionStop;

    QToolBar* m_toolBar;

    QTableView* m_tdcModelView;

    //The heart of the class
    QAbstractItemModel *m_tdcModel;
};

#endif // TDCVIEW_H
