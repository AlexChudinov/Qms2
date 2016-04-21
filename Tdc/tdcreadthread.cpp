#include <tdcreadthread.h>
#include <TDC.h>

TdcReadThread::TdcReadThread(QObject* parent, int readFrame, float activeTimeUs)
    :
      QThread(parent),
      m_setStop(true),
      m_activeTimeUs(activeTimeUs),
      m_frameSize(readFrame),
      m_overFlowsNumber(0)
{
}

TdcReadThread::~TdcReadThread()
{
}

void TdcReadThread::run()
{
    DWORD event;
    int startsNumber = 0;
    int eventsNumber = 0;
    Tdc TDC(m_activeTimeUs); //create tdc instance in a new thread

    if(!TDC)
    {
        tdcErrorStateChanged(TDC.getErrorState());
        return;
    }

    TDC.run();
    m_setStop = false;

    forever
    {
        if(TDC.isOverflow())
        {
            ++m_overFlowsNumber;
            emit overflowsNumberChanged(m_overFlowsNumber);
            TDC.reset();
            TDC.run();
        }

        event = TDC.readEvent();

        if(TDC.isDataValid(event))
        {
            if(event == 0)
                ++startsNumber;
            m_tdcBuffer.enqueue(event);
            ++eventsNumber;
        }
        if(startsNumber >= m_frameSize)
        {
            emit frameReady(eventsNumber);
            eventsNumber = 0;
            startsNumber = 0;
            if(m_setStop)
            {
                m_overFlowsNumber = 0;
                emit overflowsNumberChanged(0);
                return;
            }
        }
    }

}

void TdcReadThread::setRun()
{
    if(m_setStop)
    {
        start(QThread::TimeCriticalPriority);
    }
}

void TdcReadThread::setStop()
{
    m_setStop = true;
}

float TdcReadThread::getActiveTime() const
{
    return m_activeTimeUs;
}

int TdcReadThread::getFrameSize() const
{
    return m_frameSize;
}

void TdcReadThread::setActiveTime(float activeTime)
{
    m_activeTimeUs = activeTime;
}

void TdcReadThread::setFrameSize(int frameSize)
{
    m_frameSize = frameSize;
}

unsigned long TdcReadThread::readEvent()
{
    return m_tdcBuffer.dequeue();
}
