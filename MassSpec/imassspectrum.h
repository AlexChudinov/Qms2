/*!
  \file imassspectrum.h
  \brief Interface to different mass spectrum data formats
  \author Alexey Chudinov
  \version 1.0
  \date 07.04.2016
*/

#ifndef IMASSSPECTRUM_H
#define IMASSSPECTRUM_H

#include <QObject>

class MassSpectrumBase;

class IMassSpectrum : public QObject
{
    Q_OBJECT
public:
    IMassSpectrum(QObject *parent = 0);
    ~IMassSpectrum();

    ///Avaluable mass spectrum types
    enum MassSpecType
    {
        NODATA = 0,     ///< Mass spectrum data were not created
        TOFFILE = 1,    ///< Mass spectrum loaded from a tof-file
        TDCSTREAM = 2   ///< Mass spectrum direcly obtained from a TDC
    };

    ///Returns the integral type of the data using file filter string
    /// \param[in] strType File filter string
    /// \returns Integral type of contained mass spectrum data or MassSpecType::NODATA
    /// in the case of error.
    static MassSpecType stringToType(QString strType);

    ///Returns the string file filter which corresponds to mass spectrum data type
    /// \param[in] intType Mass spectrum type ID
    /// \returns File filter corresponding to selected ID or empty string on error
    static QString typeToString(MassSpecType intType);


    ///Check if the data pointer can belongs to another instance
    /// \return TRUE if it is or FALSE if it is not
    bool dataDeleteProhibited() const;
    ///Removes mass spectrum data from the instance
    void removeData();

    ///Check if there is some data that needs to be saved
    /// \return TRUE if it is
    bool isNeedToSave() const;
signals:
    void showMassSpec(QObject* massSpec);
    void state(QString); //signalize about current state

public slots:
    ///Loads mass spectrum from the specified data file
    /// \param[in] fileName File name
    /// \param[in] fileType File type. Supported file types can be found in IMassSpectrum::MassSpecType enumeration
    void loadMassSpec(QString fileName, QString fileFilter);

    ///Estables direct conection to TDC
    /// \param[in] tdcDataStream Time events array wrapper
    void setTdcConnection(QObject* tdcDataStream);

    ///Saves data in m_msDataStruct
    void saveData(QString fileName, QString fileFilter);
private:
    MassSpectrumBase* m_msDataStruct;
    bool m_isDeleteProhibited;
    MassSpecType m_type;

public:
    const static QVector<QString> m_fileFilters;
};

#endif // IMASSSPECTRUM_H
