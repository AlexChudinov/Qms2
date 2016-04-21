#ifndef ZOOMPLOTTOOLBAR_H
#define ZOOMPLOTTOOLBAR_H

#include <QToolBar>

class ZoomPlotToolBar : public QToolBar
{
    Q_OBJECT
public:
    ZoomPlotToolBar(QObject* parent = 0);
    ~ZoomPlotToolBar();

public slots:
    void setVZoom(bool vZoom);
    void setHZoom(bool hZoom);

signals:
    void vZoom(bool);
    void hZoom(bool);
    void zoomOut();

private:
    QAction* m_vZoomAction;
    QAction* m_hZoomAction;
    QAction* m_zoomOutAction;
};

#endif // ZOOMPLOTTOOLBAR_H
