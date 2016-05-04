#ifndef TOFMACSUPPORT_H
#define TOFMACSUPPORT_H

#include <massspectrumbase.h>
#include <QVector>

class MSDataStructI;

class TofMacSupport : public MassSpectrumBase
{
    Q_OBJECT
public:
    TofMacSupport(QObject* parent = 0);
    ~TofMacSupport();

    size_t getMassSpecNum() const;

public slots:
    void loadMassSpec(const QString& str);
protected:
    ///Recalculates buffered mass spectrum data
    void recalculateMassSpec();
private:
    MSDataStructI* m_msDataStruct;
};

#endif // TOFMACSUPPORT_H
