#include "displaywidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSlider>
#include <QDebug>

DisplayWidget::DisplayWidget(const DisplayInfo &display, QWidget *parent)
    : QWidget(parent)
    , m_displayInfo(display)
    , m_originalDisplayInfo(display)
    , m_hasChanges(false)
    , m_isSelected(false)
    , m_isPrimary(display.primary)
    , m_isEnabled(display.enabled)
    , m_isDragging(false)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_selectionAnimation(nullptr)
    , m_positionAnimation(nullptr)
    , m_opacityEffect(nullptr)
    , m_updateTimer(nullptr)
    , m_validationTimer(nullptr)
    , m_advancedDialog(nullptr)
    , m_scaleDialog(nullptr)
{
    setupUI();
    setupConnections();
    updateFromDisplayInfo();
    
    // Setup animations
    m_selectionAnimation = new QPropertyAnimation(this, "selected", this);
    m_selectionAnimation->setDuration(200);
    
    m_positionAnimation = new QPropertyAnimation(this, "geometry", this);
    m_positionAnimation->setDuration(300);
    
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
    
    // Setup timers
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(100);
    m_updateTimer->setSingleShot(true);
    
    m_validationTimer = new QTimer(this);
    m_validationTimer->setInterval(500);
    m_validationTimer->setSingleShot(true);
    
    // Set widget properties
    setAcceptDrops(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    
    // Set size policy
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(200);
}

DisplayWidget::~DisplayWidget()
{
}

DisplayInfo DisplayWidget::getDisplayInfo() const
{
    return m_displayInfo;
}

void DisplayWidget::setDisplayInfo(const DisplayInfo &display)
{
    m_displayInfo = display;
    updateFromDisplayInfo();
}

bool DisplayWidget::isSelected() const
{
    return m_isSelected;
}

void DisplayWidget::setSelected(bool selected)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
        update();
        
        if (m_selectionAnimation) {
            m_selectionAnimation->setStartValue(0.0);
            m_selectionAnimation->setEndValue(selected ? 1.0 : 0.0);
            m_selectionAnimation->start();
        }
    }
}

bool DisplayWidget::isPrimary() const
{
    return m_isPrimary;
}

void DisplayWidget::setPrimary(bool primary)
{
    if (m_isPrimary != primary) {
        m_isPrimary = primary;
        m_displayInfo.primary = primary;
        update();
        emit settingsChanged();
    }
}

bool DisplayWidget::isEnabled() const
{
    return m_isEnabled;
}

void DisplayWidget::setEnabled(bool enabled)
{
    if (m_isEnabled != enabled) {
        m_isEnabled = enabled;
        m_displayInfo.enabled = enabled;
        update();
        emit settingsChanged();
    }
}

void DisplayWidget::updateFromDisplayInfo()
{
    if (m_nameLabel) m_nameLabel->setText(m_displayInfo.name);
    if (m_resolutionLabel) m_resolutionLabel->setText(m_displayInfo.resolution);
    if (m_refreshLabel) m_refreshLabel->setText(QString("%1 Hz").arg(m_displayInfo.refreshRate));
    if (m_scaleLabel) m_scaleLabel->setText(QString("%1x").arg(m_displayInfo.scale));
    if (m_positionLabel) m_positionLabel->setText(m_displayInfo.position);
    if (m_workspaceLabel) m_workspaceLabel->setText(m_displayInfo.workspace);
    
    if (m_enabledCheckBox) m_enabledCheckBox->setChecked(m_displayInfo.enabled);
    if (m_primaryCheckBox) m_primaryCheckBox->setChecked(m_displayInfo.primary);
    
    updatePreview();
    update();
}

void DisplayWidget::applyChanges()
{
    m_originalDisplayInfo = m_displayInfo;
    m_hasChanges = false;
    emit displayChanged(m_displayInfo);
}

void DisplayWidget::resetChanges()
{
    m_displayInfo = m_originalDisplayInfo;
    updateFromDisplayInfo();
    m_hasChanges = false;
}

bool DisplayWidget::hasChanges() const
{
    return m_hasChanges;
}

void DisplayWidget::setupUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    
    // Create group box
    m_groupBox = new QGroupBox(this);
    m_groupLayout = new QVBoxLayout(m_groupBox);
    m_groupLayout->setSpacing(8);
    
    // Create preview frame
    m_previewFrame = new QFrame(m_groupBox);
    m_previewFrame->setFrameStyle(QFrame::Box);
    m_previewFrame->setMinimumHeight(80);
    m_previewFrame->setMaximumHeight(120);
    
    QVBoxLayout *previewLayout = new QVBoxLayout(m_previewFrame);
    previewLayout->setSpacing(4);
    
    m_previewLabel = new QLabel("Preview", m_previewFrame);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("font-weight: bold; color: #88c0d0;");
    
    m_nameLabel = new QLabel(m_displayInfo.name, m_previewFrame);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("font-weight: bold;");
    
    m_resolutionLabel = new QLabel(m_displayInfo.resolution, m_previewFrame);
    m_refreshLabel = new QLabel(QString("%1 Hz").arg(m_displayInfo.refreshRate), m_previewFrame);
    m_scaleLabel = new QLabel(QString("%1x").arg(m_displayInfo.scale), m_previewFrame);
    m_positionLabel = new QLabel(m_displayInfo.position, m_previewFrame);
    m_workspaceLabel = new QLabel(m_displayInfo.workspace, m_previewFrame);
    
    previewLayout->addWidget(m_previewLabel);
    previewLayout->addWidget(m_nameLabel);
    previewLayout->addWidget(m_resolutionLabel);
    previewLayout->addWidget(m_refreshLabel);
    previewLayout->addWidget(m_scaleLabel);
    previewLayout->addWidget(m_positionLabel);
    previewLayout->addWidget(m_workspaceLabel);
    
    // Create basic settings layout
    m_basicLayout = new QGridLayout();
    m_basicLayout->setSpacing(8);
    
    m_enabledCheckBox = new QCheckBox("Enabled", this);
    m_enabledCheckBox->setChecked(m_displayInfo.enabled);
    
    m_primaryCheckBox = new QCheckBox("Primary", this);
    m_primaryCheckBox->setChecked(m_displayInfo.primary);
    
    m_resolutionComboBox = new QComboBox(this);
    m_resolutionComboBox->addItem(m_displayInfo.resolution);
    m_resolutionComboBox->addItem("1920x1080");
    m_resolutionComboBox->addItem("2560x1440");
    m_resolutionComboBox->addItem("3840x2160");
    
    m_refreshRateSpinBox = new QSpinBox(this);
    m_refreshRateSpinBox->setRange(30, 360);
    m_refreshRateSpinBox->setValue(m_displayInfo.refreshRate);
    m_refreshRateSpinBox->setSuffix(" Hz");
    
    m_scaleSpinBox = new QDoubleSpinBox(this);
    m_scaleSpinBox->setRange(0.5, 3.0);
    m_scaleSpinBox->setSingleStep(0.25);
    m_scaleSpinBox->setValue(m_displayInfo.scale);
    m_scaleSpinBox->setSuffix("x");
    
    m_transformComboBox = new QComboBox(this);
    m_transformComboBox->addItems({"normal", "90", "180", "270", "flipped", "flipped-90"});
    m_transformComboBox->setCurrentText(m_displayInfo.transform);
    
    m_workspaceComboBox = new QComboBox(this);
    m_workspaceComboBox->addItem("Auto", "");
    for (int i = 1; i <= 10; ++i) {
        m_workspaceComboBox->addItem(QString::number(i));
    }
    m_workspaceComboBox->setCurrentText(m_displayInfo.workspace);
    
    m_mirrorComboBox = new QComboBox(this);
    m_mirrorComboBox->addItem("None", "");
    m_mirrorComboBox->setCurrentText(m_displayInfo.mirrorOf);
    
    // Add widgets to basic layout
    m_basicLayout->addWidget(new QLabel("Enabled:"), 0, 0);
    m_basicLayout->addWidget(m_enabledCheckBox, 0, 1);
    m_basicLayout->addWidget(new QLabel("Primary:"), 0, 2);
    m_basicLayout->addWidget(m_primaryCheckBox, 0, 3);
    
    m_basicLayout->addWidget(new QLabel("Resolution:"), 1, 0);
    m_basicLayout->addWidget(m_resolutionComboBox, 1, 1);
    m_basicLayout->addWidget(new QLabel("Refresh Rate:"), 1, 2);
    m_basicLayout->addWidget(m_refreshRateSpinBox, 1, 3);
    
    m_basicLayout->addWidget(new QLabel("Scale:"), 2, 0);
    m_basicLayout->addWidget(m_scaleSpinBox, 2, 1);
    m_basicLayout->addWidget(new QLabel("Transform:"), 2, 2);
    m_basicLayout->addWidget(m_transformComboBox, 2, 3);
    
    m_basicLayout->addWidget(new QLabel("Workspace:"), 3, 0);
    m_basicLayout->addWidget(m_workspaceComboBox, 3, 1);
    m_basicLayout->addWidget(new QLabel("Mirror:"), 3, 2);
    m_basicLayout->addWidget(m_mirrorComboBox, 3, 3);
    
    // Create position controls
    m_positionLayout = new QHBoxLayout();
    m_positionLayout->setSpacing(8);
    
    m_xSpinBox = new QSpinBox(this);
    m_xSpinBox->setRange(-10000, 10000);
    m_xSpinBox->setValue(m_displayInfo.x);
    m_xSpinBox->setSuffix(" x");
    
    m_ySpinBox = new QSpinBox(this);
    m_ySpinBox->setRange(-10000, 10000);
    m_ySpinBox->setValue(m_displayInfo.y);
    m_ySpinBox->setSuffix(" y");
    
    m_positionButton = new QPushButton("Set Position", this);
    
    m_positionLayout->addWidget(new QLabel("Position:"));
    m_positionLayout->addWidget(m_xSpinBox);
    m_positionLayout->addWidget(m_ySpinBox);
    m_positionLayout->addWidget(m_positionButton);
    m_positionLayout->addStretch();
    
    // Create button layout
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(8);
    
    m_previewButton = new QPushButton("Preview", this);
    m_advancedButton = new QPushButton("Advanced", this);
    m_resetButton = new QPushButton("Reset", this);
    m_applyButton = new QPushButton("Apply", this);
    m_deleteButton = new QPushButton("Delete", this);
    
    m_buttonLayout->addWidget(m_previewButton);
    m_buttonLayout->addWidget(m_advancedButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_resetButton);
    m_buttonLayout->addWidget(m_applyButton);
    m_buttonLayout->addWidget(m_deleteButton);
    
    // Add all layouts to group layout
    m_groupLayout->addWidget(m_previewFrame);
    m_groupLayout->addLayout(m_basicLayout);
    m_groupLayout->addLayout(m_positionLayout);
    m_groupLayout->addLayout(m_buttonLayout);
    
    // Add group box to main layout
    m_mainLayout->addWidget(m_groupBox);
    
    // Create context menu
    createContextMenu();
}

void DisplayWidget::setupConnections()
{
    // Basic settings connections
    connect(m_enabledCheckBox, &QCheckBox::toggled, this, &DisplayWidget::onEnabledToggled);
    connect(m_primaryCheckBox, &QCheckBox::toggled, this, &DisplayWidget::onPrimaryToggled);
    connect(m_resolutionComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
            this, &DisplayWidget::onResolutionChanged);
    connect(m_refreshRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &DisplayWidget::onRefreshRateChanged);
    connect(m_scaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &DisplayWidget::onScaleChanged);
    connect(m_transformComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
            this, &DisplayWidget::onTransformChanged);
    connect(m_workspaceComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
            this, &DisplayWidget::onWorkspaceChanged);
    connect(m_mirrorComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
            this, &DisplayWidget::onMirrorChanged);
    
    // Position connections
    connect(m_xSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_displayInfo.x = value;
        m_displayInfo.position = QString("%1x%2").arg(value).arg(m_displayInfo.y);
        onPositionChanged();
    });
    
    connect(m_ySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_displayInfo.y = value;
        m_displayInfo.position = QString("%1x%2").arg(m_displayInfo.x).arg(value);
        onPositionChanged();
    });
    
    connect(m_positionButton, &QPushButton::clicked, this, &DisplayWidget::onPositionChanged);
    
    // Button connections
    connect(m_previewButton, &QPushButton::clicked, this, &DisplayWidget::onPreviewClicked);
    connect(m_advancedButton, &QPushButton::clicked, this, &DisplayWidget::onAdvancedClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &DisplayWidget::onResetClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &DisplayWidget::onApplyClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &DisplayWidget::onDeleteClicked);
    
    // Timer connections
    if (m_updateTimer) connect(m_updateTimer, &QTimer::timeout, this, &DisplayWidget::onDisplayChanged);
    if (m_validationTimer) connect(m_validationTimer, &QTimer::timeout, this, [this]() {
        validateSettings();
    });
}

void DisplayWidget::updatePreview()
{
    // Update preview based on display info
    if (m_previewLabel) {
        QString previewText = QString("%1\n%2@%3Hz\nScale: %4x")
                             .arg(m_displayInfo.resolution)
                             .arg(m_displayInfo.refreshRate)
                             .arg(m_displayInfo.scale);
        m_previewLabel->setText(previewText);
    }
}

void DisplayWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_duplicateAction = new QAction("Duplicate", this);
    m_mirrorAction = new QAction("Mirror", this);
    m_unmirrorAction = new QAction("Unmirror", this);
    m_setPrimaryAction = new QAction("Set as Primary", this);
    m_setSecondaryAction = new QAction("Set as Secondary", this);
    
    m_rotate90Action = new QAction("Rotate 90°", this);
    m_rotate180Action = new QAction("Rotate 180°", this);
    m_rotate270Action = new QAction("Rotate 270°", this);
    m_flipHorizontalAction = new QAction("Flip Horizontal", this);
    m_flipVerticalAction = new QAction("Flip Vertical", this);
    
    m_scale100Action = new QAction("Scale 100%", this);
    m_scale125Action = new QAction("Scale 125%", this);
    m_scale150Action = new QAction("Scale 150%", this);
    m_scale200Action = new QAction("Scale 200%", this);
    m_customScaleAction = new QAction("Custom Scale...", this);
    
    m_contextMenu->addAction(m_duplicateAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_mirrorAction);
    m_contextMenu->addAction(m_unmirrorAction);
    m_contextMenu->addAction(m_setPrimaryAction);
    m_contextMenu->addAction(m_setSecondaryAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_rotate90Action);
    m_contextMenu->addAction(m_rotate180Action);
    m_contextMenu->addAction(m_rotate270Action);
    m_contextMenu->addAction(m_flipHorizontalAction);
    m_contextMenu->addAction(m_flipVerticalAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_scale100Action);
    m_contextMenu->addAction(m_scale125Action);
    m_contextMenu->addAction(m_scale150Action);
    m_contextMenu->addAction(m_scale200Action);
    m_contextMenu->addAction(m_customScaleAction);
    
    // Connect context menu actions
    connect(m_duplicateAction, &QAction::triggered, this, &DisplayWidget::onDuplicateClicked);
    connect(m_mirrorAction, &QAction::triggered, this, &DisplayWidget::onMirrorClicked);
    connect(m_unmirrorAction, &QAction::triggered, this, &DisplayWidget::onUnmirrorClicked);
    connect(m_setPrimaryAction, &QAction::triggered, this, &DisplayWidget::onSetPrimaryClicked);
    connect(m_setSecondaryAction, &QAction::triggered, this, &DisplayWidget::onSetSecondaryClicked);
    connect(m_rotate90Action, &QAction::triggered, this, &DisplayWidget::onRotate90Clicked);
    connect(m_rotate180Action, &QAction::triggered, this, &DisplayWidget::onRotate180Clicked);
    connect(m_rotate270Action, &QAction::triggered, this, &DisplayWidget::onRotate270Clicked);
    connect(m_flipHorizontalAction, &QAction::triggered, this, &DisplayWidget::onFlipHorizontalClicked);
    connect(m_flipVerticalAction, &QAction::triggered, this, &DisplayWidget::onFlipVerticalClicked);
    connect(m_scale100Action, &QAction::triggered, this, &DisplayWidget::onScale100Clicked);
    connect(m_scale125Action, &QAction::triggered, this, &DisplayWidget::onScale125Clicked);
    connect(m_scale150Action, &QAction::triggered, this, &DisplayWidget::onScale150Clicked);
    connect(m_scale200Action, &QAction::triggered, this, &DisplayWidget::onScale200Clicked);
    connect(m_customScaleAction, &QAction::triggered, this, &DisplayWidget::onCustomScaleClicked);
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw selection border
    if (m_isSelected) {
        painter.setPen(QPen(QColor("#5e81ac"), 3));
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
    
    // Draw hover effect
    if (m_isHovered) {
        painter.setPen(QPen(QColor("#81a1c1"), 2));
        painter.drawRect(rect().adjusted(2, 2, -2, -2));
    }
}

void DisplayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        m_dragStartPos = event->pos();
        m_originalPos = pos();
        setSelected(true);
        update();
    }
}

void DisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed && (event->pos() - m_dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
        m_isDragging = true;
        // Start drag operation
    }
}

void DisplayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        m_isDragging = false;
        update();
    }
}

void DisplayWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = true;
    update();
    setToolTip(QString("Display: %1\nResolution: %2\nPosition: %3")
               .arg(m_displayInfo.name)
               .arg(m_displayInfo.resolution)
               .arg(m_displayInfo.position));
}

void DisplayWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = false;
    update();
}

void DisplayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos());
    }
}

void DisplayWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-display-widget")) {
        event->acceptProposedAction();
    }
}

void DisplayWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-display-widget")) {
        event->acceptProposedAction();
    }
}

void DisplayWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-display-widget")) {
        // Handle drop event for display positioning
        event->acceptProposedAction();
        // TODO: Implement drop handling for display positioning
    }
}

// Slot implementations
void DisplayWidget::onDisplayChanged()
{
    updateFromDisplayInfo();
}

void DisplayWidget::onSettingsChanged()
{
    m_hasChanges = true;
    emit settingsChanged();
}

void DisplayWidget::onPrimaryToggled(bool primary)
{
    setPrimary(primary);
}

void DisplayWidget::onEnabledToggled(bool enabled)
{
    setEnabled(enabled);
}

void DisplayWidget::onResolutionChanged(const QString &resolution)
{
    QStringList parts = resolution.split('x');
    if (parts.size() == 2) {
        m_displayInfo.width = parts[0].toInt();
        m_displayInfo.height = parts[1].toInt();
        m_displayInfo.resolution = resolution;
        updatePreview();
        onSettingsChanged();
    }
}

void DisplayWidget::onRefreshRateChanged(int refreshRate)
{
    m_displayInfo.refreshRate = refreshRate;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onScaleChanged(double scale)
{
    m_displayInfo.scale = scale;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onTransformChanged(const QString &transform)
{
    m_displayInfo.transform = transform;
    onSettingsChanged();
}

void DisplayWidget::onPositionChanged()
{
    emit positionChanged(m_displayInfo.name, m_displayInfo.x, m_displayInfo.y);
    onSettingsChanged();
}

void DisplayWidget::onWorkspaceChanged(const QString &workspace)
{
    m_displayInfo.workspace = workspace;
    onSettingsChanged();
}

void DisplayWidget::onMirrorChanged(const QString &mirrorOf)
{
    m_displayInfo.mirrorOf = mirrorOf;
    onSettingsChanged();
}

// Button slot implementations
void DisplayWidget::onPreviewClicked()
{
    // TODO: Implement preview functionality
    qDebug() << "Preview clicked for display:" << m_displayInfo.name;
}

void DisplayWidget::onAdvancedClicked()
{
    // TODO: Implement advanced dialog
    qDebug() << "Advanced clicked for display:" << m_displayInfo.name;
}

void DisplayWidget::onResetClicked()
{
    resetChanges();
}

void DisplayWidget::onApplyClicked()
{
    applyChanges();
}

void DisplayWidget::onDeleteClicked()
{
    // TODO: Implement delete functionality
    qDebug() << "Delete clicked for display:" << m_displayInfo.name;
}

// Context menu slot implementations
void DisplayWidget::onDuplicateClicked()
{
    // TODO: Implement duplicate functionality
    qDebug() << "Duplicate clicked for display:" << m_displayInfo.name;
}

void DisplayWidget::onMirrorClicked()
{
    // TODO: Implement mirror functionality
    qDebug() << "Mirror clicked for display:" << m_displayInfo.name;
}

void DisplayWidget::onUnmirrorClicked()
{
    m_displayInfo.mirrorOf = "";
    onSettingsChanged();
}

void DisplayWidget::onSetPrimaryClicked()
{
    setPrimary(true);
}

void DisplayWidget::onSetSecondaryClicked()
{
    setPrimary(false);
}

void DisplayWidget::onRotate90Clicked()
{
    m_displayInfo.transform = "90";
    onSettingsChanged();
}

void DisplayWidget::onRotate180Clicked()
{
    m_displayInfo.transform = "180";
    onSettingsChanged();
}

void DisplayWidget::onRotate270Clicked()
{
    m_displayInfo.transform = "270";
    onSettingsChanged();
}

void DisplayWidget::onFlipHorizontalClicked()
{
    m_displayInfo.transform = "flipped";
    onSettingsChanged();
}

void DisplayWidget::onFlipVerticalClicked()
{
    m_displayInfo.transform = "flipped-90";
    onSettingsChanged();
}

void DisplayWidget::onScale100Clicked()
{
    m_displayInfo.scale = 1.0;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onScale125Clicked()
{
    m_displayInfo.scale = 1.25;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onScale150Clicked()
{
    m_displayInfo.scale = 1.5;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onScale200Clicked()
{
    m_displayInfo.scale = 2.0;
    updatePreview();
    onSettingsChanged();
}

void DisplayWidget::onCustomScaleClicked()
{
    // TODO: Implement custom scale dialog
    qDebug() << "Custom scale clicked for display:" << m_displayInfo.name;
}

bool DisplayWidget::validateSettings()
{
    // Basic validation
    if (m_displayInfo.width <= 0 || m_displayInfo.height <= 0) {
        showValidationError("Invalid resolution");
        return false;
    }
    
    if (m_displayInfo.refreshRate <= 0) {
        showValidationError("Invalid refresh rate");
        return false;
    }
    
    if (m_displayInfo.scale <= 0) {
        showValidationError("Invalid scale");
        return false;
    }
    
    return true;
}

void DisplayWidget::showValidationError(const QString &message)
{
    QToolTip::showText(mapToGlobal(rect().center()), message, this, rect(), 3000);
} 