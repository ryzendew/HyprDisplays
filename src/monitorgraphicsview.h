#pragma once
#include <QGraphicsView>

class MonitorGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit MonitorGraphicsView(QWidget *parent = nullptr);
    explicit MonitorGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    double m_zoomLevel = 1.0;
}; 