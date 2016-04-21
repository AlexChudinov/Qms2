#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    massSpecInter(new IMassSpectrum(this)),
    massSpec(new MassSpecView(this)),
    ui(new Ui::MainWindow),
    tdcView(new TdcView(this)),
    toolBarView(addToolBar("Управление видом приложения.")),
    logSplineModel(new LogSplinesView(this)),
    m_fileFilters()
{
    setAcceptDrops(true);
    ui->setupUi(this);

    //connect file actions
    connect(ui->actionFileOpen,SIGNAL(triggered()),this,SLOT(loadFileAction()));
    connect(ui->actionFileSave,SIGNAL(triggered()),this,SLOT(saveFileAction()));

    //Set up TDC property window
    ui->tdcPropContent->setLayout(ui->tdcPropLayout);
    ui->tdcPropLayout->addWidget(tdcView);
    ui->tdcPropWnd->setWindowTitle("Свойства TDC");
    QAction* toggleActionTdc = ui->tdcPropWnd->toggleViewAction();
    toggleActionTdc->setIcon(QIcon(":/tdc.png"));
    toggleActionTdc->setToolTip("Показать / Скрыть окно параметров TDC");
    toolBarView->addAction(toggleActionTdc);
    ui->menuView->addAction(toggleActionTdc);

    ui->centralWidget->setLayout(ui->mainLayout);
    ui->mainLayout->addWidget(massSpec);

    connect(massSpecInter,SIGNAL(showMassSpec(QObject*,IMassSpectrum::MassSpecType)),
            massSpec,SIGNAL(showMassSpec(QObject*,IMassSpectrum::MassSpecType)));
    connect(tdcView,SIGNAL(showMassSpec(QObject*)),massSpecInter,SLOT(setTdcConnection(QObject*)));
    connect(massSpecInter,SIGNAL(state(QString)),ui->statusBar,SLOT(showMessage(QString)));

    std::for_each(IMassSpectrum::m_fileFilters.begin(),
                  IMassSpectrum::m_fileFilters.end(),
                  [this](QString filter)->void
                    {
                        m_fileFilters = m_fileFilters.isEmpty()?
                                    filter
                                  : m_fileFilters + ";;" + filter;
                    }
                  );

    //Data analysis window
    ui->massSpecMathWnd->setWindowTitle("Анализ данных");
    ui->massSpecMathContents->setLayout(ui->massSpecMathLayout);
    ui->massSpecMathLayout->addWidget(logSplineModel);
    connect(massSpec,SIGNAL(showSpline(QObject*)),logSplineModel,SLOT(setPlot(QObject*)));
    connect(massSpec,SIGNAL(deleteSpline()),logSplineModel,SIGNAL(deleteSpline()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFileAction(QString fileName)
{
    QString fileFilter;
    if(fileName.isNull())
    {
        fileName = QFileDialog::getOpenFileName
            (
                this,
                "Открыть файл",
                QString(),
                m_fileFilters,
                &fileFilter
            );
    }
    if(fileName.isNull()) return;

    massSpecInter->loadMassSpec(fileName,fileFilter);
}

void MainWindow::loadFileAction(QString fileName, QString fileFilter)
{
    massSpecInter->loadMassSpec(fileName,fileFilter);
}

void MainWindow::saveFileAction()
{
    QString fileFilter, fileName;

    fileName = QFileDialog::getSaveFileName
        (
            this,
            "Открыть файл",
            QString(),
            m_fileFilters,
            &fileFilter
        );
    if(fileName.isNull()) return;

    massSpecInter->saveData(fileName,fileFilter);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(massSpecInter->isNeedToSave())
    {
        int checkSave = QMessageBox::warning
                (
                    this,
                    "Закрытие окна",
                    "Вы хотите сохранить результат работы TDC?",
                    "Да",
                    "Нет"
                );
        if(checkSave == 0)
        {
            event->ignore();
            saveFileAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* fileData = event->mimeData();
    if(fileData->hasUrls())
    {
        QUrl fileUrl = fileData->urls().at(0);
        QString fileName = fileUrl.toLocalFile();
        QStringList fileComposition = fileName.split(".");
        QString extension = fileName.at(fileComposition.size()-1);
        if(IMassSpectrum::m_fileFilters[0].contains(extension))
            loadFileAction(fileName,IMassSpectrum::m_fileFilters[0]);
        if(IMassSpectrum::m_fileFilters[1].contains(extension))
            loadFileAction(fileName,IMassSpectrum::m_fileFilters[0]);
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}
