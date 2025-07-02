#ifndef HYPRLANDINTERFACE_H
#define HYPRLANDINTERFACE_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QApplication>
#include <QScreen>
#include <QWindow>

#include "displaymanager.h"

class HyprlandInterface : public QObject
{
    Q_OBJECT

public:
    explicit HyprlandInterface(QObject *parent = nullptr);
    ~HyprlandInterface();

    bool isHyprlandRunning() const;
    bool isConnected() const;
    
    // Monitor management
    QStringList getMonitors() const;
    DisplayInfo getMonitorInfo(const QString &name) const;
    bool setMonitor(const DisplayInfo &monitor);
    bool removeMonitor(const QString &name);
    bool moveMonitor(const QString &name, int x, int y);
    bool resizeMonitor(const QString &name, int width, int height);
    bool scaleMonitor(const QString &name, double scale);
    bool transformMonitor(const QString &name, const QString &transform);
    bool mirrorMonitor(const QString &name, const QString &mirrorOf);
    bool unmirrorMonitor(const QString &name);
    bool setPrimaryMonitor(const QString &name);
    bool enableMonitor(const QString &name, bool enabled);
    
    // Workspace management
    QStringList getWorkspaces() const;
    bool moveWorkspaceToMonitor(const QString &workspace, const QString &monitor);
    bool createWorkspace(const QString &name);
    bool removeWorkspace(const QString &name);
    bool renameWorkspace(const QString &oldName, const QString &newName);
    
    // Configuration
    bool reloadConfiguration();
    bool saveConfiguration(const QString &path);
    bool loadConfiguration(const QString &path);
    QString getConfigurationPath() const;
    QString getWorkspacesPath() const;
    
    // Commands
    QString executeCommand(const QStringList &args);
    bool executeCommandAsync(const QStringList &args);
    QString executeHyprctl(const QStringList &args);
    bool executeHyprctlAsync(const QStringList &args);
    
    // Events
    void startEventMonitoring();
    void stopEventMonitoring();
    bool isEventMonitoring() const;

public slots:
    void onMonitorAdded(const QString &name);
    void onMonitorRemoved(const QString &name);
    void onMonitorChanged(const QString &name);
    void onWorkspaceChanged(const QString &name);
    void onConfigurationChanged();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessOutput();

signals:
    void connected();
    void disconnected();
    void monitorAdded(const QString &name);
    void monitorRemoved(const QString &name);
    void monitorChanged(const QString &name);
    void workspaceChanged(const QString &name);
    void configurationChanged();
    void error(const QString &message);
    void success(const QString &message);

private slots:
    void onEventTimerTimeout();
    void onConnectionTimerTimeout();
    void onReconnectTimerTimeout();

private:
    bool parseMonitorOutput(const QString &output);
    bool parseWorkspaceOutput(const QString &output);
    bool parseDeviceOutput(const QString &output);
    bool parseEventOutput(const QString &output);
    
    void updateConnectionStatus();
    void attemptReconnection();
    void setupEventMonitoring();
    void cleanupEventMonitoring();
    
    bool validateMonitorSettings(const DisplayInfo &monitor);
    bool validateWorkspaceSettings(const QString &workspace);
    
    QString buildMonitorCommand(const DisplayInfo &monitor);
    QString buildWorkspaceCommand(const QString &workspace, const QString &monitor);
    
    void logCommand(const QStringList &args);
    void logOutput(const QString &output);
    void logError(const QString &error);
    
    // Connection state
    bool m_isConnected;
    bool m_isHyprlandRunning;
    bool m_isEventMonitoring;
    
    // Processes
    QProcess *m_hyprctlProcess;
    QProcess *m_eventProcess;
    QProcess *m_commandProcess;
    
    // Timers
    QTimer *m_eventTimer;
    QTimer *m_connectionTimer;
    QTimer *m_reconnectTimer;
    
    // Data
    QList<DisplayInfo> m_monitors;
    QStringList m_workspaces;
    QJsonObject m_configuration;
    
    // Paths
    QString m_configPath;
    QString m_workspacesPath;
    QString m_monitorsPath;
    
    // Settings
    int m_reconnectInterval;
    int m_eventInterval;
    int m_maxRetries;
    int m_currentRetries;
    
    // Event monitoring
    QString m_lastEventOutput;
    QRegularExpression m_monitorEventRegex;
    QRegularExpression m_workspaceEventRegex;
    QRegularExpression m_configEventRegex;
    
    // Command queue
    QStringList m_commandQueue;
    bool m_isProcessingCommands;
    
    // Logging
    bool m_enableLogging;
    QString m_logPath;
    QFile *m_logFile;
    QTextStream *m_logStream;
};

#endif // HYPRLANDINTERFACE_H 