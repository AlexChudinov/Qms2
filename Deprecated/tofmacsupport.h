#ifndef TOFMACSUPPORT_H
#define TOFMACSUPPORT_H

#include <massspectrumbase.h>

class MSDataStructI;

class TofMacSupport : public MassSpectrumBase
{
    Q_OBJECT
public:
    TofMacSupport(QObject* parent = 0);
    ~TofMacSupport();

    //Main function implementation
    QVector<double> MassScale()    const;
    QVector<double> TimeScale()    const;
    QVector<double> Intensities()  const;
    QVector<double> Chromatogram() const;
    size_t getMassSpecNum() const;

public slots:
    void loadMassSpec(const QString& str);

private:
    MSDataStructI* m_msDataStruct;
};

#endif // TOFMACSUPPORT_H
