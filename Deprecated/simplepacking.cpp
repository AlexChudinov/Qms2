#include "simplepacking.h"

static PACKINGTYPE get_packing_type_(PACKDWORD data)
{
    if (data == 0x0000) return ZEROS;
    if (data <= 0x00FF) return BYTES;
    if (data <= 0xFFFF) return WORDS;
    return DWORDS;
}

static PACKINGTYPE check_packing_info_(PACKWORD packingInfo)
{
    return PACKINGTYPE(packingInfo & PACKEDINFOMASK);
}

static PACKWORD get_packed_elems_num_(PACKWORD packingInfo)
{
    return packingInfo & MAXPACKEDELEMSNUM;
}

//Push data pieces
static void push_byte_(PACKEDDATA& data, COMBINE byte)
{
    data.push_back(byte.byte[0]);
}

static void push_word_(PACKEDDATA& data, COMBINE word)
{
    data.push_back(word.byte[0]);
    data.push_back(word.byte[1]);
}

static void push_dword_(PACKEDDATA& data, COMBINE dword)
{
    data.push_back(dword.byte[0]);
    data.push_back(dword.byte[1]);
    data.push_back(dword.byte[2]);
    data.push_back(dword.byte[3]);
}

//Pop data pieces
static PACKDWORD pop_byte_(const PACKEDDATA& data, std::size_t n)
{
    COMBINE ret_val;
    ret_val.dword = 0;
    ret_val.byte[0] = data.at(n);
    return ret_val.dword;
}

static PACKDWORD pop_word_(const PACKEDDATA& data, std::size_t n)
{
    COMBINE ret_val;
    ret_val.dword = 0;
    ret_val.byte[0] = data.at(n);
    ret_val.byte[1] = data.at(n+1);
    return ret_val.dword;
}

static PACKDWORD pop_dword_(const PACKEDDATA& data, std::size_t n)
{
    COMBINE ret_val;
    ret_val.dword = 0;
    ret_val.byte[0] = data.at(n);
    ret_val.byte[1] = data.at(n+1);
    ret_val.byte[2] = data.at(n+2);
    ret_val.byte[3] = data.at(n+3);
    return ret_val.dword;
}

SimplePacking::SimplePacking(const RAWDATA &Data)
{
    RAWDATA::const_iterator itData = Data.begin();
    COMBINE dataElement;
    dataElement.dword = *itData;
    PACKINGTYPE packingType = get_packing_type_(*itData);
    PACKDWORD nPackElems = 0; //number of packed elements

    do
    {
        switch (packingType) {
        case BYTES:
            push_byte_(_data,dataElement);
            break;
        case WORDS:
            push_word_(_data,dataElement);
            break;
        case DWORDS:
            push_dword_(_data,dataElement);
            break;
        case ZEROS:
            ;//push nothing
        }
        if (++itData == Data.end()) break; //breaking condition

        if((++nPackElems) == MAXPACKEDELEMSNUM ||
                packingType != get_packing_type_(*itData))
        {
            _packingInfo.push_back(PACKWORD(packingType) | nPackElems);
            packingType = get_packing_type_(*itData);
            nPackElems = 0;
        }
    }
    while (true);
}

SimplePacking::operator RAWDATA() const
{
    RAWDATA ret_val;
    std::vector<PACKWORD>::const_iterator itPackInfo = _packingInfo.begin();
    std::size_t nBytesUnpack = 0; //number of unpacked Bytes

    for(; itPackInfo < _packingInfo.end(); ++itPackInfo)
    {
        switch(check_packing_info_(*itPackInfo))
        {
        case BYTES:
            for(int i = 0; i < get_packed_elems_num_(*itPackInfo); ++i)
            {
                ret_val.push_back(pop_byte_(_data,nBytesUnpack));
                nBytesUnpack += 1;
            }
            break;
        case WORDS:
            for(int i = 0; i < get_packed_elems_num_(*itPackInfo); ++i)
            {
                ret_val.push_back(pop_word_(_data,nBytesUnpack));
                nBytesUnpack += 2;
            }
            break;
        case DWORDS:
            for(int i = 0; i < get_packed_elems_num_(*itPackInfo); ++i)
            {
                ret_val.push_back(pop_dword_(_data,nBytesUnpack));
                nBytesUnpack += 4;
            }
            break;
        case ZEROS:
            ret_val.resize(ret_val.size()+get_packed_elems_num_(*itPackInfo),0);
        }
    }

    return ret_val;
}
