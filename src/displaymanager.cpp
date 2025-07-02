#include "displaymanager.h"
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

// DisplayInfo implementation
QJsonObject DisplayInfo::toJson() const
{
    QJsonObject json;
    json["name"] = name;
    json["description"] = description;
    json["manufacturer"] = manufacturer;
    json["model"] = model;
    json["serial"] = serial;
    json["width"] = width;
    json["height"] = height;
    json["refreshRate"] = refreshRate;
    json["x"] = x;
    json["y"] = y;
    json["scale"] = scale;
    json["enabled"] = enabled;
    json["primary"] = primary;
    json["resolution"] = resolution;
    json["position"] = position;
    json["transform"] = transform;
    json["mirrorOf"] = mirrorOf;
    json["workspace"] = workspace;
    json["hdr"] = hdr;
    json["sdrBrightness"] = sdrBrightness;
    json["sdrSaturation"] = sdrSaturation;
    json["vrrMode"] = vrrMode;
    QJsonArray modesArray;
    for (const QString &mode : availableModes) modesArray.append(mode);
    json["availableModes"] = modesArray;
    json["vrrCapable"] = vrrCapable;
    json["hdrCapable"] = hdrCapable;
    json["tenBit"] = tenBit;
    json["wideGamut"] = wideGamut;
    return json;
}

DisplayInfo DisplayInfo::fromJson(const QJsonObject &json)
{
    DisplayInfo info;
    info.name = json["name"].toString();
    info.description = json["description"].toString();
    info.manufacturer = json["manufacturer"].toString();
    info.model = json["model"].toString();
    info.serial = json["serial"].toString();
    info.width = json["width"].toInt();
    info.height = json["height"].toInt();
    info.refreshRate = json["refreshRate"].toInt();
    info.x = json["x"].toInt();
    info.y = json["y"].toInt();
    info.scale = json["scale"].toDouble();
    info.enabled = json["enabled"].toBool();
    info.primary = json["primary"].toBool();
    info.resolution = json["resolution"].toString();
    info.position = json["position"].toString();
    info.transform = json["transform"].toString();
    info.mirrorOf = json["mirrorOf"].toString();
    info.workspace = json["workspace"].toString();
    info.hdr = json["hdr"].toBool();
    info.sdrBrightness = json["sdrBrightness"].toDouble(1.0);
    info.sdrSaturation = json["sdrSaturation"].toDouble(1.0);
    info.vrrMode = json.contains("vrrMode") ? json["vrrMode"].toInt() : 0;
    QJsonArray modesArray = json["availableModes"].toArray();
    for (const QJsonValue &val : modesArray) info.availableModes.append(val.toString());
    info.vrrCapable = json["vrrCapable"].toBool();
    info.hdrCapable = json["hdrCapable"].toBool();
    info.tenBit = json["tenBit"].toBool();
    info.wideGamut = json["wideGamut"].toBool();
    return info;
}

// DisplayManager implementation
DisplayManager::DisplayManager(QObject *parent)
    : QObject(parent)
    , m_numWorkspaces(10)
    , m_hyprctlProcess(nullptr)
    , m_refreshTimer(nullptr)
    , m_isRefreshing(false)
{
    m_hyprctlProcess = new QProcess(this);
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(5000); // Refresh every 5 seconds
    
    connect(m_hyprctlProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DisplayManager::onProcessFinished);
    connect(m_hyprctlProcess, &QProcess::errorOccurred, this, &DisplayManager::onProcessError);
    connect(m_refreshTimer, &QTimer::timeout, this, &DisplayManager::refreshDisplays);
}

DisplayManager::~DisplayManager()
{
    if (m_hyprctlProcess) {
        m_hyprctlProcess->kill();
    }
}

QList<DisplayInfo> DisplayManager::getDisplays() const
{
    return m_displays;
}

DisplayInfo DisplayManager::getDisplay(const QString &name) const
{
    for (const DisplayInfo &display : m_displays) {
        if (display.name == name) {
            return display;
        }
    }
    return DisplayInfo();
}

void DisplayManager::setDisplay(const DisplayInfo &display)
{
    for (int i = 0; i < m_displays.size(); ++i) {
        if (m_displays[i].name == display.name) {
            m_displays[i] = display;
            emit displaysChanged();
            return;
        }
    }
    
    m_displays.append(display);
    emit displaysChanged();
}

void DisplayManager::updateDisplayInMemory(const DisplayInfo &display)
{
    for (int i = 0; i < m_displays.size(); ++i) {
        if (m_displays[i].name == display.name) {
            m_displays[i] = display;
            // Don't emit displaysChanged() to avoid triggering onDisplayChanged
            return;
        }
    }
    
    m_displays.append(display);
    // Don't emit displaysChanged() to avoid triggering onDisplayChanged
}

void DisplayManager::removeDisplay(const QString &name)
{
    for (int i = 0; i < m_displays.size(); ++i) {
        if (m_displays[i].name == name) {
            m_displays.removeAt(i);
            emit displaysChanged();
            return;
        }
    }
}

void DisplayManager::clearDisplays()
{
    m_displays.clear();
    emit displaysChanged();
}

bool DisplayManager::refreshDisplays()
{
    qDebug() << "DisplayManager::refreshDisplays() called";
    
    if (m_isRefreshing) {
        qDebug() << "Already refreshing, skipping";
        return false;
    }
    
    m_isRefreshing = true;
    
    // Execute hyprctl monitors command (JSON output)
    qDebug() << "Executing hyprctl monitors command...";
    QString output = executeHyprctlCommand({"-j", "monitors"});
    if (output.isEmpty()) {
        qWarning() << "Failed to get monitor information from Hyprland";
        m_isRefreshing = false;
        emit error("Failed to get monitor information from Hyprland");
        return false;
    }
    
    qDebug() << "Got output from hyprctl, length:" << output.length();
    qDebug() << "Output preview:" << output.left(200) << "...";
    
    if (!parseHyprctlOutput(output)) {
        qWarning() << "Failed to parse monitor information";
        m_isRefreshing = false;
        emit error("Failed to parse monitor information");
        return false;
    }
    
    qDebug() << "Successfully parsed monitors, count:" << m_displays.size();
    m_isRefreshing = false;
    emit displaysChanged();
    emit success("Displays refreshed successfully");
    return true;
}

bool DisplayManager::applyConfiguration()
{
    if (m_displays.isEmpty()) {
        emit error("No displays to configure");
        return false;
    }
    
    bool success = true;
    QStringList commands;
    
    for (const DisplayInfo &display : m_displays) {
        if (!display.enabled) {
            continue;
        }
        
        QString command = buildMonitorCommand(display);
        if (!command.isEmpty()) {
            commands.append(command);
        }
    }
    
    // Apply each command
    for (const QString &command : commands) {
        if (!executeHyprctlCommandAsync({"dispatch", "keyword", command})) {
            success = false;
            emit error(QString("Failed to apply command: %1").arg(command));
        }
    }
    
    if (success) {
        emit this->success(QString("Configuration applied successfully"));
    }
    
    return success;
}

bool DisplayManager::saveConfiguration(const QString &path)
{
    QJsonObject config;
    QJsonArray displaysArray;
    
    for (const DisplayInfo &display : m_displays) {
        displaysArray.append(display.toJson());
    }
    
    config["displays"] = displaysArray;
    config["numWorkspaces"] = m_numWorkspaces;
    
    QJsonDocument doc(config);
    QFile file(path);
    
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(QString("Failed to open file for writing: %1").arg(path));
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    emit success("Configuration saved successfully");
    return true;
}

bool DisplayManager::loadConfiguration(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Failed to open file for reading: %1").arg(path));
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        emit error("Invalid JSON configuration file");
        return false;
    }
    
    QJsonObject config = doc.object();
    QJsonArray displaysArray = config["displays"].toArray();
    
    m_displays.clear();
    for (const QJsonValue &value : displaysArray) {
        DisplayInfo display = DisplayInfo::fromJson(value.toObject());
        m_displays.append(display);
    }
    
    m_numWorkspaces = config["numWorkspaces"].toInt(10);
    
    emit displaysChanged();
    emit success("Configuration loaded successfully");
    return true;
}

QJsonObject DisplayManager::getConfiguration() const
{
    return m_configuration;
}

void DisplayManager::setConfiguration(const QJsonObject &config)
{
    m_configuration = config;
    emit configurationChanged();
}

QStringList DisplayManager::getDisplayNames() const
{
    QStringList names;
    for (const DisplayInfo &display : m_displays) {
        names.append(display.name);
    }
    return names;
}

QStringList DisplayManager::getWorkspaceNames() const
{
    return m_workspaceNames;
}

void DisplayManager::setNumWorkspaces(int num)
{
    m_numWorkspaces = num;
    
    // Update workspace names
    m_workspaceNames.clear();
    for (int i = 1; i <= num; ++i) {
        m_workspaceNames.append(QString::number(i));
    }
}

int DisplayManager::getNumWorkspaces() const
{
    return m_numWorkspaces;
}

void DisplayManager::onDisplayChanged()
{
    emit displaysChanged();
}

void DisplayManager::onConfigurationChanged()
{
    emit configurationChanged();
}

void DisplayManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode != 0) {
        emit error(QString("Process finished with exit code: %1").arg(exitCode));
    }
}

void DisplayManager::onProcessError(QProcess::ProcessError error)
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

QString DisplayManager::executeHyprctlCommand(const QStringList &args)
{
    m_hyprctlProcess->start("hyprctl", args);
    m_hyprctlProcess->waitForFinished(5000); // Wait up to 5 seconds
    
    if (m_hyprctlProcess->exitCode() != 0) {
        qWarning() << "hyprctl command failed:" << args << "Exit code:" << m_hyprctlProcess->exitCode();
        return QString();
    }
    
    return QString::fromUtf8(m_hyprctlProcess->readAllStandardOutput());
}

bool DisplayManager::executeHyprctlCommandAsync(const QStringList &args)
{
    m_hyprctlProcess->start("hyprctl", args);
    return m_hyprctlProcess->waitForStarted(5000);
}

bool DisplayManager::parseHyprctlOutput(const QString &output)
{
    m_displays.clear();
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (!doc.isArray()) return false;
    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        DisplayInfo di;
        di.name = obj["name"].toString();
        di.description = obj["description"].toString();
        di.manufacturer = obj["make"].toString();
        di.model = obj["model"].toString();
        di.serial = obj["serial"].toString();
        di.width = obj["width"].toInt();
        di.height = obj["height"].toInt();
        di.refreshRate = obj["refreshRate"].toDouble();
        di.x = obj["x"].toInt();
        di.y = obj["y"].toInt();
        di.scale = obj["scale"].toDouble();
        di.enabled = !obj["disabled"].toBool();
        di.primary = obj["focused"].toBool();
        di.resolution = QString("%1x%2").arg(di.width).arg(di.height);
        di.position = QString("%1x%2").arg(di.x).arg(di.y);
        di.transform = QString::number(obj["transform"].toInt());
        di.mirrorOf = obj["mirrorOf"].toString();
        di.workspace = obj["activeWorkspace"].toObject()["name"].toString();
        di.hdr = obj.contains("hdr") ? obj["hdr"].toBool() : false;
        di.sdrBrightness = obj.contains("sdrBrightness") ? obj["sdrBrightness"].toDouble(1.0) : 1.0;
        di.sdrSaturation = obj.contains("sdrSaturation") ? obj["sdrSaturation"].toDouble(1.0) : 1.0;
        di.vrrMode = obj.contains("vrrMode") ? obj["vrrMode"].toInt() : 0;
        di.vrrCapable = obj.contains("vrr");
        di.hdrCapable = true;
        di.tenBit = obj.contains("tenBit") ? obj["tenBit"].toBool() : false;
        di.wideGamut = obj.contains("wideGamut") ? obj["wideGamut"].toBool() : false;
        QJsonArray modes = obj["availableModes"].toArray();
        for (const QJsonValue &mode : modes) di.availableModes.append(mode.toString());
        m_displays.append(di);
    }
    sortDisplays();
    emit displaysChanged();
    return !m_displays.isEmpty();
}

QString DisplayManager::buildMonitorCommand(const DisplayInfo &monitor)
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

void DisplayManager::sortDisplays()
{
    std::sort(m_displays.begin(), m_displays.end(), [](const DisplayInfo &a, const DisplayInfo &b) {
        if (a.y != b.y) {
            return a.y < b.y; // Sort by Y first
        }
        return a.x < b.x; // Then by X
    });
} 