#ifndef TDCDATASTORAGE_H
#define TDCDATASTORAGE_H

#include <massspectrumbase.h>
#include <TDC.h>
#include <QPair>
#include <QVector>

typedef QPair<BYTE,DWORD> TdcEvent;
typedef QVector<TdcEvent> TdcEvents;

class TdcReadThread;

/*! \class TdcDataStorage tdcdatastorage.h
 * \brief This class is a consumer of \class TdcReadThread
*/
class TdcDataStorage : public MassSpectrumBase
{
    Q_OBJECT
public:
    TdcDataStorage(QObject* parent = 0, int frameSize = 5000);
    ~TdcDataStorage();

    QVector<double> MassScale() const;
    QVector<double> TimeScale() const;
    QVector<double> Intensities() const;
    QVector<double> Chromatogram() const;
    size_t getMassSpecNum() const;

    int getFrameSize() const;

    void setTdcReadThread(TdcReadThread* pTdcReadThread);

    ///Saves TDC data file in ascii format
    /// \param[in] fileName File name to save
    /// \returns 0 if Ok.
    int saveFileAscii(QString fileName) const;
signals:
    void frameRead();
    void frameSizeChanged(int frameSize);

public slots:
    void readFrame(int eventsNum);
    void setFrameSize(int frameSize);

private:
    TdcReadThread* m_pTdcReadThread;
    TdcEvents m_pTdcEvents;

    int  m_frameSize;

    //private functions:
    TdcEvents::const_iterator jumpToNextFrame(const TdcEvents::const_iterator& frame) const; //returns iterator to the next frame
    TdcEvents getCurrentFrame() const;
};

#endif // TDCDATASTORAGE_H
