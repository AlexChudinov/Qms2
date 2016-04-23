#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <tdcview.h>
#include <massspecview.h>
#include <imassspectrum.h>
#include <idataanalysis.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
public slots:
    void loadFileAction(QString fileName = QString());
    void loadFileAction(QString fileName,QString fileFilter);
    void saveFileAction();

private:
    IMassSpectrum* massSpecInter;
    MassSpecView* massSpec;
    Ui::MainWindow *ui;
    TdcView* tdcView;
    QToolBar* toolBarView;

    IDataAnalysisView* m_dataAnalysisContent;

    QString m_fileFilters;
};

#endif // MAINWINDOW_H
