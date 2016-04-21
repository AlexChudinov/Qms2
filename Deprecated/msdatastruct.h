#ifndef MSDATASTRUCT_H
#define MSDATASTRUCT_H

#include <msdatastruct_global.h>

/*Float mass spectrum representation*/
/*Forward class declaration*/
class MSDataStructD;
class MSDataStructI;

enum MSFILETYPES //File type numeration
{
    MSTOFFILE = 0x01
};

enum MSDATASTRUCTSTATE //Information about the errors that occur during execution
{
    MSOK = 0x01, //No errors
    MSERRORFILEREAD, //File read error
    MSUNSUPPORTEDFILETYPE, //Try to load unsupported file type
    MSINCONSISTENT //different sizes of mass spectrums, should have same size
};

/*Base class for MSDataStruct's*/
//Cause some base functionality is needed
template<typename DATA> class MSDataStruct
{
public:
    mutable MSDATASTRUCTSTATE _state;

    QString _fileName;

    std::vector<std::vector<DATA>> _msArrays;

    MSDataStruct()
        :
          _state(MSOK)
    {;}

    //get mass spectrum averaged over chosen range
    std::vector<double> get_averaged_mass_spectrum_(size_t from, size_t to) const;

    //get total ion current values for different mass spectrums
    std::vector<double> get_total_ion_current_() const;

    //check that all mass spectrums have the same number of points
    bool check_mass_spectrum_sizes_() const;

    //show string information about the object state
    const char* get_state_string_() const;
};


/*Float mass spectrum representation*/
class MSDataStructD : private MSDataStruct<double>
{

public:
    MSDataStructD(const MSDataStructI& msI); //Conversion from uint32_t to double
};

/*Integer mass spectrum representation*/
class MSDataStructI : private MSDataStruct<uint32_t>
{    
    //Only integer mass spectrums cam be loaded from the tof file
    void load_tof_file_();

public:

    struct tagCalibCoefs
    {
        double startTime;
        double K;
        double t0;
    } m_calibCoeffs;

    MSDataStructI(const QString &fileName, MSFILETYPES fileType);

    std::vector<double> GetAveragedMS(size_t from, size_t to) const;

    std::vector<double> GetTotIonCurrent() const;

    std::vector<double> GetTimeScale() const;

    std::vector<double> GetMassScale() const;

    std::vector<double> GetMassSpec(int idx) const;

    size_t Size() const; //return number of mass spectrums

    MSDATASTRUCTSTATE state() const;

    friend class MSDataStructD;

    friend class MSDataStructModel; //allow MSDataStructI to be driven using MSDataStructModel
};



/*MSDataStruct members:*/
template <class DATA>
bool MSDataStruct<DATA>::check_mass_spectrum_sizes_() const
{
    typename std::vector< std::vector<DATA>>::const_iterator it = _msArrays.begin();
    size_t checkSize = it->size();
    for(it = it + 1; it != _msArrays.end(); ++it)
        if (checkSize != it->size()) return false;

    return true;
}

template <class DATA>
std::vector<double> MSDataStruct<DATA>::get_averaged_mass_spectrum_(size_t from, size_t to) const
{
    //Check arguments
    //1
    if(_state != MSOK)
        return std::vector<double>(); //return empty vector
    //2
    if(!check_mass_spectrum_sizes_())
    {
        _state = MSINCONSISTENT;
        return std::vector<double>(); //return empty vector
    }
    //3
    if (from > to || from >= _msArrays.size()) return std::vector<double>(); //return empty vector

    //Estimate averaging limits:
    size_t start = from;
    size_t end   = to >= _msArrays.size() ? _msArrays.size() : to;

    std::vector<double> MS(_msArrays[0].size()); //preallocate memory for summation result

    //Set first and last iterators
    typename std::vector<std::vector<DATA>>::const_iterator
            first = _msArrays.begin() + start;
    typename std::vector<std::vector<DATA>>::const_iterator
            last  = _msArrays.begin() + end;

    //Estimate array summ
    for(auto& pArray : std::vector<std::vector<DATA>>(first,last))
    {
        std::vector<double>::iterator pMS                = MS.begin();
        typename std::vector<DATA>::const_iterator pData = pArray.begin();

        for(; pData != pArray.end(); ++pData, ++pMS)
            *pMS += *pData;
    }

    return MS;
}

template<class DATA>
std::vector<double> MSDataStruct<DATA>::get_total_ion_current_() const
{
    std::vector<double> TIC(_msArrays.size());

    size_t N = 0;
    for(auto& tic:TIC)
    {
        for(auto stic:_msArrays[N])
        {
            tic += stic;
        }
        ++N;
    }

    return TIC;
}

//Show string information about MSDataStructState
template<class DATA>
const char* MSDataStruct<DATA>::get_state_string_() const
{
    switch(_state)
    {
    case MSOK:
        return "No errors.";
    case MSERRORFILEREAD:
        return "Could not open file with mass spectrums";
    case MSUNSUPPORTEDFILETYPE:
        return "The file type is not supported";
    case MSINCONSISTENT:
        return "Mass spectrums mismatched in the numbers of data points.";
    default:
        return "It is very strange that program flow comes here. Probably, it needs debugging.\n";
    }
}

#endif // MSDATASTRUCT_H
