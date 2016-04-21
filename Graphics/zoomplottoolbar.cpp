#include <zoomplottoolbar.h>
#include <QAction>

ZoomPlotToolBar::ZoomPlotToolBar(QObject *parent)
    :
      QToolBar(qobject_cast<QWidget*>(parent)),
      m_vZoomAction(new QAction(QIcon(":/vzoom.png"),"Приближение вертикальной шкалы",parent)),
      m_hZoomAction(new QAction(QIcon(":/hzoom.png"),"Приближение горизонтальной шкалы",parent)),
      m_zoomOutAction(new QAction(QIcon(":/Zoom-Out-icon.png"),"Полная шкала",parent))
{
    m_vZoomAction->setCheckable(true);
    m_hZoomAction->setCheckable(true);

    connect(m_vZoomAction,SIGNAL(toggled(bool)),this,SIGNAL(vZoom(bool)));
    connect(m_hZoomAction,SIGNAL(toggled(bool)),this,SIGNAL(hZoom(bool)));
    connect(m_zoomOutAction,SIGNAL(triggered()),this,SIGNAL(zoomOut()));

    addAction(m_vZoomAction);
    addAction(m_hZoomAction);
    addAction(m_zoomOutAction);
}

ZoomPlotToolBar::~ZoomPlotToolBar()
{

}

void ZoomPlotToolBar::setHZoom(bool hZoom)
{
    m_hZoomAction->setChecked(hZoom);
}

void ZoomPlotToolBar::setVZoom(bool vZoom)
{
    m_vZoomAction->setChecked(vZoom);
}
