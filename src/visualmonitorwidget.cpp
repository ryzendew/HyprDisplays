#include "visualmonitorwidget.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

VisualMonitorWidget::VisualMonitorWidget(const QString &name, 
                                         const QString &resolution,
                                         int width, int height,
                                         QGraphicsItem *parent,
                                         double scaleFactor)
    : QGraphicsWidget(parent)
    , m_name(name)
    , m_resolution(resolution)
    , m_width(width)
    , m_height(height)
    , m_isPrimary(false)
    , m_isEnabled(true)
    , m_scale(1.0)
    , m_transform("normal")
    , m_hdr(false)
    , m_sdrBrightness(1.0)
    , m_sdrSaturation(1.0)
    , m_isHovered(false)
    , m_isSelected(false)
    , m_isDragging(false)
    , m_selectionAnimation(nullptr)
    , m_borderWidth(2)
    , m_cornerRadius(8)
    , m_tenBit(false)
    , m_wideGamut(false)
{
    // Set up the widget
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // Calculate scaled size for visual representation
    setMinimumSize(m_width * scaleFactor, m_height * scaleFactor);
    setMaximumSize(m_width * scaleFactor, m_height * scaleFactor);
    resize(m_width * scaleFactor, m_height * scaleFactor);
    
    // Set up animations
    m_selectionAnimation = new QPropertyAnimation(this, "opacity", this);
    m_selectionAnimation->setDuration(200);
    
    // Set up drop shadow
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setOffset(2, 2);
    setGraphicsEffect(shadowEffect);
    
    updateAppearance();
}

VisualMonitorWidget::~VisualMonitorWidget()
{
}

void VisualMonitorWidget::setPrimary(bool primary)
{
    if (m_isPrimary != primary) {
        m_isPrimary = primary;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setEnabled(bool enabled)
{
    if (m_isEnabled != enabled) {
        m_isEnabled = enabled;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setScale(double scale)
{
    if (m_scale != scale) {
        m_scale = scale;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setTransform(const QString &transform)
{
    if (m_transform != transform) {
        m_transform = transform;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setHDR(bool hdr)
{
    if (m_hdr != hdr) {
        m_hdr = hdr;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setSDRBrightness(double brightness)
{
    if (m_sdrBrightness != brightness) {
        m_sdrBrightness = brightness;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setSDRSaturation(double saturation)
{
    if (m_sdrSaturation != saturation) {
        m_sdrSaturation = saturation;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setTenBit(bool tenBit)
{
    if (m_tenBit != tenBit) {
        m_tenBit = tenBit;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::setWideGamut(bool wideGamut)
{
    if (m_wideGamut != wideGamut) {
        m_wideGamut = wideGamut;
        updateAppearance();
        update();
    }
}

void VisualMonitorWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    QRectF rect = boundingRect();
    
    // Draw background
    painter->fillRect(rect, m_fillColor);
    
    // Draw border
    QPen pen(m_borderColor, m_borderWidth);
    painter->setPen(pen);
    painter->drawRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    
    // Draw text
    painter->setPen(m_textColor);
    painter->setFont(QFont("Arial", 8));
    
    // Draw monitor name
    painter->drawText(rect, Qt::AlignTop | Qt::AlignHCenter, m_name);
    
    // Draw resolution
    painter->drawText(rect, Qt::AlignCenter, m_resolution);
    
    // Draw additional info
    QString info = QString("Scale: %1").arg(m_scale);
    if (m_hdr) info += " HDR";
    painter->drawText(rect, Qt::AlignBottom | Qt::AlignHCenter, info);
    
    // Draw primary indicator
    if (m_isPrimary) {
        painter->setPen(QPen(Qt::yellow, 3));
        painter->drawEllipse(rect.topLeft() + QPointF(5, 5), 5, 5);
    }
}

void VisualMonitorWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        emit monitorClicked(m_name);
    }
    QGraphicsWidget::mousePressEvent(event);
}

void VisualMonitorWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isDragging && (event->pos() - m_dragStartPos).manhattanLength() > 5) {
        // Start dragging
        QGraphicsWidget::mouseMoveEvent(event);
    }
}

void VisualMonitorWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
        emit monitorMoved(m_name, pos());
    }
    QGraphicsWidget::mouseReleaseEvent(event);
}

void VisualMonitorWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = true;
    updateAppearance();
    setCursor(Qt::OpenHandCursor);
}

void VisualMonitorWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = false;
    updateAppearance();
    setCursor(Qt::ArrowCursor);
}

void VisualMonitorWidget::updateAppearance()
{
    if (!m_isEnabled) {
        m_fillColor = QColor(100, 100, 100);
        m_borderColor = QColor(80, 80, 80);
        m_textColor = QColor(150, 150, 150);
    } else if (m_isPrimary) {
        m_fillColor = QColor(70, 130, 180);
        m_borderColor = QColor(255, 215, 0);
        m_textColor = Qt::white;
    } else if (m_isHovered) {
        m_fillColor = QColor(100, 150, 200);
        m_borderColor = QColor(50, 100, 150);
        m_textColor = Qt::white;
    } else {
        m_fillColor = QColor(60, 100, 140);
        m_borderColor = QColor(40, 80, 120);
        m_textColor = Qt::white;
    }
    
    if (m_hdr) {
        m_fillColor = QColor(255, 215, 0, 200); // Gold tint for HDR
    }
}

void VisualMonitorWidget::animateSelection()
{
    if (m_selectionAnimation) {
        m_selectionAnimation->setStartValue(0.7);
        m_selectionAnimation->setEndValue(1.0);
        m_selectionAnimation->start();
    }
} 