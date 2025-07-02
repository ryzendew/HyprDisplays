#include "monitorgraphicsview.h"
#include <QWheelEvent>
#include <QtMath>

MonitorGraphicsView::MonitorGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

MonitorGraphicsView::MonitorGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MonitorGraphicsView::wheelEvent(QWheelEvent *event)
{
    constexpr double zoomFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(zoomFactor, zoomFactor);
        m_zoomLevel *= zoomFactor;
    } else {
        scale(1.0 / zoomFactor, 1.0 / zoomFactor);
        m_zoomLevel /= zoomFactor;
    }
    event->accept();
} 