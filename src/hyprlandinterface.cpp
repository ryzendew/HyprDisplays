#include "hyprlandinterface.h"
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

HyprlandInterface::HyprlandInterface(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
    , m_isHyprlandRunning(false)
    , m_isEventMonitoring(false)
    , m_hyprctlProcess(nullptr)
    , m_eventProcess(nullptr)
    , m_commandProcess(nullptr)
    , m_eventTimer(nullptr)
    , m_connectionTimer(nullptr)
    , m_reconnectTimer(nullptr)
    , m_reconnectInterval(5000)
    , m_eventInterval(1000)
    , m_maxRetries(3)
    , m_currentRetries(0)
    , m_isProcessingCommands(false)
    , m_enableLogging(false)
    , m_logFile(nullptr)
    , m_logStream(nullptr)
{
    // Initialize processes
    m_hyprctlProcess = new QProcess(this);
    m_eventProcess = new QProcess(this);
    m_commandProcess = new QProcess(this);
    
    // Initialize timers
    m_eventTimer = new QTimer(this);
    m_connectionTimer = new QTimer(this);
    m_reconnectTimer = new QTimer(this);
    
    m_eventTimer->setInterval(m_eventInterval);
    m_connectionTimer->setInterval(10000); // Check connection every 10 seconds
    m_reconnectTimer->setInterval(m_reconnectInterval);
    
    // Setup paths
    m_configPath = QDir::homePath() + "/.config/hypr/hyprland.conf";
    m_workspacesPath = QDir::homePath() + "/.config/hypr/workspaces.conf";
    m_monitorsPath = QDir::homePath() + "/.config/hypr/monitors.conf";
    
    // Setup regular expressions
    m_monitorEventRegex = QRegularExpression(R"(monitoradded|monitorremoved|monitorchanged)");
    m_workspaceEventRegex = QRegularExpression(R"(workspace|workspaceadded|workspaceremoved)");
    m_configEventRegex = QRegularExpression(R"(configreloaded)");
    
    // Connect signals
    connect(m_hyprctlProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &HyprlandInterface::onProcessFinished);
    connect(m_hyprctlProcess, &QProcess::errorOccurred, this, &HyprlandInterface::onProcessError);
    
    connect(m_eventProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &HyprlandInterface::onProcessFinished);
    connect(m_eventProcess, &QProcess::errorOccurred, this, &HyprlandInterface::onProcessError);
    connect(m_eventProcess, &QProcess::readyReadStandardOutput, this, &HyprlandInterface::onProcessOutput);
    
    connect(m_commandProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &HyprlandInterface::onProcessFinished);
    connect(m_commandProcess, &QProcess::errorOccurred, this, &HyprlandInterface::onProcessError);
    
    connect(m_eventTimer, &QTimer::timeout, this, &HyprlandInterface::onEventTimerTimeout);
    connect(m_connectionTimer, &QTimer::timeout, this, &HyprlandInterface::onConnectionTimerTimeout);
    connect(m_reconnectTimer, &QTimer::timeout, this, &HyprlandInterface::onReconnectTimerTimeout);
    
    // Start connection monitoring
    m_connectionTimer->start();
    
    // Initial connection check
    updateConnectionStatus();
}

HyprlandInterface::~HyprlandInterface()
{
    stopEventMonitoring();
    
    if (m_hyprctlProcess) {
        m_hyprctlProcess->kill();
    }
    if (m_eventProcess) {
        m_eventProcess->kill();
    }
    if (m_commandProcess) {
        m_commandProcess->kill();
    }
    
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }
}

bool HyprlandInterface::isHyprlandRunning() const
{
    return m_isHyprlandRunning;
}

bool HyprlandInterface::isConnected() const
{
    return m_isConnected;
}

QStringList HyprlandInterface::getMonitors() const
{
    QStringList names;
    for (const DisplayInfo &monitor : m_monitors) {
        names.append(monitor.name);
    }
    return names;
}

DisplayInfo HyprlandInterface::getMonitorInfo(const QString &name) const
{
    for (const DisplayInfo &monitor : m_monitors) {
        if (monitor.name == name) {
            return monitor;
        }
    }
    return DisplayInfo();
}

bool HyprlandInterface::setMonitor(const DisplayInfo &monitor)
{
    if (!validateMonitorSettings(monitor)) {
        return false;
    }
    
    QString command = buildMonitorCommand(monitor);
    if (command.isEmpty()) {
        emit error("Failed to build monitor command");
        return false;
    }
    
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::removeMonitor(const QString &name)
{
    return executeCommandAsync({"dispatch", "keyword", QString("monitor,%1,disable").arg(name)});
}

bool HyprlandInterface::moveMonitor(const QString &name, int x, int y)
{
    QString command = QString("monitor,%1,addreserved,%2,%3,%4,%5")
                     .arg(name).arg(x).arg(y).arg(0).arg(0);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::resizeMonitor(const QString &name, int width, int height)
{
    QString command = QString("monitor,%1,%2x%3")
                     .arg(name).arg(width).arg(height);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::scaleMonitor(const QString &name, double scale)
{
    QString command = QString("monitor,%1,scale,%2")
                     .arg(name).arg(scale);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::transformMonitor(const QString &name, const QString &transform)
{
    QString command = QString("monitor,%1,transform,%2")
                     .arg(name).arg(transform);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::mirrorMonitor(const QString &name, const QString &mirrorOf)
{
    QString command = QString("monitor,%1,mirror,%2")
                     .arg(name).arg(mirrorOf);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::unmirrorMonitor(const QString &name)
{
    QString command = QString("monitor,%1,unmirror")
                     .arg(name);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::setPrimaryMonitor(const QString &name)
{
    QString command = QString("monitor,%1,primary")
                     .arg(name);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::enableMonitor(const QString &name, bool enabled)
{
    QString command = QString("monitor,%1,%2")
                     .arg(name).arg(enabled ? "enable" : "disable");
    return executeCommandAsync({"dispatch", "keyword", command});
}

QStringList HyprlandInterface::getWorkspaces() const
{
    return m_workspaces;
}

bool HyprlandInterface::moveWorkspaceToMonitor(const QString &workspace, const QString &monitor)
{
    QString command = QString("workspace,%1,monitor,%2")
                     .arg(workspace).arg(monitor);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::createWorkspace(const QString &name)
{
    QString command = QString("workspace,%1,add")
                     .arg(name);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::removeWorkspace(const QString &name)
{
    QString command = QString("workspace,%1,remove")
                     .arg(name);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::renameWorkspace(const QString &oldName, const QString &newName)
{
    QString command = QString("workspace,%1,rename,%2")
                     .arg(oldName).arg(newName);
    return executeCommandAsync({"dispatch", "keyword", command});
}

bool HyprlandInterface::reloadConfiguration()
{
    return executeCommandAsync({"reload"});
}

bool HyprlandInterface::saveConfiguration(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement configuration saving
    return true;
}

bool HyprlandInterface::loadConfiguration(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Implement configuration loading
    return true;
}

QString HyprlandInterface::getConfigurationPath() const
{
    return m_configPath;
}

QString HyprlandInterface::getWorkspacesPath() const
{
    return m_workspacesPath;
}

QString HyprlandInterface::executeCommand(const QStringList &args)
{
    m_hyprctlProcess->start("hyprctl", args);
    m_hyprctlProcess->waitForFinished(5000);
    
    if (m_hyprctlProcess->exitCode() != 0) {
        logError(QString("Command failed: %1").arg(args.join(' ')));
        return QString();
    }
    
    QString output = QString::fromUtf8(m_hyprctlProcess->readAllStandardOutput());
    logOutput(output);
    return output;
}

bool HyprlandInterface::executeCommandAsync(const QStringList &args)
{
    m_commandProcess->start("hyprctl", args);
    return m_commandProcess->waitForStarted(5000);
}

QString HyprlandInterface::executeHyprctl(const QStringList &args)
{
    return executeCommand(args);
}

bool HyprlandInterface::executeHyprctlAsync(const QStringList &args)
{
    return executeCommandAsync(args);
}

void HyprlandInterface::startEventMonitoring()
{
    if (m_isEventMonitoring) {
        return;
    }
    
    setupEventMonitoring();
    m_isEventMonitoring = true;
    m_eventTimer->start();
}

void HyprlandInterface::stopEventMonitoring()
{
    if (!m_isEventMonitoring) {
        return;
    }
    
    cleanupEventMonitoring();
    m_isEventMonitoring = false;
    m_eventTimer->stop();
}

bool HyprlandInterface::isEventMonitoring() const
{
    return m_isEventMonitoring;
}

void HyprlandInterface::onMonitorAdded(const QString &name)
{
    emit monitorAdded(name);
}

void HyprlandInterface::onMonitorRemoved(const QString &name)
{
    emit monitorRemoved(name);
}

void HyprlandInterface::onMonitorChanged(const QString &name)
{
    emit monitorChanged(name);
}

void HyprlandInterface::onWorkspaceChanged(const QString &name)
{
    emit workspaceChanged(name);
}

void HyprlandInterface::onConfigurationChanged()
{
    emit configurationChanged();
}

void HyprlandInterface::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    // Handle process completion
}

void HyprlandInterface::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start hyprctl process";
            break;
        case QProcess::Crashed:
            errorMsg = "hyprctl process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "hyprctl process timed out";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error to hyprctl process";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error from hyprctl process";
            break;
        default:
            errorMsg = "Unknown process error";
            break;
    }
    emit this->error(QString(errorMsg));
}

void HyprlandInterface::onProcessOutput()
{
    QString output = QString::fromUtf8(m_eventProcess->readAllStandardOutput());
    parseEventOutput(output);
}

void HyprlandInterface::onEventTimerTimeout()
{
    // Check for new events
    if (m_isEventMonitoring) {
        // Poll for events if needed
    }
}

void HyprlandInterface::onConnectionTimerTimeout()
{
    updateConnectionStatus();
}

void HyprlandInterface::onReconnectTimerTimeout()
{
    attemptReconnection();
}

void HyprlandInterface::updateConnectionStatus()
{
    bool wasConnected = m_isConnected;
    
    // Check if hyprctl is available
    QProcess testProcess;
    testProcess.start("hyprctl", {"version"});
    testProcess.waitForFinished(2000);
    
    m_isHyprlandRunning = (testProcess.exitCode() == 0);
    m_isConnected = m_isHyprlandRunning;
    
    if (m_isConnected && !wasConnected) {
        emit connected();
        m_currentRetries = 0;
    } else if (!m_isConnected && wasConnected) {
        emit disconnected();
        if (m_currentRetries < m_maxRetries) {
            m_reconnectTimer->start();
        }
    }
}

void HyprlandInterface::attemptReconnection()
{
    m_currentRetries++;
    updateConnectionStatus();
    
    if (!m_isConnected && m_currentRetries < m_maxRetries) {
        m_reconnectTimer->start();
    }
}

void HyprlandInterface::setupEventMonitoring()
{
    // Start hyprctl event monitoring
    m_eventProcess->start("hyprctl", {"-j", "events"});
}

void HyprlandInterface::cleanupEventMonitoring()
{
    if (m_eventProcess->state() != QProcess::NotRunning) {
        m_eventProcess->kill();
    }
}

bool HyprlandInterface::validateMonitorSettings(const DisplayInfo &monitor)
{
    if (monitor.name.isEmpty()) {
        emit error("Monitor name cannot be empty");
        return false;
    }
    
    if (monitor.width <= 0 || monitor.height <= 0) {
        emit error("Invalid monitor resolution");
        return false;
    }
    
    if (monitor.refreshRate <= 0) {
        emit error("Invalid refresh rate");
        return false;
    }
    
    if (monitor.scale <= 0) {
        emit error("Invalid scale factor");
        return false;
    }
    
    return true;
}

bool HyprlandInterface::validateWorkspaceSettings(const QString &workspace)
{
    if (workspace.isEmpty()) {
        emit error("Workspace name cannot be empty");
        return false;
    }
    
    return true;
}

QString HyprlandInterface::buildMonitorCommand(const DisplayInfo &monitor)
{
    QString command = QString("monitor,%1,%2x%3@%4,%5x%6,%7")
                     .arg(monitor.name)
                     .arg(monitor.width)
                     .arg(monitor.height)
                     .arg(monitor.refreshRate)
                     .arg(monitor.x)
                     .arg(monitor.y)
                     .arg(monitor.scale);
    
    if (!monitor.transform.isEmpty() && monitor.transform != "normal") {
        command += QString(",%1").arg(monitor.transform);
    }
    
    if (!monitor.mirrorOf.isEmpty()) {
        command += QString(",mirror,%1").arg(monitor.mirrorOf);
    }
    
    return command;
}

QString HyprlandInterface::buildWorkspaceCommand(const QString &workspace, const QString &monitor)
{
    return QString("workspace,%1,monitor,%2").arg(workspace).arg(monitor);
}

void HyprlandInterface::logCommand(const QStringList &args)
{
    if (m_enableLogging && m_logStream) {
        *m_logStream << "COMMAND: " << args.join(' ') << Qt::endl;
        m_logStream->flush();
    }
}

void HyprlandInterface::logOutput(const QString &output)
{
    if (m_enableLogging && m_logStream) {
        *m_logStream << "OUTPUT: " << output << Qt::endl;
        m_logStream->flush();
    }
}

void HyprlandInterface::logError(const QString &error)
{
    if (m_enableLogging && m_logStream) {
        *m_logStream << "ERROR: " << error << Qt::endl;
        m_logStream->flush();
    }
}

bool HyprlandInterface::parseMonitorOutput(const QString &output)
{
    Q_UNUSED(output)
    // TODO: Implement monitor output parsing
    return true;
}

bool HyprlandInterface::parseWorkspaceOutput(const QString &output)
{
    Q_UNUSED(output)
    // TODO: Implement workspace output parsing
    return true;
}

bool HyprlandInterface::parseDeviceOutput(const QString &output)
{
    Q_UNUSED(output)
    // TODO: Implement device output parsing
    return true;
}

bool HyprlandInterface::parseEventOutput(const QString &output)
{
    Q_UNUSED(output)
    // TODO: Implement event output parsing
    return true;
} 