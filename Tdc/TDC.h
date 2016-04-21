#ifndef _TDC_H_
#define _TDC_H_
#include <windows.h>

//TDC adresses
//TDC board
#define TDC_REG0    0x10
#define TDC_REG1    0x11
#define TDC_REG2    0x12
#define TDC_REG3    0x13
#define TDC_REG4    0x14
#define TDC_REG5    0x15
#define TDC_REG6    0x16
#define TDC_REG7    0x17
#define TDC_REG8    0x18
#define TDC_REG9    0x19
#define TDC_REG10   0x1A
#define TDC_REG11   0x1B
#define TDC_REG12   0x1C
#define TDC_REG13   0x1D
#define TDC_REG14   0x1E
#define TDC_REG15   0x1F
//TDC shifter board
#define SHIFTER_REG0    0x20
#define SHIFTER_REG1    0x21
#define SHIFTER_REG2    0x22
#define SHIFTER_REG3    0x23
#define SHIFTER_REG4    0x24
#define SHIFTER_REG5    0x25
#define SHIFTER_REG6    0x26
#define SHIFTER_REG7    0x27
#define SHIFTER_REG8    0x28
#define SHIFTER_REG9    0x29
#define SHIFTER_REG10   0x2A
#define SHIFTER_REG11   0x2B
#define SHIFTER_REG12   0x2C
#define SHIFTER_REG13   0x2D
#define SHIFTER_REG14   0x2E
#define SHIFTER_REG15   0x2F
//TDC histogrammer board
#define HIST_REG0       0x30
#define HIST_REG1       0x31
#define HIST_REG2       0x32
#define HIST_REG3       0x33
#define HIST_REG4       0x34
#define HIST_REG5       0x35
#define HIST_REG6       0x36
#define HIST_REG7       0x37
#define HIST_REG8       0x38
#define HIST_REG9       0x39
#define HIST_REG10      0x3A
#define HIST_REG11      0x3B
#define HIST_REG12      0x3C
#define HIST_REG13      0x3D
#define HIST_REG14      0x3E
#define HIST_REG15      0x3F
//Host address
#define HOST            0x7F

//Read/write bit
#define READMODE        0x00
#define WRITEMODE       0x80

//Set bit on the FishCamp board to read/write
#define READBYTE        0x0FE0
#define WRITEBYTE       0X0FF0

class Tdc
{
    //System depended constant
    const static char m_driverFileName[];

    //Only singleton TDC instance allowed (???)
    Tdc(const Tdc&);
    Tdc& operator=(const Tdc&);

    enum TDC_ERROR_STATES
    {
        TDC_NO_ERROR = 0,
        TDC_FCDRIVER_NOT_FOUND = 1,
        TDC_FCDRIVER_UM_PTR,
        TDC_FAILED_CONNECTION
    };

    //Active time is a time when the TDC is active after getting a start pulse
    //This time is stored in two byte-registers inside TDC
    union _tagTDCActiveTime
    {
        WORD activeTime;
        struct
        {
            BYTE lsByte;
            BYTE msByte;
        }
        activeTimeBytes;
    }
    m_activeTime;

    bool m_tdcError;    //TDC error bit

    TDC_ERROR_STATES m_tdcErrorState;

    HANDLE m_hFishCamp; //handle to fish camp card driver

    char* m_pBaseMem;  //user mode fish camp memory space pointer

    //Addresses of FishCamp registers
    char  *m_pWin, *m_pData, *m_pAddr, *m_pCfg;

    //Helper functions
    void  initTdc(float activeTime);            //initialization of the TDC (TEG WR and etc.?)
    void  initFishCamp(void);                   //initialization of Fish Camp
    void  writeTdc8(DWORD addr,BYTE data) const;//write data to TDC at addr
    BYTE  readTdc8(DWORD addr) const;           //read data from the TDC register
    DWORD readTdc32(DWORD addr) const;          //read wide data from TDC register
    void  setTdcError(TDC_ERROR_STATES error);  //set TDC error state
    void  setActiveTime(float activeTime);      //set TDC active time (the time of TDC activity since start pulse)
    DWORD getTdcFifoStatus() const;             //Returns status dword of the TDC FIFO memory
public:
    Tdc(float activeTime = 150.0); //150 us by default
    ~Tdc();

    operator bool() const {return !m_tdcError;} //check for errors

    DWORD readEvent() const; //Read one event from the TDC's FIFO memory

    void run() const;   //set TDC to run in asynchronous mode
    void stop() const;  //stop TDC acquisition
    void reset();   //reset TDC

    bool isOverflow() const; //true if FIFO buffer overflowed
    bool isDataValid(DWORD data) const; //true if data is a valid TDC data

    const char* getErrorState() const;

    //TDC dependent variable
    const static double m_tdcTimeStepUs;
};

#endif // _TDC_H_
