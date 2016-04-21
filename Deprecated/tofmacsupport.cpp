#include "tofmacsupport.h"
#include <massspectrumbase.h>
#include <msdatastruct.h>
#include <toffilesupport.h>
#include <QMessageBox>

TofMacSupport::TofMacSupport(QObject *parent)
    :
      MassSpectrumBase(parent),
      m_msDataStruct(nullptr)
{
}

TofMacSupport::~TofMacSupport()
{
    if(m_msDataStruct) delete m_msDataStruct;
}

QVector<double> TofMacSupport::MassScale() const
{
    if(!m_msDataStruct) return QVector<double>(); //empty if no data
    QVector<double> mz_Da =
            QVector<double>::fromStdVector(m_msDataStruct->GetMassScale());
    return mz_Da;
}

QVector<double> TofMacSupport::TimeScale() const
{
    if(!m_msDataStruct) return QVector<double>(); //empty if no data
    QVector<double> time_us =
            QVector<double>::fromStdVector(m_msDataStruct->GetTimeScale());
    return time_us;
}

QVector<double> TofMacSupport::Intensities() const
{
    if(!m_msDataStruct) return QVector<double>(); //empty if no data
    try{
        if (isShowTotal())
        {
            return
                    QVector<double>::fromStdVector
                    (
                        m_msDataStruct->GetAveragedMS(0,getMassSpecNum())
                    );
        }
        return
                QVector<double>::fromStdVector(m_msDataStruct->GetMassSpec(getMassSpecIdx()));
    }
    catch(...) //std::vector can throw a segmentation fault exception
    {
        QMessageBox::warning
                (
                    0,
                    QObject::tr("Предупреждение при вычислении интенсивностей"),
                    QObject::tr("Масс-спектра с таким номером не существует."),
                    QMessageBox::Ok, QMessageBox::Ok);

        return QVector<double>(); //return empty if the size exceeded
    }
}

QVector<double> TofMacSupport::Chromatogram() const
{
    if(!m_msDataStruct) return QVector<double>(); //empty if no data
    QVector<double> chrom =
            QVector<double>::fromStdVector(m_msDataStruct->GetTotIonCurrent());
    return chrom;
}

size_t TofMacSupport::getMassSpecNum() const
{
    return m_msDataStruct->Size();
}

void TofMacSupport::loadMassSpec(const QString &str)
{
    m_msDataStruct = new MSDataStructI(str,MSTOFFILE);

    if (m_msDataStruct->state() != MSOK)
    {
        m_isError = true;
        return;
    }

    setCalibration
            (
                m_msDataStruct->m_calibCoeffs.K,
                m_msDataStruct->m_calibCoeffs.t0
            );
    setMassSpecIdx(getMassSpecNum() - 1); //set the last one mass spec to show by default
    emit calibrationChanged();
}
