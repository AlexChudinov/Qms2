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
    recalculateMassSpec();
    emit calibrationChanged();
}

void TofMacSupport::recalculateMassSpec()
{
    if(!m_msDataStruct) return;
    try{
        if (isShowTotal())
        {
            m_I =
                    QVector<double>::fromStdVector
                    (
                        m_msDataStruct->GetAveragedMS(0,getMassSpecNum())
                    );
        }
        else
        {
            m_I =
                QVector<double>::fromStdVector(m_msDataStruct->GetMassSpec(getMassSpecIdx()));
        }
    }
    catch(...) //std::vector can throw a segmentation fault exception
    {
        QMessageBox::warning
                (
                    0,
                    QObject::tr("Предупреждение при вычислении интенсивностей"),
                    QObject::tr("Масс-спектра с таким номером не существует."),
                    QMessageBox::Ok, QMessageBox::Ok);

        return; //return empty if the size exceeded
    }

    m_mz = QVector<double>::fromStdVector(m_msDataStruct->GetMassScale());
    m_t  = QVector<double>::fromStdVector(m_msDataStruct->GetTimeScale());
    m_tic= QVector<double>::fromStdVector(m_msDataStruct->GetTotIonCurrent());
}
