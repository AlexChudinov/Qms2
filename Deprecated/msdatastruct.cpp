#include "msdatastruct.h"
#include "toffilesupport.h"

/*MSDataStructI members below*/
MSDataStructI::MSDataStructI(const QString &fileName, MSFILETYPES fileType)
    :
      MSDataStruct<uint32_t>()
{
    _fileName = fileName;
    switch(fileType)
    {
    case MSTOFFILE:
        load_tof_file_();
        return;
    default:
        _state = MSUNSUPPORTEDFILETYPE;
        return;
    }
}

void MSDataStructI::load_tof_file_()
{
    TofFileSupport in;
    in.open(_fileName);

    _msArrays = in.ReadDataArrays();

    m_calibCoeffs.startTime = in.GetStartTime();
    m_calibCoeffs.K         = in.GetCalibrationCoeffK();
    m_calibCoeffs.t0        = in.GetCalibrationCoeffT0();
}

std::vector<double> MSDataStructI::GetAveragedMS(size_t from, size_t to) const
{
    return get_averaged_mass_spectrum_(from,to);
}

std::vector<double> MSDataStructI::GetTotIonCurrent() const
{
    return get_total_ion_current_();
}

std::vector<double> MSDataStructI::GetTimeScale() const
{
    std::vector<double> time_us(_msArrays[0].size());
    double t = 0.5;
    for(auto& time:time_us)
        time = (t++)*TofFileSupport::tdcTimeStep + m_calibCoeffs.startTime;

    return time_us;
}

std::vector<double> MSDataStructI::GetMassScale() const
{
    std::vector<double> time_us = GetTimeScale();
    std::vector<double> mz_Da(time_us.size());

    for(size_t i = 0; i < time_us.size(); ++i)
        mz_Da[i] =
                (time_us[i] - m_calibCoeffs.t0)/m_calibCoeffs.K *
                (time_us[i] - m_calibCoeffs.t0)/m_calibCoeffs.K;

    return mz_Da;
}

std::vector<double> MSDataStructI::GetMassSpec(int idx) const
{
    const std::vector<unsigned int>& msArray = _msArrays.at(static_cast<unsigned int>(idx));
    std::vector<double> Intensities(msArray.size());

    for(size_t i = 0; i < Intensities.size(); ++i)
        Intensities[i] = static_cast<double>(msArray[i]);

    return Intensities;
}

MSDATASTRUCTSTATE MSDataStructI::state() const
{
    return _state;
}

size_t MSDataStructI::Size() const
{
    return _msArrays.size();
}

MSDataStructD::MSDataStructD(const MSDataStructI& msI)
    :
      MSDataStruct<double>()
{
    if (msI._state != MSOK) //input array in the error state
    {
        _state = msI._state;
        return;
    }
    std::vector<std::vector<uint32_t>>::const_iterator it = msI._msArrays.begin();

    for(auto i:msI._msArrays)
    {
        _msArrays.push_back(std::vector<double>(i.begin(),i.end()));
    }
}
