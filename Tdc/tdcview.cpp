#include <tdcdatastorage.h>
#include <tdcreadthread.h>
#include <tdcview.h>
#include <delegates.h>
#include <QAction>
#include <QToolBar>
#include <QLayout>
#include <QTableView>
#include <QHeaderView>

//Model constants
//From \class TdcParams
const char* TdcParams::m_paramsStr[TdcParams::m_itemsNum] =
   {
       "Состояние TDC",
       "Число накоплений",
       "Активное время",
       "Число переполнений"
   };
const QVariant TdcParams::m_paramsVal[TdcParams::m_itemsNum] =
    {
        "No errors.\n",
        5000,
        "151 мкс",
        0
    };
//-------------------------------------------

TdcView::TdcView(QWidget *parent)
    :
      QMainWindow(parent),
      m_actionRun
      (
          new QAction(QIcon(":/play.png"),
                      QObject::tr("Запуск регистрации"),
                      this
                      )
      ),
      m_actionStop
      (
          new QAction(QIcon(":/stop.png"),
                      QObject::tr("Полная остановка регистрации"),
                      this
                      )
      ),
      m_toolBar(addToolBar(QObject::tr("TDC интерфейс"))),
      m_tdcModelView(new QTableView(this)),
      m_tdcModel(new TdcModel(this))
{
    //set action parameters
    m_actionRun->setToolTip(QObject::tr("Запуск TDC"));
    m_actionStop->setToolTip(QObject::tr("При этом происходит выход из цикла регистрации и перезагрузка TDC"));
    m_actionStop->setCheckable(true);

    //Add actions
    m_toolBar->addAction(m_actionRun);
    m_toolBar->addAction(m_actionStop);

    //Set model
    setCentralWidget(m_tdcModelView);
    m_tdcModelView->setModel(m_tdcModel);
    m_tdcModelView->horizontalHeader()->hide();
    AccumNumberDelegate* pAcNumDelegate = new AccumNumberDelegate(this);
    m_tdcModelView->setItemDelegateForRow(1,pAcNumDelegate);
    ActiveTimeDelegate* pAcTimeDelegate = new ActiveTimeDelegate(this);
    m_tdcModelView->setItemDelegateForRow(2,pAcTimeDelegate);
    m_tdcModelView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Set start stop actions
    connect(m_actionRun,SIGNAL(triggered()),
            static_cast<TdcModel*>(m_tdcModelView->model()),SLOT(setRun()));
    connect(m_actionStop,SIGNAL(triggered()),
            static_cast<TdcModel*>(m_tdcModelView->model()),SLOT(setStop()));
    connect(static_cast<TdcModel*>(m_tdcModelView->model()),SIGNAL(tdcStopped(bool)),
            m_actionStop,SLOT(setChecked(bool)));

    //Connect to a some plotting staff
    connect(m_tdcModel,SIGNAL(showMassSpec(QObject*)),this,SIGNAL(showMassSpec(QObject*)));
}

TdcView::~TdcView()
{

}

TdcModel::TdcModel(QObject *parent)
    :
      QStandardItemModel(parent),
      m_tdc(new TdcReadThread(this)),
      m_tdcDataStream(new TdcDataStorage(this))
{
    //connect tdc to data storage
    m_tdcDataStream->setTdcReadThread(m_tdc);

    setRowCount(TdcParams::m_itemsNum);
    setColumnCount(1);
    for(int i = 0; i < TdcParams::m_itemsNum; ++i)
    {
        m_tdcModelItems[TdcParams::m_paramsStr[i]].first
                = new QStandardItem(TdcParams::m_paramsStr[i]);
        setVerticalHeaderItem
                (i,m_tdcModelItems[TdcParams::m_paramsStr[i]].first);
        if (TdcParams::m_paramsVal[i].canConvert<QString>())
        {
            m_tdcModelItems[TdcParams::m_paramsStr[i]].second
                    = new QStandardItem(TdcParams::m_paramsVal[i].toString());
            m_tdcModelItems[TdcParams::m_paramsStr[i]].second->setEditable(false);
            m_tdcModelItems[TdcParams::m_paramsStr[i]].second->setSelectable(false);
        }
        else
        {
            m_tdcModelItems[TdcParams::m_paramsStr[i]].second
                    = new QStandardItem();
            m_tdcModelItems[TdcParams::m_paramsStr[i]].second
                    ->setData(TdcParams::m_paramsVal[i].toInt(),Qt::EditRole);

        }
        setItem(i,0,m_tdcModelItems[TdcParams::m_paramsStr[i]].second);
    }
    m_tdcModelItems[TdcParams::m_paramsStr[1]].second->setEditable(true);
    m_tdcModelItems[TdcParams::m_paramsStr[1]].second->setSelectable(true);
    m_tdcModelItems[TdcParams::m_paramsStr[2]].second->setEditable(true);
    m_tdcModelItems[TdcParams::m_paramsStr[2]].second->setSelectable(true);

    //Connect signals to TDC
    connect(this,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(emitModelChanged(QStandardItem*)));
    connect(this,SIGNAL(accumNumberChanged(int)),m_tdcDataStream,SLOT(setFrameSize(int)));
    connect(this,SIGNAL(activeTimeChanged(float)),m_tdc,SLOT(setActiveTime(float)));
    connect(m_tdc,SIGNAL(tdcErrorStateChanged(const char*)),this,SLOT(setErrorState(const char*)));
    connect(m_tdc,SIGNAL(finished()),this,SLOT(setStopIcon()));
    connect(m_tdc,SIGNAL(overflowsNumberChanged(int)),this,SLOT(setOverflowsNum(int)));
    connect(m_tdc,SIGNAL(finished()),this,SIGNAL(tdcStopped()));
}

TdcModel::~TdcModel()
{

}

void TdcModel::setRun()
{
    m_tdcModelItems[TdcParams::m_paramsStr[0]].first->setIcon(QIcon(":/play.png"));
    m_tdcModelItems[TdcParams::m_paramsStr[0]].first->setToolTip("Идет накопление событий.");
    m_tdc->setRun();

    m_tdcDataStream->needToSave(true);

    emit showMassSpec(qobject_cast<QObject*>(m_tdcDataStream));
}

void TdcModel::setStop()
{
    m_tdc->setStop();
}

void TdcModel::setStopIcon()
{
    m_tdcModelItems[TdcParams::m_paramsStr[0]].first->setIcon(QIcon(":/stop.png"));
    m_tdcModelItems[TdcParams::m_paramsStr[0]].first->setToolTip("TDC остановлен.");
}

void TdcModel::setErrorState(const char *errorStateStr)
{
    m_tdcModelItems[TdcParams::m_paramsStr[0]].second->setToolTip("Ошибка TDC!");
    m_tdcModelItems[TdcParams::m_paramsStr[0]].second->setIcon(QIcon(":/warn.png"));
    m_tdcModelItems[TdcParams::m_paramsStr[0]].second->setData(QString(errorStateStr),Qt::DisplayRole);
}

void TdcModel::emitModelChanged(QStandardItem *item)
{
    if(item == m_tdcModelItems[TdcParams::m_paramsStr[1]].second)
    {
        int value = m_tdcModelItems[TdcParams::m_paramsStr[1]].second
                ->data(Qt::EditRole).toInt();
        emit accumNumberChanged(value);
    }
    //Active time:
    if(item == m_tdcModelItems[TdcParams::m_paramsStr[2]].second)
    {
        QString data =  m_tdcModelItems[TdcParams::m_paramsStr[2]].second
                ->data(Qt::EditRole).toString();
        float value = data.split(" ")[0].toFloat();
        emit activeTimeChanged(value);
    }
}

void TdcModel::setOverflowsNum(int overflowsNum)
{
    m_tdcModelItems[TdcParams::m_paramsStr[3]].second->setText(QObject::tr("%0").arg(overflowsNum));
}
