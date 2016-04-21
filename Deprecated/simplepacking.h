#ifndef SIMPLEPACKING_H
#define SIMPLEPACKING_H

#include <msdatastruct_global.h>

class SimplePacking;

/*Pack the vector of DWORD data into BYTES and WORDS depending on data value*/

#define PACKEDINFOMASK      0xC000 //mask to extract packed info from the word
#define MAXPACKEDELEMSNUM   0x3FFF //maximum elements of one type that can be packed together

enum PACKINGTYPE{
    ZEROS =  0x0000,    //array should be filled with zeros
    BYTES =  0x4000,    //array packed in bytes
    WORDS =  0x8000,    //array packed in words
    DWORDS = 0xC000,    //array packed in double words, actually unpacked
};

typedef uint32_t    PACKDWORD;
typedef uint16_t    PACKWORD;
typedef uint8_t     PACKBYTE;
typedef std::vector<PACKBYTE>  PACKEDDATA;
typedef std::vector<PACKDWORD> RAWDATA;

//Endian dependent code. Little endians is assumed.
typedef union Combine
{
    PACKBYTE byte[4];
    PACKWORD word[2];
    PACKDWORD dword;
} COMBINE;

class SimplePacking
{
    std::vector<PACKWORD> _packingInfo; //elements contains info about the packing of the data
    PACKEDDATA _data;

public:
    SimplePacking(const RAWDATA& Data); //data pack

    SimplePacking //use defined packing information
    (
            const std::vector<PACKWORD>& packingInfo,
            const PACKEDDATA& data
    )
        :
        _packingInfo(packingInfo),
        _data(data)
    {;}

    SimplePacking //create using temporary objects
    (
            std::vector<PACKWORD>&& packingInfo,
            PACKEDDATA&& data
     )
        :
          _packingInfo(packingInfo),
          _data(data)
    {;}

    //Cast to an initial data array
    operator RAWDATA() const; //data unpack
};

#endif // SIMPLEPACKING_H
