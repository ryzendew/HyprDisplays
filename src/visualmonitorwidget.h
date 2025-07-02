#ifndef VISUALMONITORWIDGET_H
#define VISUALMONITORWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class VisualMonitorWidget : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    VisualMonitorWidget(const QString &name, const QString &resolution, int width, int height, QGraphicsItem *parent = nullptr, double scaleFactor = 0.1);
    ~VisualMonitorWidget();

    QString getName() const { return m_name; }
    QString getResolution() const { return m_resolution; }
    QSize getSize() const { return QSize(m_width, m_height); }
    
    void setPrimary(bool primary);
    bool isPrimary() const { return m_isPrimary; }
    
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_isEnabled; }
    
    void setScale(double scale);
    double getScale() const { return m_scale; }
    
    void setTransform(const QString &transform);
    QString getTransform() const { return m_transform; }
    
    void setHDR(bool hdr);
    bool isHDR() const { return m_hdr; }
    
    void setSDRBrightness(double brightness);
    double getSDRBrightness() const { return m_sdrBrightness; }
    
    void setSDRSaturation(double saturation);
    double getSDRSaturation() const { return m_sdrSaturation; }
    void setTenBit(bool tenBit);
    bool isTenBit() const { return m_tenBit; }
    void setWideGamut(bool wideGamut);
    bool isWideGamut() const { return m_wideGamut; }

signals:
    void monitorMoved(const QString &name, const QPointF &pos);
    void monitorClicked(const QString &name);
    void monitorDoubleClicked(const QString &name);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    void updateAppearance();
    void animateSelection();
    
    QString m_name;
    QString m_resolution;
    int m_width;
    int m_height;
    bool m_isPrimary;
    bool m_isEnabled;
    double m_scale;
    QString m_transform;
    bool m_hdr;
    double m_sdrBrightness;
    double m_sdrSaturation;
    
    bool m_isHovered;
    bool m_isSelected;
    bool m_isDragging;
    QPointF m_dragStartPos;
    
    QPropertyAnimation *m_selectionAnimation;
    QGraphicsOpacityEffect *m_opacityEffect;
    
    // Visual properties
    QColor m_borderColor;
    QColor m_fillColor;
    QColor m_textColor;
    int m_borderWidth;
    int m_cornerRadius;
    
    bool m_tenBit = false;
    bool m_wideGamut = false;
};

#endif // VISUALMONITORWIDGET_H 