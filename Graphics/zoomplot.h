#ifndef ZOOMPLOT_H
#define ZOOMPLOT_H

#include <qcustomplot.h>

//Base plot which has horizontal zoom

class ZoomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    ZoomPlot(QWidget *parent = 0);
    virtual ~ZoomPlot();

    void zoomCheck(QMouseEvent *event);
    void zoomDrag(QMouseEvent *event);
    void zoomRelease();
    bool isZoom() const;
    void unzoom();
    bool rescalable() const;

    bool isZoomOut() const;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *);

signals:
    void vZoomChanged(bool vZoom);
    void hZoomChanged(bool hZoom);
    void zoomOutChanged(bool zoomOut);

public slots:
    void setVZoom(bool vZoom);
    void setHZoom(bool hZoom);
    void setZoomOut(bool zoomOut = true);

protected:
    QCursor* m_zoomOutCursor;
    QCursor* m_vZoomCursor;
    QCursor* m_hZoomCursor;

    QRubberBand* m_selectionArea;

    QPoint m_selectionOrigin;

    bool m_isZoomOut;
    bool m_isHZoom;
    bool m_isVZoom;
    bool m_isDrugged;
    bool m_needToRescale;
};

#endif // ZOOMPLOT_H
