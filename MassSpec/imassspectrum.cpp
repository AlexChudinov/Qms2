#include <imassspectrum.h>
#include <massspectrumbase.h>
#include <tofmacsupport.h>
#include <tdcdatastorage.h>

#include <QMessageBox>
#include <QApplication>

const QVector<QString> IMassSpectrum::m_fileFilters =
{
    "Old tof files: (*.tof)",
    "Поток событий TDC: (*.tdctxt)"
};

IMassSpectrum::IMassSpectrum(QObject *parent)
    :
      QObject(parent),
      m_msDataStruct(nullptr),
      m_isDeleteProhibited(false),
      m_type(NODATA)
{

}

IMassSpectrum::~IMassSpectrum()
{
    removeData();
}

bool IMassSpectrum::dataDeleteProhibited() const
{
    return m_isDeleteProhibited;
}

bool IMassSpectrum::isNeedToSave() const
{
    if(m_msDataStruct) return m_msDataStruct->isNeedToSave();
    else return false;
}

void IMassSpectrum::removeData()
{
    if(dataDeleteProhibited())
    {
        m_msDataStruct  = nullptr;
        m_isDeleteProhibited = false;
    }
    else
    {
        delete m_msDataStruct;
        m_msDataStruct = nullptr;
    }
}

void IMassSpectrum::loadMassSpec(QString fileName, QString fileFilter)
{
    MassSpecType type = stringToType(fileFilter);
    switch(type)
    {
    case TOFFILE:
    {
        removeData();
        m_msDataStruct = new TofMacSupport(this);

        emit state("Загрузка файла...");

        QApplication::setOverrideCursor(Qt::WaitCursor);

        reinterpret_cast<TofMacSupport*>(m_msDataStruct)->loadMassSpec(fileName);

        QApplication::restoreOverrideCursor();
        if(m_msDataStruct->isError())
        {
            QMessageBox::information
                    (
                        qobject_cast<QWidget*>(this),
                        "Oшибка",
                        "Ошибка при загрузке файла!"
                    );
            emit state(QString());
            return;
        }
        m_type = TOFFILE;
        emit state(fileName);

        emit showMassSpec(qobject_cast<QObject*>(m_msDataStruct));
        return;
    }
    default:
        QMessageBox::information(qobject_cast<QWidget*>(this),"Загрузка нового файла","Неизвестный тип файла");
    }
}

void IMassSpectrum::saveData(QString fileName, QString fileFilter)
{
    MassSpecType type = stringToType(fileFilter);

    if (m_type == NODATA)
    {
        QMessageBox::information(qobject_cast<QWidget*>(this),
                             "Сохранение файла",
                             "Пустые данные: файл не был сохранён");
        return;
    }

    switch(type)
    {
    case TOFFILE:
        QMessageBox::information(qobject_cast<QWidget*>(this),
                                 "Сохранение файла",
                                 "Нет операции сохранения для данного типа данных");
        return;
    case TDCSTREAM:
        if (m_type == TOFFILE)
            QMessageBox::information(qobject_cast<QWidget*>(this),
                "Сохранение файла",
                "Конвертация .tof-файла в поток событий невозможна.");

        m_msDataStruct->needToSave(false);

        if (m_type == TDCSTREAM)
        {
            emit state("Сохранение файла...");
            if (
                    qobject_cast<TdcDataStorage*>(m_msDataStruct)
                    ->saveFileAscii(fileName) != 0
                )
                QMessageBox::information(qobject_cast<QWidget*>(this),
                    "Сохранение файла",
                    "Ошибка при сохранении файла.");
            emit state(fileName);

        }

        return;
    case NODATA:
        return;
    }
}

void IMassSpectrum::setTdcConnection(QObject *tdcDataStream)
{
    removeData();
    m_msDataStruct = reinterpret_cast<TdcDataStorage*>(tdcDataStream);
    m_type = TDCSTREAM;
    m_isDeleteProhibited = true;        

    emit state("Накопление событий...");
    emit showMassSpec(qobject_cast<QObject*>(m_msDataStruct));
}

IMassSpectrum::MassSpecType IMassSpectrum::stringToType(QString strType)
{
    if(strType == IMassSpectrum::m_fileFilters[0]) return TOFFILE;
    if(strType == IMassSpectrum::m_fileFilters[1]) return TDCSTREAM;
    return NODATA;
}

QString IMassSpectrum::typeToString(MassSpecType intType)
{
    switch(intType)
    {
    case TOFFILE:
        return IMassSpectrum::m_fileFilters[0];
    case TDCSTREAM:
        return IMassSpectrum::m_fileFilters[1];
    case NODATA:
        return QString();
    }
    return QString();
}
