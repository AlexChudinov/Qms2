#include <TDC.h>
#include <FCDRINTR.H>
#include <winioctl.h>
#include <cmath>

const char Tdc::m_driverFileName[]  = "\\\\.\\FPCI-DIO0";
const double Tdc::m_tdcTimeStepUs = 0.000625;

///Private TDC functions which implement the main class functionality

//Read TDC FIFO status
DWORD Tdc::getTdcFifoStatus() const
{
    return readTdc32(TDC_REG14);
}

void Tdc::setActiveTime(float activeTime)
{
    m_activeTime.activeTime = static_cast<WORD>(::round((activeTime - 2.52)/2.56));
    writeTdc8(TDC_REG3,m_activeTime.activeTimeBytes.lsByte);
    writeTdc8(TDC_REG4,m_activeTime.activeTimeBytes.msByte);
}

void Tdc::setTdcError(TDC_ERROR_STATES error)
{
    m_tdcError = true;
    m_tdcErrorState = error;
}

//Write data to TDC
void Tdc::writeTdc8(DWORD addr, BYTE data) const
{
    __asm__ __volatile__
    (
        "\t"
            "movl %2,       %%eax\n\t"  //Configuration register of FishCamp into EAX
            "movl $0x0FF0, (%%eax)\n\t" //Configure to write one byte
            "movl %3,       %%eax\n\t"  //Pointer to the address FishCamp register into EAX
            "movl %1,       %%edx\n\t"  //1)Put TDC address into FishCamp register
            "orl  $0x80,    %%edx\n\t"  //Switch the write mode on
            "movl %%edx,   (%%eax)\n\t" //2)Put TDC address into FishCamp register
            "movl %4,       %%edx\n\t"  //Load data register pointer
            "movb %0,      (%%edx)\n\t" //Write data to TDC
            "movl $0x7F,   (%%eax)\n\t" //Move host board address into the address FishCam
        :
        :
            "b"(data),"m"(addr),"m"(m_pCfg),"m"(m_pAddr),"m"(m_pData)
        :"%eax","%edx"
    );
}

//Read data from the TDC register
BYTE Tdc::readTdc8(DWORD addr) const
{
    BYTE data;

    __asm__ __volatile__
    (
        "\t"
            "movl %2,       %%eax\n\t"  //Configuration register of FishCamp into EAX
            "movl $0x0FE0, (%%eax)\n\t" //Configure to read one byte
            "movl %3,       %%eax\n\t"  //Pointer to the address FishCamp register into EAX
            "movl %1,       %%edx\n\t"  //Put TDC address into FishCamp register
            "movl %%edx,   (%%eax)\n\t" //...
            "movl %4,       %%edx\n\t"  //Load data register pointer
            "movb (%%edx),      %0\n\t" //Read TDC data
            "movl $0x7F,   (%%eax)\n\t" //Move host board address into the address FishCamp
        :
            "=b"(data)
        :
            "m"(addr),"m"(m_pCfg),"m"(m_pAddr),"m"(m_pData)
        :
            "%eax","%edx"
    );

    return data;
}

//Read wide data from the TDC register
DWORD Tdc::readTdc32(DWORD addr) const
{
    DWORD data;

    __asm__ __volatile__
    (
        "\t"
            "movl %2,       %%eax\n\t"  //Move TDC address to EAX register
            "movl %3,       %%edx\n\t"  //Move FishCamp address pointer into EDX
            "movl %%eax,   (%%edx)\n\t" //Move encoded TDC address into FishCamp register
            "movl %1,       %%eax\n\t"  //Move FishCamp wide port address into EAX
            "movl (%%eax),     %0\n\t"  //Save wide data
            "movl %2,       %%eax\n\t"  //Again TDC address to EAX
            "cmpl $0x2F,    %%eax\n\t"  //If it is the TDC register #15 on the Shifter board (time event)
            "jne 1f\n\t"
            "testl %0,         %0\n\t"  //If the time event data is valid
            "js 1f\n\t"
            "orl $0x80,         %%eax\n\t"  //Up write-read bit in the TDC address
            "movl %%eax,       (%%edx)\n\t" //And put it in the FishCamp address register
        "1:\n\t"
            "movl $0x7F,       (%%edx)\n\t" //Put host address to FishCamp address register
            :
                "=b"(data)
            :
                "m"(m_pWin),"m"(addr),"m"(m_pAddr)
            : "%eax", "%edx"
    );

    return data;
}

//Fish camp board initialization. In this place the programm is strong dependent on Windows OS
void Tdc::initFishCamp()
{
    DWORD unused;

    m_hFishCamp = CreateFileA
    (
        m_driverFileName,
        GENERIC_READ|GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    //Get user mode pointer
    if (m_hFishCamp == INVALID_HANDLE_VALUE)
    {
        setTdcError(TDC_FCDRIVER_NOT_FOUND);
        return;
    }

    if (!DeviceIoControl
            (
                m_hFishCamp,
                IOCTL_GET_UM_POINTER,
                NULL, 0,
                &m_pBaseMem, sizeof(char*),
                &unused,
                NULL
            )
        )
        {
            setTdcError(TDC_FCDRIVER_UM_PTR);
            return;
        }
}

//Try TDC initialization
void Tdc::initTdc(float activeTime)
{/*! TDC board Reset/Initialization sequence
    taken from the manual as is:
    WRIТЕ   0х10 0х00   Turn off Run Enable, Acquire
    Read    0хlЕ        Look for RUN bit OFF
    WRITЕ   0х11 0х10   RESET TEG Set
    WRITE   0х11 0х00   RESET TEG Clr
    WRITE   Ох12 0х7F   Data to TEG for Synchronous mode Valuе must be 0 for Asynchronous mode
    WRITE   0х11 0х88   write СIО to TEG Reg 2 (Writepointer)
    WRITE   0х11 0х8С   Clock the Write
    WRITE   0х11 0х88   Clear the Clock
    WRIТЕ   0х11 0х00   Сlеаr the operation
    WRITE   0х12 0х7Е   Data to TEG
    WRITE   0х11 0х48   Write CIO to TEG Reg 1 (Readpointer)
    WRITE   0х11 0х4С   Clock the Write
    WRITЕ   0х11 0х48   Clear the clock
    WRTTE   0х11 0х00   Сlеаr the operation
    WRTTE   0х11 0х80   Address TEG Reg 2 (Writepointer)
    WRITE   0х11 0х84   Clock the Read
    Read    0x12        Read the result (Writepointer value)
    WRITE   0х11 0х00   Clear the operation
    WRITE   0х11 0х40   Address TEG Reg 1 (Readpointer)
    WRITE   0х11 0х44   Clock the Read
    Read    0х12        Read the result (Readpointer value)
    WRIТЕ   0х11 0х00   Clear the operation
    WRTTE   0хl3 аl     LSB Active Time Count
    WRITЕ   0х14 ah     MSB Active Time Соunt
    WRITE   0х10 0х02   Run Enable, Synchronous/Asynchronous mode.
   */
    writeTdc8(TDC_REG0,0x00); //Turn off Run Enable, Aquire
    while (readTdc32(TDC_REG14) & 0x80000000UL); //Look for RUN bit OFF

    writeTdc8(TDC_REG1,0x10); //RESET TEG Set
    writeTdc8(TDC_REG1,0x00); //RESET TEG Clr
    writeTdc8(TDC_REG2,0x00); //0x00 - async 0x7F - sync

    writeTdc8(TDC_REG1,0x88); //write to wr
    writeTdc8(TDC_REG1,0x8C); //strobe (up and down the third bit?)
    writeTdc8(TDC_REG1,0x88); //write to wr
    writeTdc8(TDC_REG1,0x00); //terminate

    writeTdc8(TDC_REG2,0x7E); //put the data to (write into RD?)
    writeTdc8(TDC_REG1,0x48); //write (the data?) to RD
    writeTdc8(TDC_REG1,0x4C); //strobe (set one bit up and down)
    writeTdc8(TDC_REG1,0x48); //write (the data?) to RD
    writeTdc8(TDC_REG1,0x00); //terminate

    writeTdc8(TDC_REG1,0x80); //read from wr
    writeTdc8(TDC_REG1,0x84); //strobe
    if(readTdc8(TDC_REG2) != 0)
    {
        setTdcError(TDC_FAILED_CONNECTION);
        writeTdc8(TDC_REG1,0x00); //terminate
        return;
    }
    writeTdc8(TDC_REG1,0x00); //terminate

    writeTdc8(TDC_REG1,0x40); //read data from RD
    writeTdc8(TDC_REG1,0x44); //strobe
    if(readTdc8(TDC_REG2) != 0x7E)
    {
        setTdcError(TDC_FAILED_CONNECTION);
        writeTdc8(TDC_REG1,0x00); //terminate
        return;
    }
    writeTdc8(TDC_REG1,0x00); //terminate

    setActiveTime(activeTime);

    /*! Shifter board Reset/Initialization a little bit different from the manual,
     * but it was checked as working
    */
    //reset shifter
    writeTdc8(SHIFTER_REG7,0x83); //set async mode and individual events accumulation
    writeTdc8(SHIFTER_REG7,0x00);
    writeTdc8(SHIFTER_REG7,0x83);

    //prog shifter levels .625 ps per chanel
    writeTdc8(SHIFTER_REG0,0x00);
    writeTdc8(SHIFTER_REG1,0x00);
    writeTdc8(SHIFTER_REG2,0x00);
    writeTdc8(SHIFTER_REG3,0x00);

    //stop inputs independent 0x04 or mushed 0x03
    writeTdc8(SHIFTER_REG5,0x03);

    //shifter stuff
    writeTdc8(SHIFTER_REG4,0x00); //page 256K
    writeTdc8(SHIFTER_REG6,0x00); //page shift

    //clear starts and events
    writeTdc8(SHIFTER_REG8,0x00);
    writeTdc8(SHIFTER_REG9,0x00);
}


///Public TDC functions which creats the interface
//Read one event from the FIFO memory
DWORD Tdc::readEvent() const
{
    return readTdc32(SHIFTER_REG15);
}
//TDC asynchronous run
void Tdc::run() const
{
    writeTdc8(TDC_REG0,0x07);
}
//TDC stop
void Tdc::stop() const
{
    writeTdc8(TDC_REG0,0x00);
}
//Try TDC reset
void Tdc::reset()
{
    float activeTime = static_cast<float>(m_activeTime.activeTime) * 2.56 + 2.52;
    initTdc(activeTime);
}

//Check TDC flags
bool Tdc::isOverflow() const
{
   DWORD status = getTdcFifoStatus();
   return status & 0x20000000L;
}

//Check particular TDC data for validity
bool Tdc::isDataValid(DWORD data) const
{
    return !(data & 0x80000000L);
}


//Show TDC errors in C-string

const char* Tdc::getErrorState() const
{
    switch(m_tdcErrorState)
    {
        case TDC_FCDRIVER_NOT_FOUND:
            return "FishCamp driver was not found.\n";
        case TDC_FCDRIVER_UM_PTR:
            return "Can not get FishCamp user-mode pointer.\n";
        case TDC_NO_ERROR:
            return "No errors.\n";
        case TDC_FAILED_CONNECTION:
            return "Fail when trying to establish the connection with the IonWerks TDC.\n";
        default:
            return "It is both some bug that you see this string or may be you are looking at the source code.\n";
    }
}

//TDC constructor
Tdc::Tdc(float activeTime)
    :
        m_tdcError(false),
        m_tdcErrorState(TDC_NO_ERROR)
{
    const Tdc& This = *this;

    initFishCamp();

    if(!This) return; //Some errors occur during fish camp initialization

    //Set Fish Camp pointers
    m_pWin = m_pBaseMem + fc_dio_data_reg0_addr;
    m_pData= m_pBaseMem + fc_dio_data_reg1_addr;
    m_pAddr= m_pBaseMem + fc_dio_data_reg2_addr;
    m_pCfg = m_pBaseMem + fc_dio_cntrl_reg_addr;

    *(LPDWORD)m_pCfg = (DWORD)WRITEBYTE; //initialize port1 to write 1 byte data onto TDC

    //TDC initialization procedure
    initTdc(activeTime);
}

//Release resources
Tdc::~Tdc()
{
    DWORD unused;

    switch (m_tdcErrorState) {
    case TDC_FCDRIVER_NOT_FOUND:
    case TDC_FCDRIVER_UM_PTR:
        return; //resources were not allocated
    case TDC_FAILED_CONNECTION:
        break; //connection with TDC was not establed
    default:
        initTdc(150.0); //reset all parameters to defaults
        break;
    }


    DeviceIoControl
            (
                m_hFishCamp,
                IOCTL_FREE_UM_POINTER, //free user mode pointer
                NULL, 0,
                &m_pBaseMem, sizeof(char*),
                &unused,
                NULL
            );
    CloseHandle(m_hFishCamp); //close driver handle
}
