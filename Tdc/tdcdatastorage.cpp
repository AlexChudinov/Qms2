#include <tdcdatastorage.h>
#include <tdcreadthread.h>
#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QTextStream>

TdcDataStorage::TdcDataStorage(QObject *parent, int frameSize)
    :
      MassSpectrumBase(parent),
      m_pTdcReadThread(nullptr),
      m_pTdcEvents(),
      m_frameSize(frameSize)
{
    setMassSpecIdx(-1); //signalize that there are no mass spectrums yet
}

TdcDataStorage::~TdcDataStorage()
{

}

void TdcDataStorage::readFrame(int eventsNum)
{
    m_pTdcEvents.resize(m_pTdcEvents.size()+eventsNum);

    std::for_each
    (
        m_pTdcEvents.end()-eventsNum,
        m_pTdcEvents.end(),
        [this](TdcEvent& event)
        {
            DWORD tdcEvent = m_pTdcReadThread->readEvent();
            BYTE chanel    = static_cast<BYTE>(tdcEvent >> 29);
            event.first    = chanel;
            event.second   = tdcEvent & 0x000FFFFFL;
        }
    );

    setMassSpecIdx(getMassSpecNum()-1);

    emit frameRead();
}

int TdcDataStorage::getFrameSize() const
{
    return m_frameSize;
}

void TdcDataStorage::setFrameSize(int frameSize)
{
    m_frameSize = frameSize;
    setMassSpecIdx(getMassSpecNum() - 1L);
    emit frameSizeChanged(m_frameSize);
}

void TdcDataStorage::setTdcReadThread(TdcReadThread *pTdcReadThread)
{
    m_pTdcReadThread = pTdcReadThread;
    connect(this,SIGNAL(frameSizeChanged(int)),m_pTdcReadThread,SLOT(setFrameSize(int)));
    connect(m_pTdcReadThread,SIGNAL(frameReady(int)),this,SLOT(readFrame(int)));
}

TdcEvents::const_iterator TdcDataStorage::jumpToNextFrame(const TdcEvents::const_iterator &frame) const
{
    TdcEvents::const_iterator nextFrame = frame;
    int nStarts = 0;
    while(nStarts < m_frameSize && nextFrame != m_pTdcEvents.end())
    {
        if((++nextFrame)->second == 0)
        {
            ++nStarts;
        }
    }
    return nextFrame;
}

TdcEvents TdcDataStorage::getCurrentFrame() const
{
    TdcEvents::const_iterator Start = m_pTdcEvents.begin(), End;

    //Find current frame:
    for(int i = 0; i < getMassSpecIdx(); ++i, Start = jumpToNextFrame(Start));

    End = jumpToNextFrame(Start);

    TdcEvents res(std::distance(Start,End));

    std::copy(Start,End,res.begin());

    return res;
}

QVector<double> TdcDataStorage::Intensities() const
{
    if (getMassSpecIdx() < 0) return QVector<double>(); //no data return empty vector

    QVector<double> res;

    if (isShowTotal())
    {
        res = QVector<double>
                (
                    std::max_element
                    (
                        m_pTdcEvents.begin(),
                        m_pTdcEvents.end(),
                        [](const TdcEvent& e1, const TdcEvent& e2)->bool{ return e1.second < e2.second; }
                    )->second
                );

        std::for_each(m_pTdcEvents.begin(),m_pTdcEvents.end(),[&res](const TdcEvent& event)->void{ if (event.second != 0) res[event.second-1]++; }); //build histogramm
    }
    else
    {
        TdcEvents currentFrame = getCurrentFrame();
        res = QVector<double>
                (
                    std::max_element
                    (
                        currentFrame.begin(),
                        currentFrame.end(),
                        [](const TdcEvent& e1, const TdcEvent& e2)->bool{ return e1.second < e2.second; }
                    )->second
                );

        std::for_each(currentFrame.begin(),currentFrame.end(),[&res](const TdcEvent& event)->void{ if (event.second != 0) res[event.second-1]++; });
    }
    return res;
}

QVector<double> TdcDataStorage::TimeScale() const
{
    if (getMassSpecIdx() < 0) return QVector<double>(); //no data return empty vector

    QVector<double> res;
    double t0 = 0.5;

    if (isShowTotal())
    {
        res = QVector<double>
                (
                    std::max_element
                    (
                        m_pTdcEvents.begin(),
                        m_pTdcEvents.end(),
                        [](const TdcEvent& e1, const TdcEvent& e2)->bool{ return e1.second < e2.second; }
                    )->second
                );
    }
    else
    {
        TdcEvents currentFrame = getCurrentFrame();
        res = QVector<double>
                (
                    std::max_element
                    (
                        currentFrame.begin(),
                        currentFrame.end(),
                        [](const TdcEvent& e1, const TdcEvent& e2)->bool{ return e1.second < e2.second; }
                    )->second
                );
    }

    std::for_each(res.begin(),res.end(),[&t0](double& time)->void{ time = (t0++) * Tdc::m_tdcTimeStepUs; });

    return res;
}

QVector<double> TdcDataStorage::MassScale() const
{
    QVector<double> res = TimeScale();
    std::for_each(res.begin(),res.end(),[this](double& mass)->void{ mass = timeToMass(mass); });
    return res;
}

QVector<double> TdcDataStorage::Chromatogram() const
{
    if (getMassSpecNum() <= 0) return QVector<double>();
    QVector<double> res(getMassSpecNum());
    TdcEvents::const_iterator pFrame = m_pTdcEvents.begin();
    std::for_each(res.begin(),res.end(),
                  [&pFrame,this](double& tic)->void
                  {
                        tic = static_cast<double>(std::distance(pFrame,jumpToNextFrame(pFrame)) - m_frameSize); //remove elements containing zeros
                        pFrame = jumpToNextFrame(pFrame);
                  }
                 );
    return res;
}

size_t TdcDataStorage::getMassSpecNum() const
{
    int massSpecNum = 0;
    TdcEvents::const_iterator pFrame = m_pTdcEvents.begin();
    for(;pFrame < m_pTdcEvents.end(); pFrame = jumpToNextFrame(pFrame), ++massSpecNum);
    return massSpecNum;
}

int TdcDataStorage::saveFileAscii(QString fileName) const
{
    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) return -1;

    QTextStream out(&file);

    std::for_each(m_pTdcEvents.begin(),
                  m_pTdcEvents.end(),
                  [&out](TdcEvent event){ out << static_cast<int>(event.first)
                                              << "\t"
                                              << event.second
                                              << "\n"; });
    file.close();
    return 0;
}
