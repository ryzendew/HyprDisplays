#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QMessageBox>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QProcess>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QIcon>
#include <QStatusBar>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QDoubleSpinBox>

#include "displaymanager.h"
#include "displaywidget.h"
#include "hyprlandinterface.h"
#include "configmanager.h"
#include "visualmonitorwidget.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QScrollArea;
class QGroupBox;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QSlider;
class QMessageBox;
class QTimer;
class QJsonObject;
class QJsonDocument;
class QJsonArray;
class QFile;
class QDir;
class QStandardPaths;
class QApplication;
class QScreen;
class QWindow;
class QProcess;
class QTextStream;
class QDebug;
class QSettings;
class QCloseEvent;
class QSystemTrayIcon;
class QMenu;
class QAction;
class QStyle;
class QIcon;
class QDoubleSpinBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setMonitorsPath(const QString &path);
    void setNumWorkspaces(int num);

public slots:
    void refreshDisplays();
    void applySettings();
    void saveConfiguration();
    void loadConfiguration();
    void showAbout();
    void toggleFullscreen();
    void showOverlay();
    void showMonitorSettings(const QString& name);
    void updateRefreshRatesForResolution(const QString& resolution, const DisplayInfo& di);

private slots:
    void onDisplayChanged();
    void onWorkspaceAssignmentChanged();
    void onSettingsChanged();
    void onApplyClicked();
    void onResetClicked();
    void onRefreshClicked();
    void onSaveClicked();
    void onLoadClicked();
    void onAboutClicked();
    void onQuitClicked();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupTrayIcon();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void updateDisplayLayout();
    void updateWorkspaceAssignments();
    void showNotification(const QString &message, bool isError = false);
    void closeEvent(QCloseEvent *event) override;

    // UI Elements
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QGraphicsScene *m_monitorLayoutScene;
    QGraphicsView *m_monitorLayoutView;
    QList<VisualMonitorWidget*> m_monitorProxyWidgets;
    QMap<QString, QPointF> m_monitorPositions;
    
    // Monitor settings panel
    QWidget *m_monitorSettingsPanel;
    QGridLayout *m_monitorSettingsLayout;
    QLabel *m_selectedMonitorLabel;
    QComboBox *m_resolutionComboBox;
    QComboBox *m_refreshRateComboBox;
    QDoubleSpinBox *m_scaleSpinBox;
    QCheckBox *m_hdrCheckBox;
    QDoubleSpinBox *m_sdrBrightnessSpinBox;
    QDoubleSpinBox *m_sdrSaturationSpinBox;
    QCheckBox *m_tenBitCheckBox;
    QCheckBox *m_wideGamutCheckBox;
    QPushButton *m_applyMonitorSettingsButton;
    QString m_selectedMonitorName;
    QComboBox *m_vrrComboBox;
    QDoubleSpinBox *m_posXSpinBox;
    QDoubleSpinBox *m_posYSpinBox;
    
    // Status and tray
    QLabel *m_statusLabel;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    
    // Core components
    DisplayManager *m_displayManager;
    HyprlandInterface *m_hyprlandInterface;
    ConfigManager *m_configManager;
    
    // Settings
    QSettings *m_settings;
    QString m_monitorsPath;
    int m_numWorkspaces;
    bool m_autoApply;
    bool m_showOverlay;
    int m_overlayTimeout;
    bool m_minimizeToTray;
    bool m_startMinimized;
    bool m_isUpdatingDisplays;
    bool m_updatingFromSpinbox = false;

    // Workspace management
    QGroupBox *m_workspacesGroup;
    QVBoxLayout *m_workspacesLayout;
    QList<QComboBox*> m_workspaceAssignments;
    
    // Settings
    QGroupBox *m_settingsGroup;
    QGridLayout *m_settingsLayout;
    QSpinBox *m_numWorkspacesSpinBox;
    QCheckBox *m_autoApplyCheckBox;
    QCheckBox *m_showOverlayCheckBox;
    QSpinBox *m_overlayTimeoutSpinBox;
    
    // Buttons
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;

    QList<DisplayInfo> m_currentDisplays;

    int m_layoutMinX = 0;
    int m_layoutMinY = 0;
    qreal m_layoutScale = 1.0;
    qreal m_layoutOffsetX = 0.0;
    qreal m_layoutOffsetY = 0.0;
};

#endif // MAINWINDOW_H 