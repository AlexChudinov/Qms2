#ifndef TOFFILESUPPORT_H
#define TOFFILESUPPORT_H

#include "simplepacking.h"
#include <QFile>
#include <iterator>

//Class to load MSDataStruct from the .tof files
class TofFileSupport;

//Standart offsets for a special data tof file
#define MASSSPECPOINTSNUMOFFS   84
#define MASSSPECSNUMOFFS        278
#define MASSSPECDATAOFFS        512

class TofFileSupport
{
    //File reading staff
    QFile* m_fileName;
    QByteArray m_fileContain;

    //Dummy read calibration coeffs from the tof file
    double read_by_name_(const QString& name) const;
public:
    TofFileSupport()
        :
          m_fileName(new QFile()){;}
    ~TofFileSupport(){ delete m_fileName;}

    void open(const QString& tofFileName);

    std::vector<RAWDATA> ReadDataArrays(); //Read all data arrays from the tof file

    static constexpr double tdcTimeStep = 0.000625;  //us

    double GetCalibrationCoeffK();
    double GetCalibrationCoeffT0();
    double GetStartTime();
};


#endif // TOFFILESUPPORT_H
