#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include "visualmonitorwidget.h"
#include <limits>
#include <cmath>
#include "monitorgraphicsview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_monitorLayoutScene(new QGraphicsScene(this))
    , m_monitorLayoutView(nullptr)
    , m_monitorSettingsPanel(nullptr)
    , m_monitorSettingsLayout(nullptr)
    , m_selectedMonitorLabel(nullptr)
    , m_resolutionComboBox(nullptr)
    , m_refreshRateComboBox(nullptr)
    , m_scaleSpinBox(nullptr)
    , m_hdrCheckBox(nullptr)
    , m_sdrBrightnessSpinBox(nullptr)
    , m_sdrSaturationSpinBox(nullptr)
    , m_applyMonitorSettingsButton(nullptr)
    , m_selectedMonitorName("")
    , m_vrrComboBox(nullptr)
    , m_statusLabel(nullptr)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_displayManager(new DisplayManager(this))
    , m_hyprlandInterface(nullptr)
    , m_configManager(new ConfigManager(this))
    , m_monitorsPath(QDir::homePath() + "/.config/hypr/monitors.conf")
    , m_numWorkspaces(10)
    , m_settings(nullptr)
    , m_autoApply(false)
    , m_showOverlay(true)
    , m_overlayTimeout(500)
    , m_minimizeToTray(false)
    , m_startMinimized(false)
    , m_isUpdatingDisplays(false)
    , m_tenBitCheckBox(nullptr)
    , m_wideGamutCheckBox(nullptr)
    , m_currentDisplays(QList<DisplayInfo>())
    , m_posXSpinBox(nullptr)
    , m_posYSpinBox(nullptr)
    , m_layoutMinX(INT_MAX)
    , m_layoutMinY(INT_MAX)
    , m_layoutScale(0.0)
    , m_layoutOffsetX(0.0)
    , m_layoutOffsetY(0.0)
    , m_updatingFromSpinbox(false)
{
    qDebug() << "MainWindow constructor started";
    setupUI();
    qDebug() << "UI setup completed";
    setupConnections();
    qDebug() << "Connections setup completed";
    loadSettings();
    qDebug() << "Settings loaded";
    
    // Initialize core components
    qDebug() << "Creating HyprlandInterface...";
    try {
        m_hyprlandInterface = new HyprlandInterface(this);
        qDebug() << "HyprlandInterface created successfully";
    } catch (const std::exception& e) {
        qCritical() << "Failed to create HyprlandInterface:" << e.what();
        m_hyprlandInterface = nullptr;
    } catch (...) {
        qCritical() << "Unknown error creating HyprlandInterface";
        m_hyprlandInterface = nullptr;
    }
    
    // Initialize core components
    m_hyprlandInterface = new HyprlandInterface(this);
    
    // Connect signals
    qDebug() << "Connecting DisplayManager signals...";
    connect(m_displayManager, &DisplayManager::displaysChanged, this, &MainWindow::onDisplayChanged);
    connect(m_displayManager, &DisplayManager::error, this, [this](const QString &message) {
        showNotification(message, true);
    });
    connect(m_displayManager, &DisplayManager::success, this, [this](const QString &message) {
        showNotification(message, false);
    });
    
    qDebug() << "Connecting HyprlandInterface signals...";
    if (m_hyprlandInterface) {
        connect(m_hyprlandInterface, &HyprlandInterface::connected, this, [this]() {
            if (m_statusLabel) m_statusLabel->setText("Connected to Hyprland");
        });
        
        connect(m_hyprlandInterface, &HyprlandInterface::disconnected, this, [this]() {
            if (m_statusLabel) m_statusLabel->setText("Disconnected from Hyprland");
        });
        
        connect(m_hyprlandInterface, &HyprlandInterface::error, this, [this](const QString &message) {
            showNotification(message, true);
        });
        connect(m_hyprlandInterface, &HyprlandInterface::success, this, [this](const QString &message) {
            showNotification(message, false);
        });
    } else {
        qWarning() << "HyprlandInterface is null, skipping signal connections";
    }
    
    // Initial refresh with safety check
    QTimer::singleShot(100, this, [this]() {
        if (m_displayManager) {
            refreshDisplays();
        }
    });
    
    // Load existing monitor.conf if it exists
    QString monitorConfPath = QDir::homePath() + "/.config/hypr/monitors.conf";
    if (QFile::exists(monitorConfPath)) {
        QTimer::singleShot(200, this, [this, monitorConfPath]() {
            if (m_configManager && m_configManager->loadHyprlandMonitors(monitorConfPath)) {
                // Get the loaded configuration
                QJsonObject loadedConfig = m_configManager->getDisplayConfig();
                QJsonArray loadedDisplays = loadedConfig["displays"].toArray();
                
                // Merge loaded settings with current display state
                if (m_displayManager) {
                    QList<DisplayInfo> currentDisplays = m_displayManager->getDisplays();
                    
                    // Create a map of loaded settings by monitor name
                    QMap<QString, QJsonObject> loadedSettings;
                    for (const QJsonValue &val : loadedDisplays) {
                        QJsonObject loadedDisplay = val.toObject();
                        QString name = loadedDisplay["name"].toString();
                        loadedSettings[name] = loadedDisplay;
                    }
                    
                    // Apply loaded settings to current displays
                    for (DisplayInfo &di : currentDisplays) {
                        if (loadedSettings.contains(di.name)) {
                            QJsonObject loaded = loadedSettings[di.name];
                            di.vrrMode = loaded["vrrMode"].toInt(0);
                            di.hdr = loaded["hdr"].toBool(false);
                            di.sdrBrightness = loaded["sdrBrightness"].toDouble(1.0);
                            di.sdrSaturation = loaded["sdrSaturation"].toDouble(1.0);
                            di.scale = loaded["scale"].toDouble(1.0);
                            di.tenBit = loaded["tenBit"].toBool(false);
                            di.wideGamut = loaded["wideGamut"].toBool(false);
                            // Update resolution and refresh rate if they differ
                            if (loaded.contains("width") && loaded.contains("height")) {
                                di.width = loaded["width"].toInt();
                                di.height = loaded["height"].toInt();
                                di.resolution = QString("%1x%2").arg(di.width).arg(di.height);
                            }
                            if (loaded.contains("refreshRate")) {
                                di.refreshRate = loaded["refreshRate"].toDouble();
                            }
                        }
                    }
                    
                    // Update the display manager with merged settings
                    m_displayManager->clearDisplays();
                    for (const DisplayInfo &di : currentDisplays) {
                        m_displayManager->setDisplay(di);
                    }
                }
                showNotification("Loaded existing monitors.conf configuration");
            }
        });
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setMonitorsPath(const QString &path)
{
    m_monitorsPath = path;
    if (m_configManager) {
        // Update config manager path
    }
}

void MainWindow::setNumWorkspaces(int num)
{
    m_numWorkspaces = num;
    if (m_displayManager) {
        m_displayManager->setNumWorkspaces(num);
    }
}

void MainWindow::refreshDisplays()
{
    if (m_displayManager) {
        m_displayManager->refreshDisplays();
    }
}

void MainWindow::applySettings()
{
    if (m_displayManager) {
        // Update DisplayInfo positions from the scene
        QList<DisplayInfo> displays = m_displayManager->getDisplays();
        for (VisualMonitorWidget *vmw : std::as_const(m_monitorProxyWidgets)) {
            QString name = vmw->getName();
            QPointF pos = vmw->pos();
            for (DisplayInfo &di : displays) {
                if (di.name == name) {
                    di.x = static_cast<int>(pos.x() / 0.1);
                    di.y = static_cast<int>(pos.y() / 0.1);
                }
            }
        }
        m_displayManager->clearDisplays();
        for (const DisplayInfo &di : displays) m_displayManager->setDisplay(di);
        if (m_displayManager->applyConfiguration()) {
            showNotification("Configuration applied successfully");
        } else {
            showNotification("Failed to apply configuration", true);
        }
    }
}

void MainWindow::saveConfiguration()
{
    if (m_configManager) {
        if (m_configManager->saveHyprlandMonitors(m_monitorsPath)) {
            showNotification("Configuration saved successfully");
        } else {
            showNotification("Failed to save configuration", true);
        }
    }
}

void MainWindow::loadConfiguration()
{
    if (m_configManager) {
        if (m_configManager->loadHyprlandMonitors(m_monitorsPath)) {
            refreshDisplays();
            showNotification("Configuration loaded successfully");
        } else {
            showNotification("Failed to load configuration", true);
        }
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About HyprDisplays",
        "<h3>HyprDisplays</h3>"
        "<p>A modern Qt6 C++ display management utility for Hyprland.</p>"
        "<p>Version: 1.0.0</p>"
        "<p>Inspired by nwg-displays</p>");
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::showOverlay()
{
    if (m_showOverlay && m_overlayTimeout > 0) {
        // TODO: Implement overlay display
        showNotification("Display overlay shown");
    }
}

void MainWindow::setupUI()
{
    // Set window properties
    setWindowTitle("HyprDisplays - Monitor Configuration");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // Apply dark theme
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 10pt;
        }
        QLabel {
            color: #e0e0e0;
            font-weight: 500;
        }
        QGroupBox {
            border: 2px solid #404040;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            font-weight: bold;
            color: #e0e0e0;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px 0 8px;
            background-color: #2b2b2b;
        }
        QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #404040;
            border: 1px solid #606060;
            border-radius: 4px;
            padding: 6px;
            color: #ffffff;
            min-height: 20px;
        }
        QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover {
            border-color: #808080;
        }
        QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: #0078d4;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #ffffff;
        }
        QPushButton {
            background-color: #0078d4;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            color: white;
            font-weight: 500;
            min-height: 20px;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QPushButton:pressed {
            background-color: #005a9e;
        }
        QPushButton:disabled {
            background-color: #404040;
            color: #808080;
        }
        QCheckBox {
            spacing: 8px;
            color: #e0e0e0;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #606060;
            border-radius: 3px;
            background-color: #404040;
        }
        QCheckBox::indicator:checked {
            background-color: #0078d4;
            border-color: #0078d4;
        }
        QCheckBox::indicator:checked::after {
            content: "✓";
            color: white;
            font-weight: bold;
            font-size: 12px;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollBar:vertical {
            background-color: #404040;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #606060;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #808080;
        }
        QGraphicsView {
            background-color: #1e1e1e;
            border: 2px solid #404040;
            border-radius: 8px;
        }
    )");

    // Create central widget and main layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create monitor arrangement area at the top
    QGroupBox *arrangementGroup = new QGroupBox("Monitor Arrangement", this);
    arrangementGroup->setMinimumHeight(350);
    QVBoxLayout *arrangementLayout = new QVBoxLayout(arrangementGroup);
    
    m_monitorLayoutView = new MonitorGraphicsView(m_monitorLayoutScene, this);
    m_monitorLayoutView->setMinimumHeight(300);
    m_monitorLayoutView->setRenderHint(QPainter::Antialiasing);
    m_monitorLayoutView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_monitorLayoutView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    arrangementLayout->addWidget(m_monitorLayoutView);
    m_mainLayout->addWidget(arrangementGroup);

    // Create monitor settings panel below arrangement
    QGroupBox *settingsGroup = new QGroupBox("Monitor Settings", this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsGroup);
    
    m_monitorSettingsPanel = new QWidget(this);
    m_monitorSettingsLayout = new QGridLayout(m_monitorSettingsPanel);
    m_monitorSettingsLayout->setSpacing(15);
    m_monitorSettingsLayout->setContentsMargins(15, 15, 15, 15);
    
    // Selected monitor label
    m_selectedMonitorLabel = new QLabel("No monitor selected", m_monitorSettingsPanel);
    m_selectedMonitorLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: #0078d4; padding: 10px;");
    m_monitorSettingsLayout->addWidget(m_selectedMonitorLabel, 0, 0, 1, 2);
    
    // Resolution
    m_resolutionComboBox = new QComboBox(m_monitorSettingsPanel);
    m_monitorSettingsLayout->addWidget(new QLabel("Resolution:"), 1, 0);
    m_monitorSettingsLayout->addWidget(m_resolutionComboBox, 1, 1);
    
    // Refresh Rate
    m_refreshRateComboBox = new QComboBox(m_monitorSettingsPanel);
    m_monitorSettingsLayout->addWidget(new QLabel("Refresh Rate:"), 2, 0);
    m_monitorSettingsLayout->addWidget(m_refreshRateComboBox, 2, 1);
    
    // Scale
    m_scaleSpinBox = new QDoubleSpinBox(m_monitorSettingsPanel);
    m_scaleSpinBox->setRange(0.5, 3.0);
    m_scaleSpinBox->setSingleStep(0.05);
    m_scaleSpinBox->setDecimals(2);
    m_monitorSettingsLayout->addWidget(new QLabel("Scale:"), 3, 0);
    m_monitorSettingsLayout->addWidget(m_scaleSpinBox, 3, 1);
    
    // VRR
    m_vrrComboBox = new QComboBox(m_monitorSettingsPanel);
    m_vrrComboBox->addItem("VRR Disabled", 0);
    m_vrrComboBox->addItem("VRR Global", 1);
    m_vrrComboBox->addItem("VRR Fullscreen Only", 2);
    m_monitorSettingsLayout->addWidget(new QLabel("Variable Refresh Rate:"), 4, 0);
    m_monitorSettingsLayout->addWidget(m_vrrComboBox, 4, 1);
    
    // HDR
    m_hdrCheckBox = new QCheckBox("Enable HDR", m_monitorSettingsPanel);
    m_monitorSettingsLayout->addWidget(m_hdrCheckBox, 5, 0, 1, 2);
    
    // SDR Brightness
    m_sdrBrightnessSpinBox = new QDoubleSpinBox(m_monitorSettingsPanel);
    m_sdrBrightnessSpinBox->setRange(0.0, 2.0);
    m_sdrBrightnessSpinBox->setSingleStep(0.01);
    m_sdrBrightnessSpinBox->setDecimals(2);
    m_monitorSettingsLayout->addWidget(new QLabel("SDR Brightness:"), 6, 0);
    m_monitorSettingsLayout->addWidget(m_sdrBrightnessSpinBox, 6, 1);
    
    // SDR Saturation
    m_sdrSaturationSpinBox = new QDoubleSpinBox(m_monitorSettingsPanel);
    m_sdrSaturationSpinBox->setRange(0.0, 2.0);
    m_sdrSaturationSpinBox->setSingleStep(0.01);
    m_sdrSaturationSpinBox->setDecimals(2);
    m_monitorSettingsLayout->addWidget(new QLabel("SDR Saturation:"), 7, 0);
    m_monitorSettingsLayout->addWidget(m_sdrSaturationSpinBox, 7, 1);
    
    // Position X/Y spinboxes
    m_posXSpinBox = new QDoubleSpinBox(m_monitorSettingsPanel);
    m_posXSpinBox->setRange(-10000, 10000);
    m_posXSpinBox->setDecimals(2);
    m_posXSpinBox->setSingleStep(0.1);
    m_monitorSettingsLayout->addWidget(new QLabel("Position X:"), 8, 0);
    m_monitorSettingsLayout->addWidget(m_posXSpinBox, 8, 1);
    m_posYSpinBox = new QDoubleSpinBox(m_monitorSettingsPanel);
    m_posYSpinBox->setRange(-10000, 10000);
    m_posYSpinBox->setDecimals(2);
    m_posYSpinBox->setSingleStep(0.1);
    m_monitorSettingsLayout->addWidget(new QLabel("Y:"), 8, 2);
    m_monitorSettingsLayout->addWidget(m_posYSpinBox, 8, 3);

    // 10bit
    m_tenBitCheckBox = new QCheckBox("Enable 10-bit Output", m_monitorSettingsPanel);
    m_monitorSettingsLayout->addWidget(m_tenBitCheckBox, 9, 0, 1, 2);

    // Wide Gamut
    m_wideGamutCheckBox = new QCheckBox("Enable Wide Gamut", m_monitorSettingsPanel);
    m_monitorSettingsLayout->addWidget(m_wideGamutCheckBox, 10, 0, 1, 2);
    
    // Apply button
    m_applyMonitorSettingsButton = new QPushButton("Apply Monitor Settings", m_monitorSettingsPanel);
    m_applyMonitorSettingsButton->setMinimumHeight(35);
    m_monitorSettingsLayout->addWidget(m_applyMonitorSettingsButton, 11, 0, 1, 2);
    
    settingsLayout->addWidget(m_monitorSettingsPanel);
    m_mainLayout->addWidget(settingsGroup);
    m_monitorSettingsPanel->setVisible(false);

    // Setup status bar
    m_statusLabel = new QLabel("Ready", this);
    m_statusLabel->setStyleSheet("color: #808080; padding: 5px;");
    statusBar()->setStyleSheet("background-color: #1e1e1e; border-top: 1px solid #404040;");
    statusBar()->addWidget(m_statusLabel);

    // Setup system tray
    setupTrayIcon();
}

void MainWindow::setupTrayIcon()
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon(":/icons/hyprdisplays.svg"));
        m_trayIcon->setToolTip("HyprDisplays");
        
        m_trayMenu = new QMenu(this);
        
        QAction *showAction = new QAction("Show", this);
        QAction *hideAction = new QAction("Hide", this);
        QAction *quitAction = new QAction("Quit", this);
        
        m_trayMenu->addAction(showAction);
        m_trayMenu->addAction(hideAction);
        m_trayMenu->addSeparator();
        m_trayMenu->addAction(quitAction);
        
        m_trayIcon->setContextMenu(m_trayMenu);
        
        connect(showAction, &QAction::triggered, this, &QWidget::show);
        connect(hideAction, &QAction::triggered, this, &QWidget::hide);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
        
        m_trayIcon->show();
    }
}

void MainWindow::setupConnections()
{
    // Connect resolution combo box to update refresh rates
    connect(m_resolutionComboBox, &QComboBox::currentTextChanged, this, [this](const QString &res) {
        if (!m_selectedMonitorName.isEmpty() && m_displayManager) {
            QList<DisplayInfo> displays = m_displayManager->getDisplays();
            for (const DisplayInfo &di : displays) {
                if (di.name == m_selectedMonitorName) {
                    updateRefreshRatesForResolution(res, di);
                    break;
                }
            }
        }
    });
    
    connect(m_applyMonitorSettingsButton, &QPushButton::clicked, this, [this]() {
        if (!m_displayManager) return;
        
        qDebug() << "Apply button clicked for monitor:" << m_selectedMonitorName;
        
        // Get current displays and update positions from the scene
        QList<DisplayInfo> displays = m_displayManager->getDisplays();
        
        // 1. Convert all visual positions to logical positions
        QMap<QString, QPoint> logicalPositions;
        for (auto it = m_monitorPositions.begin(); it != m_monitorPositions.end(); ++it) {
            QString name = it.key();
            QPointF storedPos = it.value();
            // Convert stored visual position back to logical position
            QSizeF viewSize = m_monitorLayoutView->viewport()->size();
            const qreal margin = 10.0;
            int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
            for (const DisplayInfo &d : displays) {
                minX = std::min(minX, d.x);
                minY = std::min(minY, d.y);
                maxX = std::max(maxX, d.x + d.width);
                maxY = std::max(maxY, d.y + d.height);
            }
            QRect logicalBoundingRect(minX, minY, maxX - minX, maxY - minY);
            qreal scaleX = (viewSize.width() - 2 * margin) / logicalBoundingRect.width();
            qreal scaleY = (viewSize.height() - 2 * margin) / logicalBoundingRect.height();
            qreal scale = std::min(scaleX, scaleY);
            qreal offsetX = (viewSize.width() - logicalBoundingRect.width() * scale) / 2.0;
            qreal offsetY = (viewSize.height() - logicalBoundingRect.height() * scale) / 2.0;
            int logicalX = static_cast<int>((storedPos.x() - offsetX) / scale + minX);
            int logicalY = static_cast<int>((storedPos.y() - offsetY) / scale + minY);
            logicalPositions[name] = QPoint(logicalX, logicalY);
        }
        // 2. Find the minimum X and Y among all logical positions
        int minLogicalX = INT_MAX, minLogicalY = INT_MAX;
        for (const QPoint &pt : logicalPositions) {
            minLogicalX = std::min(minLogicalX, pt.x());
            minLogicalY = std::min(minLogicalY, pt.y());
        }
        // 3. Subtract minX and minY from all logical positions
        for (auto it = logicalPositions.begin(); it != logicalPositions.end(); ++it) {
            it.value().setX(it.value().x() - minLogicalX);
            it.value().setY(it.value().y() - minLogicalY);
        }
        // 4. Assign these normalized positions to your DisplayInfo objects before saving
        for (DisplayInfo &di : displays) {
            if (logicalPositions.contains(di.name)) {
                di.x = logicalPositions[di.name].x();
                di.y = logicalPositions[di.name].y();
                qDebug() << "Normalized position for" << di.name << "is" << di.x << "x" << di.y;
            }
        }
        
        // If there's a selected monitor, also update its settings from the UI
        if (!m_selectedMonitorName.isEmpty()) {
            for (DisplayInfo &di : displays) {
                if (di.name == m_selectedMonitorName) {
                    qDebug() << "Updating settings for monitor:" << di.name;
                    qDebug() << "  HDR:" << m_hdrCheckBox->isChecked();
                    qDebug() << "  10-bit:" << m_tenBitCheckBox->isChecked();
                    qDebug() << "  Wide gamut:" << m_wideGamutCheckBox->isChecked();
                    
                    di.resolution = m_resolutionComboBox->currentText();
                    QString refreshText = m_refreshRateComboBox->currentText();
                    bool ok = false;
                    double refresh = refreshText.left(refreshText.indexOf(" Hz")).toDouble(&ok);
                    if (ok) di.refreshRate = refresh;
                    di.vrrMode = m_vrrComboBox->currentData().toInt();
                    di.scale = m_scaleSpinBox->value();
                    di.hdr = m_hdrCheckBox->isChecked();
                    di.sdrBrightness = m_sdrBrightnessSpinBox->value();
                    di.sdrSaturation = m_sdrSaturationSpinBox->value();
                    di.tenBit = m_tenBitCheckBox->isChecked();
                    di.wideGamut = m_wideGamutCheckBox->isChecked();
                    break;
                }
            }
        }
        
        // Update the display manager with all modified displays
        m_displayManager->clearDisplays();
        for (const DisplayInfo &di : displays) {
            m_displayManager->setDisplay(di);
        }
        
        // Now get the updated displays and save to config
        displays = m_displayManager->getDisplays();
        QJsonObject displayConfig;
        QJsonArray displaysArray;
        for (const DisplayInfo &di : displays) {
            qDebug() << "Saving display:" << di.name << "x:" << di.x << "y:" << di.y << "HDR:" << di.hdr << "10-bit:" << di.tenBit << "Wide gamut:" << di.wideGamut;
            displaysArray.append(di.toJson());
        }
        displayConfig["displays"] = displaysArray;
        if (m_configManager) {
            m_configManager->setDisplayConfig(displayConfig);
            QString monitorConfPath = QDir::homePath() + "/.config/hypr/monitors.conf";
            if (m_configManager->saveHyprlandMonitors(monitorConfPath)) {
                showNotification("All monitor settings updated and saved to monitors.conf (not yet applied)");
            } else {
                showNotification("Failed to save monitors.conf", true);
            }
        }
    });

    // Connect spinboxes to update model and visual widget
    connect(m_posXSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double x) {
        if (m_selectedMonitorName.isEmpty() || m_updatingFromSpinbox) return;
        m_updatingFromSpinbox = true;
        for (DisplayInfo &di : m_currentDisplays) {
            if (di.name == m_selectedMonitorName) {
                di.x = static_cast<int>(std::round(x));
                qDebug() << "[SpinBox X] Set logical X for" << di.name << ":" << di.x;
                // Move the visual widget
                for (VisualMonitorWidget *vmw : m_monitorProxyWidgets) {
                    if (vmw->getName() == di.name) {
                        QPointF newPos(m_layoutOffsetX + (di.x - m_layoutMinX) * m_layoutScale,
                                       m_layoutOffsetY + (di.y - m_layoutMinY) * m_layoutScale);
                        qDebug() << "[SpinBox X] Moving visual widget for" << di.name << "to scene pos" << newPos;
                        vmw->setPos(newPos);
                        m_monitorPositions[di.name] = newPos.toPoint();
                        break;
                    }
                }
                break;
            }
        }
        m_updatingFromSpinbox = false;
    });
    connect(m_posYSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double y) {
        if (m_selectedMonitorName.isEmpty() || m_updatingFromSpinbox) return;
        m_updatingFromSpinbox = true;
        for (DisplayInfo &di : m_currentDisplays) {
            if (di.name == m_selectedMonitorName) {
                di.y = static_cast<int>(std::round(y));
                qDebug() << "[SpinBox Y] Set logical Y for" << di.name << ":" << di.y;
                // Move the visual widget
                for (VisualMonitorWidget *vmw : m_monitorProxyWidgets) {
                    if (vmw->getName() == di.name) {
                        QPointF newPos(m_layoutOffsetX + (di.x - m_layoutMinX) * m_layoutScale,
                                       m_layoutOffsetY + (di.y - m_layoutMinY) * m_layoutScale);
                        qDebug() << "[SpinBox Y] Moving visual widget for" << di.name << "to scene pos" << newPos;
                        vmw->setPos(newPos);
                        m_monitorPositions[di.name] = newPos.toPoint();
                        break;
                    }
                }
                break;
            }
        }
        m_updatingFromSpinbox = false;
    });
}

void MainWindow::loadSettings()
{
    m_settings = new QSettings("HyprDisplays", "HyprDisplays", this);
    
    m_autoApply = m_settings->value("autoApply", false).toBool();
    m_showOverlay = m_settings->value("showOverlay", true).toBool();
    m_overlayTimeout = m_settings->value("overlayTimeout", 500).toInt();
    m_minimizeToTray = m_settings->value("minimizeToTray", false).toBool();
    m_startMinimized = m_settings->value("startMinimized", false).toBool();
    
    if (m_autoApplyCheckBox) m_autoApplyCheckBox->setChecked(m_autoApply);
    if (m_showOverlayCheckBox) m_showOverlayCheckBox->setChecked(m_showOverlay);
    if (m_overlayTimeoutSpinBox) m_overlayTimeoutSpinBox->setValue(m_overlayTimeout);
}

void MainWindow::saveSettings()
{
    if (m_settings) {
        m_settings->setValue("autoApply", m_autoApply);
        m_settings->setValue("showOverlay", m_showOverlay);
        m_settings->setValue("overlayTimeout", m_overlayTimeout);
        m_settings->setValue("minimizeToTray", m_minimizeToTray);
        m_settings->setValue("startMinimized", m_startMinimized);
    }
}

void MainWindow::updateDisplayLayout()
{
    // This method is now just a placeholder since we removed the display list
    // The monitor arrangement is handled in onDisplayChanged
}

void MainWindow::updateWorkspaceAssignments()
{
    // Clear existing assignments
    for (auto comboBox : m_workspaceAssignments) {
        comboBox->deleteLater();
    }
    m_workspaceAssignments.clear();
    
    if (!m_displayManager) return;
    
    QStringList displayNames = m_displayManager->getDisplayNames();
    
    // Create workspace assignment widgets
    for (int i = 1; i <= m_numWorkspaces; ++i) {
        QLabel *label = new QLabel(QString("Workspace %1:").arg(i), m_workspacesGroup);
        QComboBox *comboBox = new QComboBox(m_workspacesGroup);
        comboBox->addItem("Auto", "");
        comboBox->addItems(displayNames);
        
        m_workspaceAssignments.append(comboBox);
        
        // Add to layout
        QGridLayout *workspacesContainerLayout = qobject_cast<QGridLayout*>(m_workspacesGroup->findChild<QWidget*>()->layout());
        if (workspacesContainerLayout) {
            int row = (i - 1) / 2;
            int col = (i - 1) % 2 * 2;
            workspacesContainerLayout->addWidget(label, row, col);
            workspacesContainerLayout->addWidget(comboBox, row, col + 1);
        }
        
        // Connect signals
        connect(comboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
                this, &MainWindow::onWorkspaceAssignmentChanged);
    }
}

void MainWindow::showNotification(const QString &message, bool isError)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage("HyprDisplays", message, 
                               isError ? QSystemTrayIcon::Critical : QSystemTrayIcon::Information, 3000);
    }
    
    m_statusLabel->setText(message);
    
    if (isError) {
        qWarning() << "HyprDisplays Error:" << message;
    } else {
        qDebug() << "HyprDisplays:" << message;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_minimizeToTray && m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

// Private slots
void MainWindow::onDisplayChanged()
{
    if (m_isUpdatingDisplays) {
        qDebug() << "onDisplayChanged called while already updating, skipping";
        return;
    }
    m_isUpdatingDisplays = true;
    qDebug() << "onDisplayChanged called";
    if (m_monitorSettingsPanel) m_monitorSettingsPanel->setVisible(false);
    m_monitorLayoutScene->clear();
    m_monitorProxyWidgets.clear();
    m_monitorPositions.clear();
    if (!m_displayManager) { qWarning() << "[onDisplayChanged] m_displayManager is null!"; m_isUpdatingDisplays = false; return; }
    QList<DisplayInfo> displays = m_displayManager->getDisplays();
    if (displays.isEmpty()) { qWarning() << "[onDisplayChanged] No displays found!"; m_isUpdatingDisplays = false; return; }
    
    // 1. Calculate logical bounding box
    int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
    for (const DisplayInfo &d : displays) {
        minX = std::min(minX, d.x);
        minY = std::min(minY, d.y);
        maxX = std::max(maxX, d.x + d.width);
        maxY = std::max(maxY, d.y + d.height);
    }
    QRect logicalBoundingRect(minX, minY, maxX - minX, maxY - minY);

    // 2. Calculate scale factor to fill the view (allow scaling up)
    QSizeF viewSize = m_monitorLayoutView->viewport()->size();
    const qreal margin = 10.0;
    qreal scaleX = (viewSize.width() - 2 * margin) / logicalBoundingRect.width();
    qreal scaleY = (viewSize.height() - 2 * margin) / logicalBoundingRect.height();
    qreal scale = std::min(scaleX, scaleY);

    // 3. Center the arrangement in the view
    qreal offsetX = (viewSize.width() - logicalBoundingRect.width() * scale) / 2.0;
    qreal offsetY = (viewSize.height() - logicalBoundingRect.height() * scale) / 2.0;

    // Store layout parameters for stable drag mapping
    m_layoutMinX = minX;
    m_layoutMinY = minY;
    m_layoutScale = scale;
    m_layoutOffsetX = offsetX;
    m_layoutOffsetY = offsetY;

    // 4. Clear scene and add widgets
    m_monitorLayoutScene->clear();
    m_monitorProxyWidgets.clear();
    m_monitorPositions.clear();

    m_currentDisplays = m_displayManager->getDisplays();

    for (const DisplayInfo &d : m_currentDisplays) {
        VisualMonitorWidget *vmw = new VisualMonitorWidget(
            d.name, d.resolution, d.width, d.height, nullptr, scale
        );
        vmw->setPrimary(d.primary);
        vmw->setEnabled(d.enabled);
        vmw->setScale(d.scale);
        vmw->setTransform(d.transform);
        vmw->setHDR(d.hdr);
        vmw->setSDRBrightness(d.sdrBrightness);
        vmw->setSDRSaturation(d.sdrSaturation);
        vmw->setTenBit(d.tenBit);
        vmw->setWideGamut(d.wideGamut);
        // Position with offset so arrangement is centered
        QPointF pos(offsetX + (d.x - minX) * scale, offsetY + (d.y - minY) * scale);
        vmw->setPos(pos);
        m_monitorLayoutScene->addItem(vmw);
        m_monitorProxyWidgets.append(vmw);
        m_monitorPositions[d.name] = pos.toPoint();
        connect(vmw, &VisualMonitorWidget::monitorMoved, this, [this, name=d.name](const QString &, const QPointF &pos) {
            // Update the stored position
            m_monitorPositions[name] = pos.toPoint();
            // Update the logical position in the persistent model in real time using stored layout params
            for (DisplayInfo &di : m_currentDisplays) {
                if (di.name == name) {
                    di.x = static_cast<int>((pos.x() - m_layoutOffsetX) / m_layoutScale + m_layoutMinX);
                    di.y = static_cast<int>((pos.y() - m_layoutOffsetY) / m_layoutScale + m_layoutMinY);
                    qDebug() << "[monitorMoved] Scene pos:" << pos << "-> Logical:" << di.x << di.y << "(layout min:" << m_layoutMinX << m_layoutMinY << ", scale:" << m_layoutScale << ", offset:" << m_layoutOffsetX << m_layoutOffsetY << ")";
                    m_updatingFromSpinbox = true;
                    if (m_posXSpinBox) { m_posXSpinBox->blockSignals(true); m_posXSpinBox->setValue(static_cast<double>(di.x)); m_posXSpinBox->blockSignals(false); }
                    if (m_posYSpinBox) { m_posYSpinBox->blockSignals(true); m_posYSpinBox->setValue(static_cast<double>(di.y)); m_posYSpinBox->blockSignals(false); }
                    m_updatingFromSpinbox = false;
                    qDebug() << "[monitorMoved] Updated logical position for" << name << "to" << di.x << "x" << di.y;
                    break;
                }
            }
        });
        connect(vmw, &VisualMonitorWidget::monitorClicked, this, [this, name=d.name](const QString &) {
            qDebug() << "Monitor clicked:" << name;
            showMonitorSettings(name);
        });
    }

    // 5. Set scene rect to the full view size (so it always fills the view)
    m_monitorLayoutScene->setSceneRect(0, 0, viewSize.width(), viewSize.height());

    // Auto-select the primary monitor or center monitor on first load
    if (m_selectedMonitorName.isEmpty()) {
        QString monitorToSelect;
        
        // First try to find the primary monitor
        for (const DisplayInfo &d : displays) {
            if (d.primary) {
                monitorToSelect = d.name;
                break;
            }
        }
        
        // If no primary monitor, find the center monitor
        if (monitorToSelect.isEmpty() && displays.size() > 0) {
            // Calculate center of all monitors
            int totalX = 0, totalY = 0;
            for (const DisplayInfo &d : displays) {
                totalX += d.x + d.width / 2;
                totalY += d.y + d.height / 2;
            }
            int centerX = totalX / displays.size();
            int centerY = totalY / displays.size();
            
            // Find monitor closest to center
            double minDistance = std::numeric_limits<double>::max();
            for (const DisplayInfo &d : displays) {
                int monitorCenterX = d.x + d.width / 2;
                int monitorCenterY = d.y + d.height / 2;
                double distance = std::sqrt(std::pow(monitorCenterX - centerX, 2) + std::pow(monitorCenterY - centerY, 2));
                if (distance < minDistance) {
                    minDistance = distance;
                    monitorToSelect = d.name;
                }
            }
        }
        
        // If still no selection, just pick the first monitor
        if (monitorToSelect.isEmpty() && displays.size() > 0) {
            monitorToSelect = displays.first().name;
        }
        
        if (!monitorToSelect.isEmpty()) {
            qDebug() << "Auto-selecting monitor:" << monitorToSelect;
            showMonitorSettings(monitorToSelect);
        }
    }
    
    m_isUpdatingDisplays = false;
    qDebug() << "onDisplayChanged finished";
}

void MainWindow::onWorkspaceAssignmentChanged()
{
    if (m_autoApply) {
        applySettings();
    }
}

void MainWindow::onSettingsChanged()
{
    if (m_autoApply) {
        applySettings();
    }
}

void MainWindow::onApplyClicked()
{
    applySettings();
}

void MainWindow::onResetClicked()
{
    if (m_displayManager) {
        m_displayManager->loadConfiguration(m_monitorsPath);
        refreshDisplays();
        showNotification("Configuration reset");
    }
}

void MainWindow::onRefreshClicked()
{
    refreshDisplays();
}

void MainWindow::onSaveClicked()
{
    saveConfiguration();
}

void MainWindow::onLoadClicked()
{
    loadConfiguration();
}

void MainWindow::onAboutClicked()
{
    showAbout();
}

void MainWindow::onQuitClicked()
{
    qApp->quit();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    }
}

void MainWindow::showMonitorSettings(const QString& name)
{
    if (m_isUpdatingDisplays) {
        qDebug() << "showMonitorSettings called while updating displays, skipping";
        return;
    }
    qDebug() << "showMonitorSettings called for" << name;
    // Save settings for the previously selected monitor
    if (!m_selectedMonitorName.isEmpty() && m_displayManager) {
        QList<DisplayInfo> displays = m_displayManager->getDisplays();
        for (DisplayInfo &di : displays) {
            if (di.name == m_selectedMonitorName) {
                di.resolution = m_resolutionComboBox->currentText();
                QString refreshText = m_refreshRateComboBox->currentText();
                bool ok = false;
                double refresh = refreshText.left(refreshText.indexOf(" Hz")).toDouble(&ok);
                if (ok) di.refreshRate = refresh;
                di.vrrMode = m_vrrComboBox->currentData().toInt();
                di.scale = m_scaleSpinBox->value();
                di.hdr = m_hdrCheckBox->isChecked();
                di.sdrBrightness = m_sdrBrightnessSpinBox->value();
                di.sdrSaturation = m_sdrSaturationSpinBox->value();
                di.tenBit = m_tenBitCheckBox->isChecked();
                di.wideGamut = m_wideGamutCheckBox->isChecked();
            }
        }
    }
    m_selectedMonitorName = name;
    if (!m_displayManager) { qWarning() << "[showMonitorSettings] m_displayManager is null!"; return; }
    QList<DisplayInfo> displays = m_displayManager->getDisplays();
    for (const DisplayInfo &di : displays) {
        if (di.name == name) {
            if (!m_selectedMonitorLabel || !m_resolutionComboBox || !m_refreshRateComboBox || !m_scaleSpinBox || !m_hdrCheckBox || !m_sdrBrightnessSpinBox || !m_sdrSaturationSpinBox || !m_vrrComboBox || !m_tenBitCheckBox || !m_wideGamutCheckBox || !m_posXSpinBox || !m_posYSpinBox) {
                qWarning() << "[showMonitorSettings] One or more UI widgets are null when showing settings!";
                return;
            }
            m_selectedMonitorLabel->setText(QString("Settings for %1").arg(name));
            m_resolutionComboBox->blockSignals(true);
            m_resolutionComboBox->clear();
            QSet<QString> resolutions;
            for (const QString &mode : di.availableModes) {
                int atIndex = mode.indexOf('@');
                if (atIndex > 0) {
                    resolutions.insert(mode.left(atIndex));
                }
            }
            QStringList resList = resolutions.values();
            std::sort(resList.begin(), resList.end(), [](const QString &a, const QString &b) {
                QStringList aParts = a.split('x');
                QStringList bParts = b.split('x');
                if (aParts.size() == 2 && bParts.size() == 2) {
                    int aWidth = aParts[0].toInt();
                    int bWidth = bParts[0].toInt();
                    if (aWidth != bWidth) return aWidth > bWidth;
                    return aParts[1].toInt() > bParts[1].toInt();
                }
                return a > b;
            });
            m_resolutionComboBox->addItems(resList);
            m_resolutionComboBox->blockSignals(false);
            int currentResIndex = m_resolutionComboBox->findText(di.resolution);
            if (currentResIndex >= 0) {
                m_resolutionComboBox->setCurrentIndex(currentResIndex);
            }
            updateRefreshRatesForResolution(di.resolution, di);
            m_scaleSpinBox->setValue(di.scale);
            m_hdrCheckBox->setChecked(di.hdr);
            m_sdrBrightnessSpinBox->setValue(di.sdrBrightness);
            m_sdrSaturationSpinBox->setValue(di.sdrSaturation);
            m_tenBitCheckBox->setChecked(di.tenBit);
            m_wideGamutCheckBox->setChecked(di.wideGamut);
            int vrrIndex = 0;
            if (di.vrrMode == 1) vrrIndex = 1;
            else if (di.vrrMode == 2) vrrIndex = 2;
            m_vrrComboBox->setCurrentIndex(vrrIndex);
            m_vrrComboBox->setVisible(di.vrrCapable);
            m_hdrCheckBox->setVisible(di.hdrCapable);
            m_posXSpinBox->blockSignals(true);
            m_posXSpinBox->setValue(static_cast<double>(di.x));
            m_posXSpinBox->blockSignals(false);
            m_posYSpinBox->blockSignals(true);
            m_posYSpinBox->setValue(static_cast<double>(di.y));
            m_posYSpinBox->blockSignals(false);
            m_monitorSettingsPanel->setVisible(true);
            qDebug() << "showMonitorSettings finished for" << name;
            return;
        }
    }
    qWarning() << "[showMonitorSettings] No display found for" << name;
}

void MainWindow::updateRefreshRatesForResolution(const QString& resolution, const DisplayInfo& di)
{
    m_refreshRateComboBox->blockSignals(true);
    m_refreshRateComboBox->clear();
    QSet<QString> refreshRates;
    for (const QString &mode : di.availableModes) {
        int atIndex = mode.indexOf('@');
        if (atIndex > 0) {
            QString res = mode.left(atIndex);
            QString rate = mode.mid(atIndex + 1);
            if (rate.endsWith("Hz")) rate.chop(2);
            if (res == resolution) refreshRates.insert(rate.trimmed());
        }
    }
    QStringList rateList = refreshRates.values();
    std::sort(rateList.begin(), rateList.end(), [](const QString &a, const QString &b) {
        return a.toDouble() > b.toDouble();
    });
    for (const QString &rate : rateList) m_refreshRateComboBox->addItem(rate + " Hz");
    
    // Set to current refresh rate if available, else first
    QString currentRate = QString::number(di.refreshRate);
    bool found = false;
    for (const QString &rate : refreshRates) {
        if (rate == currentRate) {
            m_refreshRateComboBox->setCurrentText(rate + " Hz");
            found = true;
            break;
        }
    }
    if (!found && !refreshRates.isEmpty())
        m_refreshRateComboBox->setCurrentIndex(0);
    m_refreshRateComboBox->blockSignals(false);
} 