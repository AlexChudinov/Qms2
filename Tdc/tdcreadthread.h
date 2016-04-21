#ifndef TDCREADTHREAD_H
#define TDCREADTHREAD_H

#include <QThread>
#include <QQueue>

class TdcReadThread : public QThread
{
    Q_OBJECT
public:
    TdcReadThread(QObject *parent = 0, int readFrame = 5000, float activeTimeUs = 150);
    ~TdcReadThread();

    void run();

    unsigned long readEvent();

    float getActiveTime() const;

    int   getFrameSize() const;

signals:
    void frameReady(int);
    void tdcErrorStateChanged(const char*);
    void overflowsNumberChanged(int);

public slots:
    void setRun();
    void setStop();            //returns from the run() resulting in destroing TDC instance

    void  setFrameSize(int frameSize);
    void  setActiveTime(float activeTime);

private:
    volatile bool m_setStop; //set TDC to stop flag

    float m_activeTimeUs;    //TDC active time in microseconds

    volatile int m_frameSize;       //How mach starts should be readen
    int    m_overFlowsNumber; //Number of overflows occured in a current run


    QQueue<unsigned long> m_tdcBuffer;
};

#endif // TDCREADTHREAD_H
