#include "toffilesupport.h"
#include <cmath>
#include <QMessageBox>

void TofFileSupport::open(const QString &tofFileName)
//open in the binary mode
{
    m_fileName->setFileName(tofFileName);

    if(!m_fileName->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0,
                             "Открытие .tof файла",
                             "Ошибка при открытии .tof файла");
    }

    m_fileContain = m_fileName->readAll();

    if(m_fileContain.isEmpty())
    {
        QMessageBox::warning(0,
                          "Чтение .tof файла",
                          "Ошибка при чтении .tof файла");
    }

    m_fileName->close();
}

std::vector<RAWDATA> TofFileSupport::ReadDataArrays()
{
    PACKDWORD nMS = *reinterpret_cast<PACKDWORD*>
            (m_fileContain.data() + MASSSPECSNUMOFFS);
    std::vector<RAWDATA> ret(nMS); //preallocate some little peace of memory

    //Set pointer to read mass spectrums data
    char* massSpecData = m_fileContain.data() + MASSSPECDATAOFFS;
    for (auto& i:ret)
    {
        char* infoStart = massSpecData+sizeof(PACKDWORD);
        char* infoEnd   = infoStart+sizeof(PACKWORD) *
                *reinterpret_cast<PACKDWORD*>(massSpecData);
        char* dataStart = infoEnd + sizeof(PACKDWORD);
        char* dataEnd   = dataStart +
                *reinterpret_cast<PACKDWORD*>(infoEnd);

        i = RAWDATA(SimplePacking(std::vector<PACKWORD>
                                  (
                                      reinterpret_cast<PACKWORD*>(infoStart),
                                      reinterpret_cast<PACKWORD*>(infoEnd)),
                                  std::vector<PACKBYTE>
                                  (
                                      reinterpret_cast<PACKBYTE*>(dataStart),
                                      reinterpret_cast<PACKBYTE*>(dataEnd))
                                  )
                    );
        massSpecData = dataEnd;
    }

    return ret;
}

//Scan tof file for a string
double TofFileSupport::read_by_name_(const QString& name) const
{
    int startIdx = m_fileContain.indexOf(name) + name.size();
    int endIdx = m_fileContain.indexOf("\r\n",startIdx);
    const char* value = m_fileContain.data() + startIdx;
    return QByteArray(value,endIdx-startIdx).toDouble();
}

double TofFileSupport::GetCalibrationCoeffK()
{
    return 1./::sqrt(read_by_name_("BaseCalibK :"));
}

double TofFileSupport::GetCalibrationCoeffT0()
{
    return -read_by_name_("BaseCalibT :");
}

double TofFileSupport::GetStartTime()
{
    return read_by_name_("StartTime :");
}
