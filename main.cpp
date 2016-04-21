#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/qms.png"));
    MainWindow w;
    w.show();

    //Open associated file:
    QStringList fileNames = QApplication::arguments();
    if(fileNames.size() >= 2)
    {
        QStringList fileComposition = fileNames[1].split(".");
        QString extension = fileComposition.at(fileComposition.size()-1);
        if(IMassSpectrum::m_fileFilters[0].contains(extension))
            w.loadFileAction(fileNames[1],IMassSpectrum::m_fileFilters[0]);
        if(IMassSpectrum::m_fileFilters[1].contains(extension))
            w.loadFileAction(fileNames[1],IMassSpectrum::m_fileFilters[0]);
    }


    return a.exec();
}
