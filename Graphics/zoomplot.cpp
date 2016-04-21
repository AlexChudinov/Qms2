#include <zoomplot.h>
#include <algorithm>

ZoomPlot::ZoomPlot(QWidget *parent)
    :
      QCustomPlot(parent),
      m_zoomOutCursor(new QCursor(QPixmap(":/Zoom-Out-icon.png"))),
      m_vZoomCursor(new QCursor(QPixmap(":/vzoom.png"))),
      m_hZoomCursor(new QCursor(QPixmap(":/hzoom.png"))),
      m_selectionArea(nullptr),
      m_isZoomOut(false),
      m_isHZoom(false),
      m_isVZoom(false),
      m_isDrugged(false),
      m_needToRescale(true)
{
    yAxis->setNumberFormat(QObject::tr("ebc"));
    yAxis->setNumberPrecision(2);
}

ZoomPlot::~ZoomPlot()
{
    delete m_hZoomCursor;
    delete m_vZoomCursor;
    delete m_zoomOutCursor;
}

bool ZoomPlot::rescalable() const
{
    return m_needToRescale;
}

bool ZoomPlot::isZoom() const
{
    return m_isHZoom || m_isVZoom ;
}

void ZoomPlot::zoomCheck(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton) return; //zoom operation use left mouse button
    if(m_isHZoom && !m_isDrugged) //horizontal zoom
    {
        m_isDrugged = true;
        m_selectionOrigin = QPoint
                (
                    event->pos().x(),
                    yAxis->coordToPixel(yAxis->range().lower)
                );
        m_selectionArea = new QRubberBand(QRubberBand::Rectangle,this);
        return;
    }

    if(m_isVZoom && !m_isDrugged) //vertical zoom
    {
        m_isDrugged = true;
        m_selectionOrigin = QPoint
                (
                    xAxis->coordToPixel(xAxis->range().lower),
                    event->pos().y()
                );
        m_selectionArea = new QRubberBand(QRubberBand::Rectangle,this);
        return;
    }
}

void ZoomPlot::zoomDrag(QMouseEvent *event)
{
    if(m_isHZoom)
    {
        m_selectionArea->setGeometry
                (
                    QRect
                    (
                        m_selectionOrigin,
                        QPoint
                        (
                            event->pos().x(),
                            yAxis->coordToPixel(yAxis->range().upper)
                        )
                    ).normalized()
                );
        m_selectionArea->show();
        return;
    }

    if(m_isVZoom)
    {
        m_selectionArea->setGeometry
                (
                    QRect
                    (
                        m_selectionOrigin,
                        QPoint
                        (
                            xAxis->coordToPixel(xAxis->range().upper),
                            event->pos().y()
                        )
                    ).normalized()
                );
        m_selectionArea->show();
        return;
    }
}

void ZoomPlot::zoomRelease() //relese zoom when mouse button is released
{
    if(m_selectionArea && isZoom())
    {
        m_isDrugged = false;
        const QRect& zoomRect = m_selectionArea->geometry();

        if(m_isHZoom)
        {
            double xmin = xAxis->pixelToCoord(zoomRect.x());
            double xmax = xAxis->pixelToCoord(zoomRect.x()+zoomRect.width());
            xAxis->setRange(xmin,xmax);
            //find best values to the yRange
            if(graphCount() != 0) //if there is at least one graph on a plot
            {
                const QCPDataMap& Data = *(graph(0)->data());
                double ymin = yAxis->range().upper;
                double ymax = yAxis->range().lower;
                for(auto& xy : Data)
                {
                    if(xy.key < xmin || xy.key > xmax) continue;

                    if(ymin > xy.value) ymin = xy.value;
                    if(ymax < xy.value) ymax = xy.value;
                }
                yAxis->setRange(ymin,ymax);
            }
        }
        if(m_isVZoom)
        {
            double ymin = yAxis->pixelToCoord(zoomRect.y());
            double ymax = yAxis->pixelToCoord(zoomRect.y()+zoomRect.height());
            yAxis->setRange(ymin,ymax);
        }

        m_selectionArea->hide();
        delete m_selectionArea;
        m_selectionArea = nullptr;

        replot();
    }
}

void ZoomPlot::setHZoom(bool hZoom)
{
    if(hZoom)
    {
        m_needToRescale = false;
        m_isHZoom = true;
        if(m_isVZoom)
        {
            m_isVZoom = false;
            emit vZoomChanged(false);
        }

        setCursor(*m_hZoomCursor);
    }
    else
    {
        m_isHZoom = false;
        if(!m_isVZoom) setCursor(Qt::ArrowCursor); //vzoom is also switched off
        emit hZoomChanged(false);
    }
}

void ZoomPlot::setVZoom(bool vZoom)
{
    if(vZoom)
    {
        m_needToRescale = false;
        m_isVZoom = true;
        if(m_isHZoom)
        {
            m_isHZoom = false;
            emit hZoomChanged(false);
        }

        setCursor(*m_vZoomCursor);
    }
    else
    {
        m_isVZoom = false;
        if(!m_isHZoom) setCursor(Qt::ArrowCursor); //hzoom is also switched off
        emit vZoomChanged(false);
    }
}

void ZoomPlot::setZoomOut(bool zoomOut)
{
    if (zoomOut)
    {
        setHZoom(false);
        setVZoom(false);
        m_isZoomOut = true;
        setCursor(*m_zoomOutCursor);
    }
    else
    {
        m_isZoomOut = false;
        setCursor(Qt::ArrowCursor);
    }
}

void ZoomPlot::unzoom()
{
    if(graphCount() != 0)
    {
        graph(0)->rescaleAxes();
        replot();
    }
    m_isZoomOut = false;
    setCursor(Qt::ArrowCursor);
    m_needToRescale = true;
    emit zoomOutChanged(false);
}

void ZoomPlot::mousePressEvent(QMouseEvent *event)
{
    if(isZoom()) zoomCheck(event);
    if(m_isZoomOut) unzoom();
}

void ZoomPlot::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isDrugged) zoomDrag(event);
}

void ZoomPlot::mouseReleaseEvent(QMouseEvent *)
{
    zoomRelease();
}

bool ZoomPlot::isZoomOut() const
{
    return m_isZoomOut;
}
